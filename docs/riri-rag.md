# Data Kampus dan Pencarian Web

Riri menjawab pertanyaan tentang Universitas Catur Insan Cendekia dari basis
pengetahuan yang tertanam di dalam firmware, dan dapat diperkuat server RAG bila
alamatnya diisi.

## Alat MCP

| Alat | Fungsi |
|---|---|
| `self.campus.query` | Pertanyaan tentang kampus UCIC |
| `self.academic.policy` | Pedoman gaya jawaban akademik |
| `self.web.search` | Data terkini yang dapat berubah |
| `self.music.play` | Putar lagu |
| `self.music.stop` | Hentikan lagu |

## Prioritas Jawaban

1. **Data kampus lokal** di `main/campus_data.cc`, tertanam di firmware.
2. **Server RAG** bila `CONFIG_RIRI_SERVER_URL` diisi dan data lokal kurang.
3. **Pencarian web** untuk data terbaru yang dapat berubah.
4. **Pengetahuan bawaan model** untuk pertanyaan umum yang stabil.

Data lokal didahulukan supaya pertanyaan kampus tetap terjawab walaupun server
mati atau internet bermasalah.

## Aturan Anti Mengarang

Beberapa lapis penjagaan bekerja bersama.

**Di firmware.** Hasil data lokal diperiksa terhadap penanda lemah seperti "data
tidak ditemukan". Bila hasilnya lemah, barulah server RAG dicoba. Jawaban
dipotong pada batas aman supaya sambungan MCP tetap stabil.

**Di server.** Jawaban hanya disusun dari potongan dokumen yang benar-benar
terambil dan lolos ambang pemeringkat ulang. Bila tidak ada yang lolos, server
mengembalikan `not_found`, bukan menebak.

**Refleksi diri.** Jawaban yang tidak memuat satu pun kata kunci pertanyaan ikut
ditolak sebelum dikirim.

**Deskripsi alat.** Alat `self.campus.query` memberi tahu model agar tidak
menjawab pertanyaan akademik kampus dari pengetahuan umum sebelum mencoba alat
tersebut, dan agar menyalin nama dosen apa adanya tanpa menambah sapaan.

## Ketahanan Terhadap Salah Dengar

Transkrip suara sering meleset. Nama kampus dapat terdengar sebagai "usia isi",
"uci", atau "yusya isya". Deskripsi alat menyuruh model tetap memanggil
`self.campus.query` untuk kasus seperti itu.

Di sisi server, penulisan ulang kueri memperluas singkatan prodi seperti TI, SI,
DKV, dan MI menjadi nama panjangnya, serta menyambungkan konteks dari pertanyaan
sebelumnya untuk pertanyaan susulan yang sangat pendek.

## Pencarian Web

Bila `CONFIG_RIRI_WEB_SEARCH_SERVER` menyala dan server dapat dihubungi,
pencarian memakai DuckDuckGo lewat server, lengkap dengan penyaring wilayah
Indonesia, mode berita, rentang waktu, dan singgahan.

Bila server tidak tersedia, firmware memakai penelusuran Wikipedia bawaan yang
tertanam di `main/web_search.cc`. Penelusuran lokal ini memiliki:

- pembatas waktu total 14 detik agar sambungan MCP tidak kehabisan waktu tunggu,
- pembersih kata pengisi yang sadar batas kata sehingga kueri seperti
  "wisuda 2026" tidak kehilangan angkanya,
- penilaian khusus untuk pertanyaan jabatan publik agar yang muncul adalah nama
  pejabat saat ini, bukan ringkasan wilayah.

## Memperbarui Data Kampus

Data kampus firmware berada di `main/campus_data.cc`. Setelah mengubahnya,
hasilkan ulang berkas sumber server supaya keduanya tetap sejalan:

```powershell
python scripts/riri_export_campus.py
```

Berkas hasilnya adalah `server/data/kampus_ucic.md`. Jalankan ulang server agar
indeks vektornya dibangun ulang.
