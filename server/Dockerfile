# Server Riri Boneka: RAG kampus, pencarian web, dan pemutar musik.
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

# Model embedding dan pemeringkat ulang diunduh saat citra dibangun supaya
# permintaan pertama tidak menunggu unduhan model.
RUN python -c "\
from sentence_transformers import SentenceTransformer, CrossEncoder; \
SentenceTransformer('sentence-transformers/paraphrase-multilingual-MiniLM-L12-v2'); \
CrossEncoder('cross-encoder/ms-marco-MiniLM-L-6-v2'); \
print('model siap')"

ENV PORT=8000
EXPOSE 8000

HEALTHCHECK --interval=30s --timeout=5s --start-period=90s --retries=3 \
    CMD curl -fsS http://localhost:${PORT}/health || exit 1

# Satu pekerja saja: model embedding dan pemeringkat ulang memakai banyak
# memori, dan beban perangkat boneka memang ringan.
CMD ["sh", "-c", "uvicorn app:app --host 0.0.0.0 --port ${PORT} --workers 1"]
