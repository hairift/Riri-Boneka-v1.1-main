#include "riri_touch_controller.h"

#include "application.h"
#include "assets.h"
#include "audio_codec.h"
#include "board.h"
#include "config.h"
#include "music_player.h"

#include <algorithm>
#include <cstring>
#include <esp_log.h>
#include <nvs.h>

#define TAG "RiriTouch"

namespace {

// Interval sampel 50 ms adalah nilai yang dulu terbukti stabil di perangkat.
constexpr int kSampleIntervalMs = 50;
constexpr int kStartupSettleMs = 1200;
// Selama Riri masih menyambung Wi-Fi atau aktivasi AI, tangan kanan adalah
// tombol konfigurasi Wi-Fi. Masa ini cukup panjang untuk mencakup koneksi yang
// lambat, tetapi berhenti otomatis begitu Riri sudah siap dipakai.
constexpr int kStartupWifiGestureTimeoutMs = 30000;
constexpr int kBootGestureStableSamplesRequired = 10;
constexpr int kCalibrationSamples = 32;
constexpr int kCalibrationSampleDelayMs = 20;

// Ambang dinyatakan dalam per seribu, bukan persen, supaya sensitivitas dapat
// diatur halus. Nilai lama 220 per seribu menuntut kenaikan 22 persen sehingga
// sentuhan ringan atau sentuhan lewat kain boneka sering tidak terbaca.
//
// Ambang serendah ini aman terhadap salah picu karena tiga penjaga lain:
//   1. Hanya foil dengan kenaikan terkuat yang boleh menang pada satu putaran,
//      sehingga dua foil tidak mungkin bereaksi bersamaan.
//   2. Kenaikan harus bertahan beberapa sampel berturut-turut.
//      3 sampel x 25 ms = 75 ms, cukup untuk menolak derau sesaat.
//   3. Foil pemenang harus unggul cukup jauh dari foil kedua.
// Ambang deteksi dikembalikan ke setelan yang dulu terbukti lancar di
// perangkat, yaitu kenaikan 22 persen terhadap garis dasar.
//
// Percobaan menurunkan ambang ke 7 lalu 11 persen membuat foil bereaksi sendiri
// tanpa disentuh, karena pergeseran listrik saat radio Wi-Fi dan penguat audio
// menyala sudah cukup besar untuk melewati ambang serendah itu. Kepekaan yang
// aman datang dari kalibrasi yang diambil saat perangkat sudah tenang, bukan
// dari menurunkan ambang.
constexpr int kTouchRisePermille = 220;   // 22,0 persen
constexpr int kReleaseRisePermille = 100; // 10,0 persen, memberi histeresis
constexpr int kTouchStableSamplesRequired = 6;
constexpr int kReleaseStableSamplesRequired = 8;

// Menunggu perangkat masuk status siaga sebelum kalibrasi akhir diambil.
constexpr int kSettleWaitTimeoutMs = 25000;
constexpr int kPostSettleDelayMs = 1500;
// Masa siaga setelah kalibrasi sebelum sentuhan pertama boleh memicu.
constexpr int kArmingDelayMs = 2000;
// Sebaran sampel kalibrasi yang dianggap berisik, dalam per seribu.
constexpr int kNoisyChannelSpreadPermille = 60;

// Selisih minimum antara foil terkuat dan foil terkuat kedua pada satu putaran.
// Penjaga ini yang membuat sentuhan tangan tidak ikut memicu foil kaki walaupun
// ambangnya rendah dan kabelnya berdekatan.
constexpr int kWinnerMarginPermille = 15;
constexpr int kTriggerCooldownMs = 1800;
constexpr int kWakeCooldownMs = 3500;
constexpr int kActivationSoundWaitMs = 2400;
// Jumlah sampel berturut-turut pada zona mati sebelum baseline ditarik ulang.
// Foil yang menetap di zona mati disetel ulang lebih cepat supaya pergeseran
// lingkungan cepat terserap dan foil kembali peka.
constexpr int kStuckSamplesBeforeRebase = 60;           // sekitar 3 detik pada 50 ms
// Batas foil terkunci dalam keadaan tertekan sebelum dilepas paksa.
constexpr int kStuckPressedSamplesBeforeRelease = 600;   // sekitar 30 detik pada 50 ms

constexpr const char* kTouchSettingsNamespace = "riri_touch";
constexpr const char* kRightHandBaselineKey = "right_hand_base";

RiriTouchPoint kTouchPoints[] = {
    {"kepala",       RIRI_TOUCH_HEAD_GPIO,       TOUCH_PAD_NUM1,  "Tidur.ogg",  RiriTouchAction::PLAY_ONLY},
    {"idung",        RIRI_TOUCH_NOSE_GPIO,       TOUCH_PAD_NUM2,  "Idung.ogg",  RiriTouchAction::PLAY_ONLY},
    {"perut",        RIRI_TOUCH_BELLY_GPIO,      TOUCH_PAD_NUM3,  "Perut.ogg",  RiriTouchAction::PLAY_ONLY},
    {"tangan kanan", RIRI_TOUCH_RIGHT_HAND_GPIO, TOUCH_PAD_NUM11, "Aktif.ogg",  RiriTouchAction::WAKE_AI},
    {"tangan kiri",  RIRI_TOUCH_LEFT_HAND_GPIO,  TOUCH_PAD_NUM12, "Aktif.ogg",  RiriTouchAction::WAKE_AI},
    {"kaki kanan",   RIRI_TOUCH_RIGHT_FOOT_GPIO, TOUCH_PAD_NUM13, "Ketawa.ogg", RiriTouchAction::PLAY_ONLY},
    {"kaki kiri",    RIRI_TOUCH_LEFT_FOOT_GPIO,  TOUCH_PAD_NUM14, "Ketawa.ogg", RiriTouchAction::PLAY_ONLY},
    {"ekor",         RIRI_TOUCH_TAIL_GPIO,       TOUCH_PAD_NUM8,  "Kentut.ogg", RiriTouchAction::PLAY_ONLY},
};

constexpr int kTouchVolume = 100;

bool IsTouchPad(const RiriTouchPoint& point) {
    return point.touch_pad >= TOUCH_PAD_NUM0 && point.touch_pad < TOUCH_PAD_MAX;
}

// Pada ESP32-S3 kanal sentuh N selalu terhubung ke GPIO N. Pemetaan yang tidak
// cocok berarti foil tidak akan pernah terbaca meskipun konfigurasi berhasil,
// jadi hal itu harus terlihat jelas di log saat boot.
bool TouchPadMatchesGpio(const RiriTouchPoint& point) {
    return static_cast<int>(point.touch_pad) == static_cast<int>(point.gpio);
}

int TouchPadIndex(const RiriTouchPoint& point) {
    return static_cast<int>(point.touch_pad);
}

}  // namespace

