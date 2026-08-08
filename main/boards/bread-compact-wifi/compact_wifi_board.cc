#include "wifi_board.h"
#include "codecs/no_audio_codec.h"
#include "display/oled_display.h"
#include "system_reset.h"
#include "application.h"
#include "button.h"
#include "config.h"
#include "mcp_server.h"
#include "lamp_controller.h"
#include "led/single_led.h"
#include "assets/lang_config.h"
#include "riri_touch_controller.h"
#include "riri_rgb_indicator.h"
#include "campus_mcp_tools.h"

#include <esp_log.h>
#include <driver/i2c_master.h>
#include <esp_lcd_panel_ops.h>
#include <esp_lcd_panel_vendor.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#ifdef SH1106
#include <esp_lcd_panel_sh1106.h>
#endif

#define TAG "CompactWifiBoard"

class CompactWifiBoard : public WifiBoard {
private:
    i2c_master_bus_handle_t display_i2c_bus_;
    esp_lcd_panel_io_handle_t panel_io_ = nullptr;
    esp_lcd_panel_handle_t panel_ = nullptr;
    Display* display_ = nullptr;
    Button boot_button_;

    void InitializeDisplayI2c() {
        i2c_master_bus_config_t bus_config = {
            .i2c_port = (i2c_port_t)0,
            .sda_io_num = DISPLAY_SDA_PIN,
            .scl_io_num = DISPLAY_SCL_PIN,
            .clk_source = I2C_CLK_SRC_DEFAULT,
            .glitch_ignore_cnt = 7,
            .intr_priority = 0,
            .trans_queue_depth = 0,
            .flags = {
                .enable_internal_pullup = 1,
            },
        };
        ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &display_i2c_bus_));
    }

    void InitializeSsd1306Display() {
        // Pompa muatan OLED perlu waktu stabil setelah daya dinyalakan.
        ESP_LOGI(TAG, "Menunggu OLED stabil");
        vTaskDelay(pdMS_TO_TICKS(500));

        esp_lcd_panel_io_i2c_config_t io_config = {
            .dev_addr = 0x3C,
            .scl_speed_hz = 400 * 1000,
            .control_phase_bytes = 1,
            .dc_bit_offset = 6,
            .lcd_cmd_bits = 8,
            .lcd_param_bits = 8,
            .on_color_trans_done = nullptr,
            .user_ctx = nullptr,
            .flags = {
                .dc_low_on_data = 0,
                .disable_control_phase = 0,
            },
        };

        ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c(display_i2c_bus_, &io_config, &panel_io_));

        ESP_LOGI(TAG, "Memasang driver SSD1306");
        esp_lcd_panel_dev_config_t panel_config = {};
        panel_config.reset_gpio_num = GPIO_NUM_NC;
        panel_config.bits_per_pixel = 1;

        esp_lcd_panel_ssd1306_config_t ssd1306_config = {
            .height = static_cast<uint8_t>(DISPLAY_HEIGHT),
        };
        panel_config.vendor_config = &ssd1306_config;

#ifdef SH1106
        ESP_ERROR_CHECK(esp_lcd_new_panel_sh1106(panel_io_, &panel_config, &panel_));
#else
        ESP_ERROR_CHECK(esp_lcd_new_panel_ssd1306(panel_io_, &panel_config, &panel_));
