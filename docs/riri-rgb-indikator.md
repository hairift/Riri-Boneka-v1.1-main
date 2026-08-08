# Dua LED RGB Indikator Riri

Selain LED bawaan papan di `GPIO48`, Riri memakai dua LED RGB empat kaki
sebagai penanda yang tidak terlihat dari LED bawaan.

| LED | Fungsi |
|---|---|
| LED baterai | Sisa daya baterai |
| LED status | Jaringan, kondisi program, dan musik |

LED bawaan `GPIO48` tetap menjadi penanda status percakapan seperti sebelumnya
dan tidak diubah sama sekali.

## Pin

| LED | Warna | GPIO |
|---|---|---|
| Baterai | Merah | `GPIO17` |
| Baterai | Hijau | `GPIO21` |
| Baterai | Biru | `GPIO38` |
| Status | Merah | `GPIO39` |
| Status | Hijau | `GPIO40` |
| Status | Biru | `GPIO41` |

Semua pin di atas benar-benar bebas pada ESP32-S3 dengan flash dan PSRAM oktal.
Yang dihindari:

- `GPIO26` sampai `GPIO37` dipakai flash dan PSRAM oktal. Memakainya membuat
  papan gagal boot.
- `GPIO45` dan `GPIO46` adalah pin strapping yang menentukan tegangan flash dan
  mode boot.
- `GPIO19` dan `GPIO20` dipakai USB bawaan.
- `GPIO0` sampai `GPIO18` dan `GPIO48` sudah terpakai foil sentuh, mikrofon,
  pengeras suara, OLED, lampu uji, dan LED bawaan.

## Resistor

Untuk LED RGB katoda bersama pada tegangan 3,3 volt:

| Warna | Tegangan maju | Resistor | Arus |
|---|---|---|---|
| Merah | sekitar 2,0 V | **220 ohm** | sekitar 6 mA |
| Hijau | sekitar 3,0 V | **150 ohm** | sekitar 2 mA |
| Biru | sekitar 3,0 V | **150 ohm** | sekitar 2 mA |

**Jumlah yang dibutuhkan: enam resistor**, yaitu dua buah 220 ohm dan empat
buah 150 ohm, karena setiap LED memerlukan satu resistor per warna.

Merah diberi resistor lebih besar justru karena tegangan majunya paling rendah,
sehingga tanpa itu arusnya paling besar dan warnanya mendominasi campuran.
Hijau dan biru bertegangan maju sekitar 3,0 volt, hampir menyentuh 3,3 volt
milik ESP32-S3, jadi arusnya memang kecil dan resistornya tidak boleh besar.

Batas arus satu pin ESP32-S3 adalah 40 mA, dengan 20 mA sebagai angka aman.
Nilai di atas jauh di bawahnya, dan enam pin sekaligus hanya menarik sekitar
20 mA total.

## Cara Menyambung

LED RGB empat kaki punya satu kaki bersama yang lebih panjang.

**Katoda bersama**, yang paling umum dijual:

```text
GPIO17 --[220 ohm]--> kaki merah
GPIO21 --[150 ohm]--> kaki hijau
GPIO38 --[150 ohm]--> kaki biru
                      kaki panjang --> GND
```

Pola yang sama berlaku untuk LED status dengan `GPIO39`, `GPIO40`, dan
`GPIO41`.

**Anoda bersama**, yaitu kaki panjangnya ke 3V3, juga didukung. Ubah satu baris
di `main/boards/bread-compact-wifi/config.h`:

```c
#define RIRI_RGB_COMMON_ANODE 1
```

Tanpa itu, LED anoda bersama akan menyala terbalik: padam saat seharusnya
menyala.

Cara membedakannya dengan multimeter pada mode dioda: sentuh kaki panjang
dengan probe merah lalu kaki lain dengan probe hitam. Bila menyala, itu anoda
bersama.

## Arti Warna

### LED baterai

| Warna | Arti |
|---|---|
| Hijau | 60 persen ke atas |
| Kuning | 25 sampai 59 persen |
| Merah | di bawah 25 persen |
| Berdenyut | sedang diisi |
| Padam | pembacaan baterai tidak tersedia |

### LED status

| Warna | Arti |
|---|---|
| Hijau berkedip | program sedang menyala, jaringan belum diketahui |
| Biru berkedip | sedang memindai atau menyambung Wi-Fi |
| Biru tenang | Wi-Fi tersambung, perangkat menganggur |
| Biru terang | Wi-Fi tersambung, sedang melayani percakapan |
| Merah berkedip | jaringan bermasalah, tersendat, atau terputus |
| Oranye berkedip | mode konfigurasi Wi-Fi lewat hotspot |
| Berganti warna | lagu sedang diputar |

## Catatan Penting tentang LED Baterai

Pada boneka ini, LED baterai akan **padam** karena tidak ada sumber datanya.

Seluruh kanal ADC1 pada ESP32-S3 berada di `GPIO1` sampai `GPIO10`, dan
kesepuluh pin itu sudah terpakai delapan foil sentuh, mikrofon, pengeras suara,
serta OLED. ADC2 secara teknis ada di `GPIO11` sampai `GPIO20`, tetapi tidak
dapat dipakai bersamaan dengan Wi-Fi, sehingga pembacaannya akan gagal terus
selama perangkat daring.

Lampunya sengaja dibiarkan padam, bukan dibuat hijau, supaya tidak ada yang
mengira baterainya penuh padahal tidak ada yang diukur.

Untuk menyalakannya, satu foil harus direlakan. Foil idung adalah pilihan yang
paling ringan kehilangannya:

```c
// main/boards/bread-compact-wifi/config.h
#define RIRI_TOUCH_NOSE_GPIO  GPIO_NUM_NC   // dulunya GPIO_NUM_2
#define BATTERY_ADC_GPIO      GPIO_NUM_2    // dulunya GPIO_NUM_NC
```

Lalu pasang pembagi tegangan seperti pada Mr Great: resistor 100 kilo ohm dari
sisi baterai ke titik ADC, resistor 100 kilo ohm dari titik ADC ke GND, dan
kapasitor 100 nano farad dari titik ADC ke GND. Ambil masukannya dari sisi
baterai atau keluaran TP4056, bukan dari keluaran boost 5 volt, karena yang
terbaca akan menjadi tegangan regulator, bukan tegangan baterai.

Setelah itu foil idung tidak lagi berfungsi dan `Idung.ogg` tidak akan pernah
berbunyi.

## Rincian Teknis

Keenam kanal digerakkan PWM lewat LEDC pada 2 kHz dengan resolusi 8 bit,
memakai `LEDC_TIMER_2` dan kanal 0 sampai 5. LEDC menyediakan delapan kanal
kecepatan rendah pada ESP32-S3, jadi masih tersisa dua.

Tugas pembaruannya berjalan pada prioritas 1, jauh di bawah `opus_codec`,
`audio_output`, dan `audio_input`, sehingga tidak pernah merebut waktu prosesor
dari jalur audio waktu nyata. Warnanya diperbarui setiap 40 milidetik.

Tingkat baterai hanya dibaca setiap lima detik karena pembacaan ADC tidak
murah, dan nilainya disimpan supaya pembacaan yang gagal sesekali tidak membuat
lampunya berkedip antara warna terakhir dan padam.
