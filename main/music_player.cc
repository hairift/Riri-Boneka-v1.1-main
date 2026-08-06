#include "music_player.h"

#include "application.h"
#include "audio_codec.h"
#include "board.h"
#include "system_info.h"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <memory>
#include <new>

#include <cJSON.h>
#include <esp_log.h>
#include <esp_timer.h>
#include <psa/crypto.h>
#include <sdkconfig.h>

#include "esp_ae_rate_cvt.h"
#include "esp_audio_dec_default.h"
#include "esp_audio_simple_dec.h"
#include "esp_audio_simple_dec_default.h"

#ifndef CONFIG_MUSIC_PLAYER_ENABLE
#define CONFIG_MUSIC_PLAYER_ENABLE 0
#endif

#ifndef CONFIG_RIRI_SERVER_URL
#define CONFIG_RIRI_SERVER_URL ""
#endif

#ifndef CONFIG_RIRI_SERVER_SECRET
#define CONFIG_RIRI_SERVER_SECRET "riri-boneka-2026"
#endif

#define TAG "MusicPlayer"

namespace {

constexpr int kMusicSearchConnectId = 4;
constexpr int kMusicStreamConnectId = 5;

// Cadangan melawan Wi-Fi tersendat, penyebab utama suara patah-patah.
// scripts/music_server.py mengirim MP3 mono 24 kHz sekitar 96 kbps, jadi satu
// detik audio hanya memakai sekitar 12 KB dan kapasitas ini setara belasan
// detik. Untuk sumber 320 kbps stereo cadangannya sekitar 4,8 detik.
constexpr size_t kRingCapacity = 192 * 1024;
// Isi awal sebelum dekode dimulai. Nilai ini menahan gangguan jaringan pertama
// tanpa membuat jeda mulai terasa lama.
constexpr size_t kPrebufferBytes = 32 * 1024;
// Ambang isi ulang penyangga saat jaringan tersendat di tengah lagu.
constexpr size_t kRebufferLowWaterBytes = 6 * 1024;
constexpr size_t kRebufferTargetBytes = 24 * 1024;
constexpr int kRebufferTimeoutMs = 5000;

constexpr size_t kNetworkChunkBytes = 1460;
constexpr size_t kDecodeChunkBytes = 2048;
// Panjang header maupun footer ID3v2 selalu 10 byte.
constexpr size_t kId3HeaderBytes = 10;
// Satu bingkai MP3 stereo 16 bit paling besar 1152 sampel per kanal.
constexpr size_t kMaxPcmBufferBytes = 64 * 1024;

// Batas waktu permintaan lagu ke server.
//
// Angka ini bukan lama server menjawab, melainkan lama terburuk yang masuk
// akal. Server yang sudah panas menjawab dalam sepersepuluh detik untuk lagu
// yang pernah diminta, dan sekitar tujuh belas detik untuk lagu baru yang harus
// diunduh dan ditranskode lebih dahulu. Batas enam detik sebelumnya membuat
// setiap lagu baru selalu gagal, dan pengguna mendengar "lagunya tidak
// ditemukan" padahal server sedang menyiapkannya dengan baik.
constexpr int kSearchTimeoutMs = 40000;
// Batas waktu aliran audio. Perlu lebih longgar daripada sebelumnya karena
// jeda sesaat pada Wi-Fi rumah tidak boleh langsung memutus lagu.
constexpr int kStreamTimeoutMs = 20000;
constexpr int kMaxRedirects = 4;

constexpr int kIdleWaitMs = 30000;       // tunggu AI selesai bicara sebelum musik mulai
constexpr int kMinAnswerGraceMs = 2000;  // beri waktu server memulai TTS jawaban
constexpr int kAnswerDrainedStableMs = 800;
constexpr int kTaskStopTimeoutMs = 4000;


// Jawaban server musik hanya berisi beberapa field pendek, jadi batasnya kecil.
constexpr size_t kMusicServerResponseBytes = 8 * 1024;

// Seluruh pencarian harus selesai jauh sebelum server MCP menyerah menunggu,
// kalau tidak AI akan menjawab "ada kendala teknis" walau lagunya ada.
//
// Anggarannya harus melebihi kSearchTimeoutMs, kalau tidak anggaran inilah yang
// memutus permintaan lebih dulu dan batas waktu yang lebih longgar itu tidak
// pernah terpakai.
constexpr int64_t kResolveBudgetUs = 50LL * 1000LL * 1000LL;

constexpr EventBits_t kNetworkTaskDone = (1 << 0);
constexpr EventBits_t kDecoderTaskDone = (1 << 1);

bool g_decoder_registered = false;

std::string UrlEncode(const std::string& input) {
    static const char* hex = "0123456789ABCDEF";
    std::string out;
    out.reserve(input.size() * 3);
    for (unsigned char c : input) {
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
            (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.' || c == '~') {
            out.push_back(static_cast<char>(c));
        } else if (c == ' ') {
            out.push_back('+');
        } else {
            out.push_back('%');
            out.push_back(hex[(c >> 4) & 0x0F]);
            out.push_back(hex[c & 0x0F]);
        }
    }
    return out;
}

std::string TrimTrailingSlash(std::string value) {
    while (!value.empty() && value.back() == '/') {
        value.pop_back();
    }
    return value;
}

std::string Trim(const std::string& value) {
    size_t begin = value.find_first_not_of(" \t\r\n");
    if (begin == std::string::npos) {
        return std::string();
    }
    size_t end = value.find_last_not_of(" \t\r\n");
    return value.substr(begin, end - begin + 1);
}

std::vector<std::string> SplitCommaSeparated(const std::string& value) {
    std::vector<std::string> items;
    size_t start = 0;
    while (start <= value.size()) {
        size_t comma = value.find(',', start);
        std::string item = Trim(value.substr(start, comma - start));
        if (!item.empty()) {
            items.push_back(TrimTrailingSlash(item));
        }
        if (comma == std::string::npos) {
            break;
        }
        start = comma + 1;
    }
    return items;
}



// ---------------------------------------------------------------------------
// Autentikasi server musik
//
// Skema header di bawah ini sengaja dibuat sama persis dengan firmware musik
// xiaozhi buatan komunitas Tiongkok, supaya server stream_pcm mana pun yang
// mengikuti protokol itu langsung dapat dipakai tanpa mengubah kode.
// ---------------------------------------------------------------------------

std::string GetDeviceMac() {
    return SystemInfo::GetMacAddress();
}

std::string GetDeviceChipId() {
    // Chip ID memakai alamat MAC tanpa tanda titik dua.
    std::string mac = SystemInfo::GetMacAddress();
    mac.erase(std::remove(mac.begin(), mac.end(), ':'), mac.end());
    return mac;
}

// Kunci dinamis: SHA256 dari "MAC:ChipID:Timestamp:KunciRahasia",
// lalu 16 byte pertama ditulis sebagai heksadesimal huruf besar.
std::string GenerateDynamicKey(int64_t timestamp) {
    const std::string secret_key = CONFIG_RIRI_SERVER_SECRET;
    const std::string data = GetDeviceMac() + ":" + GetDeviceChipId() + ":" +
        std::to_string(timestamp) + ":" + secret_key;

    // ESP-IDF 6 memakai mbedtls 4 yang tidak lagi menyediakan mbedtls/sha256.h.
    // Antarmuka resminya sekarang PSA Crypto.
    unsigned char hash[32] = {};
    size_t panjang_hash = 0;
    psa_crypto_init();
    if (psa_hash_compute(PSA_ALG_SHA_256,
                         reinterpret_cast<const uint8_t*>(data.c_str()), data.length(),
                         hash, sizeof(hash), &panjang_hash) != PSA_SUCCESS) {
        ESP_LOGE(TAG, "Gagal menghitung SHA-256 untuk kunci dinamis");
        return std::string();
    }

    std::string key;
    key.reserve(32);
    for (int i = 0; i < 16; ++i) {
        char hex[3];
        snprintf(hex, sizeof(hex), "%02X", hash[i]);
        key += hex;
    }
    return key;
}

void AddAuthHeaders(Http* http) {
    if (http == nullptr) {
        return;
    }
    const int64_t timestamp = esp_timer_get_time() / 1000000;  // detik
    http->SetHeader("X-MAC-Address", GetDeviceMac());
    http->SetHeader("X-Chip-ID", GetDeviceChipId());
    http->SetHeader("X-Timestamp", std::to_string(timestamp));
    http->SetHeader("X-Dynamic-Key", GenerateDynamicKey(timestamp));
}

bool IsRedirect(int status_code) {
    return status_code == 301 || status_code == 302 || status_code == 303 ||
        status_code == 307 || status_code == 308;
}

std::string ResolveRedirectUrl(const std::string& base, const std::string& location) {
    if (location.rfind("http://", 0) == 0 || location.rfind("https://", 0) == 0) {
        return location;
    }
    // Lokasi relatif: gabungkan dengan skema dan host permintaan sebelumnya.
    size_t scheme_end = base.find("://");
    if (scheme_end == std::string::npos) {
        return std::string();
    }
    size_t host_end = base.find('/', scheme_end + 3);
    std::string origin = host_end == std::string::npos ? base : base.substr(0, host_end);
    if (!location.empty() && location.front() == '/') {
        return origin + location;
    }
    return origin + "/" + location;
}

const char* GetJsonStringValue(const cJSON* root, const char* key) {
    cJSON* item = cJSON_GetObjectItem(root, key);
    return (cJSON_IsString(item) && item->valuestring != nullptr) ? item->valuestring : nullptr;
}

// GET sederhana dengan dukungan pengalihan, dipakai untuk respons JSON kecil.
std::string HttpGetText(const std::string& url, int timeout_ms, size_t max_bytes,
                        bool with_auth = false) {
    auto network = Board::GetInstance().GetNetwork();
    if (network == nullptr) {
        return std::string();
    }

    std::string current_url = url;
    for (int hop = 0; hop <= kMaxRedirects; ++hop) {
        auto http = network->CreateHttp(kMusicSearchConnectId);
        if (!http) {
            return std::string();
        }
        http->SetTimeout(timeout_ms);
        http->SetKeepAlive(false);
        http->SetHeader("User-Agent", "ESP32-Riri-Boneka-Music/1.0");
        http->SetHeader("Accept", "application/json");
        if (with_auth) {
            AddAuthHeaders(http.get());
        }

        if (!http->Open("GET", current_url)) {
            ESP_LOGW(TAG, "Tidak dapat membuka %s", current_url.c_str());
            return std::string();
        }

        int status_code = http->GetStatusCode();
        if (IsRedirect(status_code)) {
            std::string location = http->GetResponseHeader("location");
            http->Close();
            if (location.empty()) {
                return std::string();
            }
            current_url = ResolveRedirectUrl(current_url, location);
            if (current_url.empty()) {
                return std::string();
            }
            continue;
        }

        if (status_code != 200) {
            ESP_LOGW(TAG, "Status HTTP %d dari %s", status_code, current_url.c_str());
            http->Close();
            return std::string();
        }

        std::string body;
        body.reserve(2048);
        char chunk[512];
        size_t total = 0;
        while (total < max_bytes) {
            int to_read = static_cast<int>(std::min(sizeof(chunk), max_bytes - total));
            int read = http->Read(chunk, to_read);
            if (read <= 0) {
                break;
            }
            body.append(chunk, read);
            total += static_cast<size_t>(read);
        }
        http->Close();
        return body;
    }

    ESP_LOGW(TAG, "Terlalu banyak pengalihan untuk %s", url.c_str());
    return std::string();
}

}  // namespace

