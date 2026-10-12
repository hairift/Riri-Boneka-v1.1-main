#include "single_led.h"
#include "application.h"
#include "music_player.h"  // Tambahan Riri Boneka
#include <esp_log.h>

#define TAG "SingleLed"

#define DEFAULT_BRIGHTNESS 4
#define HIGH_BRIGHTNESS 16
#define LOW_BRIGHTNESS 2

#define BLINK_INFINITE -1

/*
 * Tambahan Riri Boneka: peta warna lampu indikator.
 *
 * Boneka ini dipakai tanpa layar, jadi LED RGB bawaan menjadi satu-satunya
 * penanda kondisi perangkat. Setiap kondisi memakai warna yang jelas berbeda:
 *
 *   Biru berkedip cepat   : sedang menyala dan menyiapkan diri
 *   Biru berkedip sedang  : sedang memindai atau menyambung Wi-Fi
 *   Biru berkedip lambat  : mode konfigurasi Wi-Fi lewat hotspot
 *   Hijau redup menyala   : siaga, Wi-Fi tersambung, siap diajak bicara
 *   Kuning berkedip       : jaringan tidak stabil, sempat tersambung lalu putus
 *   Merah berkedip cepat  : ada galat jaringan atau server
 *   Cyan menyala          : sedang menghubungi server
 *   Putih menyala         : sedang mendengarkan, lebih terang saat ada suara
 *   Hijau terang menyala  : AI sedang berbicara
 *   Ungu menyala          : sedang memutar musik
 *   Hijau berkedip cepat  : pembaruan firmware berlangsung
 *   Hijau berkedip lambat : proses aktivasi perangkat
 */
#define COLOR_BLUE          0, 0, DEFAULT_BRIGHTNESS
#define COLOR_CYAN          0, DEFAULT_BRIGHTNESS, DEFAULT_BRIGHTNESS
#define COLOR_GREEN_DIM     0, LOW_BRIGHTNESS, 0
#define COLOR_GREEN         0, DEFAULT_BRIGHTNESS, 0
#define COLOR_YELLOW        DEFAULT_BRIGHTNESS, DEFAULT_BRIGHTNESS, 0
#define COLOR_RED           HIGH_BRIGHTNESS, 0, 0
#define COLOR_PURPLE        DEFAULT_BRIGHTNESS, 0, HIGH_BRIGHTNESS
#define COLOR_WHITE_DIM     LOW_BRIGHTNESS, LOW_BRIGHTNESS, LOW_BRIGHTNESS
#define COLOR_WHITE_BRIGHT  HIGH_BRIGHTNESS, HIGH_BRIGHTNESS, HIGH_BRIGHTNESS


SingleLed::SingleLed(gpio_num_t gpio) {
    if (gpio == GPIO_NUM_NC) {
        ESP_LOGW(TAG, "SingleLed initialized with GPIO_NUM_NC, LED will not function");
        return;
    }

    led_strip_config_t strip_config = {};
    strip_config.strip_gpio_num = gpio;
    strip_config.max_leds = 1;
    strip_config.color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB;
    strip_config.led_model = LED_MODEL_WS2812;

    led_strip_rmt_config_t rmt_config = {};
    rmt_config.resolution_hz = 10 * 1000 * 1000; // 10MHz

    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip_));
    led_strip_clear(led_strip_);

    esp_timer_create_args_t blink_timer_args = {
        .callback = [](void *arg) {
            auto led = static_cast<SingleLed*>(arg);
            led->OnBlinkTimer();
        },
        .arg = this,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "blink_timer",
        .skip_unhandled_events = false,
    };
    ESP_ERROR_CHECK(esp_timer_create(&blink_timer_args, &blink_timer_));
}

SingleLed::~SingleLed() {
    if (blink_timer_ != nullptr) {
        esp_timer_stop(blink_timer_);
    }
    if (led_strip_ != nullptr) {
        led_strip_del(led_strip_);
    }
}


void SingleLed::SetColor(uint8_t r, uint8_t g, uint8_t b) {
    r_ = r;
    g_ = g;
    b_ = b;
}

void SingleLed::TurnOn() {
    if (led_strip_ == nullptr) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    esp_timer_stop(blink_timer_);
    led_strip_set_pixel(led_strip_, 0, r_, g_, b_);
    led_strip_refresh(led_strip_);
}

