"""Server Riri: RAG data kampus, pencarian web, dan pemutar musik.

Endpoint yang dipakai firmware:

    GET /campus/query?q=<pertanyaan>
    GET /search?q=<kueri>&region=id-id
    GET /music/resolve?song=<judul>&artist=<penyanyi>
    GET /music/audio/<berkas>.mp3
    GET /health

Permintaan musik membawa header autentikasi kunci dinamis yang sama dengan
firmware. Endpoint RAG dan pencarian sengaja tidak menuntut autentikasi supaya
mudah diuji langsung dari peramban.
"""

from __future__ import annotations

import hashlib
import logging
import os
from contextlib import asynccontextmanager
from typing import Optional

from fastapi import FastAPI, Header, HTTPException, Query, Request
from fastapi.responses import FileResponse, JSONResponse

import music
import rag
import websearch
from config import pengaturan

logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s %(levelname)s %(name)s: %(message)s",
)
log = logging.getLogger("riri.app")

@asynccontextmanager
async def daur_hidup(_: FastAPI):
    """Persiapan saat server mulai.

    Memakai lifespan, bukan dekorator on_event yang sudah usang dan dibuang
    pada FastAPI versi baru.
    """
    log.info("Server Riri mulai")
    ok, pesan = music.perkakas_tersedia()
    if not ok:
        log.warning("Fitur musik belum siap: %s", pesan)
    if not pengaturan().youtube_api_key:
        log.warning("YOUTUBE_API_KEY belum diisi, pencarian lagu tidak akan berjalan")
    try:
        # Indeks dibangun sekali di awal supaya permintaan pertama tidak lambat.
        rag.INDEKS.bangun()
    except Exception:
        log.exception("Pembangunan indeks RAG gagal, server tetap berjalan")
    yield
    log.info("Server Riri berhenti")


app = FastAPI(
    title="Server Riri Boneka",
    description="RAG data kampus, pencarian web, dan pemutar musik untuk boneka AI Riri.",
    version="1.1.0",
    lifespan=daur_hidup,
)


# ---------------------------------------------------------------------------
# Autentikasi
# ---------------------------------------------------------------------------

def periksa_kunci(mac: Optional[str], chip: Optional[str],
                  stempel: Optional[str], kunci: Optional[str]) -> bool:
    """Periksa header X-Dynamic-Key seperti yang dibentuk firmware.

    Firmware menghitung SHA256 dari "MAC:ChipID:Timestamp:KunciRahasia" lalu
    menuliskan 16 byte pertama sebagai heksadesimal huruf besar.

    Stempel waktu tidak dibandingkan dengan jam server karena firmware memakai
    lama nyala perangkat, bukan waktu dunia nyata.
    """
    cfg = pengaturan()
    if not cfg.require_auth:
        return True
    if not all((mac, chip, stempel, kunci)):
        return False
    data = "%s:%s:%s:%s" % (mac, chip, stempel, cfg.riri_secret_key)
    harapan = hashlib.sha256(data.encode("utf-8")).hexdigest()[:32].upper()
    return harapan == kunci.upper()


def basis_url(request: Request) -> str:
    """Alamat publik server, mengikuti header proxy bila ada.

    Cloud Run menaruh layanan di belakang proxy, jadi skema dan host harus
    dibaca dari header X-Forwarded-* agar tautan audio dapat diakses perangkat.
    """
    skema = request.headers.get("x-forwarded-proto") or request.url.scheme
    host = request.headers.get("x-forwarded-host") or request.headers.get("host")
    if not host:
        host = request.url.netloc
    return "%s://%s" % (skema, host)


# ---------------------------------------------------------------------------
# Endpoint
# ---------------------------------------------------------------------------

@app.get("/health")
async def health():
    ok, pesan = music.perkakas_tersedia()
    return {
        "status": "ok",
        "rag_siap": rag.INDEKS.siap,
        "jumlah_potongan": len(rag.INDEKS.potongan),
        "musik_siap": ok,
        "catatan_musik": pesan,
        "youtube_key_terisi": bool(pengaturan().youtube_api_key),
    }


@app.get("/campus/query")
async def campus_query(q: str = Query(..., description="Pertanyaan tentang kampus"),
                       sesi: str = Query("", description="Penanda sesi untuk memori")):
    """RAG data kampus.

    Mengembalikan status not_found bila potongan yang terambil tidak cukup
    mendukung. Firmware akan memakai basis data kampus lokalnya sendiri,
    sehingga pertanyaan pengguna tetap terjawab tanpa mengarang.
    """
    if not q.strip():
        raise HTTPException(status_code=400, detail="parameter q kosong")
    try:
        hasil = rag.tanya(q, sesi)
    except Exception as exc:
        log.exception("RAG gagal")
        return JSONResponse(status_code=200, content={"status": "not_found", "error": str(exc)})
    return hasil


@app.get("/search")
async def search(q: str = Query(..., description="Kata kunci pencarian"),
                 region: str = Query("id-id", description="Wilayah hasil, contoh id-id"),
                 maks: int = Query(5, ge=1, le=10)):
    """Pencarian web memakai DuckDuckGo."""
    if not q.strip():
        raise HTTPException(status_code=400, detail="parameter q kosong")
    return await websearch.cari(q, region=region, maks=maks)


@app.get("/music/resolve")
async def music_resolve(request: Request,
                        song: str = Query("", description="Judul lagu"),
                        artist: str = Query("", description="Nama penyanyi"),
                        x_mac_address: Optional[str] = Header(None),
                        x_chip_id: Optional[str] = Header(None),
                        x_timestamp: Optional[str] = Header(None),
                        x_dynamic_key: Optional[str] = Header(None)):
    """Cari lalu siapkan lagu, kembalikan tautan MP3 siap putar."""
    if not periksa_kunci(x_mac_address, x_chip_id, x_timestamp, x_dynamic_key):
        log.warning("Autentikasi musik ditolak, periksa RIRI_SECRET_KEY")
        return JSONResponse(status_code=401,
                            content={"status": "error", "error": "verifikasi kunci dinamis gagal"})

    if not song.strip() and not artist.strip():
        return JSONResponse(status_code=400,
                            content={"status": "error", "error": "parameter song dan artist kosong"})

    hasil = await music.siapkan(song, artist, basis_url(request))
    kode = 200 if hasil.get("status") == "found" else 404
    return JSONResponse(status_code=kode, content=hasil)


@app.get("/music/audio/{nama_berkas}")
async def music_audio(nama_berkas: str):
    """Sajikan berkas MP3 yang sudah ditranskode."""
    aman = os.path.basename(nama_berkas)
    jalur = os.path.join(music.DIREKTORI_CACHE, aman)
    if not os.path.isfile(jalur):
        raise HTTPException(status_code=404, detail="berkas tidak ada")
    # FileResponse sudah mendukung permintaan Range, yang berguna saat menguji
    # berkas langsung dari peramban atau pemutar audio biasa.
    return FileResponse(jalur, media_type="audio/mpeg", filename=aman)


if __name__ == "__main__":
    import uvicorn

    uvicorn.run("app:app", host="0.0.0.0", port=int(os.environ.get("PORT", "8000")))