// ---------------------------------------------------------------------------
// MusicRingBuffer
// ---------------------------------------------------------------------------

bool MusicRingBuffer::Allocate(size_t capacity) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (data_.size() != capacity) {
        try {
            data_.assign(capacity, 0);
        } catch (const std::bad_alloc&) {
            std::vector<uint8_t>().swap(data_);
            closed_ = true;
            return false;
        }
    }
    head_ = 0;
    tail_ = 0;
    size_ = 0;
    closed_ = false;
    return true;
}

void MusicRingBuffer::Release() {
    std::lock_guard<std::mutex> lock(mutex_);
    closed_ = true;
    head_ = 0;
    tail_ = 0;
    size_ = 0;
    std::vector<uint8_t>().swap(data_);
    cv_.notify_all();
}

bool MusicRingBuffer::Write(const uint8_t* data, size_t size) {
    std::unique_lock<std::mutex> lock(mutex_);
    size_t written = 0;
    while (written < size) {
        cv_.wait(lock, [this]() { return closed_ || size_ < data_.size(); });
        if (closed_) {
            return false;
        }
        size_t free_space = data_.size() - size_;
        size_t take = std::min(free_space, size - written);
        for (size_t i = 0; i < take; ++i) {
            data_[head_] = data[written + i];
            head_ = (head_ + 1) % data_.size();
        }
        size_ += take;
        written += take;
        cv_.notify_all();
    }
    return true;
}

