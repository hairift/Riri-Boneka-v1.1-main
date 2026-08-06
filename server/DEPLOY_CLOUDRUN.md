# Menerapkan Server Riri ke Google Cloud Run

Panduan langkah demi langkah, dari nol sampai boneka dapat memakai servernya.

Cloud Run dipilih karena tidak menidurkan layanan seagresif layanan gratis lain,
memori yang tersedia jauh lebih besar, dan penerapannya dapat dilakukan lewat
satu perintah dari komputer sendiri.

## Ringkasan Biaya

Cloud Run punya kuota gratis bulanan: 2 juta permintaan, 360.000 GB-detik memori,
dan 180.000 vCPU-detik. Pemakaian satu boneka jauh di bawah itu.

Yang perlu diperhatikan: pengaturan `--min-instances=0` membuat layanan berhenti
total saat menganggur, sehingga tidak ada biaya berjalan. Konsekuensinya
permintaan pertama setelah lama diam memerlukan waktu bangun sekitar 20 sampai
40 detik. Firmware sudah menangani ini dengan memakai data kampus lokal lebih
dulu, jadi Riri tidak pernah diam menggantung.

Anda tetap perlu mengaktifkan penagihan pada proyek Google Cloud. Tanpa itu,
Cloud Run tidak dapat dipakai walaupun pemakaiannya masih di dalam kuota gratis.

## Langkah 1: Pasang Google Cloud CLI

Unduh dan jalankan pemasangnya:

https://cloud.google.com/sdk/docs/install

Setelah selesai, buka PowerShell baru lalu periksa:

```powershell
gcloud --version
```

## Langkah 2: Masuk dan Siapkan Proyek

```powershell
gcloud auth login
```

Peramban akan terbuka untuk masuk dengan akun Google Anda.

Buat proyek baru. Nama proyek harus unik di seluruh Google Cloud, jadi tambahkan
angka bila ditolak:

```powershell
gcloud projects create riri-boneka-2026 --name="Riri Boneka"
gcloud config set project riri-boneka-2026
```

Aktifkan penagihan lewat konsol web:

https://console.cloud.google.com/billing

Pilih proyek **Riri Boneka**, lalu hubungkan ke akun penagihan Anda.

Aktifkan layanan yang diperlukan:

```powershell
gcloud services enable run.googleapis.com
gcloud services enable cloudbuild.googleapis.com
gcloud services enable artifactregistry.googleapis.com
```

## Langkah 3: Terapkan Server

Masuk ke folder server, lalu jalankan satu perintah berikut. Perintah ini
membangun kontainer di Google Cloud Build dan langsung menerapkannya ke
Cloud Run.

```powershell
cd "D:\Riri-Boneka-V0.1\Riri-Boneka-v1.1-main\server"

gcloud run deploy riri-server `
  --source . `
  --region asia-southeast2 `
  --platform managed `
  --allow-unauthenticated `
  --memory 2Gi `
  --cpu 2 `
  --timeout 600 `
  --no-cpu-throttling `
  --max-instances 2 `
  --concurrency 8 `
  --set-env-vars "RIRI_SECRET_KEY=riri-boneka-2026,YOUTUBE_REGION=ID,REQUIRE_AUTH=true"