void RiriTouchController::Initialize(WifiConfigRequestCallback wifi_config_request_callback) {
    if (initialized_) {
        return;
    }

    wifi_config_request_callback_ = std::move(wifi_config_request_callback);
    saved_right_hand_baseline_ = LoadSavedRightHandBaseline();

    esp_err_t err = touch_pad_init();
    if (err == ESP_OK || err == ESP_ERR_INVALID_STATE) {
        touch_driver_ready_ = true;
        touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);
    } else {
        ESP_LOGE(TAG, "Gagal init touch pad: %s", esp_err_to_name(err));
    }

    int configured = 0;
    for (auto& point : kTouchPoints) {
        if (!IsTouchPad(point)) {
            ESP_LOGE(TAG, "%s GPIO%d bukan touch pad kapasitif ESP32-S3",
                point.name, point.gpio);
            continue;
        }
        if (!TouchPadMatchesGpio(point)) {
            ESP_LOGE(TAG, "%s salah petakan: kanal sentuh %d tidak berada di GPIO%d",
                point.name, static_cast<int>(point.touch_pad), point.gpio);
            continue;
        }
        if (ConfigureTouchPoint(point)) {
            ++configured;
            enabled_[TouchPadIndex(point)] = true;
            ESP_LOGI(TAG, "Foil %s aktif di GPIO%d (kanal %d) -> %s",
                point.name, point.gpio, static_cast<int>(point.touch_pad), point.sound_asset);
        }
    }
    ESP_LOGI(TAG, "%d dari %d titik sentuh berhasil dikonfigurasi",
        configured, static_cast<int>(sizeof(kTouchPoints) / sizeof(kTouchPoints[0])));

    if (touch_driver_ready_) {
        touch_pad_fsm_start();
        // Perangkat keras sentuh dan foil perlu waktu tenang setelah boot.
        // Kalibrasi awal ini hanya sementara; kalibrasi yang dipakai sungguhan
        // diambil ulang di Run() setelah Wi-Fi dan audio benar-benar stabil.
        vTaskDelay(pdMS_TO_TICKS(kStartupSettleMs));
        CalibrateAllPoints();
    }

    initialized_ = true;
    xTaskCreate(TouchTask, "riri_touch", 6144, this, 4, &task_handle_);
    ESP_LOGI(TAG, "Sensor sentuh Riri siap");
}

