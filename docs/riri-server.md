# Server Riri

Satu server Python menyediakan tiga layanan sekaligus: RAG data kampus,
pencarian web, dan pencarian lagu. Firmware hanya perlu satu alamat, yaitu
`CONFIG_RIRI_SERVER_URL`.

Server bersifat **opsional**. Bila alamatnya dikosongkan atau server sedang
mati, firmware tetap menjawab memakai data kampus lokal dan penelusuran
Wikipedia bawaan.

## Endpoint

| Endpoint | Fungsi |
|---|---|
| `GET /health` | Status server, kesiapan RAG, dan kesiapan musik |
| `GET /campus/query?q=...` | RAG data kampus |
| `GET /search?q=...&region=id-id` | Pencarian web |
| `GET /music/resolve?song=...&artist=...` | Cari dan siapkan lagu |
| `GET /music/audio/<berkas>.mp3` | Sajikan berkas lagu |

Permintaan musik membawa header autentikasi kunci dinamis. Endpoint RAG dan
pencarian sengaja tidak menuntut autentikasi supaya mudah diuji dari peramban.

### Autentikasi kunci dinamis

Firmware menghitung SHA256 dari `MAC:ChipID:Timestamp:KunciRahasia`, lalu
menuliskan 16 byte pertamanya sebagai heksadesimal huruf besar ke header
`X-Dynamic-Key`.

Stempel waktu tidak dibandingkan dengan jam server, karena firmware memakai lama
nyala perangkat dan bukan waktu dunia nyata.

`RIRI_SECRET_KEY` di server harus sama persis dengan `CONFIG_RIRI_SERVER_SECRET`
di firmware.

## Alur RAG Data Kampus

Alur disusun memakai **LangGraph**:

```text
masuk -> router -> penulisan ulang kueri -> pencarian hibrida
      -> pemeringkatan ulang -> penilaian kecukupan -> penyusunan jawaban
      -> refleksi diri -> keluar
```

| Tahap | Peran |
|---|---|
| **Router** | Menentukan pertanyaan ini soal kampus atau umum |
| **Penulisan ulang kueri** | Memperluas singkatan prodi seperti TI dan DKV, serta menyambungkan konteks pertanyaan sebelumnya dari memori |
| **Pencarian hibrida** | Menggabungkan pencarian vektor dan BM25 memakai Reciprocal Rank Fusion |
| **Pemeringkatan ulang** | Cross-encoder menilai ulang potongan, sifatnya wajib |
| **Corrective RAG** | Bila skor tertinggi di bawah ambang, pencarian diulang sekali dengan kata kunci inti |
| **Penyusunan jawaban** | Menyusun jawaban hanya dari potongan yang lolos ambang |
| **Refleksi diri** | Menolak jawaban yang tidak memuat satu pun kata kunci pertanyaan |

### Kenapa pencarian hibrida

Pertanyaan kampus banyak memuat nama dosen, singkatan prodi, dan angka semester.
Kemiripan vektor sering meleset pada hal seperti itu, sedangkan BM25 kuat pada
pencocokan kata persis. Menggabungkan keduanya membuat pertanyaan seperti
"dosen Algoritma dan Pemrograman 2" tidak tertukar dengan "Algoritma dan
Pemrograman".

### Kenapa tidak pernah mengarang

Jawaban hanya disusun dari potongan dokumen yang benar-benar terambil. Bila skor
pemeringkat ulang di bawah `RERANK_MIN_SCORE`, server mengembalikan status
`not_found` dan firmware memakai basis data kampus lokalnya sendiri.

Refleksi diri menambah satu lapis lagi: jawaban yang tidak memuat satu pun kata
kunci pertanyaan ikut ditolak.

## Pencarian Web

Memakai `AsyncDDGS` sehingga permintaan tidak memblokir proses. Suara boneka
tidak putus-putus saat pencarian berjalan.

| Perilaku | Keterangan |
|---|---|
| Wilayah | `region='id-id'` untuk hasil Indonesia |
| Berita | `news()` dipakai otomatis saat kueri memuat kata seperti berita, kabar, atau terkini |
| Rentang waktu | `timelimit` diisi `d`, `w`, `m`, atau `y` sesuai frasa "hari ini", "minggu ini", "bulan ini", "tahun ini" |
| Singgahan | Hasil disimpan sepuluh menit di Redis, sehingga berita yang sama tidak ditembak berulang dan tidak kena batas laju |
| Batas bacaan | Hanya dua kalimat pertama yang dikirim ke perangkat, supaya boneka cepat mulai berbicara |