size_t MusicRingBuffer::Read(uint8_t* dest, size_t max_size) {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this]() { return closed_ || size_ > 0; });
    if (size_ == 0) {
        return 0;  // buffer kosong dan sudah ditutup
    }
    size_t take = std::min(size_, max_size);
    for (size_t i = 0; i < take; ++i) {
        dest[i] = data_[tail_];
        tail_ = (tail_ + 1) % data_.size();
    }
    size_ -= take;
    cv_.notify_all();
    return take;
}

size_t MusicRingBuffer::Size() {
    std::lock_guard<std::mutex> lock(mutex_);
    return size_;
}

void MusicRingBuffer::Close() {
    std::lock_guard<std::mutex> lock(mutex_);
    closed_ = true;
    cv_.notify_all();
}

void MusicRingBuffer::Reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    head_ = 0;
    tail_ = 0;
    size_ = 0;
    closed_ = false;
    cv_.notify_all();
}

bool MusicRingBuffer::closed() {
    std::lock_guard<std::mutex> lock(mutex_);
    return closed_;
}

// ---------------------------------------------------------------------------
// MusicPlayer
// ---------------------------------------------------------------------------

MusicPlayer::MusicPlayer() {
    event_group_ = xEventGroupCreate();
    if (event_group_ != nullptr) {
        xEventGroupSetBits(event_group_, kNetworkTaskDone | kDecoderTaskDone);
    }
}

std::string MusicPlayer::GetNowPlaying() {
    std::lock_guard<std::mutex> lock(track_mutex_);
    if (!playing_.load() || track_.title.empty()) {
        return std::string();
    }
    if (track_.artist.empty()) {
        return track_.title;
    }
    return track_.title + " - " + track_.artist;
}

void MusicPlayer::Stop() {
    if (!playing_.load()) {
        return;
    }
    ESP_LOGI(TAG, "Menghentikan musik");
    stop_requested_ = true;
    ring_.Close();
}

void MusicPlayer::StopForInteraction() {
    if (!streaming_.load()) {
        return;
    }
    Stop();
}

void MusicPlayer::StopAndWait(int timeout_ms) {
    if (!playing_.load()) {
        return;
    }
    Stop();
    WaitTasksStopped(timeout_ms);
}

void MusicPlayer::WaitTasksStopped(int timeout_ms) {
    if (event_group_ == nullptr) {
        return;
    }
    xEventGroupWaitBits(event_group_, kNetworkTaskDone | kDecoderTaskDone,
        pdFALSE, pdTRUE, pdMS_TO_TICKS(timeout_ms));
}

