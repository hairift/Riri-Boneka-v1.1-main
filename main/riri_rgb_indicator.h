#pragma once

#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

/**
 * LED RGB indikator status pada boneka Riri.
 *
 * LED bawaan papan di GPIO48 tetap dipakai sebagai penanda status percakapan.
 * LED ini menambahkan informasi yang tidak terlihat dari sana:
 *
 *   biru           Wi-Fi tersambung
 *   hijau berkedip perangkat sedang menyala
 *   merah berkedip jaringan bermasalah atau tersendat
 *   berganti warna lagu sedang diputar
 *
 * Sisa daya baterai tidak ditampilkan di sini. Boneka memakai modul penunjuk
 * kapasitas baterai lithium yang berdiri sendiri dan mengukur tegangan sel
 * secara langsung, jauh lebih tepat daripada yang dapat dibaca ESP32 pada
 * boneka ini. Seluruh kanal ADC1 memang sudah terpakai foil sentuh dan jalur
 * audio.
 *
 * Warnanya digerakkan PWM lewat LEDC sehingga dapat dicampur, bukan sekadar
 * menyala penuh.
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
    void TulisWarna(Warna warna);
    Warna WarnaStatus(int denyut);

    bool siap_ = false;
    TaskHandle_t task_ = nullptr;
};