uint32_t RiriTouchController::LoadSavedRightHandBaseline() const {
    nvs_handle_t handle = 0;
    uint32_t baseline = 0;
    if (nvs_open(kTouchSettingsNamespace, NVS_READONLY, &handle) == ESP_OK) {
        nvs_get_u32(handle, kRightHandBaselineKey, &baseline);
        nvs_close(handle);
    }
    return baseline;
}

void RiriTouchController::SaveRightHandBaseline(uint32_t baseline) const {
    if (baseline == 0) {
        return;
    }

    nvs_handle_t handle = 0;
    if (nvs_open(kTouchSettingsNamespace, NVS_READWRITE, &handle) != ESP_OK) {
        ESP_LOGW(TAG, "Baseline tangan kanan tidak dapat disimpan ke NVS");
        return;
    }
    if (nvs_set_u32(handle, kRightHandBaselineKey, baseline) == ESP_OK) {
        nvs_commit(handle);
    } else {
        ESP_LOGW(TAG, "Gagal menyimpan baseline tangan kanan");
    }
    nvs_close(handle);
}

void RiriTouchController::MonitorStartupWifiConfigGesture() {
    constexpr RiriTouchPoint right_hand = {
        "tangan kanan", RIRI_TOUCH_RIGHT_HAND_GPIO, TOUCH_PAD_NUM11, "Aktif.ogg", RiriTouchAction::WAKE_AI};
    const int index = TouchPadIndex(right_hand);
    if (!touch_driver_ready_ || !enabled_[index]) {
        return;
    }

    // Baseline tersimpan membuat gesture tetap dapat dikenali bila tangan sudah
    // memegang foil sebelum ESP32-S3 dinyalakan. Bila firmware baru pertama
    // kali dipasang, gunakan kalibrasi boot ini; pengguna cukup menyentuh
    // sesudah log boot mulai atau memakai tombol BOOT satu kali.
    const uint32_t baseline = saved_right_hand_baseline_ != 0
        ? saved_right_hand_baseline_
        : baselines_[index];
    if (baseline == 0) {
        return;
    }

    ESP_LOGI(TAG, "Sentuh tangan kanan sebelum AI siap untuk konfigurasi Wi-Fi");
    int stable_samples = 0;
    const int attempts = kStartupWifiGestureTimeoutMs / kSampleIntervalMs;
    for (int attempt = 0; attempt < attempts; ++attempt) {
        const auto state = Application::GetInstance().GetDeviceState();
        if (state != kDeviceStateStarting && state != kDeviceStateActivating) {
            return;
        }

        uint32_t raw = 0;
        if (touch_pad_read_raw_data(right_hand.touch_pad, &raw) == ESP_OK &&
            raw > baseline &&
            ((static_cast<uint64_t>(raw - baseline) * 1000U) / baseline) >= kTouchRisePermille) {
            ++stable_samples;
            if (stable_samples >= kBootGestureStableSamplesRequired) {
                ESP_LOGI(TAG, "Tangan kanan disentuh: membuka konfigurasi Wi-Fi");
                if (wifi_config_request_callback_) {
                    wifi_config_request_callback_();
                } else {
                    ESP_LOGE(TAG, "Callback konfigurasi Wi-Fi belum dipasang");
                }
                return;
            }
        } else {
            stable_samples = 0;
        }
        vTaskDelay(pdMS_TO_TICKS(kSampleIntervalMs));
    }
}

bool RiriTouchController::ConfigureTouchPoint(RiriTouchPoint& point) {
    if (!touch_driver_ready_) {
        return false;
    }

    esp_err_t err = touch_pad_config(point.touch_pad);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Gagal konfigurasi %s GPIO%d: %s",
            point.name, point.gpio, esp_err_to_name(err));
        return false;
    }

    return true;
}

