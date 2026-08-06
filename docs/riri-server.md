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
| **Pemeringkatan ulang** | Cross-encoder multibahasa menilai ulang potongan, sifatnya wajib |
| **Corrective RAG** | Bila skor tertinggi di bawah ambang, pencarian diulang sekali dengan kata kunci inti |
| **Penyusunan jawaban** | Menyusun jawaban hanya dari potongan yang lolos ambang |
| **Refleksi diri** | Menolak jawaban yang tidak memuat satu pun kata kunci pertanyaan |

### Kenapa pencarian hibrida

Pertanyaan kampus banyak memuat nama dosen, singkatan prodi, dan angka semester.
Kemiripan vektor sering meleset pada hal seperti itu, sedangkan BM25 kuat pada
pencocokan kata persis. Menggabungkan keduanya membuat pertanyaan seperti
"dosen Algoritma dan Pemrograman 2" tidak tertukar dengan "Algoritma dan
Pemrograman".

### Kenapa pemeringkat ulangnya harus multibahasa

Model `ms-marco-MiniLM-L-6-v2` yang lazim dipakai hanya dilatih untuk bahasa
Inggris. Pada pertanyaan berbahasa Indonesia skornya menjadi nyaris acak:
potongan berisi "Rektor UCIC" pernah dinilai 0,0001 sementara potongan tentang
prospek karir yang tidak berhubungan dinilai 0,9891. Akibatnya pertanyaan yang
datanya jelas ada malah dijawab `not_found`.

Penggantinya, `mmarco-mMiniLMv2-L12-H384-v1`, dilatih pada empat belas bahasa
termasuk bahasa Indonesia. Pada berkas data yang sama, pertanyaan rektor naik
dari 0,0001 menjadi 0,97.

### Kenapa kueri pencarian dan kueri pemeringkatan dibedakan

Kedua tahap menginginkan bentuk kueri yang berlawanan.

Pencarian ingin kosakata seluas mungkin, karena dokumen kadang menulis prodi
sebagai singkatan dan kadang sebagai nama panjang. Karena itu kueri pencarian
berisi kalimat asli ditambah perluasan singkatannya.

Pemeringkat ulang justru menilai kecocokan pertanyaan dengan potongan, sehingga
kalimat yang penuh perluasan menjadi janggal dan skornya anjlok. "siapa rektor
ucic" dinilai 0,98 terhadap potongan yang benar, sedangkan bentuk perluasannya
hanya 0,01. Karena itu pemeringkatan memakai kalimat aslinya.

Pemeringkatan juga menilai bentuk kedua, yaitu kalimat yang nama panjangnya
dipendekkan kembali menjadi singkatan, lalu memakai skor tertinggi di antara
keduanya. Ini menutup kasus sebaliknya: penanya menyebut "Universitas Catur
Insan Cendekia" sementara dokumen menulis "UCIC".

### Kenapa berkas data tidak diberi judul pembuka

Berkas `data/kampus_ucic.md` sengaja langsung dimulai dari entri pertama. Ketika
berkas itu masih diawali judul dan keterangan, teks pembukanya ikut terpotong
menjadi satu potongan tersendiri. Karena memuat nama kampus, potongan itu
memenangkan pencarian untuk hampir semua pertanyaan, dan yang terkirim ke boneka
adalah keterangan berkas, bukan data yang ditanyakan.

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
tetap berjalan tanpa Redis.

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

## Menerapkan ke Google Cloud Run

Panduan langkah demi langkah lengkap ada di
[server/DEPLOY_CLOUDRUN.md](../server/DEPLOY_CLOUDRUN.md).

Ringkasnya, setelah Google Cloud CLI terpasang dan penagihan proyek aktif:

```powershell
cd server
gcloud run deploy riri-server `
  --source . `
  --region asia-southeast2 `
  --allow-unauthenticated `
  --memory 2Gi --cpu 2 --timeout 600 --no-cpu-throttling `
  --set-env-vars "RIRI_SECRET_KEY=riri-boneka-2026,REQUIRE_AUTH=true"
```

Ambil alamatnya:

```powershell
gcloud run services describe riri-server --region asia-southeast2 --format="value(status.url)"
```

Lalu isikan alamat itu ke firmware lewat `idf.py menuconfig`, menu
**Riri Boneka**, bagian **URL server Riri**.

Dengan `--min-instances=0` layanan berhenti total saat menganggur sehingga tidak
ada biaya berjalan. Konsekuensinya permintaan pertama setelah lama diam
memerlukan waktu bangun sekitar 20 sampai 40 detik. Firmware sudah membatasi
waktu tunggunya, jadi bila server belum bangun, Riri memakai data kampus lokal
lebih dulu dan tidak pernah diam menggantung.

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
