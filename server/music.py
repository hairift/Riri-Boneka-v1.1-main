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
import random
import re
import shutil
import subprocess
import threading
import time
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
DURASI_MIN_DETIK = 60.0
DURASI_MAKS_DETIK = 600.0
SARINGAN_DURASI = "duration>%d & duration<%d" % (DURASI_MIN_DETIK, DURASI_MAKS_DETIK)
# Jumlah hasil yang diperiksa sebelum menyerah.
#
# Dua puluh, bukan lima. Hasil teratas SoundCloud untuk penyanyi terkenal
# biasanya unggahan resmi label yang hanya berupa cuplikan tiga puluh detik, dan
# unggahan utuh baru muncul di urutan belasan. Dengan lima hasil, permintaan
# "putar lagu Bruno Mars" tidak menemukan apa pun.
JUMLAH_HASIL_CARI = 20

# Batas waktu seluruh penyiapan lagu, dari pencarian sampai berkas siap.
#
# Firmware menunggu jawaban ini sambil AI terdiam, jadi angkanya harus sedikit
# di bawah batas waktu firmware. Lagu yang pernah diminta dijawab seketika dari
# singgahan, jadi batas ini hanya terasa pada lagu yang benar-benar baru.
BATAS_SIAPKAN_DETIK = 32.0
# Banyaknya hasil yang benar-benar diunduh sebelum berpindah ke kueri berikutnya.
# Bila tiga hasil teratas yang sudah lolos penyaringan pun gagal diunduh,
# kuerinya yang bermasalah, bukan hasilnya.
BATAS_KANDIDAT_PER_KUERI = 3


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


def cari_soundcloud(kata: str, jumlah: int = JUMLAH_HASIL_CARI) -> list[dict[str, Any]]:
    """Cari beberapa lagu sekaligus di SoundCloud.

    Mengembalikan daftar, bukan satu hasil, karena hasil teratas belum tentu
    dapat diunduh dan karena permintaan yang hanya menyebut nama penyanyi perlu
    beberapa pilihan untuk diacak.

    Tidak memerlukan kunci API maupun akun, sehingga tetap bekerja ketika kuota
    YouTube Data API habis.
    """
    kata = kata.strip()
    if not kata:
        return []

    kunci = "riri:sc:%d:%s" % (jumlah, hashlib.sha256(kata.lower().encode()).hexdigest()[:24])
    tersimpan = cache.ambil(kunci)
    if tersimpan is not None:
        return tersimpan

    try:
        jalan = subprocess.run(
            # --flat-playlist membaca daftar hasil apa adanya dari SoundCloud
            # tanpa membuka satu per satu. Untuk dua puluh hasil, bedanya tiga
            # detik melawan dua puluh lima detik, dan seluruh keterangan yang
            # dibutuhkan sudah ada di daftar itu.
            ["yt-dlp", "scsearch%d:%s" % (jumlah, kata),
             "--no-warnings", "--skip-download", "--flat-playlist",
             "--print", "%(id)s\t%(duration)s\t%(title)s\t%(uploader)s\t%(url)s"],
            stdout=subprocess.PIPE, stderr=subprocess.PIPE, timeout=90, check=False,
        )
    except subprocess.TimeoutExpired:
        log.warning("Pencarian SoundCloud kehabisan waktu untuk %r", kata)
        return []

    hasil: list[dict[str, Any]] = []
    for baris in (jalan.stdout or b"").decode("utf-8", "replace").strip().split("\n"):
        bagian = baris.split("\t")
        if len(bagian) < 5 or not bagian[4].startswith("http"):
            continue
        try:
            durasi = float(bagian[1])
        except ValueError:
            continue
        # Penyaringan durasi dilakukan di sini, bukan lewat --match-filter,
        # karena daftar ringkas tidak selalu menyediakan bidang yang dipakai
        # penyaring bawaan yt-dlp.
        #
        # Batas bawahnya penting: lagu resmi label di SoundCloud sering hanya
        # berupa cuplikan tiga puluh detik. Tanpa penyaring ini, permintaan
        # "putar lagu Bruno Mars" akan memutar cuplikan, bukan lagunya.
        if not (DURASI_MIN_DETIK < durasi < DURASI_MAKS_DETIK):
            continue
        hasil.append({
            "id": bagian[0],
            "title": bagian[2],
            "artist": bagian[3],
            # Alamat langsung, supaya pengunduhan tidak perlu mencari ulang.
            "url": bagian[4],
            "sumber": "soundcloud",
        })

    if not hasil:
        galat = (jalan.stderr or b"").decode("utf-8", "replace").strip()
        log.info("SoundCloud tidak punya hasil utuh untuk %r: %s", kata, galat[-200:] or "kosong")

    cache.simpan(kunci, hasil, pengaturan().cache_ttl_music)
    return hasil