void RiriTouchController::CalibrateAllPoints(bool save_right_hand_baseline) {
    // Seluruh foil dikalibrasi berbarengan secara bergiliran, bukan satu per
    // satu sampai selesai.
    //
    // Cara lama mengambil 32 sampel untuk satu foil dulu, baru pindah ke foil
    // berikutnya, sehingga total memakan lima detik lebih. Selama lima detik itu
    // radio Wi-Fi dan penguat audio ikut menyala, dan foil yang dikalibrasi
    // paling awal mendapat garis dasar dari kondisi listrik yang sudah tidak
    // berlaku lagi. Begitu radio aktif, nilai mentahnya melompat dan langsung
    // terbaca sebagai sentuhan padahal tidak ada yang menyentuh.
    //
    // Dengan cara bergiliran, semua foil melihat kondisi listrik yang sama dan
    // seluruh kalibrasi selesai kurang dari satu detik.
    constexpr int kPointCount = static_cast<int>(sizeof(kTouchPoints) / sizeof(kTouchPoints[0]));

    static uint32_t samples[kPointCount][kCalibrationSamples];
    int valid_samples[kPointCount] = {};

    for (int round = 0; round < kCalibrationSamples; ++round) {
        vTaskDelay(pdMS_TO_TICKS(kCalibrationSampleDelayMs));
        for (int slot = 0; slot < kPointCount; ++slot) {
            const auto& point = kTouchPoints[slot];
            if (!IsTouchPad(point) || !enabled_[TouchPadIndex(point)]) {
                continue;
            }
            uint32_t raw = 0;
            if (touch_pad_read_raw_data(point.touch_pad, &raw) == ESP_OK && raw > 0) {
                samples[slot][valid_samples[slot]++] = raw;
            }
        }
    }

    for (int slot = 0; slot < kPointCount; ++slot) {
        const auto& point = kTouchPoints[slot];
        if (!IsTouchPad(point) || !enabled_[TouchPadIndex(point)]) {
            continue;
        }

        const int index = TouchPadIndex(point);
        const int count = valid_samples[slot];
        if (count == 0) {
            baselines_[index] = 1;
            ESP_LOGW(TAG, "%s GPIO%d tidak mendapat sampel kalibrasi", point.name, point.gpio);
            continue;
        }

        std::sort(samples[slot], samples[slot] + count);
        const int trim = count >= 8 ? count / 4 : 0;
        uint64_t sum = 0;
        for (int i = trim; i < count - trim; ++i) {
            sum += samples[slot][i];
        }
        const int used = count - (trim * 2);
        baselines_[index] = static_cast<uint32_t>(sum / used);

        if (save_right_hand_baseline && point.touch_pad == TOUCH_PAD_NUM11) {
            SaveRightHandBaseline(baselines_[index]);
        }

        // Sebaran sampel menjadi petunjuk apakah jalur foil itu berisik.
        // Foil dengan sebaran lebar mudah salah picu, jadi harus terlihat jelas
        // di log agar kabelnya dapat diperbaiki.
        const uint32_t lowest = samples[slot][0];
        const uint32_t highest = samples[slot][count - 1];
        const uint32_t spread_permille = baselines_[index] > 0
            ? static_cast<uint32_t>((static_cast<uint64_t>(highest - lowest) * 1000U) / baselines_[index])
            : 0;
        if (spread_permille > kNoisyChannelSpreadPermille) {
            ESP_LOGW(TAG, "%s GPIO%d berisik: baseline=%lu sebaran=%lu per seribu",
                point.name, point.gpio, static_cast<unsigned long>(baselines_[index]),
                static_cast<unsigned long>(spread_permille));
        } else {
            ESP_LOGI(TAG, "%s GPIO%d baseline stabil=%lu sebaran=%lu per seribu",
                point.name, point.gpio, static_cast<unsigned long>(baselines_[index]),
                static_cast<unsigned long>(spread_permille));
        }

        // Mesin status disetel ulang supaya sisa hitungan dari kondisi lama
        // tidak ikut terbawa setelah garis dasar berubah.
        pressed_[index] = false;
        touch_counts_[index] = 0;
        release_counts_[index] = 0;
        stuck_counts_[index] = 0;
    }
}

// Kenaikan nilai mentah terhadap garis dasar, dinyatakan dalam per seribu.
// Nilai relatif dipakai supaya foil besar dan foil kecil dapat dibandingkan
// secara adil, karena garis dasarnya memang berbeda jauh.
int RiriTouchController::RisePermille(const RiriTouchPoint& point, uint32_t raw_value) const {
    const int index = TouchPadIndex(point);
    const uint32_t baseline = std::max<uint32_t>(baselines_[index], 1);
    if (raw_value <= baseline) {
        return 0;
    }
    const uint64_t rise = static_cast<uint64_t>(raw_value - baseline) * 1000U / baseline;
    return rise > 100000 ? 100000 : static_cast<int>(rise);
}

