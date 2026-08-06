"""RAG data kampus dengan alur LangGraph.

Alur lengkapnya:

    masuk -> penulisan ulang kueri -> pencarian hibrida (vektor + BM25)
          -> pemeringkatan ulang wajib -> penilaian kecukupan (Corrective RAG)
          -> penyusunan jawaban -> refleksi diri -> keluar

Prinsip utama: **tidak pernah mengarang**. Jawaban hanya disusun dari potongan
dokumen yang benar-benar terambil. Bila skor pemeringkat ulang di bawah ambang,
server berterus terang bahwa datanya belum ada, bukan menebak.
"""

from __future__ import annotations

import logging
import math
import os
import re
import threading
from dataclasses import dataclass, field
from typing import Any, Optional

from config import jalur_data, jalur_vektor, pengaturan

log = logging.getLogger("riri.rag")


# ---------------------------------------------------------------------------
# Pengurai dokumen
# ---------------------------------------------------------------------------

def _baca_pdf(path: str) -> str:
    try:
        from pypdf import PdfReader

        reader = PdfReader(path)
        return "\n".join((hal.extract_text() or "") for hal in reader.pages)
    except Exception as exc:
        log.warning("Gagal membaca PDF %s: %s", path, exc)
        return ""


def _baca_docx(path: str) -> str:
    try:
        import docx

        dok = docx.Document(path)
        bagian = [p.text for p in dok.paragraphs]
        for tabel in dok.tables:
            for baris in tabel.rows:
                bagian.append(" | ".join(sel.text for sel in baris.cells))
        return "\n".join(bagian)
    except Exception as exc:
        log.warning("Gagal membaca DOCX %s: %s", path, exc)
        return ""


def _baca_html(path: str) -> str:
    try:
        from bs4 import BeautifulSoup

        with open(path, encoding="utf-8", errors="ignore") as f:
            return BeautifulSoup(f.read(), "lxml").get_text("\n")
    except Exception as exc:
        log.warning("Gagal membaca HTML %s: %s", path, exc)
        return ""


def _baca_teks(path: str) -> str:
    try:
        with open(path, encoding="utf-8", errors="ignore") as f:
            return f.read()
    except Exception as exc:
        log.warning("Gagal membaca teks %s: %s", path, exc)
        return ""


PENGURAI = {
    ".pdf": _baca_pdf,
    ".docx": _baca_docx,
    ".html": _baca_html,
    ".htm": _baca_html,
    ".txt": _baca_teks,
    ".md": _baca_teks,
    ".json": _baca_teks,
    ".csv": _baca_teks,
}


def potong(teks: str, ukuran: int = 700, tumpang_tindih: int = 120) -> list[str]:
    """Memotong teks per paragraf lalu menggabung sampai mendekati ukuran.

    Pemotongan mengikuti batas paragraf supaya satu potongan tetap utuh
    maknanya, bukan terpotong di tengah kalimat.
    """
    teks = re.sub(r"\n{3,}", "\n\n", teks.strip())
    if not teks:
        return []

    paragraf = [p.strip() for p in teks.split("\n\n") if p.strip()]
    hasil: list[str] = []
    kini = ""
    for p in paragraf:
        if len(kini) + len(p) + 2 <= ukuran:
            kini = (kini + "\n\n" + p).strip()
            continue
        if kini:
            hasil.append(kini)
        if len(p) <= ukuran:
            kini = p
        else:
            # Paragraf sangat panjang dipecah dengan tumpang tindih agar
            # kalimat di batas potongan tidak hilang konteksnya.
            for i in range(0, len(p), ukuran - tumpang_tindih):
                hasil.append(p[i:i + ukuran])
            kini = ""
    if kini:
        hasil.append(kini)
    return hasil


# ---------------------------------------------------------------------------
# Indeks: vektor + BM25
# ---------------------------------------------------------------------------

@dataclass
class Potongan:
    teks: str
    sumber: str
    indeks: int


