# Server Riri Boneka untuk Google Cloud Run.
#
# Cloud Run menjalankan kontainer dan mengirim variabel PORT. Aplikasi wajib
# mendengarkan pada porta itu, bukan porta tetap.
FROM python:3.11-slim

# ffmpeg diperlukan untuk mentranskode lagu menjadi MP3 mono 24 kHz.
# git diperlukan sentence-transformers saat mengunduh model.
RUN apt-get update \
    && apt-get install -y --no-install-recommends ffmpeg git curl \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Lapisan dependensi dipisah agar pembangunan ulang lebih cepat saat kode berubah.
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

COPY . .

# Model diunduh saat citra dibangun, bukan saat instance menyala.
#
# Ini penting untuk Cloud Run: instance baru dibuat setiap kali layanan bangun
# dari nol, dan mengunduh model pada saat itu membuat permintaan pertama gagal
# karena kehabisan waktu.
ENV HF_HOME=/app/.cache/huggingface \
    TRANSFORMERS_OFFLINE=0 \
    PYTHONUNBUFFERED=1
RUN python -c "\
from sentence_transformers import SentenceTransformer, CrossEncoder; \
SentenceTransformer('sentence-transformers/paraphrase-multilingual-MiniLM-L12-v2'); \
CrossEncoder('cross-encoder/ms-marco-MiniLM-L-6-v2'); \
print('model siap')"

# Sistem berkas Cloud Run bersifat sementara dan berada di memori, jadi
# direktori kerja diarahkan ke /tmp dan ukurannya dibatasi.
ENV MUSIC_CACHE_DIR=/tmp/music_cache \
    VECTOR_DIR=/tmp/vectorstore \
    MUSIC_CACHE_MAX_MB=120 \
    PORT=8080

EXPOSE 8080

# Satu pekerja saja: model embedding dan pemeringkat ulang memakai banyak
# memori, dan beban dari satu boneka memang ringan.
CMD ["sh", "-c", "exec uvicorn app:app --host 0.0.0.0 --port ${PORT} --workers 1 --timeout-keep-alive 75"]
