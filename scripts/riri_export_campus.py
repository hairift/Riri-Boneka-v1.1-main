"""Ekstrak basis data kampus dari campus_data.cc menjadi berkas Markdown.

Server RAG dan firmware jadi memakai sumber data kampus yang sama persis,
sehingga jawaban dari server dan jawaban lokal tidak pernah bertentangan.
"""
import io
import os
import re

AKAR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
src = os.path.join(AKAR, 'main', 'campus_data.cc')
tujuan_dir = os.path.join(AKAR, 'server', 'data')

s = io.open(src, encoding='utf-8', errors='ignore').read()

# Entri berbentuk: {"id","kategori","judul","isi","kata kunci"}
#
# Pemisah antar bidang harus mengizinkan spasi dan pergantian baris. Sebagian
# besar entri ditulis memanjang ke bawah, satu bidang per baris, dan pola lama
# yang menuntut `","` tanpa spasi hanya cocok untuk entri satu baris.
#
# Akibatnya seluruh entri profil kampus, termasuk rektor, visi misi, alamat,
# biaya, dan kontak, tidak pernah ikut terekspor. Pertanyaan tentang alamat
# kampus lalu dijawab tidak ditemukan oleh server padahal datanya ada di
# firmware sejak awal.
PEMISAH = r'"\s*,\s*"'
BIDANG = r'((?:[^"\\]|\\.)*)'
pola = re.compile(
    r'\{"([^"]*)' + PEMISAH + r'([^"]*)' + PEMISAH + BIDANG + PEMISAH +
    BIDANG + PEMISAH + BIDANG + r'"\s*\}'
)
entri = pola.findall(s)
print('entri ditemukan:', len(entri))

os.makedirs(tujuan_dir, exist_ok=True)

def buka_escape(t):
    t = t.replace('\\n', '\n')
    t = t.replace('\\"', '"')
    t = t.replace('\\\\', '\\')
    return t

blok = []
for ident, kategori, judul, isi, kunci in entri:
    judul_bersih = buka_escape(judul)
    isi_bersih = buka_escape(isi)
    kunci_bersih = buka_escape(kunci)
    blok.append(
        "# %s\n\nKategori: %s\nKata kunci: %s\n\n%s\n"
        % (judul_bersih, kategori, kunci_bersih, isi_bersih)
    )

tujuan = os.path.join(tujuan_dir, 'kampus_ucic.md')
with io.open(tujuan, 'w', encoding='utf-8', newline='\n') as f:
    # Berkas sengaja tidak diberi judul maupun keterangan pembuka.
    #
    # Teks pembuka ikut terpotong menjadi satu potongan tersendiri, lalu
    # memenangkan pencarian untuk hampir semua pertanyaan kampus karena
    # memuat nama kampusnya. Akibatnya jawaban yang dikirim ke boneka
    # berisi keterangan berkas, bukan data yang ditanyakan.
    f.write("\n\n---\n\n".join(blok))

print('ditulis:', tujuan, os.path.getsize(tujuan), 'byte')
