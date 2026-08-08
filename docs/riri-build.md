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

## Kata Pemicu di Tempat Ramai

Deteksi kata pemicu tidak hanya menilai kemiripan bunyi, tetapi juga
kenyaringannya.

Alasannya, kemiripan saja tidak cukup. Pada pengukuran di perangkat, kata
pemicu asli bernilai 0,139 sampai 0,257 sedangkan yang palsu bernilai 0,162
sampai 0,207. Rentangnya bertumpang tindih seluruhnya, sehingga tidak ada
ambang kemiripan yang dapat memisahkan keduanya. Menaikkan ambang hanya akan
ikut membuang panggilan yang benar.

Yang membedakan keduanya adalah jarak. Pengguna memanggil dari dekat,
sedangkan percakapan orang lain di ruangan ramai terdengar jauh lebih pelan.
Deteksi yang tenaganya di bawah `CONFIG_WAKE_WORD_MIN_RMS` ditolak, dan
alasannya dicatat di log:

```text
I CustomWakeWord: Kata pemicu ditolak karena terlalu pelan, tenaga 412 di bawah ambang 1200
```

Nilai bawaannya 1200. Rekaman uji ucapan pengguna bernilai sekitar 3300, jadi
jaraknya lebar. Naikkan bila perangkat masih terbangun sendiri di tempat ramai,
turunkan bila panggilan yang benar mulai terabaikan, dan isi nol untuk
mematikan penyaringan ini sepenuhnya.

Selain itu, deteksi kata pemicu dimatikan sejak lagu **mulai disiapkan**, bukan
sejak lagu mulai berbunyi. Menyiapkan lagu baru memakan belasan sampai puluhan
detik, dan selama itu perangkat berstatus menganggur. Di ruangan ramai, satu
kata pemicu palsu pada rentang itu membatalkan permintaan lagu yang sudah
berjalan, sehingga lagunya tidak pernah jadi diputar.

## Jaringan yang Padat

Penyangga penerimaan Wi-Fi dinaikkan dari nilai bawaan proyek hulu.

| Pengaturan | Semula | Sekarang | Bawaan ESP-IDF |
|---|---|---|---|
| `ESP_WIFI_STATIC_RX_BUFFER_NUM` | 3 | 10 | 10 |
| `ESP_WIFI_DYNAMIC_RX_BUFFER_NUM` | 6 | 32 | 32 |
| `ESP_WIFI_RX_BA_WIN` | 3 | 10 | 6 |
| `LWIP_TCPIP_RECVMBOX_SIZE` | 16 | 32 | 32 |

Nilai lamanya disalin dari papan ESP32-C3 yang memorinya jauh lebih kecil, lalu
ikut terpakai di ESP32-S3 yang punya PSRAM delapan megabyte. Komentar aslinya
menyebutkan hal itu apa adanya.

Akibatnya penyangga penerimaan sangat tipis. Pada Wi-Fi kampus yang padat, log
perangkat pernah menunjukkan sembilan puluh lima paket berturut-turut lenyap
sekaligus, yaitu lebih dari lima detik suara yang hilang begitu saja:

```text
W MQTT: Melewati paket audio 102 sampai 196 yang tidak datang
```

`CONFIG_SPIRAM_TRY_ALLOCATE_WIFI_LWIP` ikut dinyalakan sehingga penyangga
Wi-Fi dan LWIP diarahkan ke PSRAM. Tanpa itu, menaikkan jumlah penyangga akan
menggerus RAM internal yang memang terbatas.

Jendela block-ack menentukan berapa banyak paket boleh datang tidak berurutan
sebelum dianggap hilang. Nilai tiga terlalu sempit untuk jaringan yang sibuk,
tempat paket rutin datang tertukar urutannya.

## Server Tidak Pernah Tertidur

Perangkat mengirim denyut pemanasan ke server setiap empat menit selama
menganggur.

Cloud Run menghentikan layanan sepenuhnya setelah beberapa menit menganggur
supaya tidak ada biaya berjalan. Konsekuensinya, permintaan pertama sesudah
lama diam harus menunggu instance dibangunkan, dan penantian itu ditanggung
pengguna: pertanyaan kampus dijawab dari data lokal saja, dan permintaan lagu
gagal.

Denyutnya hanya dikirim saat perangkat menganggur, sehingga tidak pernah
berebut jaringan dengan aliran suara yang sedang berjalan. Satu denyut hanya
berupa satu permintaan pemeriksaan kesehatan yang sangat ringan.
