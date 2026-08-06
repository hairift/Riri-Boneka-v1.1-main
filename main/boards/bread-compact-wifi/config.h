#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

#include <driver/gpio.h>

#define AUDIO_INPUT_SAMPLE_RATE  16000
#define AUDIO_OUTPUT_SAMPLE_RATE 24000

// I2S simpleks: mikrofon dan pengeras suara memakai kanal terpisah.
#define AUDIO_I2S_METHOD_SIMPLEX

#ifdef AUDIO_I2S_METHOD_SIMPLEX

// Mikrofon INMP441
#define AUDIO_I2S_MIC_GPIO_WS   GPIO_NUM_4
#define AUDIO_I2S_MIC_GPIO_SCK  GPIO_NUM_5
#define AUDIO_I2S_MIC_GPIO_DIN  GPIO_NUM_6

// Penguat MAX98357
#define AUDIO_I2S_SPK_GPIO_DOUT GPIO_NUM_7
#define AUDIO_I2S_SPK_GPIO_BCLK GPIO_NUM_15
#define AUDIO_I2S_SPK_GPIO_LRCK GPIO_NUM_16

#else

#define AUDIO_I2S_GPIO_WS   GPIO_NUM_4
#define AUDIO_I2S_GPIO_BCLK GPIO_NUM_5
#define AUDIO_I2S_GPIO_DIN  GPIO_NUM_6
#define AUDIO_I2S_GPIO_DOUT GPIO_NUM_7

#endif


#define BUILTIN_LED_GPIO        GPIO_NUM_48
#define BOOT_BUTTON_GPIO        GPIO_NUM_0

// Tombol tambahan tidak dipasang pada boneka.
// GPIO1 sampai GPIO14 sudah dipakai foil sentuh dan jalur audio, jadi tidak
// boleh dipetakan ulang sebagai tombol.
#define TOUCH_BUTTON_GPIO       GPIO_NUM_NC
#define VOLUME_UP_BUTTON_GPIO   GPIO_NUM_NC
#define VOLUME_DOWN_BUTTON_GPIO GPIO_NUM_NC
#define ASR_BUTTON_GPIO         GPIO_NUM_NC

// OLED SSD1306 0,96 inci lewat I2C.
#define DISPLAY_SDA_PIN GPIO_NUM_10
#define DISPLAY_SCL_PIN GPIO_NUM_9
#define DISPLAY_WIDTH   128

#if CONFIG_OLED_SSD1306_128X32
#define DISPLAY_HEIGHT  32
#elif CONFIG_OLED_SSD1306_128X64
#define DISPLAY_HEIGHT  64
#elif CONFIG_OLED_SH1106_128X64
#define DISPLAY_HEIGHT  64
#define SH1106
#else
#error "OLED display type is not selected"
#endif

#define DISPLAY_MIRROR_X true
#define DISPLAY_MIRROR_Y true


// Uji MCP: kendalikan lampu.
#define LAMP_GPIO GPIO_NUM_18

// ---------------------------------------------------------------------------
// Sensor sentuh foil tembaga Riri
//
// Pada ESP32-S3 kanal sentuh N selalu berada di GPIO N, sehingga nomor pin di
// bawah ini sekaligus menentukan nomor kanalnya. Semua pin ini adalah pin
// touch-capable dan tidak bertabrakan dengan jalur audio maupun OLED.
// ---------------------------------------------------------------------------
#define RIRI_TOUCH_HEAD_GPIO        GPIO_NUM_1
#define RIRI_TOUCH_NOSE_GPIO        GPIO_NUM_2
#define RIRI_TOUCH_BELLY_GPIO       GPIO_NUM_3
#define RIRI_TOUCH_TAIL_GPIO        GPIO_NUM_8
#define RIRI_TOUCH_RIGHT_HAND_GPIO  GPIO_NUM_11
#define RIRI_TOUCH_LEFT_HAND_GPIO   GPIO_NUM_12
#define RIRI_TOUCH_RIGHT_FOOT_GPIO  GPIO_NUM_13
#define RIRI_TOUCH_LEFT_FOOT_GPIO   GPIO_NUM_14

// Pembacaan baterai lewat ADC dimatikan karena GPIO2 dipakai foil idung.
#define BATTERY_ADC_GPIO      GPIO_NUM_NC
#define BATTERY_ADC_UNIT      ADC_UNIT_1
#define BATTERY_ADC_CHANNEL   ADC_CHANNEL_1
#define BATTERY_CHARGING_GPIO GPIO_NUM_NC
#define BATTERY_R_UPPER       100000.0f
#define BATTERY_R_LOWER       100000.0f

#endif // _BOARD_CONFIG_H_