std::string MusicPlayer::Play(const std::string& song, const std::string& artist) {
    if (!CONFIG_MUSIC_PLAYER_ENABLE) {
        return std::string("Fitur pemutar musik sedang dimatikan pada perangkat ini.");
    }

    std::string cleaned = Trim(song);
    std::string cleaned_artist = Trim(artist);
    if (cleaned.empty()) {
        // Bila hanya nama penyanyi yang disebut, itu tetap dipakai sebagai kata
        // kunci pencarian supaya lagu populer penyanyi tersebut dapat diputar.
        cleaned = cleaned_artist;
        cleaned_artist.clear();
    }
    if (cleaned.empty()) {
        return std::string("Sebutkan judul lagu atau nama penyanyinya supaya saya bisa mencarinya.");
    }

    std::lock_guard<std::mutex> control_lock(control_mutex_);

    // Hentikan pemutaran sebelumnya lalu tunggu kedua tugas benar-benar keluar
    // agar tidak ada dua dekoder yang mendorong PCM ke antrean yang sama.
    Stop();
    WaitTasksStopped(kTaskStopTimeoutMs);

    auto& audio_service = Application::GetInstance().GetAudioService();
    if (!audio_service.IsInitialized()) {
        return std::string("Perangkat audio belum siap, coba lagi sebentar.");
    }

    MusicTrack track;
    if (!ResolveTrack(cleaned, cleaned_artist, track)) {
        return std::string("Lagu \"") + cleaned +
            "\" belum bisa diputar. Server musik tidak menjawab atau lagunya tidak ditemukan. "
            "Coba sebutkan judul dan nama penyanyi dengan lebih lengkap.";
    }

    {
        std::lock_guard<std::mutex> lock(track_mutex_);
        track_ = track;
    }

    stop_requested_ = false;
    if (!ring_.Allocate(kRingCapacity)) {
        ESP_LOGE(TAG, "Gagal mengalokasikan buffer musik");
        return std::string("Memori perangkat tidak cukup untuk memutar musik saat ini.");
    }

    // Ditandai sebelum tugas dibuat supaya Stop() dari sentuhan foil pada celah
    // waktu singkat ini tetap terhitung.
    playing_ = true;
    streaming_ = false;
    if (!StartTasks()) {
        playing_ = false;
        ring_.Release();
        return std::string("Tidak dapat memulai pemutar musik saat ini.");
    }

    std::string answer = "Memutar " + track.title;
    if (!track.artist.empty()) {
        answer += " dari " + track.artist;
    }
    answer += ".";
    answer += " Sebutkan berhenti atau sentuh tangan Riri untuk menghentikan lagu.";
    return answer;
}

bool MusicPlayer::StartTasks() {
    if (event_group_ == nullptr) {
        return false;
    }
    xEventGroupClearBits(event_group_, kNetworkTaskDone | kDecoderTaskDone);

    // Prioritas 1 sengaja berada di bawah opus_codec (2), audio_output (4), dan
    // audio_input (8). Musik tidak boleh merebut CPU dari jalur audio waktu
    // nyata. Saat lagu berbunyi perangkat berstatus menganggur sehingga tidak
    // ada enkode mikrofon, jadi CPU tetap lapang untuk dekode MP3.
    if (xTaskCreate(NetworkTask, "music_net", 6144, this, 1, nullptr) != pdPASS) {
        xEventGroupSetBits(event_group_, kNetworkTaskDone | kDecoderTaskDone);
        return false;
    }
    if (xTaskCreate(DecoderTask, "music_dec", 8192, this, 1, nullptr) != pdPASS) {
        // Tugas jaringan sudah berjalan, jadi hentikan lewat jalur normal.
        stop_requested_ = true;
        ring_.Close();
        xEventGroupSetBits(event_group_, kDecoderTaskDone);
        return false;
    }
    return true;
}

void MusicPlayer::NetworkTask(void* arg) {
    auto* player = static_cast<MusicPlayer*>(arg);
    player->RunNetwork();
    xEventGroupSetBits(player->event_group_, kNetworkTaskDone);
    vTaskDelete(nullptr);
}

void MusicPlayer::DecoderTask(void* arg) {
    auto* player = static_cast<MusicPlayer*>(arg);
    player->RunDecoder();
    player->streaming_ = false;
    player->playing_ = false;
    player->ring_.Release();
    if (Application::GetInstance().GetDeviceState() == kDeviceStateIdle) {
        Application::GetInstance().GetAudioService().EnableWakeWordDetection(true);
    }
    // Kembalikan warna lampu ke status perangkat yang berlaku sekarang.
    if (auto led = Board::GetInstance().GetLed()) {
        led->OnStateChanged();
    }
    xEventGroupSetBits(player->event_group_, kDecoderTaskDone);
    vTaskDelete(nullptr);
}

// ---------------------------------------------------------------------------
// Pencarian lagu
// ---------------------------------------------------------------------------

bool MusicPlayer::ResolveTrack(const std::string& song, const std::string& artist, MusicTrack& track) {
    // Seluruh pencarian lagu ditangani server Riri. Server itulah yang memakai
    // YouTube Data API sehingga rekaman penyanyi aslinya ditemukan, baik lagu
    // internasional maupun lagu populer Indonesia.
    return ResolveFromMusicServer(song, artist, track);
}

static void SplitSongAndArtist(const std::string& query, std::string& song, std::string& artist) {
    song = query;
    artist.clear();

    static const char* separators[] = {" - ", " -- ", " oleh ", " by ", nullptr};
    for (int i = 0; separators[i] != nullptr; ++i) {
        const size_t pos = query.find(separators[i]);
        if (pos != std::string::npos) {
            const std::string left = Trim(query.substr(0, pos));
            const std::string right = Trim(query.substr(pos + std::strlen(separators[i])));
            if (!left.empty() && !right.empty()) {
                // Untuk "oleh" dan "by" bagian kanan pasti penyanyi.
                // Untuk tanda hubung, konvensi umum adalah "penyanyi - judul".
                if (std::strcmp(separators[i], " oleh ") == 0 || std::strcmp(separators[i], " by ") == 0) {
                    song = left;
                    artist = right;
                } else {
                    song = right;
                    artist = left;
                }
                return;
            }
        }
    }
}

