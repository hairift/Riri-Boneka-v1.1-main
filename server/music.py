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
import glob
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
KANAL = 1

# Mutu audio diserahkan ke skala VBR yt-dlp, bukan ke -b:a di dalam
# --postprocessor-args. Nilai -b:a di sana tidak berpengaruh karena yt-dlp
# menyusun argumennya sendiri untuk pengubah audio dan nilai itulah yang menang.
#
# Angka 5 menghasilkan sekitar 47 kbps pada 24 kHz mono, yaitu kira-kira 6 KB
# per detik. Itu justru menguntungkan boneka: penyangga 192 KB di PSRAM setara
# lebih dari tiga puluh detik audio, sehingga tersendatnya Wi-Fi sesaat tidak
# terdengar sama sekali. Pada laju cuplik 24 kHz, telinga tidak dapat
# membedakannya dari laju bit yang lebih tinggi.
MUTU_AUDIO = "5"

# Klien pemutar YouTube yang dicoba berurutan sampai ada yang berhasil.
#
# YouTube memperlakukan alamat pusat data berbeda dari alamat rumahan. Dari
# Cloud Run, klien bawaan sering ditolak dengan permintaan masuk akun, padahal
# permintaan yang sama berhasil dari komputer sendiri. Klien perangkat televisi
# dan ponsel memakai jalur yang tidak menuntut hal itu.
#
# Daftar ini memang perlu ditinjau sesekali karena YouTube mengubah perilakunya
# dari waktu ke waktu. Bila semuanya gagal, log akan memuat pesan asli dari
# yt-dlp sehingga penyebabnya langsung terbaca.
#
# Urutannya berdasarkan pengujian: android_vr berhasil pada semua lagu yang
# dicoba, klien tv menolak sebagian lagu karena dianggap terlindungi DRM, dan
# web_safari kerap tidak menyediakan format audio saja.
KLIEN_YOUTUBE = ("android_vr", "", "tv", "web_safari")

# Saringan durasi wajib untuk hasil pencarian.
#
# Tanpa ini, hasil teratas SoundCloud untuk "Dusk Till Dawn" adalah unggahan
# "One Hour Version" sepanjang 3.649 detik, dan boneka akan memutar mix satu jam
# alih-alih lagunya. Batas bawah menyingkirkan potongan pendek dan cuplikan.
SARINGAN_DURASI = "duration>60 & duration<600"
# Jumlah hasil yang diperiksa sebelum menyerah. Perlu lebih dari satu justru
# karena hasil teratas sering kali yang tersaring.
JUMLAH_HASIL_CARI = 5


def _slug(nilai: str) -> str:
    nilai = unicodedata.normalize("NFKD", nilai).encode("ascii", "ignore").decode("ascii")
    nilai = re.sub(r"[^a-zA-Z0-9]+", "_", nilai).strip("_").lower()
    return nilai[:60] or "lagu"


def _gembok(kunci: str) -> threading.Lock:
    with _gembok_peta:
        if kunci not in _gembok_lagu:
            _gembok_lagu[kunci] = threading.Lock()
        return _gembok_lagu[kunci]


# Batas ukuran singgahan lagu, dalam megabita.
#
# Pada Cloud Run sistem berkas sementara berada di dalam memori, sehingga
# berkas lagu yang menumpuk memakan RAM instance sampai habis lalu prosesnya
# dimatikan. Batas ini membuang berkas terlama begitu ambangnya terlampaui.
BATAS_CACHE_MB = int(os.environ.get("MUSIC_CACHE_MAX_MB", "120"))


def rapikan_cache() -> None:
    """Buang berkas lagu terlama bila singgahan melebihi batas."""
    try:
        if not os.path.isdir(DIREKTORI_CACHE):
            return
        berkas = []
        total = 0
        for nama in os.listdir(DIREKTORI_CACHE):
            jalur = os.path.join(DIREKTORI_CACHE, nama)
            if not os.path.isfile(jalur):
                continue
            ukuran = os.path.getsize(jalur)
            berkas.append((os.path.getmtime(jalur), jalur, ukuran))
            total += ukuran

        batas = BATAS_CACHE_MB * 1024 * 1024
        if total <= batas:
            return

        # Yang paling lama tidak diakses dibuang lebih dulu.
        berkas.sort()
        for _, jalur, ukuran in berkas:
            if total <= batas:
                break
            try:
                os.remove(jalur)
                total -= ukuran
                log.info("Singgahan lagu dirapikan, membuang %s", os.path.basename(jalur))
            except OSError:
                pass
    except Exception as exc:
        log.warning("Gagal merapikan singgahan lagu: %s", exc)


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
        # Kata pencarian ikut disimpan supaya sumber cadangan dapat mencari
        # lagu yang sama tanpa perlu memanggil YouTube lagi.
        "kueri": kata,
    }
    cache.simpan(kunci, hasil, pengaturan().cache_ttl_music)
    return hasil


