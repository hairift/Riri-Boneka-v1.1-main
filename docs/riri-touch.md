# Sensor Sentuh Foil Tembaga Riri

Delapan bagian tubuh boneka dilapisi foil tembaga yang tersambung ke pin sentuh
kapasitif ESP32-S3. Menyentuh bagian tubuh membuat Riri memberi reaksi suara,
dan khusus tangan juga membangunkan AI.

## Peta Pin dan Reaksi

| Bagian tubuh | GPIO | Kanal sentuh | Suara | Reaksi tambahan |
|---|---:|---:|---|---|
| Kepala | `GPIO1` | 1 | `Tidur.ogg` | — |
| Idung | `GPIO2` | 2 | `Idung.ogg` | — |
| Perut | `GPIO3` | 3 | `Perut.ogg` | — |
| Ekor | `GPIO8` | 8 | `Kentut.ogg` | — |
| Tangan kanan | `GPIO11` | 11 | `Aktif.ogg` | Aktifkan AI |
| Tangan kiri | `GPIO12` | 12 | `Aktif.ogg` | Aktifkan AI |
| Kaki kanan | `GPIO13` | 13 | `Ketawa.ogg` | — |
| Kaki kiri | `GPIO14` | 14 | `Ketawa.ogg` | — |

Pada ESP32-S3, kanal sentuh nomor N selalu berada di GPIO nomor N. Firmware
memeriksa kecocokan ini saat menyala dan menolak memasang foil yang pemetaannya
salah, sehingga kesalahan kabel langsung terlihat di log.

### Kenapa idung memakai GPIO2 dan bukan GPIO5

`GPIO5` sudah dipakai sebagai jalur **SCK mikrofon INMP441**. Satu pin hanya
boleh punya satu fungsi. Bila foil idung dipindah ke `GPIO5`, mikrofon berhenti
bekerja dan AI tidak dapat mendengar sama sekali. Karena itu foil idung tetap
di `GPIO2`.

## Cara Kerja

Firmware membaca kenaikan nilai kapasitif tiap foil. Pada ESP32-S3 nilai mentah
**bertambah** ketika kapasitansi elektroda bertambah karena disentuh.

Ambang yang dipakai:

| Parameter | Nilai |
|---|---|
| Ambang sentuh | kenaikan 22 persen terhadap garis dasar |
| Ambang lepas | kenaikan 10 persen, memberi histeresis |
| Interval sampel | 50 ms |
| Konfirmasi sentuh | 6 sampel berturut-turut, sekitar 300 ms |
| Konfirmasi lepas | 8 sampel berturut-turut, sekitar 400 ms |
| Jeda antar pemicu | 1,8 detik per foil |

Nilai ini adalah setelan yang terbukti stabil di perangkat. Percobaan
menurunkan ambang ke 7 lalu 11 persen membuat foil bereaksi sendiri tanpa
disentuh, karena pergeseran listrik saat radio Wi-Fi dan penguat audio menyala
sudah cukup besar untuk melewati ambang serendah itu.

## Kalibrasi

Kalibrasi berjalan **dua tahap**.

Tahap pertama berjalan segera setelah papan menyala, hanya sebagai nilai
sementara.

Tahap kedua adalah yang benar-benar dipakai. Firmware menunggu sampai perangkat
masuk status siaga atau mode konfigurasi Wi-Fi, menunggu 1,5 detik lagi supaya
lonjakan terakhir mereda, baru mengambil garis dasar. Setelah itu masih ada
jeda siaga 2 detik sebelum sentuhan pertama boleh memicu.

Seluruh foil dikalibrasi **bergiliran dalam satu putaran**, bukan satu foil
sampai selesai baru pindah ke foil berikutnya. Cara lama memakan lima detik
lebih, dan radio Wi-Fi menyala di tengah proses itu, sehingga foil yang
dikalibrasi paling awal menyimpan garis dasar dari kondisi listrik yang sudah
tidak berlaku lagi. Begitu radio aktif, nilainya melompat dan langsung terbaca
sebagai sentuhan padahal tidak ada yang menyentuh.

## Pencegahan Tabrakan

Setiap putaran membaca seluruh delapan foil lebih dulu, baru menentukan mana
yang kenaikannya paling besar. Hanya foil pemenang yang boleh maju menuju
pemicu; foil lain yang ikut terangkat dikembalikan ke nol.

