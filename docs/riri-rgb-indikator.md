# LED Status dan Penunjuk Baterai Riri

Riri memakai tiga penanda visual yang saling melengkapi.

| Penanda | Sumber | Menampilkan |
|---|---|---|
| LED bawaan papan `GPIO48` | firmware | Status percakapan |
| LED RGB status | firmware | Jaringan, kondisi program, dan musik |
| Modul penunjuk kapasitas | perangkat keras sendiri | Sisa daya baterai |

Sisa daya baterai sengaja tidak ditangani firmware. Modul penunjuk kapasitas
mengukur tegangan sel secara langsung, dan itu justru lebih tepat: seluruh
kanal ADC1 pada ESP32-S3 berada di `GPIO1` sampai `GPIO10`, dan kesepuluh pin
itu sudah terpakai delapan foil sentuh, mikrofon, pengeras suara, serta OLED.
ADC2 secara teknis ada, tetapi tidak dapat dipakai bersamaan dengan Wi-Fi.

## Modul Penunjuk Kapasitas Baterai

### Disambungkan ke mana

**Ke sisi baterai pada Powerbank Module, bukan ke ESP32 dan bukan ke keluaran
5 volt.**

```text
Sel lithium 18650
   +  ────┬──────────────────► B+  Powerbank Module
          │
          └──────────────────► +   Modul penunjuk kapasitas

   -  ────┬──────────────────► B-  Powerbank Module
          │
          └──────────────────► -   Modul penunjuk kapasitas
```

Modul penunjuk kapasitas dipasang **paralel dengan sel**, yaitu pada titik yang
sama dengan `B+` dan `B-` milik Powerbank Module.

### Kenapa tidak ke keluaran 5 volt

Keluaran USB Powerbank Module adalah 5 volt hasil penguatan, dan tegangan itu
dipertahankan tetap 5 volt oleh regulator sampai baterainya benar-benar hampir
habis. Modul penunjuk kapasitas bekerja dengan membaca tegangan sel, jadi bila
disambungkan ke sana ia akan selalu menunjukkan penuh dan baru anjlok mendadak
saat regulatornya menyerah.

Selain itu, modul penunjuk kapasitas lithium 1S dirancang untuk rentang 3,0
sampai 4,2 volt. Memberinya 5 volt berarti memberi tegangan di atas rentang
kerjanya.

### Kenapa tidak ke ESP32

ESP32-S3 pada boneka ini berjalan pada 3,3 volt hasil regulator internal
papannya, yang juga sudah tetap. Titik itu bukan tegangan baterai, sehingga
pembacaannya akan sama tidak berartinya dengan membaca keluaran 5 volt.

ESP32 juga tidak perlu dilibatkan sama sekali. Modul penunjuk kapasitas bekerja
sendiri dan tidak memerlukan pin data apa pun.

### Catatan pemasangan

- Pastikan modulnya bertipe **1S**, yaitu untuk satu sel lithium 3,7 volt.
  Modul 2S sampai 4S akan salah membaca.
- Sebagian modul menyala terus dan menarik arus siaga beberapa milliampere.
  Bila boneka disimpan lama tanpa dipakai, itu ikut menguras baterai. Modul
  bertombol yang hanya menyala saat ditekan lebih hemat.
- Powerbank Module tetap yang mengurus pengisian dan proteksi. Modul penunjuk
  kapasitas hanya membaca, tidak mengubah apa pun.
- Saat pengisian berlangsung, tegangan sel naik, jadi penunjuknya akan tampak
  lebih tinggi daripada kapasitas sebenarnya. Itu wajar untuk penunjuk berbasis
  tegangan dan bukan tanda kerusakan.

## LED RGB Status

### Pin

| Warna | GPIO |
|---|---|
| Merah | `GPIO39` |
| Hijau | `GPIO40` |
| Biru | `GPIO41` |

Ketiganya benar-benar bebas pada ESP32-S3 dengan flash dan PSRAM oktal. Yang
dihindari:

- `GPIO26` sampai `GPIO37` dipakai flash dan PSRAM oktal. Memakainya membuat
  papan gagal boot.
- `GPIO45` dan `GPIO46` adalah pin strapping yang menentukan tegangan flash dan
  mode boot.
- `GPIO19` dan `GPIO20` dipakai USB bawaan.
- `GPIO0` sampai `GPIO18` dan `GPIO48` sudah terpakai foil sentuh, mikrofon,
  pengeras suara, OLED, lampu uji, dan LED bawaan.

### Resistor

Untuk LED RGB katoda bersama pada tegangan 3,3 volt:

| Warna | Tegangan maju | Resistor | Arus |
|---|---|---|---|
| Merah | sekitar 2,0 V | **220 ohm** | sekitar 6 mA |
| Hijau | sekitar 3,0 V | **150 ohm** | sekitar 2 mA |
| Biru | sekitar 3,0 V | **150 ohm** | sekitar 2 mA |

**Jumlah yang dibutuhkan: tiga resistor**, yaitu satu buah 220 ohm dan dua buah
150 ohm.

Merah diberi resistor lebih besar justru karena tegangan majunya paling rendah,
sehingga tanpa itu arusnya paling besar dan warnanya mendominasi campuran.
Hijau dan biru bertegangan maju sekitar 3,0 volt, hampir menyentuh 3,3 volt
milik ESP32-S3, jadi arusnya memang kecil dan resistornya tidak boleh besar.

Batas arus satu pin ESP32-S3 adalah 40 mA, dengan 20 mA sebagai angka aman.
Nilai di atas jauh di bawahnya.

### Cara menyambung

LED RGB empat kaki punya satu kaki bersama yang lebih panjang.

**Katoda bersama**, yang paling umum dijual:

```text
GPIO39 --[220 ohm]--> kaki merah
GPIO40 --[150 ohm]--> kaki hijau
GPIO41 --[150 ohm]--> kaki biru
                      kaki panjang --> GND
```

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

### Arti warna

| Warna | Arti |
|---|---|
| Hijau berkedip | program sedang menyala, jaringan belum diketahui |
| Biru berkedip | sedang memindai atau menyambung Wi-Fi |
| Biru tenang | Wi-Fi tersambung, perangkat menganggur |
| Biru terang | Wi-Fi tersambung, sedang melayani percakapan |
| Merah berkedip | jaringan bermasalah, tersendat, atau terputus |
| Oranye berkedip | mode konfigurasi Wi-Fi lewat hotspot |
| Berganti warna | lagu sedang diputar |

## Rincian Teknis

Ketiga kanal digerakkan PWM lewat LEDC pada 2 kHz dengan resolusi 8 bit,
memakai `LEDC_TIMER_2` dan kanal 0 sampai 2. LEDC menyediakan delapan kanal
kecepatan rendah pada ESP32-S3, jadi masih tersisa lima.

Tugas pembaruannya berjalan pada prioritas 1, jauh di bawah `opus_codec`,
`audio_output`, dan `audio_input`, sehingga tidak pernah merebut waktu prosesor
dari jalur audio waktu nyata. Warnanya diperbarui setiap 40 milidetik.

Baris berikut muncul di monitor saat LED berhasil disiapkan:

```text
I RiriRgb: LED status siap di GPIO39, GPIO40, GPIO41
```