def _token(teks: str) -> set[str]:
    return {t for t in re.findall(r"[0-9a-z぀-ヿ一-鿿가-힯]+",
                                  teks.lower()) if len(t) > 2}


def _nyambung(pilihan: dict[str, Any], kata: str, termasuk_pengunggah: bool) -> bool:
    """Periksa apakah hasil pencarian benar-benar berhubungan dengan kueri.

    SoundCloud selalu mengembalikan sesuatu, bahkan ketika tidak ada yang cocok.
    Untuk "Yorushika Saitu" ia pernah menawarkan lagu Vietnam, dan untuk
    "Yurusik" sebuah lagu Rusia. Memutar itu jauh lebih buruk daripada mencoba
    kueri yang lebih longgar, karena penggunanya mendengar lagu yang sama sekali
    bukan yang diminta dan tidak tahu kenapa.

    Cukup satu kata yang sama, sebab judul asing kerap ditulis campur aduk:
    "ヨルシカ -「言って。」/ Yorushika" memuat nama penyanyi dalam dua tulisan
    sekaligus.

    Nama pengunggah hanya ikut dinilai untuk kueri yang memang berupa nama
    penyanyi. Untuk kueri judul, nama pengunggah justru menyesatkan: judul
    "Saitu" pernah lolos lewat sebuah lagu tak berhubungan hanya karena nama
    pengunggahnya kebetulan mengandung kata itu.
    """
    dicari = _token(kata)
    if not dicari:
        return True
    teks = pilihan["title"]
    if termasuk_pengunggah:
        teks += " " + pilihan["artist"]
    return bool(dicari & _token(teks))


def _kandidat_kueri(judul: str, penyanyi: str) -> list[tuple[str, bool]]:
    """Susun kueri dari yang paling tepat ke yang paling longgar.

    Nilai kedua menandakan hasilnya boleh diacak. Itu berlaku untuk permintaan
    yang memang tidak menyebut lagu tertentu, misalnya "putar lagu Bruno Mars"
    atau "putar lagu dj Thailand": pengguna ingin lagu apa saja yang cocok, dan
    mengacak membuat permintaan yang sama tidak selalu memutar lagu yang itu-itu
    juga.

    Urutan longgar ini juga yang membuat judul asing tetap ketemu. Alih bahasa
    suara sering salah dengar judul berhuruf bukan Latin: "Yorushika" pernah
    terdengar sebagai "Saitu", "Sikap", dan "Yurusik". Ketika kueri lengkapnya
    gagal, pencarian dengan nama penyanyi saja tetap menemukan lagu penyanyi
    tersebut, sehingga permintaannya tidak berakhir dengan "lagunya tidak ada"
    padahal di SoundCloud lagunya jelas ada.
    """
    judul = judul.strip()
    penyanyi = penyanyi.strip()

    kandidat: list[tuple[str, bool]] = []

    def tambah(kata: str, acak: bool) -> None:
        kata = " ".join(kata.split())
        # Kueri yang sama boleh muncul dua kali bila cara memilihnya berbeda:
        # percobaan pertama mengambil hasil teratas, percobaan berikutnya
        # mengacak sisanya.
        if kata and (kata.lower(), acak) not in {(k.lower(), a) for k, a in kandidat}:
            kandidat.append((kata, acak))

    if judul and penyanyi:
        tambah("%s %s" % (penyanyi, judul), False)
        tambah(judul, False)
        # Judul yang salah dengar dibuang seluruhnya, sisakan penyanyinya.
        tambah(penyanyi, True)
    elif penyanyi:
        tambah(penyanyi, True)
    elif judul:
        tambah(judul, False)
        # Satu kata tunggal biasanya justru nama penyanyi, bukan judul lagu,
        # karena AI menaruh apa pun yang tidak dikenalinya ke bidang judul.
        tambah(judul, True)

    return kandidat


