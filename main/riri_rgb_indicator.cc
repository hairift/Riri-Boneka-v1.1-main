#include "riri_rgb_indicator.h"

#include "application.h"
#include "config.h"
#include "music_player.h"

#include <driver/ledc.h>
#include <esp_log.h>

#define TAG "RiriRgb"

namespace {

// Tiga kanal LEDC untuk merah, hijau, dan biru. LEDC menyediakan delapan kanal
// kecepatan rendah pada ESP32-S3, jadi masih tersisa lima.
constexpr ledc_timer_t kTimer = LEDC_TIMER_2;
constexpr ledc_mode_t kMode = LEDC_LOW_SPEED_MODE;
constexpr int kResolusiBit = 8;
constexpr uint32_t kFrekuensiHz = 2000;
constexpr int kKanalAwal = 0;  // 0, 1, 2

// Perbarui setiap 40 milidetik. Cukup halus untuk kedipan dan pergantian
// warna, dan cukup jarang untuk tidak mengganggu jalur audio.
constexpr int kJedaPerbaruiMs = 40;

}  // namespace

void RiriRgbIndicator::SiapkanKanal(int kanal, gpio_num_t gpio) {
    // Bidang intr_type sengaja tidak diisi. Bidang itu sudah usang pada
    // ESP-IDF 6 dan mengisinya menimbulkan peringatan, sedangkan nilai nol
    // dari inisialisasi kosong memang berarti interupsi dimatikan.
    ledc_channel_config_t cfg = {};
    cfg.gpio_num = gpio;
    cfg.speed_mode = kMode;
    cfg.channel = static_cast<ledc_channel_t>(kanal);
    cfg.timer_sel = kTimer;
    cfg.duty = 0;
    cfg.hpoint = 0;
    ESP_ERROR_CHECK(ledc_channel_config(&cfg));
}

void RiriRgbIndicator::Initialize() {
    if (siap_) {
        return;
    }

    ledc_timer_config_t timer_cfg = {};
    timer_cfg.speed_mode = kMode;
    timer_cfg.duty_resolution = static_cast<ledc_timer_bit_t>(kResolusiBit);
    timer_cfg.timer_num = kTimer;
    timer_cfg.freq_hz = kFrekuensiHz;
    timer_cfg.clk_cfg = LEDC_AUTO_CLK;
    ESP_ERROR_CHECK(ledc_timer_config(&timer_cfg));

    SiapkanKanal(kKanalAwal + 0, RIRI_RGB_STATUS_R_GPIO);
    SiapkanKanal(kKanalAwal + 1, RIRI_RGB_STATUS_G_GPIO);
    SiapkanKanal(kKanalAwal + 2, RIRI_RGB_STATUS_B_GPIO);

    siap_ = true;
    ESP_LOGI(TAG, "LED status siap di GPIO%d, GPIO%d, GPIO%d",
             (int)RIRI_RGB_STATUS_R_GPIO, (int)RIRI_RGB_STATUS_G_GPIO,
             (int)RIRI_RGB_STATUS_B_GPIO);

    // Prioritas 1 menjaganya jauh di bawah jalur audio waktu nyata.
    xTaskCreate(Tugas, "riri_rgb", 3072, this, 1, &task_);
}

void RiriRgbIndicator::Tugas(void* arg) {
    static_cast<RiriRgbIndicator*>(arg)->Jalan();
    vTaskDelete(nullptr);
}

void RiriRgbIndicator::TulisWarna(Warna warna) {
    const uint8_t nilai[3] = {warna.r, warna.g, warna.b};
    for (int i = 0; i < 3; ++i) {
        // LED katoda bersama menyala saat pin tinggi. Untuk LED anoda bersama,
        // nyalanya terbalik sehingga nilainya perlu dibalik juga.
        uint32_t duty = nilai[i];
#if RIRI_RGB_COMMON_ANODE
        duty = 255 - duty;
#endif
        ledc_set_duty(kMode, static_cast<ledc_channel_t>(kKanalAwal + i), duty);
        ledc_update_duty(kMode, static_cast<ledc_channel_t>(kKanalAwal + i));
    }
}

RiriRgbIndicator::Warna RiriRgbIndicator::WarnaStatus(int denyut) {
    auto& app = Application::GetInstance();

    // Lagu diputar mendapat prioritas tertinggi karena itulah keadaan yang
    // paling jelas terlihat pengguna.
    if (MusicPlayer::GetInstance().IsPlaying()) {
        // Pergantian warna pelan mengikuti roda warna sederhana.
        const int fase = (denyut * 3) % 300;
        if (fase < 100) {
            return {static_cast<uint8_t>(255 - fase * 2),
                    static_cast<uint8_t>(fase * 2), 0};
        }
        if (fase < 200) {
            const int f = fase - 100;
            return {0, static_cast<uint8_t>(255 - f * 2), static_cast<uint8_t>(f * 2)};
        }
        const int f = fase - 200;
        return {static_cast<uint8_t>(f * 2), 0, static_cast<uint8_t>(255 - f * 2)};
    }

    const bool kedip = (denyut % 25) < 13;

    switch (app.GetNetworkIndicator()) {
        case NetworkIndicator::kError:
        case NetworkIndicator::kUnstable:
            // Merah berkedip untuk jaringan bermasalah.
            return kedip ? Warna{255, 0, 0} : Warna{40, 0, 0};
        case NetworkIndicator::kScanning:
        case NetworkIndicator::kConnecting:
            return kedip ? Warna{0, 0, 255} : Warna{0, 0, 30};
        case NetworkIndicator::kConfiguring:
            return kedip ? Warna{255, 120, 0} : Warna{0, 0, 0};
        case NetworkIndicator::kConnected:
            break;
        case NetworkIndicator::kUnknown:
        default:
            // Belum ada informasi jaringan berarti perangkat masih menyiapkan
            // diri. Hijau berkedip menandakan program sedang menyala.
            return kedip ? Warna{0, 255, 0} : Warna{0, 30, 0};
    }

    // Jaringan tersambung. Warna biru tenang, sedikit lebih terang saat sedang
    // melayani percakapan supaya terlihat bedanya dari sekadar menganggur.
    return app.GetDeviceState() == kDeviceStateIdle ? Warna{0, 0, 90} : Warna{0, 0, 255};
}

void RiriRgbIndicator::Jalan() {
    int denyut = 0;
    while (true) {
        TulisWarna(WarnaStatus(denyut));
        ++denyut;
        vTaskDelay(pdMS_TO_TICKS(kJedaPerbaruiMs));
    }
}
