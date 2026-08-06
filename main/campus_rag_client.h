#pragma once

// Mengambil jawaban kampus dari Campus RAG Server eksternal.
// Mengembalikan nullptr jika server belum dikonfigurasi, tidak terhubung, atau hasilnya tidak valid.
const char* QueryCampusRagServer(const char* query, char* buf, int buf_size);
