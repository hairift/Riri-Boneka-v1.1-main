# Pemutar Musik Riri

Riri dapat mencari dan memutar lagu lewat pengeras suara MAX98357. Ucapkan
misalnya "Riri, putarkan lagu Hati-Hati di Jalan dari Tulus", lalu AI memanggil
alat musik, firmware meminta lagu ke server Riri, dan lagunya diputar.

## Cara Memakai

- "Riri, putarkan lagu Dusk Till Dawn dari Zayn Malik"
- "Setel lagu Hati-Hati di Jalan Tulus"
- "Mainkan lagu Bruno Mars"
- "Berhenti", "stop", atau "matikan lagunya" untuk menghentikan

Menyentuh foil tangan, kaki, kepala, perut, idung, atau ekor juga langsung
menghentikan lagu.

## Alat MCP

| Alat | Parameter | Fungsi |
|---|---|---|
| `self.music.play` | `song`, `artist`, keduanya opsional | Cari lalu putar lagu |
| `self.music.stop` | tidak ada | Hentikan lagu |

Judul dan penyanyi dikirim terpisah karena hasil pencariannya jauh lebih tepat
dibanding mengirim satu kalimat panjang. Bila AI hanya mengirim judul, firmware
tetap mencoba memisahkan sendiri pola "penyanyi - judul" dan "judul oleh
penyanyi".

## Alur Lengkap

```text
Pengguna -> AI -> self.music.play
         -> GET <server>/music/resolve?song=...&artist=...
         -> server mencari lewat YouTube Data API
         -> server mentranskode ke MP3 mono 24 kHz
         -> {"title","artist","audio_url"}
         -> firmware mengunduh audio_url
         -> ring buffer 192 KB di PSRAM
         -> dekode MP3 -> antrean playback -> I2S -> MAX98357
```

## Kenapa Suaranya Tidak Patah-Patah

Tiga hal bekerja bersama.

**Server mentranskode ke format yang persis dibutuhkan perangkat.** Berkas
dikirim sebagai MP3 mono 24 kHz sekitar 96 kbps. Laju cuplik itu sama dengan
`AUDIO_OUTPUT_SAMPLE_RATE` codec boneka, sehingga ESP32 tidak perlu mencampur
stereo ke mono dan tidak perlu mengubah laju cuplik sama sekali.

**Kebutuhan jaringan turun jauh.** Dari sekitar 40 KB per detik untuk MP3 320
kbps stereo menjadi sekitar 12 KB per detik, sehingga penyangga 192 KB setara
belasan detik audio.

**Ada penjaga isi ulang penyangga.** Bila data menipis di bawah 6 KB, dekoder
berhenti sejenak dan menunggu sampai penyangga terisi 24 KB lagi. Antrean
playback yang masih berisi menutupi jeda itu. Tanpa penjaga ini dekoder terbangun
hanya untuk beberapa byte, mendorong sedikit PCM, lalu menunggu lagi, dan itulah
yang terdengar sebagai patah-patah.

Baris terakhir log memberi tahu apakah jaringan menjadi penyebab:

```text
I MusicPlayer: Pemutaran musik selesai, isi ulang penyangga 0 kali
```

Nilai nol berarti aliran lancar sepanjang lagu.

## Rincian Teknis

- Dekoder memakai `esp_audio_simple_dec` bertipe `MP3`.
- Tag ID3v2 di awal berkas dilepas sendiri oleh firmware, termasuk bila tag lebih
  besar daripada satu potongan penyangga.
- Tugas `music_net` dan `music_dec` berjalan pada prioritas 1, di bawah
  `opus_codec`, `audio_output`, dan `audio_input`, sehingga musik tidak pernah
  merebut CPU dari jalur audio waktu nyata.
- Pengalihan HTTP diikuti sampai empat lompatan.
- Pencarian dibatasi 10 detik supaya server MCP tidak menyerah menunggu.

## Aturan Interaksi

Musik hanya berbunyi saat perangkat berstatus menganggur. Kalau tidak, lagu akan
masuk ke mikrofon dan terkirim ke server sebagai suara pengguna.

Saat alat musik dipanggil, AI masih membacakan jawabannya. Pemutar musik
menunggu jawaban itu selesai, lalu memanggil `Application::EndConversation()`
untuk menutup sesi percakapan supaya perangkat kembali menganggur, baru lagu
dimulai.

Kesiapan jawaban diperiksa dengan `AudioService::WaitForPlaybackQueueEmpty()`,
bukan `IsIdle()`. `IsIdle()` ikut memeriksa antrean encode mikrofon yang praktis
tidak pernah kosong selama perangkat mendengarkan, sehingga syaratnya tidak akan
pernah terpenuhi dan lagu selalu gagal tepat setelah batas tunggu.

Deteksi kata pemicu dimatikan selama lagu diputar supaya suara pengeras suara
tidak salah dikenali sebagai "hai hai". Untuk menghentikan lagu, sentuh foil
tangan boneka.

Lagu berhenti otomatis saat foil disentuh, saat AI memanggil `self.music.stop`,
saat perangkat keluar dari status menganggur, atau saat berkas lagu selesai.

## Pengaturan

| Opsi Kconfig | Bawaan | Keterangan |
|---|---|---|
| `CONFIG_MUSIC_PLAYER_ENABLE` | `y` | Mendaftarkan alat `self.music.play` dan `self.music.stop` |
| `CONFIG_RIRI_SERVER_URL` | kosong | Alamat server Riri |
| `CONFIG_RIRI_SERVER_SECRET` | `riri-boneka-2026` | Harus sama dengan `RIRI_SECRET_KEY` di server |

## Pemecahan Masalah

| Pesan di log `MusicPlayer` | Penyebab dan tindakan |
|---|---|
| `URL server musik belum diisi di menuconfig` | Isi `CONFIG_RIRI_SERVER_URL` |
| `Server musik ... tidak menjawab` | Server belum jalan, alamat salah, atau instance Cloud Run sedang dibangunkan |
| `Server musik menolak permintaan` | `CONFIG_RIRI_SERVER_SECRET` berbeda dengan `RIRI_SECRET_KEY` di server |
| `Server musik tidak menemukan lagu` | Judul terlalu pendek atau salah dengar. Sebutkan judul dan penyanyi lebih lengkap |
| `Data musik menipis, mengisi ulang penyangga` | Wi-Fi tersendat. Sesekali wajar; sering berarti sinyal lemah |
| `Perangkat menganggur, mulai memutar musik` | Normal, lagu segera terdengar |
| `Gagal membuka dekoder MP3` | Berkas bukan MP3. Pastikan server memakai transkode bawaan |
| `Gagal mengalokasikan buffer musik` | PSRAM penuh. Nyalakan ulang perangkat |

Bila server menjawab `Perkakas belum terpasang`, berarti `yt-dlp` atau `ffmpeg`
belum ada di lingkungan server. Citra Docker bawaan sudah menyertakan keduanya.