bool RiriTouchController::IsTouched(RiriTouchPoint& point, uint32_t raw_value) {
    // Pada ESP32-S3 nilai mentah naik saat kapasitansi elektroda bertambah.
    // Menganggap nilai yang turun sebagai sentuhan dahulu menjadi sumber salah
    // picu dari derau foil.
    return RisePermille(point, raw_value) >= kTouchRisePermille;
}

bool RiriTouchController::IsReleased(RiriTouchPoint& point, uint32_t raw_value) {
    return RisePermille(point, raw_value) <= kReleaseRisePermille;
}

void RiriTouchController::UpdateIdleBaseline(RiriTouchPoint& point, uint32_t raw_value) {
    int index = TouchPadIndex(point);
    uint32_t baseline = std::max<uint32_t>(baselines_[index], 1);
    // Track slow environmental drift only while the channel is confirmed idle.
    baselines_[index] = static_cast<uint32_t>((static_cast<uint64_t>(baseline) * 63U + raw_value) / 64U);
}

void RiriTouchController::HandleTouch(const RiriTouchPoint& point) {
    ESP_LOGI(TAG, "Sentuh %s", point.name);

    auto& app = Application::GetInstance();

    // Sentuhan apa pun menghentikan musik supaya suara reaksi boneka terdengar
    // jelas dan tidak bertumpuk dengan lagu yang sedang diputar. Penantian di
    // sini penting: pemutar musik membersihkan antrean playback saat berhenti,
    // dan pembersihan itu akan ikut menghapus suara reaksi bila dijalankan
    // setelah suara tersebut dimasukkan ke antrean.
    MusicPlayer::GetInstance().StopAndWait();

    if (!PlayTouchSound(point.sound_asset)) {
        ESP_LOGW(TAG, "Aset suara tidak ditemukan: %s", point.sound_asset);
    }

    if (point.action != RiriTouchAction::WAKE_AI) {
        return;
    }

    const auto state = app.GetDeviceState();
    if (state == kDeviceStateUnknown || state == kDeviceStateStarting ||
        state == kDeviceStateWifiConfiguring || state == kDeviceStateAudioTesting ||
        state == kDeviceStateUpgrading || state == kDeviceStateActivating) {
        // Jalur aktivasi manual tidak boleh memicu mode uji mikrofon saat Wi-Fi
        // masih dikonfigurasi atau perangkat belum selesai booting.
        ESP_LOGW(TAG, "AI belum siap untuk diaktifkan dari %s (state=%d)",
            point.name, static_cast<int>(state));
        return;
    }

    TickType_t now = xTaskGetTickCount();
    if (static_cast<int32_t>(now - last_any_wake_ticks_) < pdMS_TO_TICKS(kWakeCooldownMs)) {
        return;
    }
    last_any_wake_ticks_ = now;

    app.GetAudioService().WaitForPlaybackQueueEmpty(kActivationSoundWaitMs);
    // WakeUpFromTouch memakai mode dengar bawaan (auto stop) supaya server
    // mendeteksi sendiri akhir bicara pengguna dan AI pasti menjawab.
    app.WakeUpFromTouch();
}

bool RiriTouchController::PlayTouchSound(const char* asset_name) {
    if (asset_name == nullptr || asset_name[0] == '\0') {
        return false;
    }

    auto& audio_service = Application::GetInstance().GetAudioService();
    if (!audio_service.IsInitialized()) {
        ESP_LOGW(TAG, "Layanan audio belum siap, suara sentuh dilewati");
        return false;
    }

    void* ptr = nullptr;
    size_t size = 0;
    if (!Assets::GetInstance().GetAssetData(asset_name, ptr, size) || ptr == nullptr || size == 0) {
        return false;
    }

    // SetOutputVolume menulis ke NVS, jadi hanya dipanggil saat nilainya memang
    // berbeda. Tanpa penjagaan ini setiap sentuhan menulis ulang flash.
    if (auto codec = Board::GetInstance().GetAudioCodec()) {
        if (codec->output_volume() != kTouchVolume) {
            codec->SetOutputVolume(kTouchVolume);
        }
    }
    Application::GetInstance().PlaySound(
        std::string_view(static_cast<const char*>(ptr), size));
    return true;
}