def _tokenisasi(teks: str) -> list[str]:
    return re.findall(r"[a-z0-9]+", teks.lower())


class Bm25:
    """Pencarian kata kunci sederhana sebagai pasangan pencarian vektor.

    Pencarian hibrida diperlukan karena pertanyaan kampus banyak memuat nama
    diri, singkatan prodi, dan angka semester yang sering meleset bila hanya
    diandalkan pada kemiripan vektor.
    """

    def __init__(self, dokumen: list[list[str]]):
        self.dokumen = dokumen
        self.n = len(dokumen)
        self.panjang = [len(d) for d in dokumen]
        self.rata = sum(self.panjang) / self.n if self.n else 0.0
        self.df: dict[str, int] = {}
        for d in dokumen:
            for t in set(d):
                self.df[t] = self.df.get(t, 0) + 1
        self.idf = {
            t: math.log(1 + (self.n - df + 0.5) / (df + 0.5)) for t, df in self.df.items()
        }

    def skor(self, kueri: str, k1: float = 1.5, b: float = 0.75) -> list[float]:
        token = _tokenisasi(kueri)
        hasil = [0.0] * self.n
        for i, d in enumerate(self.dokumen):
            if not d:
                continue
            hitung: dict[str, int] = {}
            for t in d:
                hitung[t] = hitung.get(t, 0) + 1
            nilai = 0.0
            for t in token:
                f = hitung.get(t, 0)
                if f == 0:
                    continue
                idf = self.idf.get(t, 0.0)
                nilai += idf * (f * (k1 + 1)) / (f + k1 * (1 - b + b * self.panjang[i] / (self.rata or 1)))
            hasil[i] = nilai
        return hasil