Bila `REDIS_URL` kosong, singgahan otomatis memakai memori proses sehingga server
tetap berjalan di Render.com paket gratis.

## Musik

Pencarian memakai **YouTube Data API v3** dengan kategori Music dan wilayah `ID`,
sehingga rekaman penyanyi aslinya ditemukan, baik lagu internasional maupun lagu
populer Indonesia.

Berkas kemudian ditranskode menjadi **MP3 mono 24 kHz sekitar 96 kbps**, laju
yang sama persis dengan keluaran codec boneka. Dengan begitu ESP32 hanya perlu
mendekode, tanpa mencampur stereo ke mono dan tanpa mengubah laju cuplik.
Rinciannya ada di [panduan pemutar musik](riri-music.md).

## Menjalankan dengan Docker

```powershell
cd server
docker compose up -d --build
```

Isi kunci YouTube lebih dulu lewat berkas `.env`:

```text
YOUTUBE_API_KEY=isi-kunci-anda
```

Server akan tersedia di `http://<ip-komputer>:8000`. Redis ikut dijalankan
otomatis oleh compose.

Memeriksa kesiapan:

```powershell
curl http://localhost:8000/health
```

## Menerapkan ke Render.com

Kode servernya juga tersedia sebagai repositori tersendiri yang siap dipakai
Render, lengkap dengan panduan langkah demi langkah:

**https://github.com/hairift/Server-Python-IoT**

1. Fork atau unggah repositori itu ke akun GitHub Anda.
2. Di Render.com pilih **New**, lalu **Blueprint**, arahkan ke repositori itu.
   Berkas `render.yaml` menyiapkan layanan webnya. Singgahan memakai memori
   proses secara bawaan; Redis dapat ditambahkan belakangan lewat dashboard.
3. Isi Environment Variables:
   - `RIRI_SECRET_KEY` sama dengan `CONFIG_RIRI_SERVER_SECRET` di firmware
   - `YOUTUBE_API_KEY` berisi kunci YouTube Data API v3
4. Tunggu penerapan selesai, lalu salin URL layanannya.
5. Isikan URL itu ke firmware:

```powershell
idf.py menuconfig
# Riri Boneka -> URL server Riri
idf.py build
idf.py -p COM7 -b 460800 flash
```

Paket gratis Render menidurkan layanan yang lama menganggur, sehingga permintaan
pertama setelah lama diam dapat memakan puluhan detik. Firmware sudah membatasi
waktu tunggu supaya jawabannya tidak menggantung: bila server belum bangun,
Riri memakai data lokal lebih dulu.

## Pengaturan

| Variabel lingkungan | Bawaan | Keterangan |
|---|---|---|
| `RIRI_SECRET_KEY` | `riri-boneka-2026` | Harus sama dengan firmware |
| `YOUTUBE_API_KEY` | kosong | Kunci YouTube Data API v3 |
| `YOUTUBE_REGION` | `ID` | Wilayah pencarian lagu |
| `REDIS_URL` | kosong | Kosong berarti singgahan memori proses |
| `CACHE_TTL_SEARCH` | `600` | Umur singgahan pencarian, dalam detik |
| `RERANK_MIN_SCORE` | `0.15` | Ambang kecukupan potongan |
| `TTS_MAX_SENTENCES` | `2` | Batas kalimat yang dibacakan dari hasil web |
| `REQUIRE_AUTH` | `true` | Matikan hanya saat menguji |

## Menambah Dokumen Kampus

Taruh berkas baru di `server/data/`. Format yang dikenali: `.pdf`, `.docx`,
`.html`, `.txt`, `.md`, `.json`, dan `.csv`.

Berkas `server/data/kampus_ucic.md` dihasilkan dari `main/campus_data.cc` supaya
server dan firmware memakai sumber data yang sama persis. Bila data kampus di
firmware diperbarui, hasilkan ulang berkas itu agar keduanya tetap sejalan.

Setelah menambah dokumen, jalankan ulang server supaya indeksnya dibangun ulang.
