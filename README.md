# Riri Boneka AI

Firmware boneka AI berbahasa Indonesia untuk ESP32-S3.

Riri dapat diajak bicara, menjawab pertanyaan tentang Universitas Catur Insan
Cendekia, mencari informasi terbaru dari internet, memutar lagu, dan bereaksi
saat bagian tubuhnya disentuh.

**Versi V1.1.**

## Pratinjau Perangkat

<img src="docs/boneka_riri/Boneka%20Riri.png" width="380">

## Perangkat Keras

| Komponen | Keterangan |
|---|---|
| Papan utama | ESP32-S3 dengan PSRAM 8 MB dan flash 16 MB |
| Mikrofon | INMP441 lewat I2S |
| Penguat suara | MAX98357 I2S 3 W |
| Pengeras suara | 8 ohm 3 W |
| Layar | OLED SSD1306 0,96 inci lewat I2C, sifatnya opsional |
| Lampu indikator | LED RGB WS2812 bawaan papan, ditambah satu LED RGB status |
| Penunjuk baterai | Modul penunjuk kapasitas baterai lithium yang berdiri sendiri |
| Sensor sentuh | Foil tembaga pada delapan bagian tubuh |
| Daya | Modul powerbank dengan penaik tegangan 5 V |
| Pendingin | Kipas DC 5 V, tersambung langsung ke jalur 5 V |

## Peta Pin

| Fungsi | GPIO |
|---|---|
| INMP441 WS | `GPIO4` |
| INMP441 SCK | `GPIO5` |
| INMP441 SD | `GPIO6` |
| MAX98357 DIN | `GPIO7` |
| MAX98357 BCLK | `GPIO15` |
| MAX98357 LRC | `GPIO16` |
| OLED SDA | `GPIO10` |
| OLED SCL | `GPIO9` |
| LED RGB | `GPIO48` |
| Tombol BOOT | `GPIO0` |
| Foil kepala | `GPIO1` |
| Foil idung | `GPIO2` |
| Foil perut | `GPIO3` |
| Foil ekor | `GPIO8` |
| Foil tangan kanan | `GPIO11` |
| Foil tangan kiri | `GPIO12` |
| Foil kaki kanan | `GPIO13` |
| Foil kaki kiri | `GPIO14` |

Foil idung memakai `GPIO2` dan bukan `GPIO5`, karena `GPIO5` sudah dipakai
sebagai jalur bit clock mikrofon INMP441. Satu pin hanya boleh punya satu
fungsi; memindahkan foil ke `GPIO5` akan membuat mikrofon berhenti bekerja.

Pembacaan baterai lewat ADC dimatikan karena `GPIO2` dipakai foil idung.

## Fitur Riri

- Kata pemicu lokal **"hai hai"** untuk membangunkan AI.
- Nama hotspot konfigurasi Wi-Fi adalah **Riri**.
- Bahasa dibatasi ke **Indonesia** dan **Inggris** saja.
- **RAG data kampus** UCIC, tersedia lokal di dalam firmware dan lewat server.
- **Pencarian web** untuk data terkini, tersedia lokal dan lewat server.
- **Pemutar musik** yang mencari lagu lewat YouTube Data API.
- **Sensor sentuh foil tembaga** pada delapan bagian tubuh.
- **Lampu indikator RGB** sebagai penanda status tanpa perlu layar.

## Dokumentasi

| Dokumen | Isi |
|---|---|
| [Panduan build dan flash](docs/riri-build.md) | Cara membangun dan mengunggah firmware dengan ESP-IDF 6.0.2 |
| [Sensor sentuh foil tembaga](docs/riri-touch.md) | Pemasangan, cara kerja, dan pemecahan masalah foil |
| [Lampu indikator RGB](docs/riri-led.md) | Arti setiap warna lampu bawaan papan |
| [LED status dan penunjuk baterai](docs/riri-rgb-indikator.md) | Pin, resistor, arti warna, dan penyambungan modul kapasitas baterai |
| [Server Riri](docs/riri-server.md) | RAG kampus, pencarian web, dan musik di sisi server |
| [Pemutar musik](docs/riri-music.md) | Alur pemutaran lagu dari server sampai pengeras suara |
| [Data kampus dan pencarian web](docs/riri-rag.md) | Alat MCP, prioritas jawaban, dan aturan anti mengarang |

## Ringkasan Cepat

Membangun dan mengunggah firmware:

```powershell
cd "D:\Riri-Boneka-V0.1\Riri-Boneka-v1.1-main"
. "C:\Users\riana\esp-idf-v6.0.2\export.ps1"
idf.py set-target esp32s3
idf.py build
idf.py -p COM7 -b 460800 flash
idf.py -p COM7 monitor
```

Menjalankan server Riri di komputer sendiri:

```powershell
cd server
docker compose up -d --build
```

Menerapkan server ke Google Cloud Run, lihat
[panduan lengkapnya](server/DEPLOY_CLOUDRUN.md):

```powershell
cd server
gcloud run deploy riri-server --source . --region asia-southeast2 --allow-unauthenticated
```

Cermin kode servernya juga tersedia di
**https://github.com/hairift/Server-Python-IoT**

Rincian lengkap ada di [panduan build dan flash](docs/riri-build.md).

## Prioritas Jawaban AI

1. Data kampus lokal yang tertanam di dalam firmware.
2. Server RAG kampus bila `CONFIG_RIRI_SERVER_URL` diisi.
3. Pencarian web untuk data terbaru yang dapat berubah.
4. Pengetahuan bawaan model untuk pertanyaan umum.

Urutan ini membuat pertanyaan tentang kampus selalu punya jawaban walaupun
server sedang mati atau internet sedang bermasalah.

## Versi

Proyek ini adalah **V1.1**. Baris berikut muncul paling awal di monitor:

```text
I main: Riri Boneka AI V1.1
```

Angka `App version` yang muncul tepat sesudahnya adalah versi firmware hulu,
bukan versi proyek ini, dan sengaja dibiarkan apa adanya. Angka itu dipakai
server OTA untuk membandingkan versi, dan pembaruan berjalan otomatis saat
boot. Menurunkannya membuat server menganggap ada versi yang lebih baru,
sehingga seluruh kustomisasi di sini akan tertimpa firmware hulu.

## Catatan Rilis

Berkas di dalam `managed_components/` adalah dependensi pihak ketiga dan tidak
perlu diubah untuk kustomisasi Riri.

Proyek asalnya berlisensi MIT, lihat [LICENSE](LICENSE).