class IndeksKampus:
    """Menyimpan potongan dokumen beserta indeks vektor dan BM25."""

    def __init__(self) -> None:
        self.potongan: list[Potongan] = []
        self.bm25: Optional[Bm25] = None
        self._embedder = None
        self._reranker = None
        self._koleksi = None
        self._gembok = threading.Lock()
        self.siap = False

    # -------------------------------------------------------------- model
    def embedder(self):
        if self._embedder is None:
            from sentence_transformers import SentenceTransformer

            log.info("Memuat model embedding %s", pengaturan().embedding_model)
            self._embedder = SentenceTransformer(pengaturan().embedding_model)
        return self._embedder

    def reranker(self):
        if self._reranker is None:
            from sentence_transformers import CrossEncoder

            log.info("Memuat model pemeringkat ulang %s", pengaturan().reranker_model)
            self._reranker = CrossEncoder(pengaturan().reranker_model)
        return self._reranker

    # ------------------------------------------------------------ bangun
    def bangun(self) -> None:
        with self._gembok:
            if self.siap:
                return

            direktori = jalur_data()
            os.makedirs(direktori, exist_ok=True)
            berkas: list[str] = []
            for akar, _, nama_berkas in os.walk(direktori):
                for nama in nama_berkas:
                    if os.path.splitext(nama)[1].lower() in PENGURAI:
                        berkas.append(os.path.join(akar, nama))

            for path in sorted(berkas):
                ext = os.path.splitext(path)[1].lower()
                teks = PENGURAI[ext](path)
                nama = os.path.relpath(path, direktori)
                for i, bagian in enumerate(potong(teks)):
                    self.potongan.append(Potongan(teks=bagian, sumber=nama, indeks=i))

            log.info("Memuat %d potongan dari %d berkas", len(self.potongan), len(berkas))

            if self.potongan:
                self.bm25 = Bm25([_tokenisasi(p.teks) for p in self.potongan])
                self._bangun_vektor()

            self.siap = True

    def _bangun_vektor(self) -> None:
        try:
            import chromadb

            klien = chromadb.PersistentClient(path=jalur_vektor())
            nama = "kampus"
            try:
                klien.delete_collection(nama)
            except Exception:
                pass
            koleksi = klien.create_collection(nama, metadata={"hnsw:space": "cosine"})

            teks = [p.teks for p in self.potongan]
            vektor = self.embedder().encode(teks, batch_size=32, show_progress_bar=False)
            koleksi.add(
                ids=[f"{i}" for i in range(len(teks))],
                documents=teks,
                embeddings=[v.tolist() for v in vektor],
                metadatas=[{"sumber": p.sumber, "indeks": p.indeks} for p in self.potongan],
            )
            self._koleksi = koleksi
            log.info("Basis data vektor siap dengan %d potongan", len(teks))
        except Exception as exc:
            # Tanpa basis data vektor, pencarian tetap jalan memakai BM25 saja.
            log.warning("Basis data vektor tidak dapat dibangun (%s), memakai BM25 saja", exc)
            self._koleksi = None

    # ---------------------------------------------------------- pencarian
    def cari_hibrida(self, kueri: str, top_k: int) -> list[tuple[Potongan, float]]:
        """Gabungkan hasil vektor dan BM25 memakai Reciprocal Rank Fusion."""
        if not self.potongan:
            return []

        peringkat_vektor: dict[int, int] = {}
        if self._koleksi is not None:
            try:
                v = self.embedder().encode([kueri])[0].tolist()
                hasil = self._koleksi.query(query_embeddings=[v], n_results=min(top_k, len(self.potongan)))
                for urutan, ident in enumerate(hasil["ids"][0]):
                    peringkat_vektor[int(ident)] = urutan
            except Exception as exc:
                log.warning("Pencarian vektor gagal: %s", exc)

        peringkat_bm25: dict[int, int] = {}
        if self.bm25 is not None:
            skor = self.bm25.skor(kueri)
            urut = sorted(range(len(skor)), key=lambda i: skor[i], reverse=True)[:top_k]
            for urutan, i in enumerate(urut):
                if skor[i] > 0:
                    peringkat_bm25[i] = urutan

        # Reciprocal Rank Fusion menyatukan dua daftar peringkat tanpa perlu
        # menyamakan skala skor yang satuannya memang berbeda.
        gabungan: dict[int, float] = {}
        for daftar in (peringkat_vektor, peringkat_bm25):
            for i, urutan in daftar.items():
                gabungan[i] = gabungan.get(i, 0.0) + 1.0 / (60 + urutan)

        terbaik = sorted(gabungan.items(), key=lambda kv: kv[1], reverse=True)[:top_k]
        return [(self.potongan[i], nilai) for i, nilai in terbaik]

    def peringkat_ulang(self, kueri: list[str] | str,
                        kandidat: list[tuple[Potongan, float]]) -> list[tuple[Potongan, float]]:
        """Pemeringkatan ulang wajib memakai cross-encoder.

        Menerima beberapa bentuk kueri dan memakai skor tertinggi di antaranya.
        Ini diperlukan karena dokumen tidak konsisten: sebagian menyebut kampus
        dengan singkatan dan sebagian dengan nama panjang, sedangkan penanya
        bebas memakai bentuk yang mana pun. Menilai kedua bentuk membuat
        keduanya sama-sama terjawab tanpa harus menyeragamkan isi dokumen.
        """
        if not kandidat:
            return []
        bentuk = [kueri] if isinstance(kueri, str) else [q for q in kueri if q]
        if not bentuk:
            return kandidat
        try:
            model = self.reranker()
            tertinggi = [0.0] * len(kandidat)
            for q in bentuk:
                skor = model.predict([(q, p.teks) for p, _ in kandidat])
                for i, nilai in enumerate(skor):
                    # Skor cross-encoder berupa logit, diubah ke rentang nol
                    # sampai satu supaya ambang kecukupan mudah dibaca manusia.
                    sig = 1.0 / (1.0 + math.exp(-float(nilai)))
                    if sig > tertinggi[i]:
                        tertinggi[i] = sig
            hasil = [(kandidat[i][0], tertinggi[i]) for i in range(len(kandidat))]
            hasil.sort(key=lambda kv: kv[1], reverse=True)
            return hasil
        except Exception as exc:
            log.warning("Pemeringkat ulang gagal (%s), memakai urutan pencarian", exc)
            return kandidat


