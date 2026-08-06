"""Pengaturan server Riri.

Seluruh nilai dapat ditimpa lewat variabel lingkungan, sehingga penerapan di
Render.com cukup mengisi Environment Variables tanpa mengubah kode.
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
    youtube_api_key: str = ""
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
    reranker_model: str = "cross-encoder/ms-marco-MiniLM-L-6-v2"
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