bool MusicPlayer::ResolveFromMusicServer(const std::string& requested_song,
                                        const std::string& requested_artist,
                                        MusicTrack& track) {
    const auto servers = SplitCommaSeparated(CONFIG_RIRI_SERVER_URL);
    if (servers.empty()) {
        ESP_LOGW(TAG, "URL server musik belum diisi di menuconfig");
        return false;
    }

    std::string song = requested_song;
    std::string artist = requested_artist;
    if (artist.empty()) {
        SplitSongAndArtist(requested_song, song, artist);
    }

    const int64_t deadline_us = esp_timer_get_time() + kResolveBudgetUs;
    for (const auto& base_url : servers) {
        if (esp_timer_get_time() >= deadline_us) {
            ESP_LOGW(TAG, "Waktu pencarian habis sebelum semua server musik dicoba");
            break;
        }

        const std::string url = base_url + "/music/resolve?song=" + UrlEncode(song) +
            "&artist=" + UrlEncode(artist);
        ESP_LOGI(TAG, "Meminta lagu ke server musik: %s", url.c_str());

        std::string body = HttpGetText(url, kSearchTimeoutMs, kMusicServerResponseBytes, true);
        if (body.empty()) {
            ESP_LOGW(TAG, "Server musik %s tidak menjawab", base_url.c_str());
            continue;
        }

        cJSON* root = cJSON_Parse(body.c_str());
        std::string().swap(body);
        if (root == nullptr) {
            ESP_LOGW(TAG, "Jawaban server musik bukan JSON valid");
            continue;
        }

        // Server yang menolak autentikasi membalas JSON dengan field error.
        const char* server_error = GetJsonStringValue(root, "error");
        if (server_error != nullptr && server_error[0] != '\0') {
            ESP_LOGE(TAG, "Server musik menolak permintaan: %s", server_error);
            ESP_LOGE(TAG, "Periksa CONFIG_RIRI_SERVER_SECRET agar sama dengan server");
            cJSON_Delete(root);
            continue;
        }

        const char* audio_url = GetJsonStringValue(root, "audio_url");
        if (audio_url == nullptr || audio_url[0] == '\0') {
            cJSON_Delete(root);
            ESP_LOGW(TAG, "Server musik tidak menemukan lagu: %s", song.c_str());
            continue;
        }

        const char* title = GetJsonStringValue(root, "title");
        const char* found_artist = GetJsonStringValue(root, "artist");
        cJSON* duration = cJSON_GetObjectItem(root, "duration");

        // URL relatif diizinkan supaya server sederhana cukup mengembalikan
        // jalur berkas tanpa harus tahu alamat IP dirinya sendiri.
        track.stream_url = ResolveRedirectUrl(base_url + "/", audio_url);
        track.title = (title != nullptr && title[0] != '\0') ? title : song;
        track.artist = (found_artist != nullptr) ? found_artist : artist;
        track.duration_seconds = cJSON_IsNumber(duration) ? duration->valueint : 0;
        track.source = "server musik";
        cJSON_Delete(root);

        ESP_LOGI(TAG, "Lagu dari server musik: %s - %s", track.title.c_str(), track.artist.c_str());
        return true;
    }

    return false;
}

// ---------------------------------------------------------------------------
// Tugas jaringan
// ---------------------------------------------------------------------------

void MusicPlayer::RunNetwork() {
    std::string stream_url;
    {
        std::lock_guard<std::mutex> lock(track_mutex_);
        stream_url = track_.stream_url;
    }

    auto network = Board::GetInstance().GetNetwork();
    if (network == nullptr) {
        ring_.Close();
        return;
    }

    std::unique_ptr<Http> http;
    std::string current_url = stream_url;
    bool opened = false;
    for (int hop = 0; hop <= kMaxRedirects && !stop_requested_.load(); ++hop) {
        http = network->CreateHttp(kMusicStreamConnectId);
        if (!http) {
            break;
        }
        http->SetTimeout(kStreamTimeoutMs);
        http->SetKeepAlive(false);
        http->SetHeader("User-Agent", "ESP32-Riri-Boneka-Music/1.0");
        http->SetHeader("Accept", "*/*");

        if (!http->Open("GET", current_url)) {
            ESP_LOGW(TAG, "Gagal membuka aliran musik");
            break;
        }

        int status_code = http->GetStatusCode();
        if (IsRedirect(status_code)) {
            std::string location = http->GetResponseHeader("location");
            http->Close();
            if (location.empty()) {
                break;
            }
            current_url = ResolveRedirectUrl(current_url, location);
            if (current_url.empty()) {
                break;
            }
            continue;
        }

        if (status_code != 200 && status_code != 206) {
            ESP_LOGW(TAG, "Status aliran musik %d", status_code);
            http->Close();
            break;
        }

        opened = true;
        break;
    }

    if (!opened) {
        ring_.Close();
        return;
    }

    std::vector<uint8_t> chunk(kNetworkChunkBytes);
    int64_t total = 0;
    while (!stop_requested_.load()) {
        int read = http->Read(reinterpret_cast<char*>(chunk.data()), static_cast<int>(chunk.size()));
        if (read <= 0) {
            break;
        }
        total += read;
        if (!ring_.Write(chunk.data(), static_cast<size_t>(read))) {
            break;  // buffer ditutup karena pemutaran dihentikan
        }
    }

    http->Close();
    ESP_LOGI(TAG, "Unduhan musik selesai, %lld byte", static_cast<long long>(total));
    // Tutup buffer agar dekoder tahu tidak ada data lagi setelah sisa terkuras.
    ring_.Close();
}

