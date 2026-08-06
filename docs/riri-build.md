# Panduan Build dan Flash Riri Boneka

Proyek ini memakai **ESP-IDF 6.0.2**, berbeda dengan versi sebelumnya yang
memakai ESP-IDF 5.5. Perintahnya ikut berubah, terutama karena lingkungan
ESP-IDF sekarang berada di folder rumah pengguna.

## Sekali Saja: Pasang Perkakas ESP-IDF

Bila `C:\Users\riana\.espressif` masih kosong, perkakasnya belum terpasang:

```powershell
cd C:\Users\riana\esp-idf-v6.0.2
.\install.ps1 esp32s3
```

Perintah ini mengunduh pengompilasi Xtensa, OpenOCD, dan lingkungan Python.
Ukurannya beberapa ratus megabita, jadi jalankan sekali saja dan tunggu sampai
muncul tulisan selesai.

## Setiap Membuka Terminal Baru

```powershell
cd "D:\Riri-Boneka-V0.1\Riri-Boneka-v1.1-main"
. "C:\Users\riana\esp-idf-v6.0.2\export.ps1"
```

Titik dan spasi di depan alamat berkas itu wajib. Tanpa titik tersebut, variabel
lingkungan hanya berlaku di dalam skrip dan hilang begitu skrip selesai.

## Build

Pertama kali saja, tentukan sasaran papannya:

```powershell
idf.py set-target esp32s3
```

Lalu bangun firmware:

```powershell
idf.py build
```

Bila ingin mengubah pengaturan seperti alamat server Riri:

```powershell
idf.py menuconfig
```

Menu khusus boneka ada di **Riri Boneka**, berisi alamat server, kunci rahasia,
saklar RAG kampus, saklar pencarian web, dan saklar pemutar musik.

## Flash dan Monitor

```powershell
idf.py -p COM7 -b 460800 flash
idf.py -p COM7 monitor
```

Ganti `COM7` bila papan terbaca di porta lain. Keluar dari monitor dengan
`Ctrl` lalu `]`.

Bila pengunggahan sering gagal, turunkan kecepatannya:

```powershell
idf.py -p COM7 -b 115200 flash
```

## Membersihkan Hasil Build

Ganti sasaran atau pindah versi ESP-IDF memerlukan pembersihan penuh:

```powershell
idf.py fullclean
idf.py set-target esp32s3
idf.py build
```

## Yang Harus Terlihat di Monitor

Setelah menyala, urutan berikut menandakan semuanya berjalan:

```text
I RiriTouch: Foil kepala aktif di GPIO1 (kanal 1) -> Tidur.ogg
I RiriTouch: 8 dari 8 titik sentuh berhasil dikonfigurasi
I RiriTouch: Perangkat tenang setelah ... ms, menunggu jeda stabil
I RiriTouch: Kalibrasi akhir selesai, sensor sentuh aktif dalam 2000 ms
I RiriTouch: Sensor sentuh siap menerima sentuhan
I CampusMCP: Campus + Web Search + Music tools registered
```

Jangan menyentuh foil sebelum baris **Sensor sentuh siap menerima sentuhan**
muncul. Sebelum itu foil memang sengaja belum aktif karena garis dasarnya
sedang diambil.

## Masalah yang Sering Muncul

| Gejala | Penyebab dan tindakan |
|---|---|
| `idf.py` tidak dikenali | `export.ps1` belum dijalankan di terminal itu |
| Gagal saat `set-target` | Jalankan `idf.py fullclean` lebih dulu |
| Pengompilasi tidak ditemukan | `install.ps1 esp32s3` belum selesai dijalankan |
| Flash gagal di tengah | Turunkan baud ke `115200`, periksa kabel USB data |
| OLED gagal init di log | Wajar bila layar memang tidak dipasang; boneka tetap jalan dan status dibaca dari lampu RGB |
