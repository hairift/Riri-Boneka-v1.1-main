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

// ---------------------------------------------------------------------------
// Dua LED RGB indikator tambahan
//
// Semua pin di bawah ini benar-benar bebas pada ESP32-S3 dengan flash dan PSRAM
// oktal. GPIO26 sampai GPIO37 dipakai flash dan PSRAM, GPIO45 dan GPIO46 adalah
// pin strapping, sedangkan GPIO19 dan GPIO20 dipakai USB bawaan, jadi semuanya
// dihindari.
//
// LED baterai: merah hampir habis, kuning menengah, hijau penuh.
// LED status : biru saat Wi-Fi tersambung, hijau berkedip saat menyala, merah
//              saat jaringan bermasalah, berganti warna saat lagu diputar.
// ---------------------------------------------------------------------------
#define RIRI_RGB_BATTERY_R_GPIO GPIO_NUM_17
#define RIRI_RGB_BATTERY_G_GPIO GPIO_NUM_21
#define RIRI_RGB_BATTERY_B_GPIO GPIO_NUM_38
#define RIRI_RGB_STATUS_R_GPIO  GPIO_NUM_39
#define RIRI_RGB_STATUS_G_GPIO  GPIO_NUM_40
#define RIRI_RGB_STATUS_B_GPIO  GPIO_NUM_41

// Isi 1 bila memakai LED RGB anoda bersama, yaitu kaki panjangnya ke 3V3.
// Untuk katoda bersama, yaitu kaki panjangnya ke GND, biarkan 0.
#define RIRI_RGB_COMMON_ANODE 0

// Pembacaan baterai lewat ADC dimatikan karena GPIO2 dipakai foil idung.
//
// Seluruh kanal ADC1 pada ESP32-S3 berada di GPIO1 sampai GPIO10, dan pada
// boneka ini kesepuluh pin itu sudah terpakai foil sentuh, mikrofon, pengeras
// suara, dan OLED. ADC2 tidak dapat dipakai bersamaan dengan Wi-Fi.
//
// Akibatnya LED baterai tidak punya sumber data dan sengaja dibiarkan padam,
// bukan dibuat hijau, supaya tidak ada yang mengira baterainya penuh. Untuk
// menyalakannya, satu foil harus direlakan: ubah RIRI_TOUCH_NOSE_GPIO menjadi
// GPIO_NUM_NC lalu isi BATTERY_ADC_GPIO dengan GPIO_NUM_2.
#define BATTERY_ADC_GPIO      GPIO_NUM_NC
#define BATTERY_ADC_UNIT      ADC_UNIT_1
#define BATTERY_ADC_CHANNEL   ADC_CHANNEL_1
#define BATTERY_CHARGING_GPIO GPIO_NUM_NC
#define BATTERY_R_UPPER       100000.0f
#define BATTERY_R_LOWER       100000.0f

#endif // _BOARD_CONFIG_H_