#endif
        ESP_LOGI(TAG, "Driver SSD1306 berhasil dipasang");

        // Panel kadang belum siap pada percobaan pertama, jadi inisialisasi
        // diulang beberapa kali sebelum menyerah ke tampilan kosong.
        bool init_ok = false;
        for (int attempt = 1; attempt <= 3; ++attempt) {
            ESP_LOGI(TAG, "Inisialisasi OLED percobaan %d dari 3", attempt);
            ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_));
            vTaskDelay(pdMS_TO_TICKS(100));
            if (esp_lcd_panel_init(panel_) == ESP_OK) {
                init_ok = true;
                break;
            }
            ESP_LOGW(TAG, "Inisialisasi OLED gagal, mencoba lagi");
            vTaskDelay(pdMS_TO_TICKS(200));
        }

        if (!init_ok) {
            // Boneka tetap berfungsi penuh tanpa layar; lampu RGB menjadi
            // penanda status utamanya.
            ESP_LOGE(TAG, "OLED tidak dapat diinisialisasi, memakai tampilan kosong");
            display_ = new NoDisplay();
            return;
        }

        ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel_, false));
        ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_, true));

        display_ = new OledDisplay(panel_io_, panel_, DISPLAY_WIDTH, DISPLAY_HEIGHT,
            DISPLAY_MIRROR_X, DISPLAY_MIRROR_Y);
    }

    void InitializeButtons() {
        // Boneka hanya memakai tombol BOOT bawaan papan. Semua interaksi lain
        // memakai foil tembaga dan kata pemicu suara.
        boot_button_.OnClick([this]() {
            auto& app = Application::GetInstance();
            if (app.GetDeviceState() == kDeviceStateStarting) {
                EnterWifiConfigMode();
                return;
            }
            if (app.GetDeviceState() == kDeviceStateWifiConfiguring ||
                app.GetDeviceState() == kDeviceStateAudioTesting) {
                // Papan ini menyediakan Wi-Fi lewat hotspot, jadi tombol BOOT
                // tidak boleh masuk ke mode uji mikrofon saat konfigurasi.
                ESP_LOGW(TAG, "Tombol BOOT diabaikan saat konfigurasi Wi-Fi aktif");
                return;
            }
            app.ToggleChatState();
        });
    }

    // Inisialisasi alat MCP, sensor sentuh, dan basis pengetahuan kampus.
    void InitializeTools() {
        static LampController lamp(LAMP_GPIO);
        RiriTouchController::GetInstance().Initialize();
        RiriRgbIndicator::GetInstance().Initialize();
        static CampusMcpTools campus_mcp;
    }

public:
    CompactWifiBoard() :
        boot_button_(BOOT_BUTTON_GPIO) {
        InitializeDisplayI2c();
        InitializeSsd1306Display();
        InitializeButtons();
        InitializeTools();
    }

    virtual Led* GetLed() override {
        static SingleLed led(BUILTIN_LED_GPIO);
        return &led;
    }

    virtual AudioCodec* GetAudioCodec() override {
#ifdef AUDIO_I2S_METHOD_SIMPLEX
        // Mikrofon INMP441 dan penguat MAX98357 pada mode I2S simpleks.
        static NoAudioCodecSimplex audio_codec(AUDIO_INPUT_SAMPLE_RATE, AUDIO_OUTPUT_SAMPLE_RATE,
            AUDIO_I2S_SPK_GPIO_BCLK, AUDIO_I2S_SPK_GPIO_LRCK, AUDIO_I2S_SPK_GPIO_DOUT,
            AUDIO_I2S_MIC_GPIO_SCK, AUDIO_I2S_MIC_GPIO_WS, AUDIO_I2S_MIC_GPIO_DIN);
        static bool input_gain_configured = false;
        if (!input_gain_configured) {
            // Rekaman INMP441 masih menyisakan ruang puncak sekitar 12 dB.
            // Penguatan dua kali menaikkan kejelasan suara tanpa memotong
            // puncak percakapan normal.
            audio_codec.SetInputGain(2.0f);
            input_gain_configured = true;
        }
#else
        static NoAudioCodecDuplex audio_codec(AUDIO_INPUT_SAMPLE_RATE, AUDIO_OUTPUT_SAMPLE_RATE,
            AUDIO_I2S_GPIO_BCLK, AUDIO_I2S_GPIO_WS, AUDIO_I2S_GPIO_DOUT, AUDIO_I2S_GPIO_DIN);
#endif
        return &audio_codec;
    }

    virtual Display* GetDisplay() override {
        return display_;
    }

    virtual bool GetBatteryLevel(int& level, bool& charging, bool& discharging) override {
        // Pembacaan baterai dimatikan karena GPIO2 dipakai foil idung.
        (void)level;
        (void)charging;
        (void)discharging;
        return false;
    }
};

DECLARE_BOARD(CompactWifiBoard);