INDEKS = IndeksKampus()


# ---------------------------------------------------------------------------
# Memori percakapan singkat
# ---------------------------------------------------------------------------

_memori: dict[str, list[str]] = {}
_memori_gembok = threading.Lock()


def ingat(sesi: str, kueri: str) -> None:
    if not sesi:
        return
    with _memori_gembok:
        riwayat = _memori.setdefault(sesi, [])
        riwayat.append(kueri)
        del riwayat[:-4]  # cukup empat pertanyaan terakhir


def riwayat(sesi: str) -> list[str]:
    if not sesi:
        return []
    with _memori_gembok:
        return list(_memori.get(sesi, []))


# ---------------------------------------------------------------------------
# Alur LangGraph
# ---------------------------------------------------------------------------

@dataclass
class Keadaan:
    kueri: str
    sesi: str = ""
    # Kueri untuk tahap pencarian: kueri asli ditambah perluasan singkatan.
    kueri_cari: str = ""
    # Kueri untuk tahap pemeringkatan ulang: kalimat alami, ditambah bentuk
    # yang singkatannya dipendekkan bila memang ada.
    kueri_peringkat: list[str] = field(default_factory=list)
    rute: str = "kampus"
    kandidat: list[tuple[Potongan, float]] = field(default_factory=list)
    terpilih: list[tuple[Potongan, float]] = field(default_factory=list)
    jawaban: str = ""
    cukup: bool = False
    catatan: str = ""


SINGKATAN = {
    "ti": "teknik informatika",
    "si": "sistem informasi",
    "dkv": "desain komunikasi visual",
    "mi": "manajemen informatika",
    "akt": "akuntansi",
    "mjn": "manajemen",
    "ucic": "universitas catur insan cendekia",
    "cic": "universitas catur insan cendekia",
}

KATA_KAMPUS = (
    "ucic", "cic", "kampus", "universitas", "rektor", "dosen", "prodi", "jurusan",
    "fakultas", "semester", "mata kuliah", "matkul", "kurikulum", "akreditasi",
    "biaya", "dpp", "pmb", "beasiswa", "wisuda", "skripsi", "magang", "alamat",
    "visi", "misi", "fasilitas", "laboratorium", "pendaftaran",
)


def simpul_rute(k: Keadaan) -> Keadaan:
    """Router: tentukan apakah pertanyaan ini soal kampus atau bukan."""
    rendah = k.kueri.lower()
    k.rute = "kampus" if any(kata in rendah for kata in KATA_KAMPUS) else "umum"
    return k


