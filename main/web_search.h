#pragma once

// Cari ke Wikipedia lalu baca infobox untuk mendapatkan data jabatan terbaru.
// Menggunakan Search API Wikipedia, bagian wikitext 0, dan REST Summary API.
// Mengembalikan teks jawaban di buf. Mengembalikan nullptr jika gagal.
const char* WebSearchDuckDuckGo(const char* query, char* buf, int buf_size);

// Bangunkan server Riri di latar belakang segera setelah jaringan siap.
//
// Cloud Run menghentikan layanan sepenuhnya saat menganggur supaya tidak ada
// biaya berjalan, dan permintaan pertama sesudahnya harus menunggu instance
// dibangunkan. Tanpa pemanasan ini, permintaan pertama penggunalah yang
// menanggung waktu tunggu itu, dan hasilnya adalah "tidak dapat memberi tahu
// berita" atau lagu yang gagal diputar padahal servernya sehat.
//
// Aman dipanggil berkali-kali. Pemanggilan berikutnya diabaikan bila pemanasan
// sebelumnya masih berjalan.
void RiriServerWarmUp();