// ---------------------------------------------------------------------------
// Tugas dekoder
// ---------------------------------------------------------------------------

void MusicPlayer::RunDecoder() {
    auto& app = Application::GetInstance();
    auto& audio_service = app.GetAudioService();
    auto* codec = Board::GetInstance().GetAudioCodec();
    if (codec == nullptr) {
        ring_.Close();
        return;
    }

    if (!g_decoder_registered) {
        // Decoder MP3 dan pengurai bingkainya harus terdaftar sebelum
        // esp_audio_simple_dec_open() dipanggil.
        esp_audio_err_t decoder_result = esp_audio_dec_register_default();
        if (decoder_result != ESP_AUDIO_ERR_OK) {
            ESP_LOGE(TAG, "Gagal mendaftarkan decoder audio, kode %d", static_cast<int>(decoder_result));
            ring_.Close();
            return;
        }
        esp_audio_err_t simple_decoder_result = esp_audio_simple_dec_register_default();
        if (simple_decoder_result != ESP_AUDIO_ERR_OK) {
            ESP_LOGE(TAG, "Gagal mendaftarkan parser audio, kode %d", static_cast<int>(simple_decoder_result));
            ring_.Close();
            return;
        }
        g_decoder_registered = true;
        ESP_LOGI(TAG, "Decoder MP3 siap digunakan");
    }

    // Musik hanya boleh terdengar saat perangkat menganggur, kalau tidak lagu
    // akan masuk ke mikrofon dan dikirim ke server sebagai suara pengguna.
    //
    // Saat tool dipanggil, AI masih membacakan jawabannya. Setelah jawaban
    // selesai perangkat berpindah ke status mendengarkan, bukan menganggur,
    // jadi sesi percakapan harus ditutup dulu secara eksplisit.
    int waited_ms = 0;
    int drained_ms = 0;
    bool end_requested = false;
    while (!stop_requested_.load() && waited_ms < kIdleWaitMs) {
        auto state = app.GetDeviceState();
        if (state == kDeviceStateIdle) {
            break;
        }
        if (state == kDeviceStateConnecting) {
            // Status ini hanya muncul saat pengguna membangunkan AI lagi.
            ESP_LOGI(TAG, "Pengguna memanggil AI, permintaan musik dibatalkan");
            stop_requested_ = true;
            break;
        }

        // Hanya status mendengarkan yang dipakai sebagai tanda jawaban selesai.
        // Status berbicara tidak dipakai karena antrean audio bisa sesaat kosong
        // di sela paket TTS, dan menutup sesi di titik itu memotong kalimat AI.
        //
        // Kesiapan diperiksa lewat WaitForPlaybackQueueEmpty, bukan IsIdle().
        // IsIdle() ikut memeriksa antrean encode mikrofon yang praktis tidak
        // pernah kosong selama perangkat mendengarkan, sehingga syaratnya tidak
        // akan pernah terpenuhi dan musik selalu gagal setelah batas tunggu.
        bool playback_drained = audio_service.WaitForPlaybackQueueEmpty(0);
        if (state == kDeviceStateListening && playback_drained && waited_ms >= kMinAnswerGraceMs) {
            drained_ms += 100;
        } else {
            drained_ms = 0;
        }

        if (drained_ms >= kAnswerDrainedStableMs && !end_requested) {
            end_requested = true;
            app.EndConversation();
        }

        vTaskDelay(pdMS_TO_TICKS(100));
        waited_ms += 100;
    }
    if (stop_requested_.load()) {
        ESP_LOGW(TAG, "Pemutaran dibatalkan sebelum mulai");
        ring_.Close();
        return;
    }
    if (app.GetDeviceState() != kDeviceStateIdle) {
        ESP_LOGW(TAG, "Perangkat tidak kembali menganggur dalam %d ms (state=%d), musik dibatalkan",
            kIdleWaitMs, static_cast<int>(app.GetDeviceState()));
        ring_.Close();
        return;
    }
    ESP_LOGI(TAG, "Perangkat menganggur, mulai memutar musik");

    // Isi buffer awal supaya lagu tidak putus-putus pada detik pertama.
    while (!stop_requested_.load() && !ring_.closed() && ring_.Size() < kPrebufferBytes) {
        vTaskDelay(pdMS_TO_TICKS(50));
    }
    ESP_LOGI(TAG, "Buffer awal terisi %u byte", static_cast<unsigned>(ring_.Size()));
    if (stop_requested_.load()) {
        ring_.Close();
        return;
    }

    // Server Riri mengirim audio/mpeg, jadi pengurai bingkai MP3 yang dipakai.
    esp_audio_simple_dec_cfg_t dec_cfg = {};
    dec_cfg.dec_type = ESP_AUDIO_SIMPLE_DEC_TYPE_MP3;
    dec_cfg.dec_cfg = nullptr;
    dec_cfg.cfg_size = 0;
    dec_cfg.use_frame_dec = false;

    esp_audio_simple_dec_handle_t decoder = nullptr;
    if (esp_audio_simple_dec_open(&dec_cfg, &decoder) != ESP_AUDIO_ERR_OK || decoder == nullptr) {
        ESP_LOGE(TAG, "Gagal membuka dekoder MP3");
        ring_.Close();
        return;
    }

    const int output_rate = codec->output_sample_rate();
    const size_t frame_samples = static_cast<size_t>(output_rate) * 60 / 1000;

    esp_ae_rate_cvt_handle_t resampler = nullptr;
    int resampler_src_rate = 0;

    std::vector<uint8_t> chunk(kDecodeChunkBytes);
    // Sisa byte yang belum membentuk satu bingkai MP3 utuh harus dibawa ke
    // putaran berikutnya, kalau dibuang audio akan terdengar patah-patah.
    std::vector<uint8_t> input;
    input.reserve(kDecodeChunkBytes * 4);
    std::vector<uint8_t> pcm_out(8192);
    std::vector<int16_t> mono;
    std::vector<int16_t> pending;
    pending.reserve(frame_samples * 2);

    // Jangan biarkan suara dari speaker dikenali keliru sebagai "hai hai".
    // Pengguna tetap dapat membangunkan Riri melalui foil tangan yang memang
    // dirancang sebagai kontrol saat musik sedang diputar.
    audio_service.EnableWakeWordDetection(false);
    streaming_ = true;
    // Beri tahu lampu indikator supaya warnanya berubah menjadi ungu.
    if (auto led = Board::GetInstance().GetLed()) {
        led->OnStateChanged();
    }

    // Berkas MP3 sering diawali tag ID3v2 berisi metadata judul
    // dan sampul album. Byte itu bukan bingkai MP3. Tag dilepas sendiri di sini
    // agar pemutaran tidak bergantung pada perilaku pengurai bawaan.
    bool id3_checked = false;
    size_t id3_skip_remaining = 0;

    int rebuffer_count = 0;

    bool eos = false;
    while (!stop_requested_.load()) {
        // Penjaga isi ulang penyangga. Tanpa ini, saat jaringan tersendat
        // dekoder terbangun hanya untuk beberapa byte, mendorong sedikit PCM,
        // lalu menunggu lagi, sehingga suara terdengar patah-patah. Di sini
        // dekoder memilih diam sejenak dan membiarkan antrean playback yang
        // masih berisi menutupi jeda, lalu melanjutkan setelah data cukup.
        if (!ring_.closed() && ring_.Size() < kRebufferLowWaterBytes) {
            ESP_LOGW(TAG, "Data musik menipis, mengisi ulang penyangga");
            ++rebuffer_count;
            int waited = 0;
            while (!stop_requested_.load() && !ring_.closed() &&
                   ring_.Size() < kRebufferTargetBytes && waited < kRebufferTimeoutMs) {
                vTaskDelay(pdMS_TO_TICKS(20));
                waited += 20;
            }
            if (stop_requested_.load()) {
                break;
            }
        }

        size_t filled = ring_.Read(chunk.data(), chunk.size());
        if (filled == 0) {
            eos = true;
        } else {
            input.insert(input.end(), chunk.begin(), chunk.begin() + filled);
        }

        if (!id3_checked && input.size() >= kId3HeaderBytes) {
            id3_checked = true;
            if (input[0] == 'I' && input[1] == 'D' && input[2] == '3') {
                // Ukuran tag ditulis sebagai empat byte syncsafe, tiap byte
                // hanya memakai 7 bit rendah.
                const size_t tag_size =
                    (static_cast<size_t>(input[6] & 0x7F) << 21) |
                    (static_cast<size_t>(input[7] & 0x7F) << 14) |
                    (static_cast<size_t>(input[8] & 0x7F) << 7) |
                    (static_cast<size_t>(input[9] & 0x7F));
                const bool has_footer = (input[5] & 0x10) != 0;
                id3_skip_remaining = kId3HeaderBytes + tag_size + (has_footer ? kId3HeaderBytes : 0);
                ESP_LOGI(TAG, "Melewati tag ID3v2 sebesar %u byte",
                    static_cast<unsigned>(id3_skip_remaining));
            }
        }

        if (id3_skip_remaining > 0) {
            // Tag dapat lebih besar daripada satu potongan, jadi sisanya
            // dibuang pada putaran berikutnya.
            const size_t drop = std::min(id3_skip_remaining, input.size());
            input.erase(input.begin(), input.begin() + drop);
            id3_skip_remaining -= drop;
            if (id3_skip_remaining > 0 && !eos) {
                continue;
            }
        }

        size_t offset = 0;
        while (offset < input.size() && !stop_requested_.load()) {
            esp_audio_simple_dec_raw_t raw = {};
            raw.buffer = input.data() + offset;
            raw.len = static_cast<uint32_t>(input.size() - offset);
            raw.eos = eos;
            raw.consumed = 0;
            raw.frame_recover = ESP_AUDIO_SIMPLE_DEC_RECOVERY_NONE;

            esp_audio_simple_dec_out_t frame = {};
            frame.buffer = pcm_out.data();
            frame.len = static_cast<uint32_t>(pcm_out.size());
            frame.needed_size = 0;
            frame.decoded_size = 0;

            esp_audio_err_t ret = esp_audio_simple_dec_process(decoder, &raw, &frame);
            if (ret == ESP_AUDIO_ERR_BUFF_NOT_ENOUGH) {
                size_t needed = frame.needed_size > 0 ? frame.needed_size : pcm_out.size() * 2;
                if (needed > kMaxPcmBufferBytes) {
                    ESP_LOGW(TAG, "Bingkai musik minta %u byte, di luar batas wajar",
                        static_cast<unsigned>(needed));
                    stop_requested_ = true;
                    break;
                }
                pcm_out.assign(needed, 0);
                continue;  // ulangi bingkai yang sama dengan penyangga lebih besar
            }
            if (ret != ESP_AUDIO_ERR_OK) {
                ESP_LOGW(TAG, "Dekode musik gagal, kode %d", static_cast<int>(ret));
                stop_requested_ = true;
                break;
            }

            if (raw.consumed == 0 && frame.decoded_size == 0) {
                break;  // butuh lebih banyak data mentah, sisanya dibawa ke putaran berikutnya
            }
            offset += raw.consumed;

            if (frame.decoded_size == 0) {
                continue;
            }

            esp_audio_simple_dec_info_t info = {};
            if (esp_audio_simple_dec_get_info(decoder, &info) != ESP_AUDIO_ERR_OK ||
                info.sample_rate == 0 || info.channel == 0) {
                continue;
            }

            // Campur ke mono karena keluaran boneka hanya satu speaker.
            const int16_t* samples = reinterpret_cast<const int16_t*>(pcm_out.data());
            size_t sample_count = frame.decoded_size / sizeof(int16_t);
            if (info.channel >= 2) {
                size_t frames = sample_count / info.channel;
                mono.resize(frames);
                for (size_t i = 0; i < frames; ++i) {
                    int32_t sum = 0;
                    for (int ch = 0; ch < info.channel; ++ch) {
                        sum += samples[i * info.channel + ch];
                    }
                    mono[i] = static_cast<int16_t>(sum / info.channel);
                }
            } else {
                mono.assign(samples, samples + sample_count);
            }

            if (mono.empty()) {
                continue;
            }

            if (static_cast<int>(info.sample_rate) != output_rate) {
                if (resampler == nullptr || resampler_src_rate != static_cast<int>(info.sample_rate)) {
                    if (resampler != nullptr) {
                        esp_ae_rate_cvt_close(resampler);
                        resampler = nullptr;
                    }
                    esp_ae_rate_cvt_cfg_t cvt_cfg = {};
                    cvt_cfg.src_rate = info.sample_rate;
                    cvt_cfg.dest_rate = static_cast<uint32_t>(output_rate);
                    cvt_cfg.channel = 1;
                    cvt_cfg.bits_per_sample = ESP_AUDIO_BIT16;
                    cvt_cfg.complexity = 2;
                    cvt_cfg.perf_type = ESP_AE_RATE_CVT_PERF_TYPE_SPEED;
                    if (esp_ae_rate_cvt_open(&cvt_cfg, &resampler) != ESP_AE_ERR_OK || resampler == nullptr) {
                        ESP_LOGE(TAG, "Gagal membuka resampler %lu -> %d",
                            static_cast<unsigned long>(info.sample_rate), output_rate);
                        stop_requested_ = true;
                        break;
                    }
                    resampler_src_rate = static_cast<int>(info.sample_rate);
                }

                uint32_t max_out = 0;
                esp_ae_rate_cvt_get_max_out_sample_num(resampler, static_cast<uint32_t>(mono.size()), &max_out);
                std::vector<int16_t> resampled(max_out);
                uint32_t actual_out = max_out;
                if (esp_ae_rate_cvt_process(resampler, static_cast<esp_ae_sample_t>(mono.data()),
                        static_cast<uint32_t>(mono.size()),
                        static_cast<esp_ae_sample_t>(resampled.data()), &actual_out) != ESP_AE_ERR_OK) {
                    continue;
                }
                resampled.resize(actual_out);
                pending.insert(pending.end(), resampled.begin(), resampled.end());
            } else {
                pending.insert(pending.end(), mono.begin(), mono.end());
            }

            while (pending.size() >= frame_samples && !stop_requested_.load()) {
                std::vector<int16_t> block(pending.begin(), pending.begin() + frame_samples);
                pending.erase(pending.begin(), pending.begin() + frame_samples);
                if (!audio_service.PushPcmToPlaybackQueue(std::move(block))) {
                    ESP_LOGW(TAG, "Antrean playback penuh terlalu lama, musik dihentikan");
                    stop_requested_ = true;
                    break;
                }
            }

            // Perangkat keluar dari mode menganggur berarti pengguna sedang
            // berinteraksi dengan AI, jadi musik harus mengalah.
            if (app.GetDeviceState() != kDeviceStateIdle) {
                ESP_LOGI(TAG, "AI aktif, musik dihentikan");
                stop_requested_ = true;
                break;
            }
        }

        if (offset > 0) {
            input.erase(input.begin(), input.begin() + offset);
        } else if (input.size() > kDecodeChunkBytes * 8) {
            // Aliran rusak dan parser tidak menemukan bingkai apa pun.
            ESP_LOGW(TAG, "Aliran musik tidak dapat diurai, pemutaran dihentikan");
            break;
        }

        if (eos) {
            break;
        }
    }

    if (!stop_requested_.load() && !pending.empty()) {
        std::vector<int16_t> block(pending.begin(), pending.end());
        audio_service.PushPcmToPlaybackQueue(std::move(block));
    }

    if (stop_requested_.load()) {
        // Hentikan segera: buang sisa audio yang belum sempat keluar ke speaker.
        audio_service.ClearPlaybackQueue();
    } else {
        audio_service.WaitForPlaybackQueueEmpty(15000);
    }

    if (resampler != nullptr) {
        esp_ae_rate_cvt_close(resampler);
    }
    esp_audio_simple_dec_close(decoder);
    ring_.Close();
    // Jumlah isi ulang menjadi petunjuk langsung apakah suara patah-patah
    // berasal dari jaringan. Nilai nol berarti aliran lancar sepanjang lagu.
    ESP_LOGI(TAG, "Pemutaran musik selesai, isi ulang penyangga %d kali", rebuffer_count);
}