void RiriTouchController::TouchTask(void* arg) {
    static_cast<RiriTouchController*>(arg)->Run();
    vTaskDelete(nullptr);
}

void RiriTouchController::Run() {
    // Sensor sentuh dibuat dari konstruktor board, yaitu sebelum
    // Application::Initialize() menyiapkan layanan audio. Menyentuh foil pada
    // celah waktu itu akan memakai codec yang belum ada, jadi tunggu dulu.
    while (!Application::GetInstance().GetAudioService().IsInitialized()) {
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    // Tangan kanan dipantau sepanjang Riri menyambung Wi-Fi atau mengaktifkan
    // AI. Pada fase ini sentuhan tidak memutar Aktif.ogg: ia selalu membuka
    // konfigurasi Wi-Fi. Setelah Riri idle, alur sentuh biasa di bawah tetap
    // mempertahankan reaksi suara dan fungsi membangunkan AI.
    MonitorStartupWifiConfigGesture();

    // Tunggu sampai perangkat benar-benar tenang sebelum mengambil garis dasar
    // yang dipakai sungguhan.
    //
    // Menyalakan radio Wi-Fi dan penguat audio menggeser nilai kapasitif semua
    // foil. Bila garis dasar diambil sebelum keduanya aktif, pergeseran itu
    // terbaca sebagai sentuhan padahal tidak ada yang menyentuh. Titik paling
    // aman adalah setelah perangkat selesai menyambung dan masuk status siaga,
    // atau setelah masuk mode konfigurasi Wi-Fi bila sambungan gagal.
    {
        auto& app = Application::GetInstance();
        int waited_ms = 0;
        while (waited_ms < kSettleWaitTimeoutMs) {
            const auto state = app.GetDeviceState();
            if (state == kDeviceStateIdle || state == kDeviceStateWifiConfiguring) {
                break;
            }
            vTaskDelay(pdMS_TO_TICKS(100));
            waited_ms += 100;
        }
        ESP_LOGI(TAG, "Perangkat tenang setelah %d ms, menunggu jeda stabil", waited_ms);
    }

    // Jeda tambahan supaya lonjakan terakhir dari aktivasi dan pemutaran suara
    // sambutan tidak ikut terekam ke dalam garis dasar.
    vTaskDelay(pdMS_TO_TICKS(kPostSettleDelayMs));

    if (touch_driver_ready_) {
        CalibrateAllPoints(true);
        ESP_LOGI(TAG, "Kalibrasi akhir selesai, sensor sentuh aktif dalam %d ms",
            kArmingDelayMs);
    }

    // Masa siaga sebelum sentuhan pertama boleh memicu. Tanpa jeda ini, sisa
    // goyangan nilai tepat setelah kalibrasi masih sempat terbaca sebagai
    // sentuhan.
    vTaskDelay(pdMS_TO_TICKS(kArmingDelayMs));
    ESP_LOGI(TAG, "Sensor sentuh siap menerima sentuhan");

    constexpr int kPointCount = static_cast<int>(sizeof(kTouchPoints) / sizeof(kTouchPoints[0]));
    uint32_t raw_values[kPointCount] = {};
    bool sample_ok[kPointCount] = {};
    int rise_permille[kPointCount] = {};

    while (true) {
        TickType_t now = xTaskGetTickCount();

        // Tahap pertama: baca semua foil lebih dulu dalam satu putaran, lalu
        // cari mana yang kenaikannya paling besar. Membaca semuanya sekaligus
        // membuat perbandingan adil karena diambil pada kondisi listrik yang
        // sama.
        int best_slot = -1;
        int best_rise = 0;
        int second_rise = 0;
        for (int slot = 0; slot < kPointCount; ++slot) {
            auto& point = kTouchPoints[slot];
            sample_ok[slot] = false;
            rise_permille[slot] = 0;
            if (!IsTouchPad(point) || !enabled_[TouchPadIndex(point)] || !touch_driver_ready_) {
                continue;
            }
            uint32_t raw = 0;
            if (touch_pad_read_raw_data(point.touch_pad, &raw) != ESP_OK) {
                continue;
            }
            raw_values[slot] = raw;
            sample_ok[slot] = true;
            rise_permille[slot] = RisePermille(point, raw);

            if (rise_permille[slot] > best_rise) {
                second_rise = best_rise;
                best_rise = rise_permille[slot];
                best_slot = slot;
            } else if (rise_permille[slot] > second_rise) {
                second_rise = rise_permille[slot];
            }
        }

        // Bila hanya satu foil yang melewati ambang sentuh, tidak ada yang bisa
        // bertabrakan sehingga sentuhan langsung sah. Selisih minimum baru
        // ditagih ketika dua foil sama-sama melewati ambang, karena di situlah
        // kopling antar kabel dapat menyalakan bagian tubuh yang salah.
        const bool winner_is_clear =
            best_slot >= 0 &&
            (second_rise < kTouchRisePermille ||
             (best_rise - second_rise) >= kWinnerMarginPermille);

        // Tahap kedua: jalankan mesin status memakai nilai yang sudah dibaca.
        for (int slot = 0; slot < kPointCount; ++slot) {
            auto& point = kTouchPoints[slot];
            if (!sample_ok[slot]) {
                continue;
            }
            const uint32_t raw = raw_values[slot];
            const bool may_win = winner_is_clear && slot == best_slot;

            int index = TouchPadIndex(point);
            if (pressed_[index]) {
                if (IsReleased(point, raw)) {
                    if (release_counts_[index] < 255) {
                        ++release_counts_[index];
                    }
                    if (release_counts_[index] >= kReleaseStableSamplesRequired) {
                        pressed_[index] = false;
                        release_counts_[index] = 0;
                        touch_counts_[index] = 0;
                        stuck_counts_[index] = 0;
                        UpdateIdleBaseline(point, raw);
                    }
                } else {
                    release_counts_[index] = 0;
                    // Foil yang terkunci dalam keadaan tertekan sangat lama
                    // hampir pasti mengalami pergeseran lingkungan permanen,
                    // bukan jari yang menempel selama itu. Tanpa pelepasan
                    // paksa, foil tersebut tidak akan pernah bereaksi lagi
                    // karena nilainya tidak pernah turun ke ambang lepas.
                    if (++stuck_counts_[index] >= kStuckPressedSamplesBeforeRelease) {
                        stuck_counts_[index] = 0;
                        pressed_[index] = false;
                        baselines_[index] = raw;
                        ESP_LOGW(TAG, "%s GPIO%d tertekan terlalu lama, garis dasar disetel ulang ke %lu",
                            point.name, point.gpio, static_cast<unsigned long>(raw));
                    }
                }
            } else {
                // Hanya foil pemenang yang boleh maju menuju pemicu. Foil lain
                // yang ikut naik dikembalikan ke nol supaya tidak ada dua
                // reaksi yang bertabrakan dalam satu sentuhan.
                if (IsTouched(point, raw) && may_win) {
                    if (touch_counts_[index] < 255) {
                        ++touch_counts_[index];
                    }
                    if (touch_counts_[index] >= kTouchStableSamplesRequired) {
                        pressed_[index] = true;
                        touch_counts_[index] = 0;
                        release_counts_[index] = 0;
                        stuck_counts_[index] = 0;
                        if (static_cast<int32_t>(now - last_trigger_ticks_[index]) >=
                            pdMS_TO_TICKS(kTriggerCooldownMs)) {
                            last_trigger_ticks_[index] = now;
                            HandleTouch(point);
                        }
                    }
                } else {
                    touch_counts_[index] = 0;
                    if (IsReleased(point, raw)) {
                        stuck_counts_[index] = 0;
                        UpdateIdleBaseline(point, raw);
                    } else if (IsTouched(point, raw)) {
                        // Nilainya tinggi tetapi kalah dari foil lain. Ini foil
                        // tetangga yang ikut terangkat, bukan foil macet, jadi
                        // penghitung zona mati tidak boleh ikut berjalan.
                        stuck_counts_[index] = 0;
                    } else if (++stuck_counts_[index] >= kStuckSamplesBeforeRebase) {
                        // Nilai bertahan di antara ambang lepas dan ambang
                        // sentuh. Tanpa penarikan ulang ini foil tersebut diam
                        // selamanya karena dua cabang di atas tidak pernah
                        // dijalankan.
                        stuck_counts_[index] = 0;
                        baselines_[index] = raw;
                        ESP_LOGW(TAG, "%s GPIO%d menetap di zona mati, baseline disetel ulang ke %lu",
                            point.name, point.gpio, static_cast<unsigned long>(raw));
                    }
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(kSampleIntervalMs));
    }
}