# Ditandai benar setelah YouTube menolak alamat server ini sekali.
_youtube_diblokir = False


def _catat_youtube_diblokir(galat: str) -> None:
    """Ingat bahwa YouTube menolak alamat server ini.

    Pemeriksaan bot berlaku untuk seluruh alamat, bukan per lagu, jadi setelah
    sekali ditolak percobaan berikutnya pasti ditolak juga. Mencobanya lagi
    hanya membuang tujuh detik pada setiap permintaan lagu, dan tujuh detik itu
    langsung terasa sebagai boneka yang diam menunggu.
    """
    global _youtube_diblokir
    if _youtube_diblokir:
        return
    if "not a bot" in galat or "Sign in to confirm" in galat:
        _youtube_diblokir = True
        log.warning("YouTube menolak alamat server ini, sumber lagu beralih ke SoundCloud")


def _daftar_percobaan(meta: dict[str, Any]) -> list[tuple[str, str, list[str]]]:
    """Susun urutan sumber unduhan yang akan dicoba.

    Setiap butir berisi nama untuk log, alamat atau kueri untuk yt-dlp, dan
    tambahan argumen.
    """
    alamat = meta.get("url") or ""
    if not alamat:
        return []

    if meta.get("sumber") == "soundcloud":
        # Alamat SoundCloud langsung, tidak perlu mencari ulang.
        return [("soundcloud", alamat, ["--match-filter", SARINGAN_DURASI])]

    if _youtube_diblokir:
        return []

    percobaan: list[tuple[str, str, list[str]]] = []
    for klien in KLIEN_YOUTUBE:
        # Saringan durasi juga dipasang di sini karena hasil teratas YouTube pun
        # kadang berupa unggahan versi satu jam.
        tambahan = ["--match-filter", SARINGAN_DURASI]
        if klien:
            tambahan += ["--extractor-args", "youtube:player_client=" + klien]
        percobaan.append(("youtube/" + (klien or "bawaan"), alamat, tambahan))
    return percobaan


def _bersihkan(sementara: str) -> None:
    """Buang berkas separuh jadi dari percobaan yang gagal.

    Wajib, karena berkas sisa akan dikira hasil percobaan berikutnya dan lagu
    yang salah atau terpotong akhirnya ikut terkirim ke boneka.
    """
    for sisa in glob.glob(sementara + ".*"):
        try:
            os.remove(sisa)
        except OSError:
            pass


def _unduh(percobaan: list[tuple[str, str, list[str]]], sementara: str,
           batas_detik: float) -> bool:
    """Coba setiap sumber berurutan sampai ada yang menghasilkan berkas MP3."""
    tenggat = time.monotonic() + batas_detik
    for nama, alamat, tambahan in percobaan:
        sisa = tenggat - time.monotonic()
        if sisa <= 1.0:
            log.warning("Unduhan lagu dihentikan, waktunya habis sebelum %s", nama)
            break
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
                timeout=sisa, check=False,
            )
        except subprocess.TimeoutExpired:
            log.warning("Unduhan lagu kehabisan waktu lewat %s", nama)
            _bersihkan(sementara)
            continue

        # yt-dlp keluar dengan kode 101 setelah --max-downloads tercapai. Itu
        # justru pertanda berhasil, jadi keberadaan berkasnya yang menentukan.
        if os.path.exists(sementara + ".mp3"):
            log.info("Lagu berhasil diunduh lewat %s", nama)
            return True

        galat = (jalan.stderr or b"").decode("utf-8", "replace").strip()
        log.warning("Unduhan lagu gagal lewat %s: %s", nama, galat[-400:] or "tanpa keterangan")
        if nama.startswith("youtube/"):
            _catat_youtube_diblokir(galat)
        _bersihkan(sementara)
    return False


