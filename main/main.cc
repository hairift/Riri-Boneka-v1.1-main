#include <esp_log.h>
#include <esp_err.h>
#include <nvs.h>
#include <nvs_flash.h>
#include <driver/gpio.h>
#include <esp_event.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "application.h"

#define TAG "main"

// Versi proyek ini, terpisah dari PROJECT_VER.
//
// PROJECT_VER sengaja dibiarkan mengikuti versi firmware hulu karena angka itu
// dipakai server OTA untuk membandingkan versi. Menurunkannya membuat server
// menganggap ada versi yang lebih baru, dan pembaruan berjalan otomatis saat
// boot, sehingga seluruh kustomisasi di sini akan tertimpa firmware hulu.
#define VERSI_RIRI "V1.1"

extern "C" void app_main(void)
{
    // Initialize NVS flash for WiFi configuration
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "Erasing NVS flash to fix corruption");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "Riri Boneka AI %s", VERSI_RIRI);

    // Initialize and run the application
    auto& app = Application::GetInstance();
    app.Initialize();
    app.Run();  // This function runs the main event loop and never returns
}
