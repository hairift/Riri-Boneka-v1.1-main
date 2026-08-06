"""Pencarian web untuk Riri.

Memakai AsyncDDGS supaya permintaan tidak memblokir proses, sehingga suara
boneka tidak putus-putus saat pencarian berjalan. Hasil disinggahkan sepuluh
menit agar berita yang sama tidak ditembak berulang kali dan tidak kena batas
laju DuckDuckGo.

Jawaban yang dikirim ke perangkat sengaja dipangkas menjadi dua kalimat pertama
supaya boneka cepat mulai berbicara.
"""

from __future__ import annotations

import hashlib
import logging
import re
from typing import Any, Optional

import cache
from config import pengaturan

log = logging.getLogger("riri.search")

# Kata yang menandakan pengguna meminta berita, bukan pengetahuan umum.
KATA_BERITA = (
    "berita", "kabar", "terkini", "terbaru", "hari ini", "minggu ini",
    "bulan ini", "update", "news", "peristiwa", "kejadian",
)

# Rentang waktu DuckDuckGo: d harian, w mingguan, m bulanan, y tahunan.
RENTANG = {
    "hari ini": "d",
    "harian": "d",
    "minggu ini": "w",
    "mingguan": "w",
    "bulan ini": "m",
    "bulanan": "m",
    "tahun ini": "y",
}


def _kunci(prefiks: str, *bagian: str) -> str:
    mentah = "|".join(bagian)
    return f"riri:{prefiks}:" + hashlib.sha256(mentah.encode("utf-8")).hexdigest()[:24]


def deteksi_berita(kueri: str) -> bool:
    rendah = kueri.lower()
    return any(kata in rendah for kata in KATA_BERITA)


def deteksi_rentang(kueri: str) -> Optional[str]:
    rendah = kueri.lower()
    for frasa, kode in RENTANG.items():
        if frasa in rendah:
            return kode
    return None


def batasi_kalimat(teks: str, maksimum: int) -> str:
    """Ambil beberapa kalimat pertama saja.

    Membacakan seluruh isi halaman membuat jawaban suara sangat panjang dan
    terasa lambat, jadi hanya inti yang dikirim ke perangkat.
    """
    teks = re.sub(r"\s+", " ", teks or "").strip()
    if not teks:
        return ""
    kalimat = re.split(r"(?<=[.!?])\s+", teks)
    return " ".join(kalimat[:maksimum]).strip()


async def cari(kueri: str, region: str = "id-id", maks: int = 5) -> dict[str, Any]:
    """Cari di DuckDuckGo lalu susun jawaban ringkas."""
    cfg = pengaturan()
    berita = deteksi_berita(kueri)
    rentang = deteksi_rentang(kueri)

    kunci = _kunci("search", kueri.lower(), region, str(berita), rentang or "-")
    tersimpan = cache.ambil(kunci)
    if tersimpan is not None:
        log.info("Hasil pencarian diambil dari singgahan: %s", kueri)
        return tersimpan

    hasil: list[dict[str, str]] = []
    try:
        from duckduckgo_search import AsyncDDGS

        async with AsyncDDGS() as ddgs:
            if berita:
                # news() khusus berita, mendukung penyaring rentang waktu.
                items = await ddgs.anews(
                    kueri, region=region, safesearch="moderate",
                    timelimit=rentang or "w", max_results=maks,
                )
                for it in items or []:
                    hasil.append({
                        "judul": it.get("title", ""),
                        "isi": it.get("body", ""),
                        "url": it.get("url", ""),
                        "sumber": it.get("source", ""),
                        "tanggal": it.get("date", ""),
                    })
            else:
                items = await ddgs.atext(
                    kueri, region=region, safesearch="moderate",
                    timelimit=rentang, max_results=maks,
                )
                for it in items or []:
                    hasil.append({
                        "judul": it.get("title", ""),
                        "isi": it.get("body", ""),
                        "url": it.get("href", ""),
                        "sumber": "",
                        "tanggal": "",
                    })
    except Exception as exc:
        log.warning("Pencarian DuckDuckGo gagal: %s", exc)

    if not hasil:
        jawaban = ""
    else:
        utama = hasil[0]
        inti = batasi_kalimat(utama.get("isi", ""), cfg.tts_max_sentences)
        bagian = []
        if utama.get("judul"):
            bagian.append(utama["judul"].strip())
        if inti:
            bagian.append(inti)
        jawaban = ". ".join(b.rstrip(".") for b in bagian if b)
        if jawaban:
            jawaban += "."
        # Satu hasil pendukung ditambahkan agar AI punya pembanding, tetapi
        # tetap singkat supaya jawaban suara tidak bertele-tele.
        if len(hasil) > 1:
            kedua = batasi_kalimat(hasil[1].get("isi", ""), 1)
            if kedua:
                jawaban += " " + kedua

    balasan = {
        "status": "found" if jawaban else "not_found",
        "answer": jawaban,
        "is_news": berita,
        "timelimit": rentang,
        "region": region,
        "results": hasil[:maks],
    }

    if jawaban:
        cache.simpan(kunci, balasan, cfg.cache_ttl_search)
    return balasan
