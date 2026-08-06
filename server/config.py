"""Pengaturan server Riri.

Seluruh nilai dapat ditimpa lewat variabel lingkungan, sehingga penerapan di
Cloud Run cukup memakai --set-env-vars tanpa mengubah kode.
"""

from __future__ import annotations

import os
from functools import lru_cache

from pydantic_settings import BaseSettings


class Pengaturan(BaseSettings):
    # ---------------------------------------------------------------- umum
    # Kunci rahasia harus sama dengan CONFIG_RIRI_SERVER_SECRET di firmware.
    riri_secret_key: str = "riri-boneka-2026"
    # Pemeriksaan kunci dinamis dapat dimatikan saat menguji dari peramban.
    require_auth: bool = True

    # ------------------------------------------------------------- YouTube
    # Kunci YouTube Data API v3 untuk mencari lagu.
    #
    # PERINGATAN KEAMANAN
    # Nilai bawaan di bawah ini ikut tersimpan di repositori publik atas
    # permintaan pemilik proyek. Kunci yang terlihat publik biasanya ditemukan
    # perayap otomatis dalam hitungan jam, lalu kuotanya dipakai orang lain
    # sampai habis. Bila pencarian lagu tiba-tiba berhenti bekerja, itulah
    # penyebab yang paling mungkin.
    #
    # Cara memperbaikinya: buat kunci baru di Google Cloud Console, hapus yang
    # lama, lalu isi lewat variabel lingkungan YOUTUBE_API_KEY saat menerapkan.
    # Nilai dari variabel lingkungan selalu mengalahkan nilai bawaan ini.
    youtube_api_key: str = "AIzaSyD5eA84gab7SrwXDJSKA32QrtqmrK2JtPw"
    # Wilayah pencarian lagu. ID membuat lagu populer Indonesia lebih mudah
    # ditemukan tanpa mengurangi jangkauan lagu internasional.
    youtube_region: str = "ID"

    # --------------------------------------------------------------- Redis
    # Kosongkan untuk memakai singgahan di memori proses.
    redis_url: str = ""
    # Hasil pencarian web disimpan sepuluh menit. Berita yang sama tidak
    # ditembak berulang kali sehingga tidak kena batas laju DuckDuckGo.
    cache_ttl_search: int = 600
    cache_ttl_music: int = 3600

    # ----------------------------------------------------------------- RAG
    # Berkas sumber pengetahuan kampus.
    data_dir: str = "data"
    # Direktori penyimpanan basis data vektor.
    vector_dir: str = "vectorstore"
    # Model embedding multibahasa yang ringan dan mendukung bahasa Indonesia.
    embedding_model: str = "sentence-transformers/paraphrase-multilingual-MiniLM-L12-v2"
    # Model pemeringkat ulang. Wajib dipakai agar potongan yang dikirim ke
    # penjawab benar-benar yang paling relevan.
    #
    # Harus multibahasa. Model ms-marco-MiniLM sebelumnya hanya dilatih untuk
    # bahasa Inggris, sehingga skornya nyaris acak untuk pertanyaan berbahasa
    # Indonesia: potongan berisi "Rektor UCIC" pernah dinilai 0,0001 sementara
    # potongan yang tidak berhubungan dinilai 0,9891. Model mMARCO dilatih pada
    # empat belas bahasa termasuk bahasa Indonesia dan ukurannya masih cukup
    # kecil untuk Cloud Run.
    reranker_model: str = "cross-encoder/mmarco-mMiniLMv2-L12-H384-v1"
    # Jumlah potongan yang diambil sebelum dan sesudah pemeringkatan ulang.
    retrieve_top_k: int = 20
    rerank_top_k: int = 5
    # Ambang skor pemeringkat ulang. Di bawah nilai ini jawaban dianggap tidak
    # didukung sumber, sehingga server berterus terang daripada mengarang.
    rerank_min_score: float = 0.15

    # ------------------------------------------------------------- jawaban
    # Batas kalimat yang dibacakan dari hasil web agar suara cepat terdengar.
    tts_max_sentences: int = 2
    # Batas panjang jawaban yang dikirim ke perangkat.
    answer_max_chars: int = 1200

    class Config:
        env_prefix = ""
        env_file = ".env"
        extra = "ignore"


@lru_cache
def pengaturan() -> Pengaturan:
    return Pengaturan()


def jalur_data() -> str:
    p = pengaturan().data_dir
    return p if os.path.isabs(p) else os.path.join(os.path.dirname(os.path.abspath(__file__)), p)


def jalur_vektor() -> str:
    p = pengaturan().vector_dir
    return p if os.path.isabs(p) else os.path.join(os.path.dirname(os.path.abspath(__file__)), p)
