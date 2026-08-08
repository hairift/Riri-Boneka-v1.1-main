#include "riri_rgb_indicator.h"

#include "application.h"
#include "board.h"
#include "config.h"
#include "music_player.h"

#include <driver/ledc.h>
#include <esp_log.h>
#include <esp_timer.h>

#define TAG "RiriRgb"

namespace {

// Enam kanal LEDC, tiga untuk tiap LED. LEDC menyediakan delapan kanal
// kecepatan rendah pada ESP32-S3, jadi masih tersisa dua.
constexpr ledc_timer_t kTimer = LEDC_TIMER_2;
constexpr ledc_mode_t kMode = LEDC_LOW_SPEED_MODE;
constexpr int kResolusiBit = 8;
constexpr uint32_t kFrekuensiHz = 2000;

constexpr int kKanalBateraiAwal = 0;  // 0,1,2 untuk merah, hijau, biru
constexpr int kKanalStatusAwal = 3;   // 3,4,5

// Perbarui setiap 40 milidetik. Cukup halus untuk denyut dan pergantian warna,
// dan cukup jarang untuk tidak mengganggu jalur audio.
constexpr int kJedaPerbaruiMs = 40;

// Pembacaan baterai lewat ADC tidak murah, jadi hanya dilakukan sesekali.
constexpr int64_t kJedaBacaBateraiUs = 5LL * 1000LL * 1000LL;

// Ambang tingkat baterai.
constexpr int kBateraiPenuh = 60;    // di atas ini hijau
constexpr int kBateraiMenengah = 25; // di atas ini kuning, di bawahnya merah

uint8_t Redup(uint8_t nilai, int persen) {
    int hasil = nilai * persen / 100;
    return static_cast<uint8_t>(hasil > 255 ? 255 : (hasil < 0 ? 0 : hasil));
}

}  // namespace

void RiriRgbIndicator::SiapkanKanal(int kanal, gpio_num_t gpio) {
    ledc_channel_config_t cfg = {};
    cfg.gpio_num = gpio;
    cfg.speed_mode = kMode;
    cfg.channel = static_cast<ledc_channel_t>(kanal);
    cfg.intr_type = LEDC_INTR_DISABLE;
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

    SiapkanKanal(kKanalBateraiAwal + 0, RIRI_RGB_BATTERY_R_GPIO);
    SiapkanKanal(kKanalBateraiAwal + 1, RIRI_RGB_BATTERY_G_GPIO);
    SiapkanKanal(kKanalBateraiAwal + 2, RIRI_RGB_BATTERY_B_GPIO);
    SiapkanKanal(kKanalStatusAwal + 0, RIRI_RGB_STATUS_R_GPIO);
    SiapkanKanal(kKanalStatusAwal + 1, RIRI_RGB_STATUS_G_GPIO);
    SiapkanKanal(kKanalStatusAwal + 2, RIRI_RGB_STATUS_B_GPIO);

    siap_ = true;
    ESP_LOGI(TAG, "LED indikator siap: baterai di GPIO%d/%d/%d, status di GPIO%d/%d/%d",
             (int)RIRI_RGB_BATTERY_R_GPIO, (int)RIRI_RGB_BATTERY_G_GPIO,
             (int)RIRI_RGB_BATTERY_B_GPIO, (int)RIRI_RGB_STATUS_R_GPIO,
             (int)RIRI_RGB_STATUS_G_GPIO, (int)RIRI_RGB_STATUS_B_GPIO);

    // Prioritas 1 menjaganya jauh di bawah jalur audio waktu nyata.
    xTaskCreate(Tugas, "riri_rgb", 3072, this, 1, &task_);
}

void RiriRgbIndicator::Tugas(void* arg) {
    static_cast<RiriRgbIndicator*>(arg)->Jalan();
    vTaskDelete(nullptr);
}

void RiriRgbIndicator::TulisWarna(int kanal_awal, Warna warna) {
    const uint8_t nilai[3] = {warna.r, warna.g, warna.b};
    for (int i = 0; i < 3; ++i) {
        // LED katoda bersama menyala saat pin tinggi. Untuk LED anoda bersama,
        // nyalanya terbalik sehingga nilainya perlu dibalik juga.
        uint32_t duty = nilai[i];
#if RIRI_RGB_COMMON_ANODE
        duty = 255 - duty;
#endif
        ledc_set_duty(kMode, static_cast<ledc_channel_t>(kanal_awal + i), duty);
        ledc_update_duty(kMode, static_cast<ledc_channel_t>(kanal_awal + i));
    }
}

RiriRgbIndicator::Warna RiriRgbIndicator::WarnaBaterai(int denyut) {
    const int64_t sekarang = esp_timer_get_time();
    if (sekarang >= baca_baterai_berikutnya_us_) {
        baca_baterai_berikutnya_us_ = sekarang + kJedaBacaBateraiUs;
        int level = 0;
        bool mengisi = false;
        bool memakai = false;
        if (Board::GetInstance().GetBatteryLevel(level, mengisi, memakai)) {
            level_baterai_ = level;
            sedang_mengisi_ = mengisi;
        } else {
            level_baterai_ = -1;
        }
    }

    if (level_baterai_ < 0) {
        // Papan tidak menyediakan pembacaan baterai. Lampu dipadamkan, bukan
        // dibuat hijau, supaya tidak ada yang mengira baterainya penuh.
        return {0, 0, 0};
    }

    Warna warna;
    if (level_baterai_ >= kBateraiPenuh) {
        warna = {0, 255, 0};        // hijau
    } else if (level_baterai_ >= kBateraiMenengah) {
        warna = {255, 150, 0};      // kuning
    } else {
        warna = {255, 0, 0};        // merah
    }

    if (sedang_mengisi_) {
        // Denyut lembut menandakan sedang diisi, tanpa mengubah warnanya
        // supaya tingkat baterainya tetap terbaca.
        const int fase = denyut % 100;
        const int terang = fase < 50 ? 40 + fase : 40 + (100 - fase);
        warna = {Redup(warna.r, terang), Redup(warna.g, terang), Redup(warna.b, terang)};
    }
    return warna;
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
        TulisWarna(kKanalBateraiAwal, WarnaBaterai(denyut));
        TulisWarna(kKanalStatusAwal, WarnaStatus(denyut));
        ++denyut;
        vTaskDelay(pdMS_TO_TICKS(kJedaPerbaruiMs));
    }
}