Bila dua foil sama-sama melewati ambang, foil pemenang masih harus unggul
minimal 1,5 persen dari foil kedua. Bila keduanya naik hampir sama tinggi, itu
tanda kopling antar kabel dan tidak ada yang dipicu. Ketika hanya satu foil yang
melewati ambang, aturan selisih ini tidak berlaku sehingga sentuhan tunggal
tetap langsung sah.

## Jaring Pengaman

- **Zona mati.** Bila nilai foil menetap di antara ambang lepas dan ambang
  sentuh selama sekitar 3 detik, garis dasarnya ditarik ulang. Tanpa ini foil
  tersebut diam selamanya karena tidak pernah memicu maupun memperbarui garis
  dasarnya.
- **Terkunci tertekan.** Bila foil bertahan dalam keadaan tertekan sekitar 30
  detik, firmware melepaskannya paksa dan menyetel ulang garis dasar. Keadaan
  seperti itu hampir pasti pergeseran lingkungan permanen, bukan jari yang
  menempel selama itu.
- **Kanal berisik.** Sebaran sampel kalibrasi dicatat di log. Foil dengan
  sebaran lebar diberi peringatan supaya kabelnya dapat diperbaiki.

## Reaksi Tangan

Menyentuh tangan memutar `Aktif.ogg` lalu memanggil
`Application::WakeUpFromTouch()`. Fungsi ini memakai mode dengar bawaan
sehingga server ikut mendeteksi kapan pengguna berhenti bicara.

Jalur `StartListening()` sengaja **tidak** dipakai karena memasang mode
manual stop: perangkat akan menunggu tombol dilepas, padahal foil tembaga tidak
punya kejadian lepas seperti itu, sehingga AI mendengar terus tanpa pernah
menjawab.

Bila Wi-Fi belum tersambung atau papan masih dalam mode konfigurasi, sentuhan
tangan hanya memutar `Aktif.ogg` tanpa mengaktifkan AI.

Sentuhan pada bagian mana pun juga menghentikan musik yang sedang diputar,
supaya suara reaksi tidak bertumpuk dengan lagu.

## Aset Suara

Berkas suara reaksi berada di `main/assets/tambahan/`:

`Aktif.ogg`, `Idung.ogg`, `Kentut.ogg`, `Ketawa.ogg`, `Perut.ogg`, `Tidur.ogg`

Folder ini ikut dimasukkan ke `generated_assets.bin` saat
`CONFIG_FLASH_DEFAULT_ASSETS=y`.

Sebelum memutar suara reaksi, firmware menaikkan volume keluaran ke 100 agar
terdengar jelas. Pengaturan itu hanya ditulis saat nilainya memang berbeda,
karena menyimpan volume berarti menulis ke NVS. Tanpa penjagaan ini setiap
sentuhan akan menulis ulang flash dan memperpendek umur perangkat.

Suara reaksi juga tidak diputar sebelum layanan audio selesai disiapkan. Sensor
sentuh dibuat dari konstruktor board yang berjalan lebih dulu, sehingga
sentuhan pada celah waktu itu dulunya memakai pointer codec yang masih kosong.

## Pemasangan Kabel

- Pakai foil tembaga sebagai elektroda sentuh.
- Buat kabel sensor sependek mungkin agar derau tidak besar.
- Jangan dekatkan kabel sensor dengan jalur pengeras suara, penaik tegangan,
  atau kabel kipas.
- Cukup sambungkan foil langsung ke pin sentuh, tanpa resistor tambahan.
- Jangan menyentuh foil sampai baris `Sensor sentuh siap menerima sentuhan`
  muncul di monitor.

## Pemecahan Masalah

| Gejala di log `RiriTouch` | Penyebab dan tindakan |
|---|---|
| `... salah petakan: kanal sentuh N tidak berada di GPIO M` | Nomor kanal dan pin tidak cocok. Periksa `config.h`. |
| `... berisik: baseline=... sebaran=...` | Kabel foil terlalu panjang atau dekat jalur daya. Perpendek dan jauhkan. |
| `... menetap di zona mati, garis dasar disetel ulang` | Pergeseran lingkungan sedang diserap. Wajar bila sesekali. |
| `... tertekan terlalu lama, garis dasar disetel ulang` | Foil menyentuh badan boneka terus-menerus, atau ada kebocoran arus. |
| Foil bereaksi tanpa disentuh | Pastikan menunggu baris `Sensor sentuh siap menerima sentuhan`. Bila tetap terjadi, kirim baris `baseline stabil=... sebaran=...` untuk melihat kanal mana yang bermasalah. |
| `... tidak mendapat sampel kalibrasi` | Foil tidak tersambung atau kabelnya putus. |
