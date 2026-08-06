"""Pencarian dan penyiapan lagu untuk Riri.

Pencarian memakai YouTube Data API v3 sehingga rekaman penyanyi aslinya
ditemukan, baik lagu internasional maupun lagu populer Indonesia. Berkas
kemudian ditranskode menjadi MP3 mono 24 kHz sekitar 96 kbps, laju yang sama
persis dengan keluaran codec boneka.

Transkode di sisi server penting: dengan begitu ESP32 tidak perlu mencampur
stereo ke mono dan tidak perlu mengubah laju cuplik, sehingga suara tidak
patah-patah dan kebutuhan jaringan turun menjadi sekitar 12 KB per detik.
"""

from __future__ import annotations

import asyncio
import hashlib
import logging
import os
import re
import shutil
import subprocess
import threading
import unicodedata
from typing import Any, Optional

import cache
from config import pengaturan

log = logging.getLogger("riri.music")

# Direktori singgahan lagu. Dapat dialihkan lewat MUSIC_CACHE_DIR karena pada
# sebagian layanan hosting folder aplikasi bersifat hanya-baca.
DIREKTORI_CACHE = os.environ.get(
    "MUSIC_CACHE_DIR",
    os.path.join(os.path.dirname(os.path.abspath(__file__)), "music_cache"),
)
_gembok_lagu: dict[str, threading.Lock] = {}
_gembok_peta = threading.Lock()

# Laju cuplik ini harus sama dengan AUDIO_OUTPUT_SAMPLE_RATE pada firmware.
LAJU_CUPLIK = 24000
BITRATE = "96k"
KANAL = 1


def _slug(nilai: str) -> str:
    nilai = unicodedata.normalize("NFKD", nilai).encode("ascii", "ignore").decode("ascii")
    nilai = re.sub(r"[^a-zA-Z0-9]+", "_", nilai).strip("_").lower()
    return nilai[:60] or "lagu"


def _gembok(kunci: str) -> threading.Lock:
    with _gembok_peta:
        if kunci not in _gembok_lagu:
            _gembok_lagu[kunci] = threading.Lock()
        return _gembok_lagu[kunci]


def perkakas_tersedia() -> tuple[bool, str]:
    hilang = [t for t in ("yt-dlp", "ffmpeg") if shutil.which(t) is None]
    if hilang:
        return False, "Perkakas belum terpasang: " + ", ".join(hilang)
    return True, ""


def cari_youtube(judul: str, penyanyi: str) -> Optional[dict[str, Any]]:
    """Cari satu lagu teratas lewat YouTube Data API v3."""
    cfg = pengaturan()
    kata = " ".join(x for x in (penyanyi.strip(), judul.strip()) if x)
    if not kata:
        return None

    if not cfg.youtube_api_key:
        log.warning("YOUTUBE_API_KEY kosong, pencarian lagu tidak dapat dilakukan")
        return None

    kunci = "riri:yt:" + hashlib.sha256(kata.lower().encode()).hexdigest()[:24]
    tersimpan = cache.ambil(kunci)
    if tersimpan is not None:
        return tersimpan

    try:
        from googleapiclient.discovery import build

        layanan = build("youtube", "v3", developerKey=cfg.youtube_api_key, cache_discovery=False)
        permintaan = layanan.search().list(
            q=kata + " audio",
            part="snippet",
            type="video",
            # Kategori 10 adalah Music, sehingga klip wawancara dan video reaksi
            # jauh berkurang.
            videoCategoryId="10",
            regionCode=cfg.youtube_region,
            maxResults=3,
        )
        balasan = permintaan.execute()
    except Exception as exc:
        log.warning("Pencarian YouTube gagal: %s", exc)
        return None

    butir = (balasan.get("items") or [])
    if not butir:
        return None

    utama = butir[0]
    hasil = {
        "id": utama["id"]["videoId"],
        "title": utama["snippet"].get("title", judul or kata),
        "artist": utama["snippet"].get("channelTitle", penyanyi or ""),
        "url": "https://www.youtube.com/watch?v=" + utama["id"]["videoId"],
    }
    cache.simpan(kunci, hasil, pengaturan().cache_ttl_music)
    return hasil


def transkode(meta: dict[str, Any]) -> Optional[str]:
    """Unduh lalu ubah menjadi MP3 mono 24 kHz. Mengembalikan nama berkas."""
    nama_berkas = "%s_%s.mp3" % (_slug(meta["title"]), _slug(meta["id"]))
    tujuan = os.path.join(DIREKTORI_CACHE, nama_berkas)

    with _gembok(nama_berkas):
        if os.path.exists(tujuan) and os.path.getsize(tujuan) > 32 * 1024:
            log.info("Memakai berkas singgahan: %s", nama_berkas)
            return nama_berkas

        os.makedirs(DIREKTORI_CACHE, exist_ok=True)
        sementara = tujuan + ".part"
        perintah = [
            "yt-dlp", meta["url"],
            "--no-playlist", "--no-warnings",
            "-f", "bestaudio/best",
            "-x", "--audio-format", "mp3",
            "--postprocessor-args",
            "ffmpeg:-ac %d -ar %d -b:a %s" % (KANAL, LAJU_CUPLIK, BITRATE),
            "-o", sementara + ".%(ext)s",
        ]
        log.info("Mengunduh dan mentranskode: %s", meta["title"])
        try:
            subprocess.check_call(
                perintah, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, timeout=600
            )
        except (subprocess.CalledProcessError, subprocess.TimeoutExpired) as exc:
            log.warning("Unduhan lagu gagal: %s", exc)
            return None

        dihasilkan = sementara + ".mp3"
        if not os.path.exists(dihasilkan):
            log.warning("Berkas hasil transkode tidak ditemukan")
            return None

        os.replace(dihasilkan, tujuan)
        log.info("Lagu siap: %s (%.1f MB)", nama_berkas, os.path.getsize(tujuan) / 1048576.0)
        return nama_berkas


async def siapkan(judul: str, penyanyi: str, basis_url: str) -> dict[str, Any]:
    """Cari lalu siapkan lagu, kembalikan metadata dan tautan audionya."""
    ok, pesan = perkakas_tersedia()
    if not ok:
        return {"status": "error", "error": pesan}

    lup = asyncio.get_running_loop()
    meta = await lup.run_in_executor(None, cari_youtube, judul, penyanyi)
    if meta is None:
        return {"status": "not_found", "error": "lagu tidak ditemukan"}

    nama_berkas = await lup.run_in_executor(None, transkode, meta)
    if nama_berkas is None:
        return {"status": "error", "error": "gagal menyiapkan berkas audio"}

    return {
        "status": "found",
        "title": meta["title"],
        "artist": meta["artist"],
        "duration": 0,
        "audio_url": "%s/music/audio/%s" % (basis_url.rstrip("/"), nama_berkas),
        "lyric_url": "",
    }
