#pragma once

#include <driver/gpio.h>
#include <driver/touch_pad.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <cstdint>
#include <string_view>

enum class RiriTouchAction {
    PLAY_ONLY,
    WAKE_AI,
};

struct RiriTouchPoint {
    const char* name;
    gpio_num_t gpio;
    touch_pad_t touch_pad;
    const char* sound_asset;
    RiriTouchAction action;
};

class RiriTouchController {
public:
    static RiriTouchController& GetInstance() {
        static RiriTouchController instance;
        return instance;
    }

    void Initialize();

private:
    RiriTouchController() = default;
    ~RiriTouchController() = default;

    static void TouchTask(void* arg);
    void Run();
    bool ConfigureTouchPoint(RiriTouchPoint& point);
    void CalibrateAllPoints();
    int RisePermille(const RiriTouchPoint& point, uint32_t raw_value) const;
    bool IsTouched(RiriTouchPoint& point, uint32_t raw_value);
    bool IsReleased(RiriTouchPoint& point, uint32_t raw_value);
    void UpdateIdleBaseline(RiriTouchPoint& point, uint32_t raw_value);
    void HandleTouch(const RiriTouchPoint& point);
    bool PlayTouchSound(const char* asset_name);

    bool initialized_ = false;
    bool touch_driver_ready_ = false;
    TaskHandle_t task_handle_ = nullptr;
    bool enabled_[TOUCH_PAD_MAX] = {};
    uint32_t baselines_[TOUCH_PAD_MAX] = {};
    uint8_t touch_counts_[TOUCH_PAD_MAX] = {};
    uint8_t release_counts_[TOUCH_PAD_MAX] = {};
    uint16_t stuck_counts_[TOUCH_PAD_MAX] = {};
    bool pressed_[TOUCH_PAD_MAX] = {};
    TickType_t last_trigger_ticks_[TOUCH_PAD_MAX] = {};
    TickType_t last_any_wake_ticks_ = 0;
};
