#pragma once

// Cari ke Wikipedia lalu baca infobox untuk mendapatkan data jabatan terbaru.
// Menggunakan Search API Wikipedia, bagian wikitext 0, dan REST Summary API.
// Mengembalikan teks jawaban di buf. Mengembalikan nullptr jika gagal.
const char* WebSearchDuckDuckGo(const char* query, char* buf, int buf_size);
