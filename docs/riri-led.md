# Lampu Indikator RGB Riri

Boneka ini dirancang agar tetap dapat dipakai tanpa layar. LED RGB WS2812 bawaan
papan pada `GPIO48` menjadi penanda utama kondisi perangkat.

## Peta Warna

| Warna dan pola | Arti |
|---|---|
| Biru berkedip cepat | Sedang menyala dan menyiapkan diri |
| Biru berkedip sedang | Sedang memindai atau menyambung Wi-Fi |
| Biru berkedip lambat | Mode konfigurasi Wi-Fi lewat hotspot bernama Riri |
| **Hijau redup menyala** | **Wi-Fi tersambung, siaga, siap diajak bicara** |
| **Kuning berkedip** | **Jaringan tidak stabil, sempat tersambung lalu putus** |
| **Merah berkedip cepat** | **Ada galat jaringan atau server** |
| Cyan menyala | Sedang menghubungi server |
| Putih menyala | Sedang mendengarkan; makin terang saat suara terdeteksi |
| Hijau terang menyala | AI sedang berbicara |
| **Ungu menyala** | **Sedang memutar musik** |
| Hijau berkedip cepat | Pembaruan firmware berlangsung |
| Hijau berkedip lambat | Proses aktivasi perangkat |

## Cara Membacanya

Tiga warna yang paling sering diperhatikan sehari-hari:

- **Hijau redup** berarti semua beres. Riri siap dipanggil dengan kata pemicu
  "hai hai" atau disentuh tangannya.
- **Kuning berkedip** berarti Wi-Fi sempat masuk lalu putus. Biasanya sinyal
  lemah atau router sedang sibuk. Riri akan menyambung ulang sendiri.
- **Merah berkedip** berarti ada galat jaringan atau server. Periksa koneksi
  internet dan alamat server di menuconfig.

Putih saat mendengarkan sengaja dibuat berubah terang mengikuti suara, sehingga
terlihat langsung apakah mikrofon benar-benar menangkap suara pengguna.

## Cara Kerja di Kode

Status jaringan disimpan terpisah dari status perangkat, memakai enum
`NetworkIndicator` di `main/application.h`. Setiap kejadian jaringan memanggil
`Application::SetNetworkIndicator()`, yang langsung memicu pembaruan lampu.

Pembaruan langsung ini diperlukan karena perubahan jaringan tidak selalu
mengubah status perangkat. Tanpa itu, lampu baru berubah saat ada kejadian lain,
sehingga pengguna tidak tahu Wi-Fi sedang bermasalah.

Warna musik dibaca dari `MusicPlayer::IsStreaming()`, yaitu penanda yang bernilai
benar hanya ketika lagu sudah benar-benar terdengar dari pengeras suara, bukan
saat lagu masih menunggu giliran.

Seluruh peta warna berada di `main/led/single_led.cc`.