```

Tanda petik terbalik di akhir baris adalah penyambung baris PowerShell. Bila
menyalin ke Command Prompt, gabungkan menjadi satu baris.

Wilayah `asia-southeast2` adalah Jakarta, yang paling dekat sehingga jawabannya
paling cepat sampai ke boneka.

Pembangunan pertama memakan waktu sekitar sepuluh sampai lima belas menit karena
memasang ffmpeg, yt-dlp, dan mengunduh model embedding serta pemeringkat ulang
ke dalam citra.

Bila muncul pertanyaan `Allow unauthenticated invocations`, jawab **y**.

### Penjelasan setiap pilihan

| Pilihan | Alasan |
|---|---|
| `--memory 2Gi` | Model embedding dan pemeringkat ulang perlu ruang; 512 Mi tidak cukup |
| `--cpu 2` | Transkode lagu dan pencarian berjalan bersamaan |
| `--timeout 600` | Menyiapkan lagu dapat memakan beberapa menit |
| `--no-cpu-throttling` | Tanpa ini, transkode di latar belakang dicekik saat tidak ada permintaan masuk |
| `--max-instances 2` | Membatasi biaya bila ada lonjakan permintaan |
| `--concurrency 8` | Satu boneka tidak perlu lebih; menjaga memori aman |

## Langkah 4: Ambil Alamat Layanan

```powershell
gcloud run services describe riri-server --region asia-southeast2 --format="value(status.url)"
```

Hasilnya berbentuk:

```text
https://riri-server-xxxxxxxxxx-et.a.run.app
```

Uji langsung dari peramban:

```text
https://riri-server-xxxxxxxxxx-et.a.run.app/health
```

Jawaban yang benar:

```json
{
  "status": "ok",
  "rag_siap": true,
  "jumlah_potongan": 77,
  "musik_siap": true,
  "youtube_key_terisi": true
}
```

Coba juga RAG kampus dan pencarian web:

```text
.../campus/query?q=rektor UCIC
.../search?q=berita hari ini&region=id-id
```

## Langkah 5: Hubungkan Boneka ke Server

```powershell
cd "D:\Riri-Boneka-V0.1\Riri-Boneka-v1.1-main"
. "C:\Users\riana\esp-idf-v6.0.2\export.ps1"
idf.py menuconfig
```

Masuk ke menu **Riri Boneka**, lalu isi:

| Pengaturan | Isi |
|---|---|
| URL server Riri | alamat dari Langkah 4, tanpa garis miring di akhir |
| Kunci rahasia server Riri | `riri-boneka-2026` |

Simpan dengan `S`, keluar dengan `Q`, lalu:

```powershell
idf.py build
idf.py -p COM7 -b 460800 flash
idf.py -p COM7 monitor
```

## Langkah 6: Uji dari Boneka

Ucapkan "hai hai" lalu coba:

- "Siapa rektor UCIC?" untuk menguji RAG kampus
- "Berita hari ini apa?" untuk menguji pencarian web
- "Putarkan lagu Hati-Hati di Jalan dari Tulus" untuk menguji musik

Di monitor akan terlihat:

```text
I MusicPlayer: Meminta lagu ke server musik: https://riri-server-....run.app/music/resolve?...
I MusicPlayer: Lagu dari server musik: Hati-Hati di Jalan - Tulus
I MusicPlayer: Perangkat menganggur, mulai memutar musik
```

## Memperbarui Server

Setelah mengubah kode, jalankan perintah penerapan yang sama. Cloud Run membuat
revisi baru dan memindahkan lalu lintas ke sana tanpa waktu mati.

```powershell
cd "D:\Riri-Boneka-V0.1\Riri-Boneka-v1.1-main\server"
gcloud run deploy riri-server --source . --region asia-southeast2
```

Pilihan yang sudah diatur sebelumnya tetap tersimpan, jadi tidak perlu diulang.

## Melihat Log

```powershell
gcloud run services logs read riri-server --region asia-southeast2 --limit 50
```

Atau lewat konsol web:

https://console.cloud.google.com/run

## Mengganti Kunci YouTube

Kunci bawaan tersimpan di dalam `config.py`. Untuk menggantinya tanpa mengubah
kode, isi lewat variabel lingkungan yang selalu menang atas nilai bawaan:

```powershell
gcloud run services update riri-server `
  --region asia-southeast2 `
  --update-env-vars "YOUTUBE_API_KEY=kunci-baru-anda"
```

Ini juga cara memperbaikinya bila kuota kunci lama habis dipakai orang lain.

## Menghentikan Layanan

Bila ingin berhenti sepenuhnya agar tidak ada kemungkinan biaya:

```powershell
gcloud run services delete riri-server --region asia-southeast2
```

Firmware akan otomatis kembali memakai data kampus lokal dan penelusuran
Wikipedia bawaan, sehingga boneka tetap berfungsi tanpa server.

## Menjalankan di Komputer Sendiri

Untuk menguji sebelum menerapkan:

```powershell
cd "D:\Riri-Boneka-V0.1\Riri-Boneka-v1.1-main\server"
docker compose up -d --build
```

Lalu isi `CONFIG_RIRI_SERVER_URL` dengan `http://<ip-komputer>:8000`.

Tanpa Docker:

```powershell
pip install -r requirements.txt
# pasang ffmpeg lalu pastikan ada di PATH
python app.py
```

## Pemecahan Masalah

| Gejala | Penyebab dan tindakan |
|---|---|
| `PERMISSION_DENIED` saat deploy | Penagihan belum diaktifkan, atau layanan belum di-enable. Ulangi Langkah 2. |
| Build gagal saat mengunduh model | Jaringan Cloud Build sedang bermasalah. Jalankan ulang perintah deploy. |
| `/health` menunjukkan `musik_siap: false` | ffmpeg gagal terpasang di citra. Periksa log Cloud Build. |
| `youtube_key_terisi: false` | Kunci kosong. Isi lewat `--update-env-vars`. |
| Permintaan pertama sangat lambat | Instance sedang dibangunkan. Wajar untuk `--min-instances=0`. |
| Lagu tidak ditemukan | Kuota YouTube Data API habis. Batas gratisnya 10.000 unit per hari, satu pencarian memakai 100 unit. |
| Instance sering dimatikan | Memori kurang. Naikkan ke `--memory 4Gi`. |

### Ingin permintaan pertama selalu cepat

Setel satu instance agar selalu menyala. Ini menimbulkan biaya berjalan, jadi
lakukan hanya bila memang diperlukan:

```powershell
gcloud run services update riri-server --region asia-southeast2 --min-instances 1
```
