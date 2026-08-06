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
         -> server mengunduh dari YouTube, atau SoundCloud bila ditolak
         -> server mentranskode ke MP3 mono 24 kHz
         -> {"title","artist","audio_url"}
         -> firmware mengunduh audio_url
         -> ring buffer 192 KB di PSRAM
         -> dekode MP3 -> antrean playback -> I2S -> MAX98357
```

## Dari Mana Lagunya Diambil

Pencarian selalu memakai YouTube Data API karena hasilnya paling tepat, terutama
untuk lagu Indonesia. Pengunduhannya punya dua jalur.

**YouTube** dicoba lebih dulu, dengan beberapa jenis klien pemutar berurutan.

**SoundCloud** dipakai bila semua percobaan YouTube ditolak. Ini bukan
kemewahan, melainkan keharusan saat server berjalan di Cloud Run: YouTube
memperlakukan alamat pusat data berbeda dari alamat rumahan, dan menolak setiap
permintaan dengan pesan "Sign in to confirm you're not a bot". Pesan itu muncul
untuk semua jenis klien dan tidak dapat dihindari tanpa memasukkan berkas cookie
akun YouTube ke server, yang berisiko membuat akunnya diblokir dan tidak boleh
ikut tersimpan di repositori publik.

Lagu yang sama dari SoundCloud biasanya diunggah ulang oleh pengguna lain, jadi
judulnya tepat tetapi nama pengunggahnya bukan penyanyi aslinya. Untuk boneka
ini hasilnya tidak terdengar berbeda.

SoundCloud juga menjadi jaring pengaman ketika kuota YouTube Data API habis,
karena pencariannya sama sekali tidak memerlukan kunci API.

Log memberi tahu jalur mana yang dipakai:

```text
I riri.music: Lagu berhasil diunduh lewat soundcloud
```

Bila server dijalankan di komputer sendiri lewat Docker, alamatnya adalah alamat
rumahan sehingga jalur YouTube langsung berhasil dan SoundCloud tidak terpakai.

## Kenapa Suaranya Tidak Patah-Patah

Tiga hal bekerja bersama.

**Server mentranskode ke format yang persis dibutuhkan perangkat.** Berkas
dikirim sebagai MP3 mono 24 kHz sekitar 47 kbps. Laju cuplik itu sama dengan
`AUDIO_OUTPUT_SAMPLE_RATE` codec boneka, sehingga ESP32 tidak perlu mencampur
stereo ke mono dan tidak perlu mengubah laju cuplik sama sekali.

**Kebutuhan jaringan turun jauh.** Dari sekitar 40 KB per detik untuk MP3 320
kbps stereo menjadi sekitar 6 KB per detik.

**Lagunya dimuat seluruhnya ke PSRAM sebelum diputar.** Penyangganya diminta
4 MB, sedangkan lagu sepuluh menit pun di bawah 3,5 MB. Inilah yang paling
menentukan pada Wi-Fi yang buruk: begitu nada pertama berbunyi, jaringan sudah
tidak dipakai lagi sama sekali, sehingga gangguan jaringan setelah itu tidak
mungkin terdengar. Sebelumnya penyangganya hanya 192 KB dan sisa lagu tetap
harus mengalir selama pemutaran.

Ukurannya diturunkan bertahap menjadi 2 MB, 1 MB, dan seterusnya sampai 256 KB
bila PSRAM tidak menyediakan blok sebesar itu. Model pengenal suara sudah
memakai sebagian besar PSRAM dan sisanya belum tentu berupa satu blok utuh:
permintaan 4 MB pernah gagal seluruhnya, dan yang terdengar hanya "memori
perangkat tidak cukup" tanpa lagu sama sekali. Penyangga yang lebih kecil hanya
berarti sebagian lagu masih mengalir selama pemutaran, dan itu jauh lebih baik
daripada tidak memutar apa pun.

Ambang isi awal ikut dibatasi tiga perempat kapasitas yang benar-benar didapat.
Bila ambangnya melebihi kapasitas, tugas jaringan berhenti menulis karena
penyangga penuh sementara tugas dekoder menunggu ambang yang tidak mungkin
tercapai, dan keduanya saling menunggu selamanya.

**Unduhan baru dimulai setelah Riri selesai bicara.** Mengunduh ratusan kilobyte
bersamaan dengan penguraian TTS membuat paket suara terlambat sampai ke antrean
pemutaran, dan itulah sebabnya suara AI dulu selalu patah-patah tepat ketika
hendak memutar lagu. Jedanya ditutupi nada tunggu.

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

## Kenapa Pencariannya di Latar Belakang

Alat `self.music.play` menjawab seketika bahwa lagunya sedang dicari, lalu
pencarian dan pengunduhan berjalan sesudahnya.

Sebabnya, server MCP menyerah menunggu jawaban alat sekitar sepuluh detik,
sedangkan menyiapkan lagu baru memerlukan belasan sampai tiga puluhan detik.
Ketika pencarian masih menahan jawaban alat, AI selalu terlanjur berkata "gagal
karena waktu habis" walaupun servernya sedang menyiapkan lagunya dengan baik,
dan lagu yang sudah jadi itu terbuang percuma.

Karena tidak ada lagi yang menunggu dalam diam, pencarian kini diulang sampai
tiga kali. Percobaan kedua biasanya langsung berhasil, sebab server sudah
selesai menyiapkan lagu yang sama dan tinggal mengambilnya dari singgahan.

## Nada Tunggu

Selama lagu disiapkan, boneka membunyikan `loading.ogg` berulang. Nada itu
berhenti begitu lagunya siap, dan antrean pemutaran dikosongkan supaya sisanya
tidak menimpa awal lagu. Nada tunggu juga berhenti sendiri bila pengguna
memanggil AI di tengah penantian.

Nada tunggu berjalan di tugasnya sendiri, bukan di dalam tugas dekoder.
Mengantre berkas `.ogg` menunggu sampai seluruh isinya masuk antrean, dan
penantian itu dibatasi kecepatan pemutaran. Untuk berkas berdurasi enam belas
detik, memutarnya di dalam tugas dekoder berarti dekoder tidak sempat menyadari
bahwa lagunya sudah siap sampai belasan detik kemudian.

`AudioService::PlaySound` menerima fungsi pembatalan agar berkas panjang dapat
dipotong di tengah. Tanpa itu, nada tunggu tetap mengantre sampai habis dan
terdengar menimpa awal lagu maupun suara reaksi sentuhan.

Penghentiannya memakai `AudioService::ResetDecoder`, bukan `ClearPlaybackQueue`.
`ClearPlaybackQueue` hanya mengosongkan antrean pemutaran, sedangkan nada tunggu
masih menyisakan paket di antrean dekode yang memuat sampai 2,4 detik audio.
Paket itu tetap terurai dan berbunyi menimpa awal lagu, sehingga lagunya
terdengar bertabrakan dengan nada tunggu dan seolah patah-patah.

Sentuhan foil menghentikan nada tunggu lebih dulu, baru memainkan suara
reaksinya, sehingga keduanya tidak pernah bertumpuk.

Tanpa nada tunggu, boneka terdengar membisu begitu saja dan tidak ada cara
membedakan "sedang menyiapkan" dari "macet".

Berkas nada tunggu berada di `main/assets/tambahan/loading.ogg` dan ikut masuk
ke `generated_assets.bin`. Seluruh isi `assets/tambahan` terdaftar sebagai
ketergantungan CMake, jadi menambah berkas baru di sana memang memicu
pembangunan ulang partisi aset. Tanpa pendaftaran itu, pembangunan tetap
berhasil dan firmware tetap terpasang, tetapi berkas barunya diam-diam tidak
ikut dan baru ketahuan saat perangkat melaporkan asetnya tidak ditemukan.

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

Deteksi kata pemicu dimatikan selama lagu diputar.

Papan ini tidak punya peredam gema, jadi mikrofon mendengar lagunya sendiri dan
bagian lagu mana pun dapat dikenali keliru sebagai "hai hai". Pada pengujian,
satu potongan lagu bernilai 0,162 sementara kata pemicu asli pernah bernilai
0,139, sehingga keduanya memang tidak dapat dipisahkan oleh ambang mana pun.
Ketika deteksi dibiarkan menyala, lagu berhenti sendiri di tengah tanpa ada yang
memintanya.

Untuk menghentikan lagu, sentuh bagian mana pun dari boneka. Menyentuh foil
tangan sekaligus membangunkan AI, sehingga percakapan dapat dilanjutkan seperti
biasa.

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
| `Server musik tidak menemukan lagu` | Judul terlalu pendek atau salah dengar. Sebutkan judul dan penyanyi lebih lengkap. Bila log server memuat `Unduhan lagu gagal lewat` pada semua sumber, lagunya memang tidak ada di kedua sumber |
| `Data musik menipis, mengisi ulang penyangga` | Wi-Fi tersendat. Sesekali wajar; sering berarti sinyal lemah |
| `Perangkat menganggur, mulai memutar musik` | Normal, lagu segera terdengar |
| `Gagal membuka dekoder MP3` | Berkas bukan MP3. Pastikan server memakai transkode bawaan |
| `Gagal mengalokasikan buffer musik` | PSRAM penuh. Nyalakan ulang perangkat |

Bila server menjawab `Perkakas belum terpasang`, berarti `yt-dlp` atau `ffmpeg`
belum ada di lingkungan server. Citra Docker bawaan sudah menyertakan keduanya.