def cari_soundcloud(judul: str, penyanyi: str) -> Optional[dict[str, Any]]:
    """Cari lagu di SoundCloud, dipakai saat YouTube tidak dapat dihubungi.

    Tidak memerlukan kunci API maupun akun, sehingga tetap bekerja ketika kuota
    YouTube Data API habis.
    """
    kata = " ".join(x for x in (penyanyi.strip(), judul.strip()) if x)
    if not kata:
        return None

    kunci = "riri:sc:" + hashlib.sha256(kata.lower().encode()).hexdigest()[:24]
    tersimpan = cache.ambil(kunci)
    if tersimpan is not None:
        return tersimpan

    try:
        jalan = subprocess.run(
            ["yt-dlp", "scsearch%d:%s" % (JUMLAH_HASIL_CARI, kata),
             "--no-warnings", "--skip-download",
             "--match-filter", SARINGAN_DURASI,
             "--print", "%(id)s\t%(title)s\t%(uploader)s"],
            stdout=subprocess.PIPE, stderr=subprocess.PIPE, timeout=60, check=False,
        )
    except subprocess.TimeoutExpired:
        log.warning("Pencarian SoundCloud kehabisan waktu")
        return None

    baris = (jalan.stdout or b"").decode("utf-8", "replace").strip().split("\n")[0]
    bagian = baris.split("\t")
    if jalan.returncode != 0 or len(bagian) < 3:
        galat = (jalan.stderr or b"").decode("utf-8", "replace").strip()
        log.warning("Pencarian SoundCloud gagal: %s", galat[-300:] or "tanpa hasil")
        return None

    hasil = {
        "id": bagian[0],
        "title": bagian[1],
        "artist": bagian[2],
        # Tanpa alamat YouTube, _daftar_percobaan langsung memakai SoundCloud.
        "url": "",
        "kueri": kata,
    }
    cache.simpan(kunci, hasil, pengaturan().cache_ttl_music)
    return hasil


def _daftar_percobaan(meta: dict[str, Any]) -> list[tuple[str, str, list[str]]]:
    """Susun urutan sumber unduhan yang akan dicoba.

    Setiap butir berisi nama untuk log, alamat atau kueri untuk yt-dlp, dan
    tambahan argumen.
    """
    percobaan: list[tuple[str, str, list[str]]] = []
    if meta.get("url"):
        for klien in KLIEN_YOUTUBE:
            # Saringan durasi juga dipasang di sini karena hasil teratas
            # YouTube pun kadang berupa unggahan versi satu jam.
            tambahan = ["--match-filter", SARINGAN_DURASI]
            if klien:
                tambahan += ["--extractor-args", "youtube:player_client=" + klien]
            percobaan.append(("youtube/" + (klien or "bawaan"), meta["url"], tambahan))

    kueri = (meta.get("kueri") or "").strip()
    if kueri:
        percobaan.append((
            "soundcloud",
            "scsearch%d:%s" % (JUMLAH_HASIL_CARI, kueri),
            ["--match-filter", SARINGAN_DURASI, "--max-downloads", "1"],
        ))
    return percobaan


def _unduh(percobaan: list[tuple[str, str, list[str]]], sementara: str) -> bool:
    """Coba setiap sumber berurutan sampai ada yang menghasilkan berkas MP3."""
    for nama, alamat, tambahan in percobaan:
        perintah = [
            "yt-dlp", alamat,
            "--no-playlist", "--no-warnings",
            "-f", "bestaudio/best",
            "-x", "--audio-format", "mp3", "--audio-quality", MUTU_AUDIO,
            "--postprocessor-args",
            "ffmpeg:-ac %d -ar %d" % (KANAL, LAJU_CUPLIK),
            "-o", sementara + ".%(ext)s",
        ] + tambahan
        try:
            # stderr ditangkap, bukan dibuang, karena tanpa isinya kegagalan
            # unduhan hanya terlihat sebagai "exit status 1" di log dan
            # penyebabnya mustahil ditebak.
            jalan = subprocess.run(
                perintah, stdout=subprocess.DEVNULL, stderr=subprocess.PIPE,
                timeout=600, check=False,
            )
        except subprocess.TimeoutExpired:
            log.warning("Unduhan lagu kehabisan waktu lewat %s", nama)
            continue

        # yt-dlp keluar dengan kode 101 setelah --max-downloads tercapai. Itu
        # justru pertanda berhasil, jadi keberadaan berkasnya yang menentukan.
        if os.path.exists(sementara + ".mp3"):
            log.info("Lagu berhasil diunduh lewat %s", nama)
            return True

        galat = (jalan.stderr or b"").decode("utf-8", "replace").strip()
        log.warning("Unduhan lagu gagal lewat %s: %s", nama, galat[-400:] or "tanpa keterangan")
        # Berkas separuh jadi dari percobaan gagal harus dibersihkan supaya
        # tidak tertukar dengan hasil percobaan berikutnya.
        for sisa in glob.glob(sementara + ".*"):
            try:
                os.remove(sisa)
            except OSError:
                pass
    return False


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
        log.info("Mengunduh dan mentranskode: %s", meta["title"])

        if not _unduh(_daftar_percobaan(meta), sementara):
            return None

        dihasilkan = sementara + ".mp3"
        if not os.path.exists(dihasilkan):
            log.warning("Berkas hasil transkode tidak ditemukan")
            return None

        os.replace(dihasilkan, tujuan)
        log.info("Lagu siap: %s (%.1f MB)", nama_berkas, os.path.getsize(tujuan) / 1048576.0)
        rapikan_cache()
        return nama_berkas


async def siapkan(judul: str, penyanyi: str, basis_url: str) -> dict[str, Any]:
    """Cari lalu siapkan lagu, kembalikan metadata dan tautan audionya."""
    ok, pesan = perkakas_tersedia()
    if not ok:
        return {"status": "error", "error": pesan}

    lup = asyncio.get_running_loop()
    meta = await lup.run_in_executor(None, cari_youtube, judul, penyanyi)
    if meta is None:
        # Kuota YouTube Data API habis atau kuncinya kosong. SoundCloud tidak
        # memerlukan kunci, jadi pencarian tetap dapat dilanjutkan.
        meta = await lup.run_in_executor(None, cari_soundcloud, judul, penyanyi)
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
