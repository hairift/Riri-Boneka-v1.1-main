#pragma once

// Entri basis pengetahuan kampus yang disimpan di flash
struct CampusEntry {
    const char* id;
    const char* category;
    const char* title;
    const char* content;
    const char* keywords;  // kata kunci dipisahkan koma untuk pencarian
};

// Cari data kampus berdasarkan kueri. Mengembalikan konten yang cocok (maksimal 4 hasil).
// Didefinisikan di campus_data.cc
const char* SearchCampusData(const char* query, char* buf, int buf_size);
int GetCampusEntryCount();