def transkode(meta: dict[str, Any], batas_detik: float = BATAS_SIAPKAN_DETIK) -> Optional[str]:
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

        if not _unduh(_daftar_percobaan(meta), sementara, batas_detik):
            return None

        dihasilkan = sementara + ".mp3"
        if not os.path.exists(dihasilkan):
            log.warning("Berkas hasil transkode tidak ditemukan")
            return None

        os.replace(dihasilkan, tujuan)
        log.info("Lagu siap: %s (%.1f MB)", nama_berkas, os.path.getsize(tujuan) / 1048576.0)
        rapikan_cache()
        return nama_berkas


def _siapkan_sinkron(judul: str, penyanyi: str) -> Optional[tuple[dict[str, Any], str]]:
    """Cari lalu unduh lagu, makin longgar sampai ada yang berhasil.

    Berhenti pada BATAS_SIAPKAN_DETIK karena firmware menunggu jawaban ini
    sambil AI terdiam. Lebih baik menyerah dan berterus terang daripada membuat
    boneka membisu lama, dan pengguna dapat meminta ulang.
    """
    tenggat = time.monotonic() + BATAS_SIAPKAN_DETIK
    dicoba: set[str] = set()

    for urutan, (kata, boleh_acak) in enumerate(_kandidat_kueri(judul, penyanyi)):
        if time.monotonic() > tenggat:
            log.warning("Penyiapan lagu dihentikan karena kehabisan waktu")
            break

        # YouTube hanya dipakai pada kueri paling tepat. Untuk kueri longgar,
        # SoundCloud memberi daftar yang jauh lebih mudah dicoba satu per satu.
        if urutan == 0 and not _youtube_diblokir:
            meta = cari_youtube(judul, penyanyi)
            if meta is not None:
                berkas = transkode(meta)
                if berkas is not None:
                    return meta, berkas

        kandidat = [p for p in cari_soundcloud(kata) if _nyambung(p, kata, boleh_acak)]
        if boleh_acak:
            random.shuffle(kandidat)

        for pilihan in kandidat[:BATAS_KANDIDAT_PER_KUERI]:
            sisa = tenggat - time.monotonic()
            if sisa <= 0:
                break
            if pilihan["url"] in dicoba:
                continue
            dicoba.add(pilihan["url"])
            berkas = transkode(pilihan, batas_detik=sisa)
            if berkas is not None:
                if urutan > 0:
                    log.info("Lagu ditemukan lewat kueri longgar %r: %s", kata, pilihan["title"])
                return pilihan, berkas

    return None


async def siapkan(judul: str, penyanyi: str, basis_url: str) -> dict[str, Any]:
    """Cari lalu siapkan lagu, kembalikan metadata dan tautan audionya."""
    ok, pesan = perkakas_tersedia()
    if not ok:
        return {"status": "error", "error": pesan}

    lup = asyncio.get_running_loop()
    hasil = await lup.run_in_executor(None, _siapkan_sinkron, judul, penyanyi)
    if hasil is None:
        return {"status": "not_found", "error": "lagu tidak ditemukan"}
    meta, nama_berkas = hasil

    return {
        "status": "found",
        "title": meta["title"],
        "artist": meta["artist"],
        "duration": 0,
        "audio_url": "%s/music/audio/%s" % (basis_url.rstrip("/"), nama_berkas),
        "lyric_url": "",
    }