def simpul_tulis_ulang(k: Keadaan) -> Keadaan:
    """Perluas singkatan dan sambungkan konteks dari pertanyaan sebelumnya.

    Transkrip suara sering memendekkan atau salah dengar. Penulisan ulang ini
    membuat pencarian tetap mengena tanpa perlu model bahasa besar.

    Hasilnya dua kueri, bukan satu, karena kedua tahap berikutnya menginginkan
    bentuk yang berlawanan:

    - Pencarian ingin kosakata seluas mungkin. Dokumen menulis prodi kadang
      sebagai singkatan dan kadang sebagai nama panjang, jadi keduanya perlu
      ada agar BM25 maupun vektor sama-sama menemukannya.
    - Pemeringkat ulang justru ingin kalimat alami. Ia menilai kecocokan
      pertanyaan dengan potongan, sehingga kueri yang penuh perluasan menjadi
      kalimat janggal yang skornya anjlok. "siapa rektor ucic" dinilai 0,98
      terhadap potongan yang benar, sementara bentuk perluasannya hanya 0,01.
    """
    teks = k.kueri.strip()
    rendah = teks.lower()

    # Kumpulkan perluasan sebagai tambahan, bukan pengganti, supaya kata asli
    # yang dipakai dokumen tidak ikut hilang.
    perluasan: list[str] = []
    for pendek, panjang in SINGKATAN.items():
        if re.search(rf"\b{pendek}\b", rendah) and panjang not in rendah:
            perluasan.append(panjang)

    sebelumnya = riwayat(k.sesi)
    if sebelumnya and len(teks.split()) <= 4:
        # Pertanyaan susulan yang sangat pendek diberi konteks sebelumnya,
        # misalnya "kalau yang semester dua?" setelah bertanya mata kuliah.
        teks = sebelumnya[-1] + " " + teks
        rendah = teks.lower()

    # Bentuk kedua: nama panjang dipendekkan kembali menjadi singkatan, supaya
    # pertanyaan "rektor Universitas Catur Insan Cendekia" tetap cocok dengan
    # potongan yang menulisnya "Rektor UCIC".
    dipendekkan = rendah
    for pendek, panjang in SINGKATAN.items():
        if panjang in dipendekkan:
            dipendekkan = dipendekkan.replace(panjang, pendek)

    k.kueri_peringkat = [teks]
    if dipendekkan != rendah:
        k.kueri_peringkat.append(dipendekkan)
    k.kueri_cari = " ".join([rendah] + perluasan).strip()
    return k


def simpul_cari(k: Keadaan) -> Keadaan:
    cfg = pengaturan()
    INDEKS.bangun()
    k.kandidat = INDEKS.cari_hibrida(k.kueri_cari or k.kueri, cfg.retrieve_top_k)
    return k


def simpul_peringkat(k: Keadaan) -> Keadaan:
    cfg = pengaturan()
    diperingkat = INDEKS.peringkat_ulang(k.kueri_peringkat or k.kueri, k.kandidat)
    k.terpilih = diperingkat[: cfg.rerank_top_k]
    return k


def simpul_koreksi(k: Keadaan) -> Keadaan:
    """Corrective RAG: nilai apakah potongan yang didapat cukup mendukung.

    Bila skor tertinggi masih di bawah ambang, pencarian diulang sekali dengan
    kueri yang lebih longgar sebelum menyerah.
    """
    cfg = pengaturan()
    tertinggi = k.terpilih[0][1] if k.terpilih else 0.0
    if tertinggi >= cfg.rerank_min_score:
        k.cukup = True
        return k

    if k.catatan != "diulang":
        k.catatan = "diulang"
        # Percobaan kedua melebarkan pencarian dengan kata kunci inti saja,
        # tetapi tetap memeringkat memakai kalimat aslinya dengan alasan yang
        # sama seperti pada simpul_tulis_ulang.
        inti = " ".join(t for t in _tokenisasi(k.kueri_cari or k.kueri) if len(t) > 3)
        if inti:
            k.kandidat = INDEKS.cari_hibrida(inti, cfg.retrieve_top_k)
            k.terpilih = INDEKS.peringkat_ulang(
                k.kueri_peringkat or k.kueri, k.kandidat)[: cfg.rerank_top_k]
            tertinggi = k.terpilih[0][1] if k.terpilih else 0.0

    k.cukup = tertinggi >= cfg.rerank_min_score
    return k


def simpul_susun(k: Keadaan) -> Keadaan:
    cfg = pengaturan()
    if not k.cukup or not k.terpilih:
        k.jawaban = ""
        return k

    bagian: list[str] = []
    sumber_terpakai: set[str] = set()
    total = 0
    for p, skor in k.terpilih:
        if skor < cfg.rerank_min_score:
            continue
        blok = p.teks.strip()
        if total + len(blok) > cfg.answer_max_chars:
            blok = blok[: max(0, cfg.answer_max_chars - total)]
        if not blok:
            break
        bagian.append(blok)
        sumber_terpakai.add(p.sumber)
        total += len(blok)
        if total >= cfg.answer_max_chars:
            break

    if not bagian:
        k.jawaban = ""
        return k

    k.jawaban = (
        "Data kampus UCIC yang relevan. Jawab dalam bahasa Indonesia penuh dan "
        "gunakan hanya keterangan di bawah ini.\n\n" + "\n\n".join(bagian)
    )
    if sumber_terpakai:
        k.jawaban += "\n\nSumber: " + ", ".join(sorted(sumber_terpakai))
    return k


