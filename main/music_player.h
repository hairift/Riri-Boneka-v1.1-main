#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <freertos/task.h>

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <string>
#include <vector>

/*
 * Pemutar musik daring untuk Riri Boneka.
 *
 * Alur data:
 *   (HTTP MP3) -> [tugas jaringan] -> {ring buffer PSRAM} -> [tugas dekoder]
 *              -> mono -> resample bila perlu -> antrean playback AudioService
 *
 * Sumber lagu dicoba berurutan:
 *
 *   Seluruh pencarian lagu ditangani server Riri:
 *
 *       GET <server>/music/resolve?song=<judul>&artist=<penyanyi>
 *       -> {"title": "...", "artist": "...", "audio_url": "http://...mp3"}
 *
 *   Permintaan membawa header autentikasi X-MAC-Address, X-Chip-ID,
 *   X-Timestamp, dan X-Dynamic-Key. Server memakai YouTube Data API sehingga
 *   rekaman penyanyi aslinya ditemukan, baik lagu internasional maupun lagu
 *   populer Indonesia, lalu mentranskodenya menjadi MP3 mono 24 kHz supaya
 *   ESP32 tidak perlu mengubah laju cuplik.
 */

// Penyangga byte antar tugas dengan pemblokiran dua arah.
class MusicRingBuffer {
public:
    bool Allocate(size_t capacity);
    void Release();

    // Menulis seluruh data. Mengembalikan false jika buffer ditutup lebih dulu.
    bool Write(const uint8_t* data, size_t size);
    // Membaca sampai max_size byte. Mengembalikan 0 saat buffer kosong dan ditutup.
    size_t Read(uint8_t* dest, size_t max_size);

    size_t Size();
    void Close();
    void Reset();
    bool closed();

private:
    std::vector<uint8_t> data_;
    size_t head_ = 0;
    size_t tail_ = 0;
    size_t size_ = 0;
    bool closed_ = true;
    std::mutex mutex_;
    std::condition_variable cv_;
};

struct MusicTrack {
    std::string title;
    std::string artist;
    std::string stream_url;
    std::string source;
    int duration_seconds = 0;
};

class MusicPlayer {
public:
    static MusicPlayer& GetInstance() {
        static MusicPlayer instance;
        return instance;
    }

    MusicPlayer(const MusicPlayer&) = delete;
    MusicPlayer& operator=(const MusicPlayer&) = delete;

    /*
     * Mencari lalu memutar lagu. Mengembalikan kalimat status untuk dijawab AI.
     * Penyanyi boleh dikosongkan; bila kosong, judul akan dipisah sendiri saat
     * berpola "penyanyi - judul" atau "judul oleh penyanyi".
     */
    std::string Play(const std::string& song, const std::string& artist = std::string());

    // Menghentikan pemutaran. Aman dipanggil dari tugas mana pun dan tidak memblokir.
    void Stop();

    /*
     * Menghentikan musik karena pengguna kembali berinteraksi dengan AI.
     * Berbeda dengan Stop(), permintaan ini diabaikan selama lagu masih menunggu
     * giliran, karena perpindahan status otomatis setelah AI selesai bicara
     * bukan interaksi baru dari pengguna.
     */
    void StopForInteraction();

    /*
     * Menghentikan musik lalu menunggu tugas pemutar benar-benar keluar.
     * Dipakai pemanggil yang akan langsung memutar audio lain setelahnya,
     * supaya pembersihan antrean oleh pemutar musik tidak ikut menghapus
     * suara baru tersebut. Jangan dipanggil dari tugas utama aplikasi.
     */
    void StopAndWait(int timeout_ms = 1500);

    bool IsPlaying() const { return playing_.load(); }

    // Benar hanya saat lagu sudah benar-benar terdengar dari speaker.
    // Dipakai indikator LED agar warna musik tidak menyala saat lagu masih
    // menunggu giliran.
    bool IsStreaming() const { return streaming_.load(); }

    std::string GetNowPlaying();

private:
    MusicPlayer();
    ~MusicPlayer() = default;

    static void NetworkTask(void* arg);
    static void DecoderTask(void* arg);
    void RunNetwork();
    void RunDecoder();

    bool ResolveTrack(const std::string& song, const std::string& artist, MusicTrack& track);
    bool ResolveFromMusicServer(const std::string& song, const std::string& artist, MusicTrack& track);

    bool StartTasks();
    void WaitTasksStopped(int timeout_ms);

    // Menunggu Riri selesai bicara dan sesi percakapan ditutup. Mengembalikan
    // salah bila permintaan dibatalkan atau perangkat tidak kunjung menganggur.
    bool TungguPerangkatMenganggur();

    // Nada tunggu yang berbunyi selama lagu disiapkan.
    //
    // Diulang selama penantian dan dipotong begitu lagunya siap, sehingga
    // pengguna selalu dapat membedakan "sedang mengunduh" dari "macet".
    void MulaiNadaTunggu();
    void LanjutkanNadaTunggu();
    void HentikanNadaTunggu();

    int64_t nada_tunggu_berikutnya_us_ = 0;
    bool nada_tunggu_aktif_ = false;
    std::atomic<bool> sesi_ditutup_{false};

    std::mutex control_mutex_;
    std::mutex track_mutex_;
    MusicTrack track_;
    // Permintaan pengguna, disimpan sampai tugas jaringan sempat mencarinya.
    std::string permintaan_judul_;
    std::string permintaan_penyanyi_;

    MusicRingBuffer ring_;
    std::atomic<bool> playing_{false};
    std::atomic<bool> streaming_{false};
    std::atomic<bool> stop_requested_{false};
    EventGroupHandle_t event_group_ = nullptr;
};
