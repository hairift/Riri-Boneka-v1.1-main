#pragma once

#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <atomic>

/**
 * Dua LED RGB indikator tambahan pada boneka Riri.
 *
 * LED bawaan papan di GPIO48 tetap dipakai sebagai penanda status percakapan.
 * Dua LED ini menambahkan informasi yang tidak terlihat dari sana:
 *
 *   LED baterai  merah bila hampir habis, kuning bila menengah, hijau bila
 *                penuh, dan berdenyut lembut saat sedang diisi.
 *   LED status   biru saat Wi-Fi tersambung, hijau berkedip saat perangkat
 *                sedang menyala, merah saat jaringan bermasalah, dan berganti
 *                warna terus-menerus saat lagu diputar.
 *
 * Keduanya digerakkan PWM lewat LEDC sehingga warnanya dapat dicampur, bukan
 * sekadar menyala penuh.
 */
class RiriRgbIndicator {
public:
    static RiriRgbIndicator& GetInstance() {
        static RiriRgbIndicator instance;
        return instance;
    }

    RiriRgbIndicator(const RiriRgbIndicator&) = delete;
    RiriRgbIndicator& operator=(const RiriRgbIndicator&) = delete;

    void Initialize();

private:
    RiriRgbIndicator() = default;
    ~RiriRgbIndicator() = default;

    struct Warna {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    };

    static void Tugas(void* arg);
    void Jalan();

    void SiapkanKanal(int kanal, gpio_num_t gpio);
    void TulisWarna(int kanal_awal, Warna warna);
    Warna WarnaBaterai(int denyut);
    Warna WarnaStatus(int denyut);

    bool siap_ = false;
    TaskHandle_t task_ = nullptr;

    // Nilai baterai disimpan supaya pembacaan yang gagal tidak membuat
    // lampunya berkedip-kedip antara warna terakhir dan padam.
    int level_baterai_ = -1;
    bool sedang_mengisi_ = false;
    int64_t baca_baterai_berikutnya_us_ = 0;
};