def simpul_refleksi(k: Keadaan) -> Keadaan:
    """Self-reflection: pastikan jawaban benar-benar bersumber dari dokumen.

    Bila jawaban kosong atau tidak memuat satu pun kata kunci pertanyaan,
    jawaban ditolak supaya perangkat memakai basis data lokalnya sendiri
    daripada menyampaikan keterangan yang tidak didukung.
    """
    if not k.jawaban:
        k.cukup = False
        return k

    kata = [t for t in _tokenisasi(k.kueri_cari or k.kueri) if len(t) > 3]
    if kata:
        rendah = k.jawaban.lower()
        cocok = sum(1 for t in kata if t in rendah)
        if cocok == 0:
            log.info("Refleksi menolak jawaban: tidak ada kata kunci yang cocok")
            k.cukup = False
            k.jawaban = ""
    return k


def _bangun_graf():
    """Susun alur memakai LangGraph bila tersedia."""
    try:
        from langgraph.graph import END, StateGraph

        graf = StateGraph(Keadaan)
        graf.add_node("rute", simpul_rute)
        graf.add_node("tulis_ulang", simpul_tulis_ulang)
        graf.add_node("cari", simpul_cari)
        graf.add_node("peringkat", simpul_peringkat)
        graf.add_node("koreksi", simpul_koreksi)
        graf.add_node("susun", simpul_susun)
        graf.add_node("refleksi", simpul_refleksi)

        graf.set_entry_point("rute")
        graf.add_edge("rute", "tulis_ulang")
        graf.add_edge("tulis_ulang", "cari")
        graf.add_edge("cari", "peringkat")
        graf.add_edge("peringkat", "koreksi")
        graf.add_edge("koreksi", "susun")
        graf.add_edge("susun", "refleksi")
        graf.add_edge("refleksi", END)
        return graf.compile()
    except Exception as exc:
        log.warning("LangGraph tidak tersedia (%s), memakai alur berurutan", exc)
        return None


_GRAF = None
_graf_siap = False


def _graf():
    global _GRAF, _graf_siap
    if not _graf_siap:
        _graf_siap = True
        _GRAF = _bangun_graf()
    return _GRAF


def tanya(kueri: str, sesi: str = "") -> dict[str, Any]:
    """Jalankan alur RAG dan kembalikan jawaban beserta status kecukupannya."""
    keadaan = Keadaan(kueri=kueri, sesi=sesi)

    graf = _graf()
    if graf is not None:
        hasil = graf.invoke(keadaan)
        # LangGraph dapat mengembalikan kamus atau objek keadaan.
        if isinstance(hasil, dict):
            keadaan = Keadaan(**{k: v for k, v in hasil.items() if k in Keadaan.__dataclass_fields__})
        else:
            keadaan = hasil
    else:
        for simpul in (simpul_rute, simpul_tulis_ulang, simpul_cari,
                       simpul_peringkat, simpul_koreksi, simpul_susun, simpul_refleksi):
            keadaan = simpul(keadaan)

    ingat(sesi, kueri)

    return {
        "status": "found" if keadaan.cukup and keadaan.jawaban else "not_found",
        "answer": keadaan.jawaban,
        "tool_text": keadaan.jawaban,
        "route": keadaan.rute,
        "score": round(keadaan.terpilih[0][1], 4) if keadaan.terpilih else 0.0,
        "sources": sorted({p.sumber for p, _ in keadaan.terpilih}) if keadaan.terpilih else [],
    }