void SingleLed::TurnOff() {
    if (led_strip_ == nullptr) {
        return;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    esp_timer_stop(blink_timer_);
    led_strip_clear(led_strip_);
}

void SingleLed::BlinkOnce() {
    Blink(1, 100);
}

void SingleLed::Blink(int times, int interval_ms) {
    StartBlinkTask(times, interval_ms);
}

void SingleLed::StartContinuousBlink(int interval_ms) {
    StartBlinkTask(BLINK_INFINITE, interval_ms);
}

void SingleLed::StartBlinkTask(int times, int interval_ms) {
    if (led_strip_ == nullptr) {
        return;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    esp_timer_stop(blink_timer_);
    
    blink_counter_ = times * 2;
    blink_interval_ms_ = interval_ms;
    esp_timer_start_periodic(blink_timer_, interval_ms * 1000);
}

void SingleLed::OnBlinkTimer() {
    std::lock_guard<std::mutex> lock(mutex_);
    blink_counter_--;
    if (blink_counter_ & 1) {
        led_strip_set_pixel(led_strip_, 0, r_, g_, b_);
        led_strip_refresh(led_strip_);
    } else {
        led_strip_clear(led_strip_);

        if (blink_counter_ == 0) {
            esp_timer_stop(blink_timer_);
        }
    }
}


void SingleLed::OnStateChanged() {
    auto& app = Application::GetInstance();
    auto device_state = app.GetDeviceState();
    auto network = app.GetNetworkIndicator();

    switch (device_state) {
        case kDeviceStateStarting:
            // Saat booting, kondisi jaringan yang lebih dulu diperlihatkan
            // karena di situlah kegagalan paling sering terjadi.
            if (network == NetworkIndicator::kScanning || network == NetworkIndicator::kConnecting) {
                SetColor(COLOR_BLUE);
                StartContinuousBlink(250);
            } else if (network == NetworkIndicator::kError) {
                SetColor(COLOR_RED);
                StartContinuousBlink(150);
            } else {
                SetColor(COLOR_BLUE);
                StartContinuousBlink(100);
            }
            break;

        case kDeviceStateWifiConfiguring:
            // Menunggu pengguna membuka hotspot dan mengisi sandi Wi-Fi.
            SetColor(COLOR_BLUE);
            StartContinuousBlink(500);
            break;

        case kDeviceStateIdle:
            if (MusicPlayer::GetInstance().IsStreaming()) {
                SetColor(COLOR_PURPLE);
                TurnOn();
            } else if (network == NetworkIndicator::kError) {
                SetColor(COLOR_RED);
                StartContinuousBlink(150);
            } else if (network == NetworkIndicator::kUnstable) {
                SetColor(COLOR_YELLOW);
                StartContinuousBlink(300);
            } else if (network == NetworkIndicator::kScanning ||
                       network == NetworkIndicator::kConnecting) {
                SetColor(COLOR_BLUE);
                StartContinuousBlink(250);
            } else if (network == NetworkIndicator::kConnected) {
                // Siaga dan siap diajak bicara. Nyala redup supaya tidak silau
                // tetapi tetap terlihat bahwa boneka hidup dan siap.
                SetColor(COLOR_GREEN_DIM);
                TurnOn();
            } else {
                TurnOff();
            }
            break;

        case kDeviceStateConnecting:
            SetColor(COLOR_CYAN);
            TurnOn();
            break;

        case kDeviceStateListening:
        case kDeviceStateAudioTesting:
            // Lebih terang saat suara pengguna terdeteksi, sehingga terlihat
            // bahwa mikrofon benar-benar bekerja.
            if (app.IsVoiceDetected()) {
                SetColor(COLOR_WHITE_BRIGHT);
            } else {
                SetColor(COLOR_WHITE_DIM);
            }
            TurnOn();
            break;

        case kDeviceStateSpeaking:
            SetColor(COLOR_GREEN);
            TurnOn();
            break;

        case kDeviceStateNotifying:
            SetColor(0, DEFAULT_BRIGHTNESS, 0);
            TurnOn();
            break;

        case kDeviceStateUpgrading:
            SetColor(COLOR_GREEN);
            StartContinuousBlink(100);
            break;

        case kDeviceStateActivating:
            SetColor(COLOR_GREEN);
            StartContinuousBlink(500);
            break;

        default:
            ESP_LOGW(TAG, "Status LED tidak dikenal: %d", device_state);
            return;
    }
}

