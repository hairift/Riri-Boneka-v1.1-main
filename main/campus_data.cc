#include "campus_data.h"
#include "campus_index_generated.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

// =====================================================================
// Basis pengetahuan kampus UCIC yang disimpan di flash (rodata)
// Dioptimalkan untuk ESP32-S3: ringkas, tetapi tetap lengkap untuk operasi kampus
// =====================================================================

static const CampusEntry CAMPUS_DB[] = {

// ===== PROFIL KAMPUS =====
{"profil_ucic","profil","Profil UCIC",
"UCIC (Universitas Catur Insan Cendekia) adalah universitas di Cirebon yang berdiri tahun 2019 dari penggabungan STMIK CIC dan AP CIC. Fokus pada teknologi, bisnis, dan sains untuk menghasilkan lulusan yang adaptif dan kompeten di era digital.",
"ucic,cic,universitas,catur insan,cendekia,cirebon,kampus,sejarah,university,profile,about,tentang"},

{"rektor","profil","Rektor UCIC",
"Rektor UCIC adalah Assoc. Prof. Dr. Chandra Lukita, S.E., M.M., M.TI.",
"rektor,pimpinan,ketua,chandra,lukita,rector,president,leader"},

{"visi_misi","visi_misi","Visi dan Misi UCIC",
"Visi: Menjadi universitas berbasis teknologi dan kewirausahaan yang inovatif, adaptif, dan berdaya saing global.\nMisi: Menghasilkan lulusan berkualitas, mendorong inovasi teknologi dan bisnis, melaksanakan Tri Dharma Perguruan Tinggi, menyediakan pendidikan terjangkau, serta mengembangkan kerja sama nasional dan internasional.",
"visi,misi,tujuan,orientasi,cita cita,vision,mission,goal"},

{"great_values","nilai","CIC GREAT - Nilai Dasar UCIC",
"Nilai dasar UCIC disebut CIC GREAT: Creativity, Initiative, Critical Thinking, Grit, Respectful, Entrepreneurship, Accountable, dan Teamwork.",
"great,nilai,nilai dasar,nilai kampus,karakter kampus,budaya kampus,cic great,ucic great,great values,core value,core values,creativity,initiative,critical thinking,grit,respectful,entrepreneurship,accountable,teamwork"},

{"logo_ucic","profil","Filosofi Logo UCIC",
"Perisai biru melambangkan kejujuran, emas berarti optimisme, obor-buku-pena mewakili semangat akademik, serta udang dan Mega Mendung sebagai identitas budaya Cirebon.",
"logo,filosofi,lambang,simbol,makna,symbol,meaning"},

{"lokasi_kampus","lokasi","Lokasi Kampus UCIC",
"UCIC memiliki dua kampus di Cirebon:\n- Kampus 1: Jl. Kesambi No. 202\n- Kampus 2: Jl. Kesambi No. 58A",
"lokasi,alamat,kampus,kesambi,cirebon,gedung,tempat,location,address,where,dimana"},

{"kontak_ucic","kontak","Kontak UCIC",
"Alamat: Jl. Kesambi No. 202, Cirebon.\nEmail: info@ucic.ac.id\nWhatsApp/Telepon: +62 895-1231-4188",
"kontak,email,telepon,nomor,hubungi,whatsapp,wa,hp,contact,phone,call"},

{"akreditasi","akreditasi","Akreditasi UCIC",
"Akreditasi institusi UCIC adalah Baik Sekali dari BAN-PT.",
"akreditasi,mutu,kualitas,ban pt,peringkat,baik sekali,accreditation,quality,ranking"},

{"akreditasi_prodi","akreditasi","Akreditasi Program Studi UCIC",
"Akreditasi program studi UCIC:\n- Teknik Informatika: Baik Sekali\n- Sistem Informasi: Baik Sekali\n- Desain Komunikasi Visual: Baik\n- Akuntansi: Baik\n- Manajemen: Baik Sekali\n- Bisnis Digital: Baik\n- Pendidikan Kepelatihan Keolahragaan: Baik\n- Manajemen Informatika: Baik Sekali\n- Manajemen Bisnis: Baik Sekali",
"akreditasi prodi,akreditasi jurusan,akreditasi ti,akreditasi si,akreditasi dkv,akreditasi akt,akreditasi akuntansi,akreditasi mjn,akreditasi manajemen,akreditasi bisdi,akreditasi bisnis digital,akreditasi pikor,akreditasi pkor,akreditasi mi,akreditasi mb,baik sekali,baik,ban pt"},

{"fasilitas","fasilitas","Fasilitas UCIC",
"Fasilitas Universitas Catur Insan Cendekia:\n- Campus 1\n- Campus Hub\n- Convention Hall\n- Multimedia, DM, 3D Virtual Reality Lab\n- Ruangan Kelas\n- AI & Data Analysis Lab\n- Computer Lab\n- Ruang Meeting\nKeunggulan UCIC:\n- Berbasis Teknologi\n- Biaya Terjangkau\n- Career Ready System\n- Dosen yang Kompeten\n- Lokasi Strategis di Kota Cirebon\n- Belajar Mini Class Gratis",
"fasilitas,campus 1,campus hub,convention hall,multimedia,dm,3d virtual reality lab,ai data analysis lab,computer lab,ruang meeting,keunggulan,berbasis teknologi,biaya terjangkau,career ready system,dosen kompeten,lokasi strategis,mini class gratis,facility,facilities,infrastructure"},

// ===== FAKULTAS & PRODI =====
{"jurusan_ucic","jurusan","Fakultas dan Program Studi UCIC",
"UCIC memiliki 3 fakultas:\n- Fakultas Teknologi Informasi (FTI): S1 Teknik Informatika (TI), S1 Sistem Informasi (SI), S1 Desain Komunikasi Visual (DKV), D3 Manajemen Informatika (MI).\n- Fakultas Ekonomi dan Bisnis (FEB): S1 Akuntansi (Akt), S1 Manajemen (Mjn), S1 Bisnis Digital (Bisdi), D3 Manajemen Bisnis (MB).\n- Fakultas Pendidikan dan Sains (FPS): S1 Pendidikan Kepelatihan Keolahragaan (Pikor/PKOR).",
"jurusan ucic,fakultas ucic,program studi ucic,prodi ucic,3 fakultas,semua jurusan,semua prodi,jurusan apa saja,fakultas apa saja,universitas catur insan cendekia,cic,ucic"},

{"jurusan_fti","jurusan","Fakultas Teknologi Informasi (FTI)",
"Fakultas Teknologi Informasi (FTI) membuka:\n- S1 Teknik Informatika (TI)\n- S1 Sistem Informasi (SI)\n- S1 Desain Komunikasi Visual (DKV)\n- D3 Manajemen Informatika (MI)",
"fakultas,teknologi,informatika,teknik,sistem informasi,dkv,desain,fti,mi,faculty,technology,information"},

{"jurusan_feb","jurusan","Fakultas Ekonomi dan Bisnis (FEB)",
"Fakultas Ekonomi dan Bisnis (FEB) membuka:\n- S1 Akuntansi (Akt)\n- S1 Manajemen (Mjn)\n- S1 Bisnis Digital (Bisdi)\n- D3 Manajemen Bisnis (MB)",
"fakultas,ekonomi,bisnis,akuntansi,manajemen,bisnis digital,manajemen bisnis,feb,faculty,business,economics"},

{"jurusan_fps","jurusan","Fakultas Pendidikan dan Sains (FPS)",
"Fakultas Pendidikan dan Sains (FPS) membuka:\n- S1 Pendidikan Kepelatihan Keolahragaan (Pikor/PKOR)",
"fakultas,pendidikan,sains,olahraga,kepelatihan,keolahragaan,fps,pikor,pkor,faculty,education,sports"},

{"singkatan_prodi","jurusan","Singkatan Program Studi UCIC",
"Singkatan program studi UCIC:\n- Teknik Informatika = TI\n- Sistem Informasi = SI\n- Desain Komunikasi Visual = DKV\n- Akuntansi = Akt\n- Manajemen = Mjn\n- Bisnis Digital = Bisdi\n- Pendidikan Kepelatihan Keolahragaan = Pikor atau PKOR\n- Manajemen Informatika = MI\n- Manajemen Bisnis = MB",
"singkatan prodi,singkatan jurusan,kepanjangan ti,kepanjangan si,kepanjangan dkv,kepanjangan akt,kepanjangan mjn,kepanjangan bisdi,kepanjangan pikor,kepanjangan pkor,kepanjangan mi,kepanjangan mb,ti,si,dkv,akt,mjn,bisdi,pikor,pkor,mi,mb"},

{"prodi_ti","jurusan","Detail S1 Teknik Informatika",
"Fokus pada pemrograman, pengembangan software, AI, cybersecurity, dan database.",
"teknik informatika,ti,programming,software,ai,karir,prospek kerja,lulusan,computer science,career"},

{"prodi_si","jurusan","Detail S1 Sistem Informasi",
"Menggabungkan teknologi dan bisnis untuk pengelolaan data dan sistem perusahaan.",
"sistem informasi,si,karir,prospek kerja,bisnis,data,information system,career"},

{"prodi_dkv","jurusan","Detail S1 Desain Komunikasi Visual",
"Mencakup desain grafis, animasi, UI/UX, branding, dan multimedia.",
"dkv,desain komunikasi visual,desain,grafis,animasi,ui ux,karir,design,graphic,visual"},

{"prodi_akuntansi","jurusan","Detail S1 Akuntansi",
"Mencakup pengelolaan keuangan, audit, perpajakan, dan pelaporan keuangan.",
"akuntansi,akt,keuangan,audit,pajak,karir,accounting,finance,tax"},

{"prodi_manajemen","jurusan","Detail S1 Manajemen",
"Mencakup manajemen bisnis, pemasaran, dan sumber daya manusia.",
"manajemen,mjn,bisnis,pemasaran,sdm,karir,organisasi,management,business,marketing,career"},

{"prodi_bisdi","jurusan","Detail S1 Bisnis Digital",
"Mencakup e-commerce, startup, financial technology, digital marketing, dan strategi bisnis digital.",
"bisnis digital,bisdig,bisdi,e commerce,startup,digital marketing,karir,online,digital business"},

{"prodi_pkor","jurusan","Detail S1 Pendidikan Kepelatihan Keolahragaan",
"Mencakup ilmu kepelatihan olahraga, fisiologi, biomekanika, psikologi olahraga, dan manajemen olahraga.",
"pkor,pikor,pendidikan kepelatihan keolahragaan,olahraga,kepelatihan,keolahragaan,atlet,pelatih,fisiologi,karir,sports,coaching,athlete"},

{"prodi_d3mi","jurusan","Detail D3 Manajemen Informatika",
"Fokus pada desain digital, konten digital, dan media sosial. Kurikulum meliputi komputer grafis, videografi, copywriting, UI/UX, e-commerce, dan monetisasi konten.",
"d3,manajemen informatika,mi,diploma,media digital,konten digital,d3 mi"},

{"prodi_d3mb","jurusan","Detail D3 Manajemen Bisnis",
"Fokus pada bisnis ritel, pemasaran, layanan pelanggan, pengelolaan keuangan bisnis, supply chain, dan technopreneurship.",
"d3 manajemen bisnis,d3 mb,mb,manajemen bisnis,diploma bisnis,ritel,pemasaran,supply chain,e commerce"},

// ===== RINGKASAN KURIKULUM =====
{"kurikulum_ti","kurikulum","Ringkasan Kurikulum S1 Teknik Informatika",
"Kurikulum S1 Teknik Informatika tersedia dari semester satu sampai delapan. Fokus awal pada dasar komputer, algoritma, basis data, jaringan, dan sistem operasi; semester tengah pada OOP, UI/UX, AI, data mining, mobile, dan keamanan; semester akhir pada IoT, machine learning, proyek, MBKM, magang, dan skripsi.",
"kurikulum,teknik informatika,ti,semester,mata kuliah,curriculum"},

{"kurikulum_si","kurikulum","Ringkasan Kurikulum S1 Sistem Informasi",
"Kurikulum S1 Sistem Informasi tersedia dari semester satu sampai delapan. Fokus awal pada sistem dan bisnis; semester tengah pada basis data, UI/UX, agile, ERP, dan pengembangan aplikasi; semester akhir pada proyek sistem informasi, technopreneurship, magang, KKN, dan skripsi.",
"kurikulum,sistem informasi,si,semester,mata kuliah,curriculum"},

{"kurikulum_dkv","kurikulum","Ringkasan Kurikulum S1 Desain Komunikasi Visual",
"Kurikulum S1 Desain Komunikasi Visual tersedia dari semester satu sampai delapan. Materi meliputi dasar desain, tipografi, fotografi, advertising, packaging, animasi 3D, media interaktif, konten digital, proyek DKV, MBKM, dan skripsi.",
"kurikulum,dkv,desain komunikasi visual,semester,mata kuliah,curriculum"},

{"kurikulum_d3mi","kurikulum","Ringkasan Kurikulum D3 Manajemen Informatika",
"Kurikulum D3 Manajemen Informatika tersedia dari semester satu sampai enam. Fokus pada media digital, komputer grafis, fotografi, videografi, branding, konten digital, startup digital, UI/UX, e-commerce, sertifikasi, dan tugas akhir.",
"kurikulum,d3 manajemen informatika,manajemen informatika,mi,semester,mata kuliah,curriculum"},

{"kurikulum_manajemen","kurikulum","Ringkasan Kurikulum S1 Manajemen",
"Kurikulum S1 Manajemen tersedia dari semester satu sampai delapan. Fokus pada ekonomi, pemasaran, SDM, keuangan, operasi, technopreneurship, digital transformation, kepemimpinan, magang, dan skripsi.",
"kurikulum,manajemen,semester,mata kuliah,curriculum"},

{"kurikulum_bisnisdigital","kurikulum","Ringkasan Kurikulum S1 Bisnis Digital",
"Kurikulum S1 Bisnis Digital tersedia dari semester satu sampai delapan. Fokus pada konsep bisnis digital, SEO, digital marketing, fintech, startup, IMC, business intelligence, digital business development, MBKM, dan skripsi.",
"kurikulum,bisnis digital,bisdig,bisdi,semester,mata kuliah,curriculum"},

{"kurikulum_akuntansi","kurikulum","Ringkasan Kurikulum S1 Akuntansi",
"Kurikulum S1 Akuntansi tersedia dari semester satu sampai delapan. Fokus pada akuntansi keuangan, audit, perpajakan, sistem informasi akuntansi, metodologi penelitian, etika profesi, magang, dan skripsi.",
"kurikulum,akuntansi,semester,mata kuliah,curriculum"},

{"kurikulum_pkor","kurikulum","Ringkasan Kurikulum S1 Pendidikan Kepelatihan Keolahragaan",
"Kurikulum S1 Pendidikan Kepelatihan Keolahragaan tersedia dari semester satu sampai delapan. Fokus pada anatomi, fisiologi, kepelatihan cabang olahraga, biomekanika, psikologi olahraga, metodologi penelitian, MBKM, praktik lapangan, dan skripsi.",
"kurikulum,pkor,pikor,pendidikan kepelatihan keolahragaan,pendidikan kepelatihan olahraga,semester,mata kuliah,curriculum"},

{"kurikulum_d3mb","kurikulum","Ringkasan Kurikulum D3 Manajemen Bisnis",
"Kurikulum D3 Manajemen Bisnis tersedia dari semester satu sampai enam. Fokus pada bisnis dasar, ritel, perpajakan, akuntansi bisnis, SDM, supply chain, pemasaran ritel, kepemimpinan, seminar bisnis, dan tugas akhir.",
"kurikulum,d3 manajemen bisnis,manajemen bisnis,d3 mb,semester,mata kuliah,curriculum"},

// ===== PROSPEK KARIR, UNGGULAN, DAN KURIKULUM =====
{"id_prodi_ti_karir","prospek_karir","Prospek Karir S1 Teknik Informatika","Software Engineer, Front End Web Developer, Back End Web Developer, Full Stack Developer, Mobile Application Developer, UI/UX Designer, Data Scientist / ML Engineer, IoT Engineer, Pengembangan Sistem Berbasis AI, IT Officer, Network Administrator, Digital Entrepreneur, Cyber Security Specialist","teknik informatika,karir,prospek kerja,lulusan ti,software engineer,data scientist,cyber security"},
{"id_prodi_ti_unggulan","matakuliah_unggulan","Mata Kuliah Unggulan S1 Teknik Informatika","Pemrograman Internet, Mobile Programming, Artificial Intelligence, Internet of Things, Machine Learning, UI/UX Designer, Computer Vision and NLP","teknik informatika,mata kuliah unggulan,ai,iot,mobile programming"},
{"id_kurikulum_ti_sem1","kurikulum","Kurikulum S1 Teknik Informatika Semester 1","Pancasila dan Kewarganegaraan, Bahasa Indonesia, Pembangunan Karakter dan Etika Profesi, General English, Pemrograman Internet, Aljabar Linear dan Matriks, Arsitektur dan Organisasi Komputer, Algoritma dan Pemrograman","kurikulum,teknik informatika,semester 1,mata kuliah"},
{"id_kurikulum_ti_sem2","kurikulum","Kurikulum S1 Teknik Informatika Semester 2","Pendidikan Agama, Intermediate English, Sistem Basis Data, Komunikasi Data dan Jaringan, Rekayasa Aplikasi Internet, Teknik Multimedia Digital, Sistem Operasi, Struktur Data","kurikulum,teknik informatika,semester 2,mata kuliah"},
{"id_kurikulum_ti_sem3","kurikulum","Kurikulum S1 Teknik Informatika Semester 3","English for Specific Purpose, Statistik for Data Science, Pemrograman Berorientasi Objek, Administrasi Jaringan, Analisis dan Desain Perangkat Lunak, UI/UX Designer, Kecerdasan Buatan, Manajemen Data & Informasi","kurikulum,teknik informatika,semester 3,mata kuliah"},
{"id_kurikulum_ti_sem4","kurikulum","Kurikulum S1 Teknik Informatika Semester 4","TOEFL Preparation, Mobile Programming, Keamanan Sistem Informasi dan Jaringan, Sistem Digital, Data Mining, Sistem Penunjang Keputusan, Grafika Komputer & Pengolahan Citra Digital, Sistem Pakar (Mata Kuliah Peminatan), Sistem Terdistribusi, Pengujian dan Pemeliharaan Sistem","kurikulum,teknik informatika,semester 4,mata kuliah"},
{"id_kurikulum_ti_sem5","kurikulum","Kurikulum S1 Teknik Informatika Semester 5","Student Exchange, Technopreneurship, Pengembangan Aplikasi Mobile, Machine Learning, Internet of Things, Computer Vision and NLP, Game Cerdas, Jaminan Kualitas Perangkat Lunak, Manajemen Proyek Perangkat Lunak","kurikulum,teknik informatika,semester 5,mata kuliah"},
{"id_kurikulum_ti_sem6","kurikulum","Kurikulum S1 Teknik Informatika Semester 6","Student Exchange, Kapita Selekta, Proyek Technopreneurship, Realitas Virtual dan Augmentasi, Metodologi Penelitian & Riset Pustaka, Proyek Teknologi Informasi, KKN Tematik, MBKM (Studi Independen Bersertifikat), Magang (Studi Independen Mandiri)","kurikulum,teknik informatika,semester 6,mata kuliah"},
{"id_kurikulum_ti_sem7","kurikulum","Kurikulum S1 Teknik Informatika Semester 7","MBKM (Magang Bersertifikat), Magang Internship Mandiri","kurikulum,teknik informatika,semester 7,magang,mbkm"},
{"id_kurikulum_ti_sem8","kurikulum","Kurikulum S1 Teknik Informatika Semester 8","Skripsi","kurikulum,teknik informatika,semester 8,skripsi"},

{"id_prodi_si_karir","prospek_karir","Prospek Karir S1 Sistem Informasi","Application Engineer, Digital Marketing, Research and Development (R&D), Data Scientist, Software Developer, System Analyst, Quality Software System, System Designer, Business Application Developer, e-Business Manager","sistem informasi,karir,prospek kerja,system analyst,business application,digital marketing"},
{"id_prodi_si_unggulan","matakuliah_unggulan","Mata Kuliah Unggulan S1 Sistem Informasi","Audit Sistem Informasi, Enterprise Resource Planning (ERP), Proyek Technopreneurship, E-CRM, Agile Development, UI/UX Design, Business Intelligence","sistem informasi,mata kuliah unggulan,erp,audit si,agile"},
{"id_kurikulum_si_sem1","kurikulum","Kurikulum S1 Sistem Informasi Semester 1","Pancasila dan Kewarganegaraan, Bahasa Indonesia, Pembangunan Karakter dan Etika Profesi, General English, Matematika Bisnis, Manajemen dan Organisasi, Sistem dan Teknologi Informasi, Algoritma dan Pemrograman","kurikulum,sistem informasi,semester 1,mata kuliah"},
{"id_kurikulum_si_sem2","kurikulum","Kurikulum S1 Sistem Informasi Semester 2","Pendidikan Agama, Teknologi dan Tren E-commerce, Intermediate English, Arsitektur SI/TI Perusahaan, Tata Kelola Teknologi Informasi, Analisa Proses Bisnis, Pemrograman Web (HTML-CSS), Algoritma dan Pemrograman 2","kurikulum,sistem informasi,semester 2,mata kuliah"},
{"id_kurikulum_si_sem3","kurikulum","Kurikulum S1 Sistem Informasi Semester 3","Statistik, Sistem Informasi Manajemen, English for Specific Purpose, Interaksi Manusia dan Komputer, Analisa dan Perancangan Sistem Informasi, Sistem Basis Data, Pemrograman Internet","kurikulum,sistem informasi,semester 3,mata kuliah"},
{"id_kurikulum_si_sem4","kurikulum","Kurikulum S1 Sistem Informasi Semester 4","TOEFL Preparation, UI/UX Design, Rekayasa Perangkat Lunak: Agile Scrum Introduction, Analisa Sistem Berorientasi Objek, Pemrograman Internet Intermediate (Framework, MVC), Mata Kuliah Pilihan 1, Mata Kuliah Pilihan 2","kurikulum,sistem informasi,semester 4,mata kuliah"},
{"id_kurikulum_si_sem5","kurikulum","Kurikulum S1 Sistem Informasi Semester 5","Pengembangan Aplikasi Mobile, Student Exchange (Agile Implementation), Jaringan Komputer, Technopreneurship, Rekayasa Perangkat Lunak, Pemrograman Internet Intermediate, Mata Kuliah Pilihan 1, Mata Kuliah Pilihan 2","kurikulum,sistem informasi,semester 5,mata kuliah"},
{"id_kurikulum_si_sem6","kurikulum","Kurikulum S1 Sistem Informasi Semester 6","Student Exchange (Audit SI), Student Exchange (Interaksi Manusia & Komputer), Student Exchange (Audio Visual Effects), Metodologi Penelitian dan Riset Pustaka, Proyek Technopreneurship, Proyek Sistem Informasi, Kapita Selekta, Mata Kuliah Pilihan 1","kurikulum,sistem informasi,semester 6,mata kuliah"},
{"id_kurikulum_si_sem7","kurikulum","Kurikulum S1 Sistem Informasi Semester 7","Magang Internship Mandiri, Magang MBKM (Magang Bersertifikat), KKN Tematik","kurikulum,sistem informasi,semester 7,magang,kkn"},
{"id_kurikulum_si_sem8","kurikulum","Kurikulum S1 Sistem Informasi Semester 8","Skripsi","kurikulum,sistem informasi,semester 8,skripsi"},

{"id_prodi_dkv_karir","prospek_karir","Prospek Karir S1 Desain Komunikasi Visual","Desainer Grafis, UI/UX Designer, Ilustrator, Desainer Komik, Fotografer/Videografer, Desainer Kemasan, Desainer Identitas Visual, Spesialis Media Sosial, Periklanan dan Pemasaran Visual, Produser Konten Digital, Pendidik dan Instruktur, Kewirausahaan, Editor Grafis, Desainer Seni dan Hiburan, Digital Marketing","dkv,desain komunikasi visual,karir,prospek kerja,graphic designer,illustrator"},
{"id_prodi_dkv_unggulan","matakuliah_unggulan","Mata Kuliah Unggulan S1 Desain Komunikasi Visual","Ilustrasi, Fotografi, Komik, 3D Animasi, Media Interaktif, Desain Komunikasi Visual Identitas, Creative Content Advertising","dkv,mata kuliah unggulan,ilustrasi,fotografi,animasi 3d"},
{"id_kurikulum_dkv_sem1","kurikulum","Kurikulum S1 Desain Komunikasi Visual Semester 1","Pancasila dan Kewarganegaraan, Bahasa Indonesia, Pembangunan Karakter dan Etika Profesi, General English, Dasar Desain Grafis, Basic Tipografi, Bahasa Rupa, Computer Graphic for Graphic Design, Estetika Visual Desain","kurikulum,dkv,semester 1,mata kuliah"},
{"id_kurikulum_dkv_sem2","kurikulum","Kurikulum S1 Desain Komunikasi Visual Semester 2","Pendidikan Agama, Teknologi dan Tren E-commerce, Intermediate English, Sosiologi Desain, Design Thinking, Kinetic Tipografi, Metoda Produksi Grafika, Komik Digital","kurikulum,dkv,semester 2,mata kuliah"},
{"id_kurikulum_dkv_sem3","kurikulum","Kurikulum S1 Desain Komunikasi Visual Semester 3","English for Specific Purpose, 3D Artist, Desain Komunikasi Visual Identitas, Basic Fotografi, Advertising Copywriting, Semiotika Visual, Web Design, Tinjauan dan Kritik Desain","kurikulum,dkv,semester 3,mata kuliah"},
{"id_kurikulum_dkv_sem4","kurikulum","Kurikulum S1 Desain Komunikasi Visual Semester 4","TOEFL Preparation, Desain Komunikasi Visual Promosi, Applied Photography, Audio Visual Effects, Packaging Design, Media Interaktif, Animasi 3D, Creative Content Advertising, Inovasi Grafis","kurikulum,dkv,semester 4,mata kuliah"},
{"id_kurikulum_dkv_sem5","kurikulum","Kurikulum S1 Desain Komunikasi Visual Semester 5","Technopreneurship, Desain Komunikasi Visual Sosial, Metodologi Penelitian Desain, Manajemen Desain & Creativepreneur, Game Desain, UI/UX Designer, Visual Merchandising, Video Brand Production, Student Exchange (Fotografi)","kurikulum,dkv,semester 5,mata kuliah"},
{"id_kurikulum_dkv_sem6","kurikulum","Kurikulum S1 Desain Komunikasi Visual Semester 6","Kapita Selekta, Proyek Technopreneurship, Strategi Konten Digital, Psikologi Persepsi, Project DKV, MBKM (Studi Independen Bersertifikat)","kurikulum,dkv,semester 6,mata kuliah"},
{"id_kurikulum_dkv_sem7","kurikulum","Kurikulum S1 Desain Komunikasi Visual Semester 7","Penguasaan Teknologi (MBKM), Tata Tulis Karya Ilmiah (MBKM), Etika Profesi Kerja (MBKM), Komunikasi dan Teamwork (MBKM), Ide, Inisiatif dan Tanggung Jawab (MBKM), KKN Tematik","kurikulum,dkv,semester 7,magang,kkn"},
{"id_kurikulum_dkv_sem8","kurikulum","Kurikulum S1 Desain Komunikasi Visual Semester 8","Skripsi","kurikulum,dkv,semester 8,skripsi"},

{"id_prodi_d3mi_karir","prospek_karir","Prospek Karir D3 Manajemen Informatika","Web Designer, Content Creator, Software Developer, Pengajar Teknologi Informasi, Network Administrator, IT Consultant","d3 manajemen informatika,karir,prospek kerja,web designer,content creator"},
{"id_prodi_d3mi_unggulan","matakuliah_unggulan","Mata Kuliah Unggulan D3 Manajemen Informatika","Pemrograman Web, Digital Content Marketing, Fotografi Digital, UI/UX Design, Kecerdasan Buatan","d3 manajemen informatika,mata kuliah unggulan,web programming,digital marketing"},
{"id_kurikulum_d3mi_sem1","kurikulum","Kurikulum D3 Manajemen Informatika Semester 1","Pancasila dan Kewarganegaraan, Bahasa Indonesia, Pembangunan Karakter dan Etika Profesi, General English, Pengantar Media Digital, Komputer Grafis 1, Komunikasi dan Presentasi Dasar, AI Technician","kurikulum,d3 manajemen informatika,semester 1,mata kuliah"},
{"id_kurikulum_d3mi_sem2","kurikulum","Kurikulum D3 Manajemen Informatika Semester 2","Pendidikan Agama, Intermediate English, Design Thinking, Fotografi Digital, Komputer Grafis 2 (Data Visualization), Statistik Informatika, Digital Copywriting","kurikulum,d3 manajemen informatika,semester 2,mata kuliah"},
{"id_kurikulum_d3mi_sem3","kurikulum","Kurikulum D3 Manajemen Informatika Semester 3","English for Specific Purpose, Technopreneurship, Videografi, Brand Identity (Corporate & Personal Branding), Manajemen Media Sosial, Visual Storytelling, Digital Content Marketing","kurikulum,d3 manajemen informatika,semester 3,mata kuliah"},
{"id_kurikulum_d3mi_sem4","kurikulum","Kurikulum D3 Manajemen Informatika Semester 4","TOEFL Preparation, Proyek Technopreneurship, Fotografi dan Videografi Lanjutan, Digital Bisnis (Startup), Integrated Marketing Communication, Digital Content Interaktif, Monetisasi Konten Digital, Mata Kuliah Pilihan","kurikulum,d3 manajemen informatika,semester 4,mata kuliah"},
{"id_kurikulum_d3mi_sem5","kurikulum","Kurikulum D3 Manajemen Informatika Semester 5","Tren dan Eksplorasi Content Digital, E-Commerce, UI/UX Design, Proyek Manajemen Informatika, Etika dan Regulasi Media Digital, Sertifikasi Kompetensi & HAKI Produk Digital, Manajemen Proyek Media Digital","kurikulum,d3 manajemen informatika,semester 5,mata kuliah"},
{"id_kurikulum_d3mi_sem6","kurikulum","Kurikulum D3 Manajemen Informatika Semester 6","Tugas Akhir, Kapita Selekta, Tata Tulis Karya Ilmiah, Influencer Networking & Collaboration, Kampanye Konten Digital, Optimasi Content Digital, Mata Kuliah Pilihan","kurikulum,d3 manajemen informatika,semester 6,mata kuliah"},

{"id_prodi_manajemen_karir","prospek_karir","Prospek Karir S1 Manajemen","Account Officer, Bagian Operasional dan Logistik, Product Manager, Market Researcher, Event Planner/Event Organizer, Wirausaha, CEO, Direktur, Manajer, Credit Analyst, Treasury Supervisor, Head of Operations Business","manajemen,karir,prospek kerja,manager,entrepreneur,business"},
{"id_prodi_manajemen_unggulan","matakuliah_unggulan","Mata Kuliah Unggulan S1 Manajemen","Manajemen Keuangan, Manajemen Operasional, Manajemen Pemasaran, Manajemen Sumber Daya Manusia","manajemen,mata kuliah unggulan,keuangan,pemasaran,sdm"},
{"id_kurikulum_manajemen_sem1","kurikulum","Kurikulum S1 Manajemen Semester 1","Pendidikan Agama, Pancasila dan Kewarganegaraan, General English, Bahasa Indonesia, Ekonomi Mikro, Akuntansi Keuangan Dasar, Teknologi Informasi untuk Bisnis, Manajemen & Bisnis Dasar","kurikulum,manajemen,semester 1,mata kuliah"},
{"id_kurikulum_manajemen_sem2","kurikulum","Kurikulum S1 Manajemen Semester 2","Pengembangan Karakter, Manajemen Pemasaran, Matematika Ekonomi dan Bisnis, Hukum Bisnis, Komunikasi Bisnis dan Negosiasi, Perilaku Konsumen, Ekonomi Makro, Sistem Informasi Manajemen","kurikulum,manajemen,semester 2,mata kuliah"},
{"id_kurikulum_manajemen_sem3","kurikulum","Kurikulum S1 Manajemen Semester 3","Manajemen Sumber Daya Manusia, Digital Marketing, Manajemen Keuangan, Manajemen Operasi, Technopreneurship, Akuntansi Biaya, Big Data Analytics","kurikulum,manajemen,semester 3,mata kuliah"},
{"id_kurikulum_manajemen_sem4","kurikulum","Kurikulum S1 Manajemen Semester 4","English for Business, Manajemen Proyek Teknologi Informasi, Manajemen Anggaran, Perilaku Organisasional, Technopreneurship Lanjutan, Manajemen Strategik, Teori Portofolio dan Analisis Investasi","kurikulum,manajemen,semester 4,mata kuliah"},
{"id_kurikulum_manajemen_sem5","kurikulum","Kurikulum S1 Manajemen Semester 5","Manajemen Inovasi Berbasis Teknologi, Bisnis Digital, Statistika dan Analisis Bisnis, Metodologi Penelitian, Manajemen Transformasi Digital, Analisa Laporan Keuangan (Student Exchange), Mata Kuliah Pilihan","kurikulum,manajemen,semester 5,mata kuliah"},
{"id_kurikulum_manajemen_sem6","kurikulum","Kurikulum S1 Manajemen Semester 6","E-Commerce & Platform, Etika Profesi Bisnis, Seminar Manajemen, Metode Pengambilan Keputusan (Student Exchange), Kapita Selekta, Mata Kuliah Pilihan","kurikulum,manajemen,semester 6,mata kuliah"},
{"id_kurikulum_manajemen_sem7","kurikulum","Kurikulum S1 Manajemen Semester 7","Magang Studi Independen, Kepemimpinan Bisnis, Kepemimpinan Entrepreneur, Kemampuan Kerja Tim, Literasi Digital, Literasi Data","kurikulum,manajemen,semester 7,magang"},
{"id_kurikulum_manajemen_sem8","kurikulum","Kurikulum S1 Manajemen Semester 8","Skripsi","kurikulum,manajemen,semester 8,skripsi"},

{"id_prodi_bisnisdigital_karir","prospek_karir","Prospek Karir S1 Bisnis Digital","Technopreneur, Digital Marketing Manager, Social Media Marketing Manager, Data Intelligence Analyst, e-Commerce Practitioner, Fintech Practitioner, Startup Consultant, Startup Ecosystem Team, Financial Planner","bisnis digital,karir,prospek kerja,digital marketing,startup,e commerce"},
{"id_prodi_bisnisdigital_unggulan","matakuliah_unggulan","Mata Kuliah Unggulan S1 Bisnis Digital","Technopreneurship, Financial Technology, Model Bisnis UMKM, Analisis Bisnis Digital, Startup Business Model","bisnis digital,mata kuliah unggulan,fintech,startup,umkm"},
{"id_kurikulum_bisnisdigital_sem1","kurikulum","Kurikulum S1 Bisnis Digital Semester 1","Bahasa Indonesia, Pancasila dan Kewarganegaraan, Pengembangan Karakter & Etika Profesi, General English, Ekonomika Bisnis, Etika dan Komunikasi Bisnis, Konsep dan Ide Bisnis Digital, Manajemen Bisnis","kurikulum,bisnis digital,semester 1,mata kuliah"},
{"id_kurikulum_bisnisdigital_sem2","kurikulum","Kurikulum S1 Bisnis Digital Semester 2","Pendidikan Agama, Graphic Design (Video Animation), Financial Management, Web Design, Search Engine Optimization (SEO), Statistika Bisnis, Digital Marketing, Management Information System","kurikulum,bisnis digital,semester 2,mata kuliah"},
{"id_kurikulum_bisnisdigital_sem3","kurikulum","Kurikulum S1 Bisnis Digital Semester 3","Technopreneurship, Financial Technology, Model Bisnis UMKM (K1), Analisis Bisnis Digital (K2), Startup Business Model (K3), Analisa Investasi Bisnis & Portfolio, Metodologi Penelitian Bisnis, Studi Kelayakan Bisnis","kurikulum,bisnis digital,semester 3,mata kuliah"},
{"id_kurikulum_bisnisdigital_sem4","kurikulum","Kurikulum S1 Bisnis Digital Semester 4","Project Technopreneurship, English for Business, e-Customer Relationship Management, Academic Writing and Publication, Customer Behaviour, Integrated Marketing Communication (IMC)","kurikulum,bisnis digital,semester 4,mata kuliah"},
{"id_kurikulum_bisnisdigital_sem5","kurikulum","Kurikulum S1 Bisnis Digital Semester 5","Business Intelligence, Digital Business Platform, Design Thinking Method, Supply Chain Management, Student Exchange: Fotografi (DKV)","kurikulum,bisnis digital,semester 5,mata kuliah"},
{"id_kurikulum_bisnisdigital_sem6","kurikulum","Kurikulum S1 Bisnis Digital Semester 6","Digital Business Development (Project) - Digital MSMEs-Preneur, Digital Business Development (Project) - Digital Business Consultant, Digital Business Development (Project) - Digital Business Startup, Crowdfunding & Fintech Ecosystem, Enterprise Resources Planning, Kapita Selekta, Writing Business Report","kurikulum,bisnis digital,semester 6,mata kuliah"},
{"id_kurikulum_bisnisdigital_sem7","kurikulum","Kurikulum S1 Bisnis Digital Semester 7","Digital Business Development (Project), MBKM (Kewirausahaan/SIB/Penelitian/PKM)","kurikulum,bisnis digital,semester 7,mbkm"},
{"id_kurikulum_bisnisdigital_sem8","kurikulum","Kurikulum S1 Bisnis Digital Semester 8","Skripsi","kurikulum,bisnis digital,semester 8,skripsi"},

{"id_prodi_akuntansi_karir","prospek_karir","Prospek Karir S1 Akuntansi","Auditor Internal, Auditor Eksternal, Accounting Staff, Konsultan Keuangan, Data Analyst, Konsultan Perpajakan","akuntansi,karir,prospek kerja,auditor,pajak,keuangan"},
{"id_prodi_akuntansi_unggulan","matakuliah_unggulan","Mata Kuliah Unggulan S1 Akuntansi","Akuntansi Keuangan, Auditing, Sistem Informasi Akuntansi, Perpajakan","akuntansi,mata kuliah unggulan,auditing,pajak,sistem informasi akuntansi"},
{"id_kurikulum_akuntansi_sem1","kurikulum","Kurikulum S1 Akuntansi Semester 1","Akuntansi Keuangan Dasar, Ekonomi Mikro, General English, Manajemen & Bisnis Dasar, Pancasila & Kewarganegaraan, Pendidikan Agama, Teknologi Informasi untuk Bisnis, Bahasa Indonesia","kurikulum,akuntansi,semester 1,mata kuliah"},
{"id_kurikulum_akuntansi_sem2","kurikulum","Kurikulum S1 Akuntansi Semester 2","Ekonomi Makro, Hukum Bisnis, Komunikasi Bisnis & Negosiasi, Matematika Ekonomi dan Bisnis, Pengembangan Karakter, Praktikum Akuntansi Keuangan, Akuntansi Keuangan Menengah, Akuntansi Biaya","kurikulum,akuntansi,semester 2,mata kuliah"},
{"id_kurikulum_akuntansi_sem3","kurikulum","Kurikulum S1 Akuntansi Semester 3","Akuntansi Manajemen, Hukum Perpajakan, Manajemen Keuangan, Sistem Informasi Akuntansi, Technopreneurship, Akuntansi Sektor Publik, Akuntansi Keuangan Lanjutan","kurikulum,akuntansi,semester 3,mata kuliah"},
{"id_kurikulum_akuntansi_sem4","kurikulum","Kurikulum S1 Akuntansi Semester 4","Akuntansi Entitas Mikro, Kecil dan Menengah, Akuntansi Internasional, Auditing 1, Technopreneurship Lanjutan, Teknologi Keuangan, Akuntansi Perpajakan, English for Business, Bisnis Digital (Student Exchange Prodi Manajemen)","kurikulum,akuntansi,semester 4,mata kuliah"},
{"id_kurikulum_akuntansi_sem5","kurikulum","Kurikulum S1 Akuntansi Semester 5","Metodologi Penelitian, Akuntansi Syariah, Analisis Laporan Keuangan, Auditing 2, Statistika dan Analisis Bisnis, Praktikum Perpajakan, Agile Implementation (Student Exchange Prodi SI)","kurikulum,akuntansi,semester 5,mata kuliah"},
{"id_kurikulum_akuntansi_sem6","kurikulum","Kurikulum S1 Akuntansi Semester 6","Seminar Akuntansi, Teori Akuntansi, Kapita Selekta, Teori Portofolio dan Analisis Investasi, Etika Profesi Akuntan, Praktikum Auditing, Mata Kuliah Pilihan 1 (Lintas Prodi UCIC)","kurikulum,akuntansi,semester 6,mata kuliah"},
{"id_kurikulum_akuntansi_sem7","kurikulum","Kurikulum S1 Akuntansi Semester 7","KKN Tematik, Penatausahaan Dokumen Perkantoran, Pengoperasian Aplikasi dan Alat Bantu Perkantoran, Mengolah Data Perkantoran, Kreativitas dan Inisiatif, Etika Profesi Kerja, Komunikasi dan Teamwork, Magang","kurikulum,akuntansi,semester 7,kkn,magang"},
{"id_kurikulum_akuntansi_sem8","kurikulum","Kurikulum S1 Akuntansi Semester 8","Skripsi","kurikulum,akuntansi,semester 8,skripsi"},

{"id_prodi_pkor_karir","prospek_karir","Prospek Karir S1 Pendidikan Kepelatihan Keolahragaan","Guru Olahraga, Pelatih Profesional, Wasit/Juri Profesional, Sport Entrepreneur","pkor,pikor,pendidikan kepelatihan keolahragaan,pendidikan kepelatihan olahraga,karir,prospek kerja,pelatih,guru olahraga"},
{"id_prodi_pkor_unggulan","matakuliah_unggulan","Mata Kuliah Unggulan S1 Pendidikan Kepelatihan Keolahragaan","Ilmu Kepelatihan Olahraga, Biomekanika Olahraga, Psikologi Olahraga, Fisiologi Latihan dan Kinerja Atlet, Manajemen Kepelatihan Olahraga","pkor,pikor,pendidikan kepelatihan keolahragaan,mata kuliah unggulan,kepelatihan,biomekanika,psikologi olahraga"},
{"id_kurikulum_pkor_sem1","kurikulum","Kurikulum S1 Pendidikan Kepelatihan Keolahragaan Semester 1","Bahasa Indonesia, Pancasila dan Kewarganegaraan, General English, Pengembangan Karakter, Anatomi, Pembelajaran Motorik, Pelatihan Cabor Atletik, Pelatihan Cabor Renang, Pelatihan Cabor Futsal","kurikulum,pkor,pikor,semester 1,mata kuliah"},
{"id_kurikulum_pkor_sem2","kurikulum","Kurikulum S1 Pendidikan Kepelatihan Keolahragaan Semester 2","Pendidikan Agama, Fisiologi Olahraga, Manajemen dan Administrasi Olahraga, Pelatihan Cabor Sepak Bola, Pelatihan Cabor Basket, Pelatihan Cabor Voli, Pelatihan Cabor Taekwondo, English for Education, Landasan Pendidikan, Kurikulum dan Pembelajaran","kurikulum,pkor,pikor,semester 2,mata kuliah"},
{"id_kurikulum_pkor_sem3","kurikulum","Kurikulum S1 Pendidikan Kepelatihan Keolahragaan Semester 3","Technopreneurship, English for Sport, Bimbingan Konseling, Teori Belajar dan Pembelajaran, Pelatihan Cabor Pencak Silat, Pelatihan Cabor Karate, Pengantar Ilmu Keolahragaan, Perwasitan Olahraga, Teori dan Metodologi Kepelatihan Olahraga, Manajemen Pertandingan/Perlombaan","kurikulum,pkor,pikor,semester 3,mata kuliah"},
{"id_kurikulum_pkor_sem4","kurikulum","Kurikulum S1 Pendidikan Kepelatihan Keolahragaan Semester 4","Biomekanika Olahraga, Ilmu Gizi Olahraga, Teknologi Kepelatihan Olahraga, Analisis Performa Olahraga Berbasis AI, Technopreneurship Lanjutan, Kurikulum dan Pembelajaran, Sport Pedagogy","kurikulum,pkor,pikor,semester 4,mata kuliah"},
{"id_kurikulum_pkor_sem5","kurikulum","Kurikulum S1 Pendidikan Kepelatihan Keolahragaan Semester 5","Metode Penelitian Pendidikan Olahraga, Manajemen Kepelatihan Olahraga, Didaktik Metodik Olahraga, Psikologi Olahraga, Evaluasi Kepelatihan Olahraga, Kinesiologi dan Massage Olahraga","kurikulum,pkor,pikor,semester 5,mata kuliah"},
{"id_kurikulum_pkor_sem6","kurikulum","Kurikulum S1 Pendidikan Kepelatihan Keolahragaan Semester 6","Microteaching, Kapita Selekta, Statistika, Pemandu Bakat Olahraga, Seminar Magang Kepelatihan Olahraga, Evaluasi dan Sistem Organisasi Pertandingan, Pemasaran Olahraga, Pengembangan Program Latihan Berbasis Data","kurikulum,pkor,pikor,semester 6,mata kuliah"},
{"id_kurikulum_pkor_sem7","kurikulum","Kurikulum S1 Pendidikan Kepelatihan Keolahragaan Semester 7","MBKM, Profesi Kependidikan, Praktek Lapangan Persekolahan, Sarana dan Prasarana Olahraga, Tes dan Pengukuran Olahraga, Strategi dan Taktik Olahraga Berbasis Digital, Pencegahan dan Perawatan Cedera Olahraga","kurikulum,pkor,pikor,semester 7,mbkm"},
{"id_kurikulum_pkor_sem8","kurikulum","Kurikulum S1 Pendidikan Kepelatihan Keolahragaan Semester 8","Skripsi","kurikulum,pkor,pikor,semester 8,skripsi"},

{"id_prodi_d3mb_karir","prospek_karir","Prospek Karir D3 Manajemen Bisnis","E-Commerce Specialist, Media Social Specialist, Marketing Staff, Technopreneur","d3 manajemen bisnis,karir,prospek kerja,marketing,e commerce"},
{"id_prodi_d3mb_unggulan","matakuliah_unggulan","Mata Kuliah Unggulan D3 Manajemen Bisnis","Manajemen Bisnis Ritel, Manajemen Tataletak & Planogram, E-Commerce, Technopreneurship, Manajemen Logistik dan Supply Chain","d3 manajemen bisnis,mata kuliah unggulan,ritel,e commerce,supply chain"},
{"id_kurikulum_d3mb_sem1","kurikulum","Kurikulum D3 Manajemen Bisnis Semester 1","Pembangunan Karakter dan Etika Profesi, Pancasila, Kewarganegaraan, dan Anti Korupsi, General English, Bahasa Indonesia, Pengantar Akuntansi, Teknologi Informasi untuk Bisnis, Manajemen & Bisnis Dasar, Pengantar Ilmu Ekonomi Bisnis","kurikulum,d3 manajemen bisnis,semester 1,mata kuliah"},
{"id_kurikulum_d3mb_sem2","kurikulum","Kurikulum D3 Manajemen Bisnis Semester 2","Intermediate English, Pendidikan Agama, Matematika Ekonomi dan Bisnis, Dasar-Dasar Perpajakan, Perilaku Konsumen, Manajemen Tataletak & Planogram, Pengelolaan Keuangan Ritel","kurikulum,d3 manajemen bisnis,semester 2,mata kuliah"},
{"id_kurikulum_d3mb_sem3","kurikulum","Kurikulum D3 Manajemen Bisnis Semester 3","English for Specific Purpose, Technopreneurship, Sistem Akuntansi, Akuntansi Keuangan Bisnis, Manajemen Sumber Daya Manusia, Praktikum Akuntansi Manual, Manajemen Logistik dan Supply Chain, Manajemen Hubungan Pelanggan, Hukum Bisnis","kurikulum,d3 manajemen bisnis,semester 3,mata kuliah"},
{"id_kurikulum_d3mb_sem4","kurikulum","Kurikulum D3 Manajemen Bisnis Semester 4","Technopreneurship Lanjutan, TOEFL Preparation, Metodologi Penelitian Bisnis, Penganggaran Perusahaan, Aplikasi Software Akuntansi, Studi Kelayakan Bisnis, Manajemen Kas dan Portofolio, Strategi Pemasaran Ritel, Manajemen Kualitas","kurikulum,d3 manajemen bisnis,semester 4,mata kuliah"},
{"id_kurikulum_d3mb_sem5","kurikulum","Kurikulum D3 Manajemen Bisnis Semester 5","Kepemimpinan, Kemampuan Bekerjasama, Kemampuan Berkomunikasi, Kreativitas, Kemampuan Administrasi, Kemampuan Merumuskan Masalah Teknis, Seminar Bisnis","kurikulum,d3 manajemen bisnis,semester 5,mata kuliah"},
{"id_kurikulum_d3mb_sem6","kurikulum","Kurikulum D3 Manajemen Bisnis Semester 6","Kapita Selekta, Tugas Akhir, Komunikasi Bisnis","kurikulum,d3 manajemen bisnis,semester 6,mata kuliah"},

// ===== BIAYA, PENDAFTARAN, BEASISWA =====
{"biaya_kuliah","biaya","Biaya Kuliah UCIC",
"Ringkasan biaya PMB 2026 UCIC: biaya pendaftaran Rp250.000, perlengkapan Rp500.000, DPP tercantum Rp3.000.000 tetapi diskon 100% jadi gratis, biaya gedung tercantum Rp4.000.000 tetapi diskon 100% jadi gratis. Tersedia skema pembayaran 3 bulan, 1 semester, dan 1 tahun. Pada rincian opsi, komponen Total adalah total pembayaran per prodi sesuai opsi yang dipilih. Gunakan kata kunci opsi 1, opsi 2, opsi 3, nama prodi, kelas sore, atau RPI untuk rincian lengkap.",
"biaya,ukt,spp,kuliah,bayar,cicilan,harga,tarif,uang,dpp,gedung,diskon,gratis,total,tuition,fee,cost,price,berapa,pmb 2026"},

{"biaya_dpp_gedung","biaya","Informasi DPP dan Gedung UCIC",
"Pada rincian PMB 2026 UCIC, DPP tercantum Rp3.000.000 dan biaya gedung tercantum Rp4.000.000. Namun DPP dan gedung mendapat diskon 100% sehingga gratis. Pada rincian biaya, komponen Total adalah total pembayaran sesuai opsi dan prodi yang dipilih.",
"dpp,biaya dpp,dpp gratis,gedung,biaya gedung,gedung gratis,diskon 100,diskon seratus persen,gratis,total,total pembayaran"},

{"biaya_pmb_opsi1","biaya","Biaya PMB 2026 - Opsi 1 (3 Bulan)",
"Rincian biaya pembayaran Tahap 1 (1/2 Semester: 3 Bulan) PMB Gelombang 1 Februari UCIC 2026. Catatan: DPP Rp3.000.000 dan Gedung Rp4.000.000 mendapat diskon 100% jadi gratis. Komponen Total adalah total pembayaran.\n- S1 Teknik Informatika: Tahap-1 Rp2.150.000, Pendaftaran Rp250.000, Perlengkapan Rp500.000, DPP Rp3.000.000, Gedung Rp4.000.000, Total Rp2.900.000.\n- S1 Teknik Informatika (Sore): Tahap-1 Rp2.220.000, Pendaftaran Rp250.000, Perlengkapan Rp500.000, DPP Rp3.000.000, Gedung Rp4.000.000, Total Rp2.970.000.\n- S1 Sistem Informasi: Tahap-1 Rp2.150.000, Pendaftaran Rp250.000, Perlengkapan Rp500.000, DPP Rp3.000.000, Gedung Rp4.000.000, Total Rp2.900.000.\n- S1 Sistem Informasi (Sore): Tahap-1 Rp2.220.000, Pendaftaran Rp250.000, Perlengkapan Rp500.000, DPP Rp3.000.000, Gedung Rp4.000.000, Total Rp2.970.000.\n- S1 Desain Komunikasi Visual: Tahap-1 Rp2.150.000, Pendaftaran Rp250.000, Perlengkapan Rp500.000, DPP Rp3.000.000, Gedung Rp4.000.000, Total Rp2.900.000.\n- S1 DKV (Sore): Tahap-1 Rp2.220.000, Pendaftaran Rp250.000, Perlengkapan Rp500.000, DPP Rp3.000.000, Gedung Rp4.000.000, Total Rp2.970.000.\n- S1 Manajemen: Tahap-1 Rp1.850.000, Pendaftaran Rp250.000, Perlengkapan Rp500.000, DPP Rp3.000.000, Gedung Rp4.000.000, Total Rp2.600.000.\n- S1 Manajemen (Sore): Tahap-1 Rp1.935.000, Pendaftaran Rp250.000, Perlengkapan Rp500.000, DPP Rp3.000.000, Gedung Rp4.000.000, Total Rp2.685.000.\n- S1 Manajemen (Kelas RPI): Tahap-1 Rp2.100.000, Pendaftaran Rp250.000, Perlengkapan Rp500.000, DPP Rp3.000.000, Gedung Rp4.000.000, Total Rp2.850.000.\n- S1 Akuntansi: Tahap-1 Rp1.850.000, Pendaftaran Rp250.000, Perlengkapan Rp500.000, DPP Rp3.000.000, Gedung Rp4.000.000, Total Rp2.600.000.\n- S1 Akuntansi (Sore): Tahap-1 Rp1.935.000, Pendaftaran Rp250.000, Perlengkapan Rp500.000, DPP Rp3.000.000, Gedung Rp4.000.000, Total Rp2.685.000.\n- S1 Pendidikan Kepelatihan Keolahragaan: Tahap-1 Rp1.290.000, Pendaftaran Rp250.000, Perlengkapan Rp500.000, DPP Rp3.000.000, Gedung Rp4.000.000, Total Rp2.040.000.\n- S1 Bisnis Digital: Tahap-1 Rp1.340.000, Pendaftaran Rp250.000, Perlengkapan Rp500.000, DPP Rp3.000.000, Gedung Rp4.000.000, Total Rp2.090.000.\n- S1 Bisnis Digital (Sore): Tahap-1 Rp1.440.000, Pendaftaran Rp250.000, Perlengkapan Rp500.000, DPP Rp3.000.000, Gedung Rp4.000.000, Total Rp2.190.000.\n- D3 Manajemen Bisnis: Tahap-1 Rp1.095.000, Pendaftaran Rp250.000, Perlengkapan Rp500.000, DPP Rp3.000.000, Gedung Rp4.000.000, Total Rp1.845.000.\n- D3 Manajemen Informatika: Tahap-1 Rp1.095.000, Pendaftaran Rp250.000, Perlengkapan Rp500.000, DPP Rp3.000.000, Gedung Rp4.000.000, Total Rp1.845.000.\n- D3 Manajemen Informatika (Sore): Tahap-1 Rp1.245.000, Pendaftaran Rp250.000, Perlengkapan Rp500.000, DPP Rp3.000.000, Gedung Rp4.000.000, Total Rp1.955.000.",
"biaya,ukt,spp,kuliah,bayar,cicilan,pmb,2026,opsi 1,3 bulan,tahap 1"},

{"biaya_pmb_opsi2","biaya","Biaya PMB 2026 - Opsi 2 (1 Semester)",
"Rincian biaya pembayaran Tahap 1 & 2 (1 Semester: 6 Bulan) PMB Gelombang 1 Februari UCIC 2026. Catatan: DPP Rp3.000.000 dan Gedung Rp4.000.000 mendapat diskon 100% jadi gratis. Komponen Total adalah total pembayaran.\n- S1 Teknik Informatika: Tahap-1 Rp4.130.000, Pendaftaran Rp250.000, Perlengkapan Rp500.000, DPP Rp3.000.000, Gedung Rp4.000.000, Total Rp4.880.000.\n- S1 Teknik Informatika (Sore): Tahap-1 Rp4.270.000, Pendaftaran Rp250.000, Perlengkapan Rp500.000, DPP Rp3.000.000, Gedung Rp4.000.000, Total Rp5.020.000.\n- S1 Sistem Informasi: Tahap-1 Rp4.130.000, Pendaftaran Rp250.000, Perlengkapan Rp500.000, DPP Rp3.000.000, Gedung Rp4.000.000, Total Rp4.880.000.\n- S1 Sistem Informasi (Sore): Tahap-1 Rp4.270.000, Pendaftaran Rp250.000, Perlengkapan Rp500.000, DPP Rp3.000.000, Gedung Rp4.000.000, Total Rp5.020.000.\n- S1 Desain Komunikasi Visual: Tahap-1 Rp4.130.000, Pendaftaran Rp250.000, Perlengkapan Rp500.000, DPP Rp3.000.000, Gedung Rp4.000.000, Total Rp4.880.000.\n- S1 DKV (Sore): Tahap-1 Rp4.270.000, Pendaftaran Rp250.000, Perlengkapan Rp500.000, DPP Rp3.000.000, Gedung Rp4.000.000, Total Rp5.020.000.\n- S1 Manajemen: Tahap-1 Rp3.530.000, Pendaftaran Rp250.000, Perlengkapan Rp500.000, DPP Rp3.000.000, Gedung Rp4.000.000, Total Rp4.280.000.\n- S1 Manajemen (Sore): Tahap-1 Rp3.700.000, Pendaftaran Rp250.000, Perlengkapan Rp500.000, DPP Rp3.000.000, Gedung Rp4.000.000, Total Rp4.450.000.\n- S1 Manajemen (Kelas RPI): Tahap-1 Rp4.030.000, Pendaftaran Rp250.000, Perlengkapan Rp500.000, DPP Rp3.000.000, Gedung Rp4.000.000, Total Rp4.780.000.\n- S1 Akuntansi: Tahap-1 Rp3.530.000, Pendaftaran Rp250.000, Perlengkapan Rp500.000, DPP Rp3.000.000, Gedung Rp4.000.000, Total Rp4.280.000.\n- S1 Akuntansi (Sore): Tahap-1 Rp3.700.000, Pendaftaran Rp250.000, Perlengkapan Rp500.000, DPP Rp3.000.000, Gedung Rp4.000.000, Total Rp4.450.000.\n- S1 Pendidikan Kepelatihan Keolahragaan: Tahap-1 Rp2.410.000, Pendaftaran Rp250.000, Perlengkapan Rp500.000, DPP Rp3.000.000, Gedung Rp4.000.000, Total Rp3.160.000.\n- S1 Bisnis Digital: Tahap-1 Rp2.510.000, Pendaftaran Rp250.000, Perlengkapan Rp500.000, DPP Rp3.000.000, Gedung Rp4.000.000, Total Rp3.260.000.\n- S1 Bisnis Digital (Sore): Tahap-1 Rp2.710.000, Pendaftaran Rp250.000, Perlengkapan Rp500.000, DPP Rp3.000.000, Gedung Rp4.000.000, Total Rp3.460.000.\n- D3 Manajemen Bisnis: Tahap-1 Rp2.020.000, Pendaftaran Rp250.000, Perlengkapan Rp500.000, DPP Rp3.000.000, Gedung Rp4.000.000, Total Rp2.770.000.\n- D3 Manajemen Informatika: Tahap-1 Rp2.020.000, Pendaftaran Rp250.000, Perlengkapan Rp500.000, DPP Rp3.000.000, Gedung Rp4.000.000, Total Rp2.770.000.\n- D3 Manajemen Informatika (Sore): Tahap-1 Rp2.320.000, Pendaftaran Rp250.000, Perlengkapan Rp500.000, DPP Rp3.000.000, Gedung Rp4.000.000, Total Rp3.070.000.",
"biaya,ukt,spp,kuliah,bayar,cicilan,pmb,2026,opsi 2,6 bulan,1 semester"},

{"biaya_pmb_opsi3","biaya","Biaya PMB 2026 - Opsi 3 (1 Tahun)",
"Rincian biaya pembayaran Tahap 1, 2, 3, 4 (2 Semester: 1 Tahun) PMB Gelombang 1 Februari UCIC 2026. Catatan: DPP Rp3.000.000 dan Gedung Rp4.000.000 mendapat diskon 100% jadi gratis. Komponen Total adalah total pembayaran.\n- S1 Teknik Informatika: Tahap-1 Rp8.260.000, Pendaftaran Rp250.000, Perlengkapan Rp500.000, DPP Rp3.000.000, Gedung Rp4.000.000, Total Rp9.010.000.\n- S1 Teknik Informatika (Sore): Tahap-1 Rp8.540.000, Pendaftaran Rp250.000, Perlengkapan Rp500.000, DPP Rp3.000.000, Gedung Rp4.000.000, Total Rp9.290.000.\n- S1 Sistem Informasi: Tahap-1 Rp8.260.000, Pendaftaran Rp250.000, Perlengkapan Rp500.000, DPP Rp3.000.000, Gedung Rp4.000.000, Total Rp9.010.000.\n- S1 Sistem Informasi (Sore): Tahap-1 Rp8.540.000, Pendaftaran Rp250.000, Perlengkapan Rp500.000, DPP Rp3.000.000, Gedung Rp4.000.000, Total Rp9.290.000.\n- S1 Manajemen: Tahap-1 Rp7.060.000, Pendaftaran Rp250.000, Perlengkapan Rp500.000, DPP Rp3.000.000, Gedung Rp4.000.000, Total Rp7.810.000.\n- S1 Manajemen (Sore): Tahap-1 Rp7.400.000, Pendaftaran Rp250.000, Perlengkapan Rp500.000, DPP Rp3.000.000, Gedung Rp4.000.000, Total Rp8.150.000.\n- S1 Manajemen (Kelas RPI): Tahap-1 Rp8.060.000, Pendaftaran Rp250.000, Perlengkapan Rp500.000, DPP Rp3.000.000, Gedung Rp4.000.000, Total Rp8.810.000.",
"biaya,ukt,spp,kuliah,bayar,cicilan,pmb,2026,opsi 3,1 tahun,2 semester"},

{"pendaftaran_pmb","pendaftaran","Pendaftaran Mahasiswa Baru (PMB)",
"Cara daftar: isi formulir online atau offline, lengkapi dokumen, lalu bayar biaya pendaftaran sesuai skema PMB yang berlaku. Pada rincian PMB 2026 yang tersimpan, biaya pendaftaran tercantum Rp250.000.\nSyarat: lulusan SMA/SMK/MA, fotokopi ijazah, KTP, KK, dan pas foto terbaru.\nInfo PMB: +62 895-1231-4188.",
"daftar,pendaftaran,pmb,mahasiswa baru,registrasi,cara daftar,masuk,persyaratan,register,admission,enrollment,how to apply"},

{"beasiswa","beasiswa","Program Beasiswa UCIC",
"UCIC menyediakan beasiswa prestasi akademik, prestasi non-akademik (olahraga/seni), KIP Kuliah (pemerintah), beasiswa internal kampus, dan beasiswa olahraga. Hubungi bagian kemahasiswaan untuk info lebih lanjut.",
"beasiswa,bantuan,gratis,potongan,scholarship,prestasi,kip,financial aid,grant,free,bantuan biaya"},

{"magang","jurusan","Program Magang UCIC",
"Mahasiswa dapat magang di semester akhir untuk mendapat pengalaman kerja nyata, membangun jaringan profesional, dan memperkuat portofolio sebagai bekal karir.",
"magang,internship,kerja,praktik,industri,pengalaman,intern,job training"},

// ===== RINGKASAN DOSEN =====
{"dosen_fti","dosen","Dosen Fakultas Teknologi Informasi (FTI) UCIC",
"Agus Sevtiana, Arif Nursetyo, Bambang Sugiarto, Kusnadi, Lena Magdalena, Linda Norhan, Marsani Asfi, Mesi Febima, Muhammad Hatta, Muhammad Afif Sulhan, Petrus Sokibi, Prema A.D.K., Ridho Taufiq Subagio, Rifqi Fahrudin, Rinaldi Adam, Unang Solihin, Verina Tita Nabila, Victor Asih, Willy Eka Septian, Wiwiek Nurkomala Dewi, Dzulfiqar Fickri Rosyid, Galih M.P., Ine Rachmawati, Tiara Khodijah.",
"dosen fti,dosen teknik informatika,dosen sistem informasi,dosen dkv,dosen mi,dosen teknologi informasi,lecturer fti,teacher fti,pengajar fti"},

{"dosen_feb","dosen","Dosen Fakultas Ekonomi dan Bisnis (FEB) UCIC",
"Aan Kanivia, Ade Elza Surachman, Dessy Kumala Dewi, Dewi Anggun Puspitarini, Fanny Andriani Setiawan, Safitri Akbari, Aditya Kurniawan Chandra, Agung Supriyadi, Amroni, Dewi Laily Purnamasari, Feri Fauzi, Gytha N.D.P.G., Hartanto, Ika Kartika, Mualif Ismail, Muthia Fariza, Ni Wayan Fitriana Ayu Lestari, Oryz Agnu Dian Wulandari, Siti Nurhajjah, Suwandi, Turini, Chandra Lukita, Ika Iryanti, Muhammad Fadheel Djamaly, Winsen Setiawan.",
"dosen feb,dosen ekonomi,dosen bisnis,dosen akuntansi,dosen manajemen,lecturer feb,teacher feb,pengajar feb"},

{"dosen_fps","dosen","Dosen Fakultas Pendidikan dan Sains (FPS) UCIC",
"Helmi Akmal Fauzan, Kidwan Alif, Nadia Nur Paramitha, Nur Muhamad Hildan, Yogi Slamet.",
"dosen fps,dosen olahraga,dosen pkor,dosen pendidikan,lecturer fps,teacher fps,pengajar fps"},

{"dosen_mku","dosen","Dosen Mata Kuliah Umum (MKU) UCIC",
"Chiang Yulius Darmawan, Mohammad Umar Fakhrudin, Sudadi Pranata, Dr. Taufan Hunneman, Viar Dwi Kartika, Yuni Awalaturrohmah Solihah.",
"dosen mku,dosen umum,dosen pancasila,dosen bahasa,dosen english,lecturer general,pengajar umum"},

// ===== DOSEN AKUNTANSI & KEUANGAN =====
{"dosen_aan_kanivia","dosen","Aan Kanivia, S.E., M.Ak.","Mata kuliah: Auditing 1 & 2, Praktikum Auditing, Praktikum Akuntansi Keuangan, Hukum Perpajakan, Audit Internal, Analisis Laporan Keuangan, Bisnis Digital, Penatausahaan Dokumen Perkantoran.","aan kanivia,akuntansi,auditing,pajak,dosen feb,dosen akuntansi"},
{"dosen_ade_elza","dosen","Ade Elza Surachman, S.E., M.Ak., Ak.","Mata kuliah: Akuntansi Biaya, Akuntansi Manajemen, Akuntansi Sosial & Lingkungan, Sustainability Reporting, Teori Portofolio & Analisis Investasi, Etika Profesi Kinerja, Ekonomi Makro.","ade elza surachman,ade eliza s,akuntansi,biaya,investasi,etika,dosen feb"},
{"dosen_dessy_kumala","dosen","Dessy Kumala Dewi, S.E., M.Ak.","Mata kuliah: Akuntansi Internasional, Akuntansi Entitas Privat, Analisis Investasi, Statistika & Analisis Bisnis, Metode Kualitatif & Kuantitatif, Mengolah Data Perkantoran.","dessy kumala dewi,akuntansi,investasi,statistika,metode penelitian,dosen feb"},
{"dosen_dewi_anggun","dosen","Dewi Anggun Puspitarini, S.E., M.Ak.","Mata kuliah: Ekonomi Makro, Etika Profesi Akuntan, Akuntansi Syariah, Akuntansi Keperilakuan.","dewi anggun,ekonomi makro,akuntansi syariah,akuntansi keperilakuan,dosen feb"},
{"dosen_fanny_andriani","dosen","Fanny Andriani Setiawan, S.E., M.M., Ak., CA","Mata kuliah: Akuntansi Keuangan Menengah, Manajemen Keuangan.","fanny andriani,fanny andrian,akuntansi keuangan,manajemen keuangan,dosen feb"},
{"dosen_safitri_akbari","dosen","Safitri Akbari, S.E., M.Ak.","Mata kuliah: Akuntansi Keuangan Dasar, Menengah & Lanjutan, Akuntansi Perpajakan, Teori Akuntansi, Manajemen Anggaran, Sistem Akuntansi, Analisis Laporan Keuangan, Pengoperasian Aplikasi dan Alat Bantu Perkantoran.","safitri akbari,safiri akbari,akuntansi,anggaran,pajak,sistem akuntansi,pengoperasian aplikasi dan alat bantu perkantoran,dosen feb"},

// ===== DOSEN MANAJEMEN & BISNIS =====
{"dosen_aditya_kurniawan","dosen","Aditya Kurniawan Chandra, B.Com. (Acc & Fin), M.M.","Mata kuliah: Financial Technology, Bisnis Digital, Komunikasi Bisnis & Negosiasi, Sistem Informasi Akuntansi.","aditya kurniawan,aditya kurniawan chandra,bcom,acc fin,financial technology,bisnis digital,komunikasi bisnis,dosen feb"},
{"dosen_agung_supriyadi","dosen","Agung Supriyadi, S.E., M.Ak.","Mata kuliah: AI for Accounting, Financial Technology, Teknologi Keuangan (FinTech), Manajemen Keuangan, Akuntansi Keuangan Dasar, Praktikum Perpajakan, Teknologi Informasi untuk Bisnis, Ekonomi Makro, Komunikasi & Teamwork.","agung supriyadi,aquung supriyadi,ai accounting,fintech,teknologi keuangan,manajemen keuangan,teknologi informasi bisnis,dosen feb"},
{"dosen_amroni","dosen","Amroni, S.E., M.M.","Mata kuliah: Aplikasi Software Akuntansi, Pendidikan Agama Islam, Teknologi Keuangan, Akuntansi Syariah, Kemampuan Menyelesaikan Masalah Teknis, Kreativitas dan Inisiatif, Sistem Informasi Akuntansi, Manajemen Bisnis.","amroni,aplikasi software akuntansi,pendidikan agama islam,teknologi keuangan,akuntansi syariah,kreativitas,kreatifitas,sistem informasi akuntansi,manajemen bisnis,dosen feb"},
{"dosen_dewi_laily","dosen","Dewi Laily Purnamasari, S.T., M.M.","Mata kuliah: Sustainability & ESG in Business, Manajemen Hubungan Pelanggan, Manajemen Tataletak & Planogram, Manajemen & Bisnis Dasar, Kreativitas.","dewi laily,sustainability,esg,manajemen bisnis,ritel,kreativitas,kreatifitas,dosen feb"},
{"dosen_feri_fauzi","dosen","Feri Fauzi, S.E., M.M.","Mata kuliah: Manajemen Keuangan, Technopreneurship, Technopreneurship Lanjutan, Seminar Strategi Pemasaran, Sustainability & ESG in Business, Ekonomi Makro, Pendidikan Agama Islam, Manajemen & Bisnis Dasar, Kemampuan Kerja Tim.","feri fauzi,manajemen keuangan,technopreneurship,technopreneurship lanjutan,pemasaran,agama islam,manajemen bisnis dasar,dosen feb"},
{"dosen_gytha_nurhana","dosen","Gytha N.D.P.G., S.E., M.M.","Mata kuliah: Manajemen Anggaran, Manajemen Kas & Portofolio, Matematika Ekonomi & Bisnis, Ekonomi Mikro, Etika Profesi Bisnis, Pengantar Ilmu Ekonomi Bisnis, Kemampuan Merumuskan Masalah Teknis, Manajemen & Bisnis Dasar.","gytha nurhana,gytha nurhana dhea,manajemen anggaran,ekonomi mikro,matematika ekonomi,kemampuan merumuskan masalah teknis,manajemen bisnis dasar,dosen feb"},
{"dosen_hartanto","dosen","Hartanto, S.E., M.M.","Mata kuliah: Digital Marketing, Ekonomika Bisnis, Academic Writing 2.","hartanto,digital marketing,ekonomi bisnis,academic writing,academik writing,dosen feb"},
{"dosen_ika_kartika","dosen","Ika Kartika, S.Kom., M.E.","Mata kuliah: Akuntansi Keuangan Bisnis, Penganggaran Perusahaan, Ekspor-Impor, Teknologi Informasi untuk Bisnis, Kepemimpinan Entrepreneur, Kemampuan Administrasi.","ika kartika,akuntansi bisnis,ekspor impor,teknologi informasi,kewirausahaan,dosen feb"},
{"dosen_mualif_ismail","dosen","Mualif Ismail, S.M., M.M.","Mata kuliah: Perilaku Organisasional, Statistika & Analisis Bisnis, Manajemen Operasi, Manajemen Anggaran.","mualif ismail,perilaku organisasi,statistika bisnis,manajemen operasi,dosen feb"},
{"dosen_muthia_fariza","dosen","Muthia Fariza, S.IP., M.M.","Mata kuliah: Manajemen Strategis, Manajemen SDM, Manajemen Kualitas, Perilaku Konsumen, Komunikasi Bisnis, Kepemimpinan, Manajemen & Bisnis Dasar, Metode Kualitatif & Kuantitatif.","muthia fariza,manajemen strategis,sdm,perilaku konsumen,kepemimpinan,manajemen bisnis dasar,dosen feb"},
{"dosen_ni_wayan","dosen","Ni Wayan Fitriana Ayu Lestari, S.E., M.M.","Mata kuliah: Manajemen SDM, Kedisiplinan Kerja, Hukum Bisnis.","ni wayan,ni wayan fal,manajemen sdm,hukum bisnis,kedisiplinan,dosen feb"},
{"dosen_oryz_agnu","dosen","Oryz Agnu Dian Wulandari, S.E., M.M.","Mata kuliah: Manajemen Pemasaran (AI for Marketing), Digital Marketing, Statistik, Statistika Bisnis, Technopreneurship Lanjutan, Etika & Komunikasi Bisnis, Kreativitas.","oryz agnu,uryz agnu,manajemen pemasaran,digital marketing,statistik,technopreneurship,dosen feb"},
{"dosen_siti_nurhajjah","dosen","Siti Nurhajjah, S.Ab., M.Si.","Mata kuliah: Strategi Pemasaran Ritel, Metodologi Penelitian Bisnis, Manajemen Operasi, Manajemen Strategik, Hukum Bisnis, Pengantar Akuntansi, Praktikum Akuntansi Manual, Manajemen & Bisnis Dasar, Kemampuan Berkomunikasi, Etika Profesi Bisnis.","siti nurhajjah,sitta nurhajjah,pemasaran ritel,metodologi penelitian,manajemen strategik,hukum bisnis,pengantar akuntansi,praktikum akuntansi manual,kemampuan berkomunikasi,dosen feb"},
{"dosen_suwandi","dosen","Suwandi, S.E., M.M.","Mata kuliah: Technopreneurship Lanjutan, Financial Management, Manajemen Inovasi Berbasis Teknologi, Manajemen Transformasi Digital, Statistika & Analisis Bisnis, Matematika Ekonomi & Bisnis, Perilaku Organisasional, Kemampuan Komunikasi, Teknologi Informasi untuk Bisnis.","suwandi,suwardi,technopreneurship,manajemen keuangan,statistika,matematika ekonomi,perilaku organisasional,kemampuan komunikasi,inovasi teknologi,dosen feb"},
{"dosen_turini","dosen","Turini, S.E., M.M.","Mata kuliah: Dasar-Dasar Perpajakan, Studi Kelayakan Bisnis, Teknologi Informasi untuk Bisnis, Pembangunan Karakter & Etika Profesi.","turini,y turini,perpajakan,studi kelayakan bisnis,etika profesi,dosen feb"},

// ===== DOSEN TEKNOLOGI INFORMASI =====
{"dosen_agus_sevtiana","dosen","Agus Sevtiana, S.T., M.Kom.","Mata kuliah: UI & UX Design, Design Thinking, E-Commerce & Platform, Komputer Grafis & Multimedia, Digital Content Interaktif, E-Government, Computer Animation, Jaringan Komputer.","agus sevtiana,agus sevitana,agus seviana,agus sevitana,ui ux,komputer grafis,multimedia,e commerce,design thinking,dosen fti"},
{"dosen_arif_nursetyo","dosen","Arif Nursetyo, S.Kom., M.Kom.","Mata kuliah: Kecerdasan Buatan, Computer Vision & NLP, Etika dalam AI, Pengujian & Pemeliharaan Sistem.","arif nursetyo,arif nurestyo,software testing,ai,etika ai,computer vision,nlp,dosen fti"},
{"dosen_bambang_sugiarto","dosen","Bambang Sugiarto, S.Kom., M.Kom.","Mata kuliah: Rekayasa Aplikasi Internet, Basic Mobile Programming, Mobile Programming, Pemrograman Internet, Pemrograman OOP, Pemrograman Berorientasi Objek, Realitas Virtual dan Augmentasi, VR & AR, Sistem Pakar, Pengembangan Aplikasi Mobile, Student Exchange (AR), Student Exchange (Augmented Reality).","bambang sugiarto,rekayasa aplikasi internet,basic mobile programming,mobile programming,pemrograman internet,pemrograman berorientasi objek,realitas virtual,vr ar,augmented reality,sistem pakar,pbo,dosen fti"},
{"dosen_kusnadi","dosen","Kusnadi, S.Kom., M.Kom.","Mata kuliah: Algoritma & Pemrograman, Struktur Data, UI & UX Designer, IoT, Proyek Technopreneurship, Komunikasi & Teamwork, Kerja Tim dan Kolaborasi, Keterampilan Presentasi, Keterampilan Interpersonal, Penguasaan Teknologi.","kusnadi,struktur data,algoritma,algoritma dan pemrograman,ui ux,iot,technopreneurship,kerja tim,kolaborasi,penguasaan teknologi,dosen fti"},
{"dosen_lena_magdalena","dosen","Lena Magdalena, S.Kom., MMSI.","Mata kuliah: Manajemen dan Organisasi, Rekayasa Perangkat Lunak (Agile Scrum & Implementation), Analisa Sistem Berorientasi Objek, Analisa & Perancangan Sistem Informasi, Audit Sistem Informasi, Teknologi dan Tren E-Commerce, Komunikasi & Teamwork, Ide, Inisiatif dan Tanggung Jawab.","lena magdalena,rpl,agile scrum,introduction,implementation,audit sistem informasi,analisa sistem,e commerce,manajemen dan organisasi,komunikasi teamwork,dosen fti"},
{"dosen_linda_norhan","dosen","Linda Norhan, S.T., M.M.","Mata kuliah: Monetisasi Konten Digital, Digital Copywriting, Brand Identity, Analisa Proses Bisnis, Analisa & Perancangan Sistem Informasi, Matematika Bisnis, Aljabar Linear dan Matriks, Manajemen Periklanan.","linda norhan,linda norman,konten digital,copywriting,branding,analisa proses bisnis,matematika bisnis,aljabar linear,dosen fti"},
{"dosen_marsani_asfi","dosen","Marsani Asfi, S.Si., M.Si.","Mata kuliah: Algoritma & Pemrograman, Algoritma dan Pemrograman 2, Grafika Komputer & Pengolahan Citra, Machine Learning, Kecerdasan Buatan, Metodologi Penelitian, Metodologi Penelitian dan Riset Pustaka, Matematika Ekonomi & Bisnis, Manajemen Inovasi Berbasis Teknologi.","marsani asfi,algoritma,algoritma dan pemrograman 2,grafika komputer,machine learning,ai,metodologi penelitian,riset pustaka,manajemen inovasi,dosen fti"},
{"dosen_mesi_febima","dosen","Mesi Febima, M.Kom.","Mata kuliah: Business Intelligence, Manajemen Proyek, Sistem Informasi Manajemen, E-CRM, AI for Web Design, Pemrograman Internet, Pemrograman Internet 1, Analisa Proses Bisnis, Integrasi Sistem Enterprise, Etika Profesi Kerja, Tata Tulis Karya Ilmiah, Penguasaan Teknologi.","mesi febima,e crm,business intelligence,manajemen proyek,sistem informasi manajemen,web design,pemrograman internet 1,tata tulis karya ilmiah,penguasaan teknologi,dosen fti"},
{"dosen_muhammad_hatta","dosen","Muhammad Hatta, M.Kom.","Mata kuliah: Algoritma dan Pemrograman 2, Machine Learning, Mobile Programming, Keamanan Sistem Informasi & Jaringan, Pemrograman OOP, Pemrograman Berorientasi Objek, Pemrograman Internet (Framework MVC), Pemrograman Internet Intermediate (Framework, MVC), Perancangan Aplikasi Mobile.","muhammad hatta,mobile programming,machine learning,algoritma dan pemrograman 2,keamanan sistem,pbo,pemrograman berorientasi objek,framework mvc,pemrograman internet intermediate,dosen fti"},
{"dosen_muhammad_afif","dosen","Muhammad Afif Sulhan, M.Kom.","Mata kuliah: Domain & Manajemen Web Server, Keamanan Web.","muhammad afif,web server,keamanan web,domain,dosen fti"},
{"dosen_petrus_sokibi","dosen","Petrus Sokibi, S.Kom., M.Kom.","Mata kuliah: Algoritma & Pemrograman, Struktur Data, Sistem Basis Data, Manajemen Data & Informasi, Arsitektur SI/IT Perusahaan, Technopreneurship, Sistem dan Teknologi Informasi, Etika Profesi Kerja, Ide, Inisiatif dan Tanggung Jawab, Pancasila & Kewarganegaraan, Pancasila dan Kewarganegaraan dan Anti Korupsi, Pendidikan Agama Katolik, Pendidikan Agama Kristen, Pendidikan Agama Budha, Pendidikan Agama Hindu, Pendidikan Agama Konghucu.","petrus sokibi,petrus s,struktur data,basis data,algoritma dan pemrograman,arsitektur si,technopreneurship,sistem dan teknologi informasi,etika profesi kerja,anti korupsi,dosen fti,agama,agama katolik,agama kristen,agama budha,agama hindu,agama konghucu,katolik,kristen,budha,hindu,konghucu"},
{"dosen_prema_adk","dosen","Prema A.D.K., M.Kom.","Mata kuliah: Pemrograman Internet (Framework MVC), Pemrograman Internet Intermediate (Framework, MVC), AI for Web Design (HTML, CSS, Copilot).","prema adk,pemrograman internet,pemrograman internet intermediate,framework mvc,html,css,copilot,web design,ai,dosen fti"},
{"dosen_ricky_perdana","dosen","Ricky Perdana Kusuma, M.Kom.","Mata kuliah: Integrated Marketing Communication, Optimasi Aplikasi Web, IMC, Etika & Regulasi Media Digital, Komunikasi Data, Kewirausahaan Digital, Penguasaan Pemrograman & Design.","ricky perdana,integrated marketing communication,imc,optimasi web,etika media digital,komunikasi data,kewirausahaan digital,penguasaan perongruaman design,dosen fti"},
{"dosen_ridho_taufiq","dosen","Ridho Taufiq Subagio, S.T., M.Kom.","Mata kuliah: Komunikasi Data & Jaringan, Sistem Digital, IoT, Internet of Things, Sistem Penunjang Keputusan, Jaminan Kualitas Perangkat Lunak, Audit Sistem Informasi, Analisis dan Desain Perangkat Lunak, Metodologi Penelitian, Metodologi Penelitian & Riset Pustaka, Tata Tulis Karya Ilmiah.","ridho taufiq,ridho taufiq s,jaringan komputer,sistem digital,iot,internet of things,audit si,analisis dan desain perangkat lunak,metodologi penelitian,riset pustaka,dosen fti"},
{"dosen_rifqi_fahrudin","dosen","Rifqi Fahrudin, S.Kom., M.Kom.","Mata kuliah: UI/UX Design, UI/UX Design (Intermediate), UI & UX Designer, Interaksi Manusia & Komputer, Proyek Technopreneurship, Student Exchange (Agile Implementation).","rifqi fahrudin,ui ux,ui ux intermediate,ui ux designer,interaksi manusia komputer,technopreneurship,agile,dosen fti"},
{"dosen_rinaldi_adam","dosen","Rinaldi Adam, M.Comp.","Mata kuliah: Data Mining & Data Warehouse, Sistem Penunjang Keputusan, Manajemen Proyek Perangkat Lunak.","rinaldi adam,kinaidi adam,data mining,data warehouse,sistem penunjang keputusan,manajemen proyek,dosen fti"},
{"dosen_unang_solihin","dosen","Unang Solihin, M.Kom.","Mata kuliah: Sistem Terdistribusi, Keamanan Sistem Informasi & Jaringan, Jaringan Komputer, Administrasi Jaringan, Manajemen Proyek.","unang solihin,sistem terdistribusi,keamanan jaringan,administrasi jaringan,dosen fti"},
{"dosen_verina_tita","dosen","Verina Tita Nabila, M.Si.","Mata kuliah: Data Mining, Machine Learning, Deep Learning, Matematika Ekonomi & Bisnis.","verina tita,data mining,machine learning,deep learning,matematika,dosen fti"},
{"dosen_victor_asih","dosen","Victor Asih, S.Si., M.T.","Mata kuliah: Sistem Operasi, Arsitektur & Organisasi Komputer.","victor asih,sistem operasi,arsitektur komputer,dosen fti"},
{"dosen_willy_eka","dosen","Willy Eka Septian, S.Kom., M.Kom.","Mata kuliah: Rekayasa Aplikasi Internet, Web Programming, Web Programming 1 (Konsep OOP PHP), Web Design, Pemrograman Internet, E-Commerce & Platform, Enterprise Resource Planning, Basis Data & Implementasi, Manajemen Proyek Perangkat Lunak, UI/UX Design, UI/UX Design (Dasar), Manajemen Transformasi Digital, Implementasi Arsitektur Enterprise, Management Information System.","willy eka,willy septian eka,wilyi eka,web programming,web design,rekayasa aplikasi internet,pemrograman internet,erp,e commerce,basis data implementasi,manajemen proyek,ui ux,dosen fti"},
{"dosen_wiwiek_nurkomala","dosen","Wiwiek Nurkomala Dewi, S.T., M.M.","Mata kuliah: Statistik for Data Science, Matematika Informatika, Digital Bisnis Startup, Kewirausahaan Digital, Tata Tulis Karya Ilmiah, Etika Profesi Kerja, Komunikasi Data, Komunikasi & Teamwork, HaKI & Publikasi, Penerapan Teknologi Industri.","wiwiek nurkomala,wiwiek nd,statistika data science,startup digital,haki,etika profesi,komunikasi data,kewirausahaan digital,dosen fti"},

// ===== DOSEN DKV =====
{"dosen_dzulfiqar_fickri","dosen","Dzulfiqar Fickri Rosyid, M.Ds.","Mata kuliah: Komunikasi Digital, Sosiologi Desain, Desain Komunikasi Visual Sosial, Bahasa Rupa, Semiotika Visual, Intelligent Audio Visual Effect, Eksperimen & Ideasi Visual, Inovasi Grafis, Video Brand Production, Game Cerdas, Game Desain.","dzulfiqar fickri,dkv,komunikasi visual,desain komunikasi visual sosial,audio visual,game cerdas,game desain,semiotika,dosen fti"},
{"dosen_galih_mp","dosen","Galih M.P., M.Ds.","Mata kuliah: Creative Content Advertising, Desain Komunikasi Visual Promosi, Design Thinking, Estetika Visual Desain, Tinjauan & Kritik Desain, Technopreneurship.","galih mp,dkv,advertising,desain promosi,design thinking,estetika,dosen fti"},
{"dosen_ine_rachmawati","dosen","Ine Rachmawati, M.Sn.","Mata kuliah: Fotografi Digital, Applied Photography, Basic Fotografi, Videografi, Student Exchange (Fotografi), Dasar Desain Grafis, Metoda Produksi Grafika, Media Interaktif, Psikologi Persepsi, Portofolio DKV, Metodologi Penelitian Desain, Tata Tulis Karya Ilmiah, Ide, Inisiatif dan Tanggung Jawab.","ine rachmawati,ine kachmawati,dkv,fotografi,applied photography,basic fotografi,videografi,student exchange fotografi,desain grafis,metodologi penelitian,dosen fti"},
{"dosen_tiara_khodijah","dosen","Tiara Khodijah, M.Ds.","Mata kuliah: Graphic Design, Animasi 3D, Tipografi, Kinetic Tipografi, Packaging Design, Desain Komunikasi Visual Identitas, Desain Display, Strategic Konten Digital, Manajemen Desain & Creativepreneur, Komunikasi & Teamwork.","tiara khodijah,tiara khodiiah,dkv,animasi 3d,tipografi,kinetic tipografi,branding,packaging,dosen fti"},

// ===== DOSEN FPS =====
{"dosen_helmi_akmal","dosen","Helmi Akmal Fauzan, S.Pd., M.Pd.","Mata kuliah: Pelatihan Cabang Renang, Pelatihan Cabang Atletik, Anatomi, Bahasa Indonesia.","helmi akmal,pelatihan cabor renang,pelatihan caber renang,pelatihan cabor atletik,pelatihan caber atletik,olahraga,renang,atletik,anatomi,bahasa indonesia,dosen fps"},
{"dosen_kidwan_alif","dosen","Kidwan Alif, S.Pd., M.Pd.","Mata kuliah: Fisiologi Olahraga.","kidwan alif,fisiologi olahraga,dosen fps"},
{"dosen_nadia_nur","dosen","Nadia Nur Paramitha, S.Pd., M.Pd.","Mata kuliah: Ilmu Keolahragaan Dasar, Pembelajaran Motorik.","nadia nur,ilmu keolahragaan,pembelajaran motorik,dosen fps"},
{"dosen_nur_hildan","dosen","Nur Muhamad Hildan, S.Pd., M.Pd.","Mata kuliah: Pelatihan Cabor Sepak Bola, Pelatihan Cabor Futsal, Bahasa Indonesia.","nur hildan,sepak bola,futsal,olahraga,bahasa indonesia,dosen fps"},
{"dosen_yogi_slamet","dosen","Yogi Slamet, S.Pd., M.Pd.","Mata kuliah: Kurikulum & Pembelajaran.","yogi slamet,kurikulum,pembelajaran,dosen fps"},

// ===== DOSEN MKU =====
{"dosen_chiang_yulius","dosen","Chiang Yulius Darmawan, M.Psi., Psi.","Mata kuliah: Perilaku Organisasional, Psikologi Persepsi, Pembangunan Karakter & Etika Profesi.","chiang yulius,chiang yulius darmawan,psikologi,perilaku organisasi,etika profesi,dosen umum"},
{"dosen_moh_umar","dosen","Mohammad Umar Fakhrudin, S.Pd., M.Pd.","Mata kuliah: General English, Intermediate English, TOEFL Preparation, English for Specific Purpose, Bahasa Indonesia.","moh umar,moh umar fahkrudin,mohammad umar fakhrudin,bahasa inggris,toefl,bahasa indonesia,dosen umum"},
{"dosen_sudadi_pranata","dosen","Sudadi Pranata, S.E., M.Si.","Mata kuliah: Academic Writing 1 & 2, Pancasila & Kewarganegaraan, Metodologi Penelitian, Kecerdasan Buatan, Data Analytics for Business Decision, Manajemen Operasi, Manajemen SDM, Literasi Digital, Komunikasi Bisnis & Negosiasi, Kemampuan Kerja Tim.","sudadi pranata,academic writing,academik writing,pancasila,metodologi penelitian,data analytics,komunikasi bisnis,kemampuan kerja tim,dosen umum"},
{"dosen_taufan_hunneman","dosen","Dr. Taufan Hunneman","Mata kuliah: Pancasila & Kewarganegaraan, Hukum Bisnis.","taufan hunneman,taufan huneman,pancasila,hukum bisnis,dosen umum"},
{"dosen_viar_dwi","dosen","Viar Dwi Kartika, S.Pd., M.Pd.","Mata kuliah: General English, Intermediate English, English for Business, English for Specific Purpose, TOEFL Preparation, Kemampuan Bekerjasama.","viar dwi,bahasa inggris,english for business,english for specific purpose,toefl,dosen umum"},
{"dosen_yuni_awalaturrohmah","dosen","Yuni Awalaturrohmah Solihah, M.Pd.","Mata kuliah: General English, Intermediate English, English for Business, TOEFL Preparation, English for Specific Purpose, Advertising Copywriting, Etika Profesi Kerja.","yuni awalaturrohmah,yuni a s,yuni a.s,bahasa inggris,toefl,copywriting,dosen umum"},

// ===== DOSEN TAMBAHAN =====
{"dosen_chandra_lukita","dosen","Assoc. Prof. Dr. Chandra Lukita, S.E., M.M., M.TI.","Rektor UCIC. Mata kuliah: Technopreneurship Lanjutan, Studi Kelayakan Bisnis, Data Analytics for Business Decision, Academic Writing 2.","chandra lukita,rektor,technopreneurship,data analytics,academic writing,academik writing,studi kelayakan bisnis"},
{"dosen_ika_iryanti","dosen","Dr. Ika Iryanti, S.E., M.M.","Mata kuliah: Manajemen Strategis, Manajemen Strategik, Analisis Investasi, Seminar Strategi Pemasaran, Metodologi Penelitian, Matematika Ekonomi & Bisnis.","ika iryanti,manajemen strategis,investasi,metodologi penelitian,dosen feb"},
{"dosen_muhammad_fadheel","dosen","Dr. Muhammad Fadheel Djamaly, M.Sc.","Mata kuliah: Ekspor-Impor.","muhammad fadheel,ekspor impor,bisnis internasional"},
{"dosen_winsen_setiawan","dosen","Winsen Setiawan, M.T.","Mata kuliah: Ekonomi Makro.","winsen setiawan,ekonomi makro"},
{"dosen_team_ai","dosen","Team AI","Mata kuliah: Kecerdasan Buatan (untuk berbagai program studi).","team ai,kecerdasan buatan,artificial intelligence"},

// ===== FAQ & RINGKASAN =====
{"faq_ucic","profil","Rekomendasi Jurusan UCIC",
"Tertarik komputer & teknologi? Pilih Teknik Informatika atau Sistem Informasi.\nSuka desain & visual? Pilih Desain Komunikasi Visual.\nMinat bisnis & keuangan? Pilih Manajemen atau Akuntansi.\nIngin bisnis berbasis digital? Pilih Bisnis Digital.\nHobi atau berprestasi di olahraga? Pilih Pendidikan Kepelatihan Keolahragaan.\nIngin kuliah 3 tahun? Pilih D3 Manajemen Informatika atau D3 Manajemen Bisnis.",
"faq,tanya jawab,ucic bagus,peluang kerja,rekomendasi jurusan,komputer,desain,bisnis,good,recommendation,bagus"},

{"data_lengkap","profil","Data Ringkas UCIC",
"Universitas Catur Insan Cendekia (UCIC), Cirebon.\nRektor: Assoc. Prof. Dr. Chandra Lukita, S.E., M.M., M.TI.\nKampus 1: Jl. Kesambi No. 202 | Kampus 2: Jl. Kesambi No. 58A. WA: +62 895-1231-4188 | Email: info@ucic.ac.id.\nFakultas: FTI (S1 TI, SI, DKV; D3 MI), FEB (S1 Akt, Mjn, Bisdi; D3 MB), FPS (S1 Pendidikan Kepelatihan Keolahragaan/Pikor/PKOR).\nBeasiswa: prestasi akademik, non-akademik, KIP Kuliah, internal, olahraga.",
"ucic,lengkap,data,semua,informasi,universitas,kampus,complete,all,summary,ringkasan,jurusan apa saja"},
};

static const int CAMPUS_DB_SIZE = sizeof(CAMPUS_DB) / sizeof(CAMPUS_DB[0]);

int GetCampusEntryCount() {
    return CAMPUS_DB_SIZE;
}

namespace {

struct SearchQuery {
    std::string normalized;
    std::vector<std::string> tokens;
    std::vector<std::string> phrases;
};

struct Match {
    int idx = -1;
    int score = 0;
};

std::string NormalizeText(const char* text) {
    if (text == nullptr) {
        return "";
    }

    std::string normalized;
    normalized.reserve(strlen(text) + 1);
    bool previous_space = true;
    for (const unsigned char* ptr = reinterpret_cast<const unsigned char*>(text); *ptr != '\0'; ++ptr) {
        unsigned char ch = *ptr;
        if (std::isalnum(ch)) {
            normalized.push_back(static_cast<char>(std::tolower(ch)));
            previous_space = false;
        } else if (!previous_space) {
            normalized.push_back(' ');
            previous_space = true;
        }
    }

    while (!normalized.empty() && normalized.back() == ' ') {
        normalized.pop_back();
    }
    return normalized;
}

void PushUnique(std::vector<std::string>& values, const std::string& value) {
    if (value.empty()) {
        return;
    }
    if (std::find(values.begin(), values.end(), value) == values.end()) {
        values.push_back(value);
    }
}

bool ContainsText(const std::string& haystack, const std::string& needle) {
    return !needle.empty() && haystack.find(needle) != std::string::npos;
}

bool IsLecturerEntry(const CampusEntry* entry);
template <size_t N>
void InsertMatch(Match (&top)[N], int idx, int score);
bool AppendLecturerEntry(char* buf, int buf_size, int& pos, const CampusEntry* entry);
bool IsIgnoredToken(const std::string& token);

void ReplaceAll(std::string& text, const std::string& from, const std::string& to) {
    if (from.empty() || from == to) {
        return;
    }

    size_t pos = 0;
    while ((pos = text.find(from, pos)) != std::string::npos) {
        text.replace(pos, from.size(), to);
        pos += to.size();
    }
}

std::vector<std::string> SplitWords(const std::string& text) {
    std::vector<std::string> words;
    size_t start = 0;
    while (start < text.size()) {
        size_t end = text.find(' ', start);
        std::string word = text.substr(start, end == std::string::npos ? std::string::npos : end - start);
        if (!word.empty()) {
            words.push_back(word);
        }
        if (end == std::string::npos) {
            break;
        }
        start = end + 1;
    }
    return words;
}

int LimitedLevenshteinDistance(const std::string& left, const char* right, int max_distance) {
    size_t left_len = left.size();
    size_t right_len = strlen(right);
    if (left_len == 0) {
        return right_len <= static_cast<size_t>(max_distance) ? static_cast<int>(right_len) : max_distance + 1;
    }
    if (right_len == 0) {
        return left_len <= static_cast<size_t>(max_distance) ? static_cast<int>(left_len) : max_distance + 1;
    }
    if (left_len > right_len + static_cast<size_t>(max_distance) ||
        right_len > left_len + static_cast<size_t>(max_distance)) {
        return max_distance + 1;
    }

    std::vector<int> prev(right_len + 1);
    std::vector<int> curr(right_len + 1);
    for (size_t j = 0; j <= right_len; ++j) {
        prev[j] = static_cast<int>(j);
    }

    for (size_t i = 1; i <= left_len; ++i) {
        curr[0] = static_cast<int>(i);
        int row_best = curr[0];
        for (size_t j = 1; j <= right_len; ++j) {
            int cost = left[i - 1] == right[j - 1] ? 0 : 1;
            curr[j] = std::min({prev[j] + 1, curr[j - 1] + 1, prev[j - 1] + cost});
            row_best = std::min(row_best, curr[j]);
        }
        if (row_best > max_distance) {
            return max_distance + 1;
        }
        prev.swap(curr);
    }

    return prev[right_len];
}

int TokenMatchPercent(const std::string& query_token, const char* vocab_token) {
    if (query_token.empty() || vocab_token == nullptr || *vocab_token == '\0') {
        return 0;
    }
    if (query_token == vocab_token) {
        return 100;
    }

    size_t query_len = query_token.size();
    size_t vocab_len = strlen(vocab_token);
    if (query_len >= 5 && (ContainsText(vocab_token, query_token) || ContainsText(query_token, vocab_token))) {
        return 76;
    }

    if (query_len >= 4 && std::tolower(static_cast<unsigned char>(query_token[0])) == std::tolower(static_cast<unsigned char>(vocab_token[0]))) {
        int distance = LimitedLevenshteinDistance(query_token, vocab_token, 1);
        if (distance <= 1) {
            return 72;
        }
    }

    if (query_len >= 6 && vocab_len >= 6) {
        size_t prefix = 0;
        size_t common = std::min(query_len, vocab_len);
        while (prefix < common && query_token[prefix] == static_cast<char>(std::tolower(static_cast<unsigned char>(vocab_token[prefix])))) {
            ++prefix;
        }
        if (prefix >= 5) {
            return 64;
        }
    }

    return 0;
}

std::string SimplifyApproxToken(std::string token) {
    ReplaceAll(token, "ph", "f");
    ReplaceAll(token, "sy", "s");
    ReplaceAll(token, "sh", "s");
    ReplaceAll(token, "kh", "k");
    ReplaceAll(token, "dj", "j");
    ReplaceAll(token, "tj", "c");
    ReplaceAll(token, "oe", "u");
    ReplaceAll(token, "q", "k");
    ReplaceAll(token, "v", "f");
    ReplaceAll(token, "x", "ks");

    std::string simplified;
    simplified.reserve(token.size());
    char previous = '\0';
    for (char ch : token) {
        if (ch == previous && std::isalpha(static_cast<unsigned char>(ch))) {
            continue;
        }
        simplified.push_back(ch);
        previous = ch;
    }
    return simplified;
}

std::string ConsonantSkeleton(const std::string& token) {
    std::string simplified = SimplifyApproxToken(token);
    std::string skeleton;
    skeleton.reserve(simplified.size());
    for (char ch : simplified) {
        char lower = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
        if (lower == 'a' || lower == 'i' || lower == 'u' || lower == 'e' || lower == 'o') {
            continue;
        }
        skeleton.push_back(lower);
    }
    return skeleton;
}

bool EditDistanceWithin(const std::string& lhs, const std::string& rhs, int max_distance) {
    if (lhs == rhs) {
        return true;
    }

    const int lhs_size = static_cast<int>(lhs.size());
    const int rhs_size = static_cast<int>(rhs.size());
    if (std::abs(lhs_size - rhs_size) > max_distance) {
        return false;
    }

    std::vector<int> previous(rhs_size + 1);
    std::vector<int> current(rhs_size + 1);
    for (int j = 0; j <= rhs_size; ++j) {
        previous[j] = j;
    }

    for (int i = 1; i <= lhs_size; ++i) {
        current[0] = i;
        int best_in_row = current[0];
        for (int j = 1; j <= rhs_size; ++j) {
            int cost = lhs[static_cast<size_t>(i - 1)] == rhs[static_cast<size_t>(j - 1)] ? 0 : 1;
            current[j] = std::min({previous[j] + 1, current[j - 1] + 1, previous[j - 1] + cost});
            best_in_row = std::min(best_in_row, current[j]);
        }
        if (best_in_row > max_distance) {
            return false;
        }
        previous.swap(current);
    }

    return previous[rhs_size] <= max_distance;
}

bool TokensApproximatelyMatch(const std::string& lhs, const std::string& rhs) {
    if (lhs == rhs) {
        return true;
    }
    if (lhs.size() < 3 || rhs.size() < 3) {
        return false;
    }

    std::string simplified_lhs = SimplifyApproxToken(lhs);
    std::string simplified_rhs = SimplifyApproxToken(rhs);
    if (simplified_lhs == simplified_rhs) {
        return true;
    }

    const int max_len = static_cast<int>(std::max(simplified_lhs.size(), simplified_rhs.size()));
    const int max_distance = max_len >= 8 ? 2 : 1;
    if (EditDistanceWithin(simplified_lhs, simplified_rhs, max_distance)) {
        return true;
    }

    std::string skeleton_lhs = ConsonantSkeleton(simplified_lhs);
    std::string skeleton_rhs = ConsonantSkeleton(simplified_rhs);
    if (skeleton_lhs.size() >= 4 && skeleton_rhs.size() >= 4 &&
        std::abs(static_cast<int>(skeleton_lhs.size()) - static_cast<int>(skeleton_rhs.size())) <= 1 &&
        skeleton_lhs.front() == skeleton_rhs.front()) {
        int skeleton_distance = std::max(1, max_distance - 1);
        if (EditDistanceWithin(skeleton_lhs, skeleton_rhs, skeleton_distance)) {
            return true;
        }
    }

    return false;
}

bool ContainsApproxToken(const std::string& text, const std::string& token) {
    if (token.size() < 4) {
        return false;
    }
    for (const auto& word : SplitWords(text)) {
        if (TokensApproximatelyMatch(word, token)) {
            return true;
        }
    }
    return false;
}

bool LooksLikeUcicReference(const std::string& normalized) {
    if (ContainsText(normalized, "u c i c") || ContainsText(normalized, "u c i")) {
        return true;
    }

    const std::vector<std::string> words = SplitWords(normalized);
    bool has_campus_word = false;
    int matched_anchors = 0;
    std::array<bool, 5> anchor_hit = {{false, false, false, false, false}};
    static const std::array<const char*, 5> kCampusAnchors = {{
        "ucic", "cic", "catur", "insan", "cendekia"
    }};

    for (const auto& word : words) {
        if (word == "ucic" || word == "cic" || word == "uci") {
            return true;
        }
        if (word == "universitas" || word == "kampus" || word == "university" || word == "campus") {
            has_campus_word = true;
        }
        for (size_t i = 0; i < kCampusAnchors.size(); ++i) {
            if (!anchor_hit[i] && TokensApproximatelyMatch(word, kCampusAnchors[i])) {
                anchor_hit[i] = true;
                ++matched_anchors;
            }
        }
    }

    return matched_anchors >= 2 || (has_campus_word && matched_anchors >= 1);
}

bool LooksLikeImplicitUcicAcademicQuery(const std::string& normalized) {
    static const std::array<const char*, 55> kAcademicSignals = {{
        "semester",
        "belajar apa",
        "belajar apa aja",
        "isi semester",
        "mata ajar",
        "kurikulum semester",
        "kurikulum",
        "mata kuliah",
        "matakuliah",
        "matkul",
        "dosen",
        "pengampu",
        "mengajar",
        "prodi",
        "jurusan",
        "fakultas",
        "profil",
        "sejarah",
        "tentang kampus",
        "logo",
        "filosofi",
        "teknik informatika",
        "sistem informasi",
        "desain komunikasi visual",
        "dkv",
        "akuntansi",
        "manajemen",
        "bisnis digital",
        "pkor",
        "pikor",
        "manajemen informatika",
        "manajemen bisnis",
        "biaya kuliah",
        "pmb",
        "beasiswa",
        "rektor",
        "dpp",
        "gedung",
        "total",
        "akreditasi",
        "alamat",
        "kontak",
        "fasilitas",
        "visi misi",
        "nilai dasar",
        "cic great",
        "rekomendasi jurusan",
        "jurusan yang cocok",
        "prodi yang cocok",
        "cocok buat saya",
        "cocok untuk saya",
        "bedanya",
        "perbedaan",
        "bandingkan",
        "vs"
    }};

    for (const char* signal : kAcademicSignals) {
        if (ContainsText(normalized, signal)) {
            return true;
        }
    }
    return ContainsApproxToken(normalized, "rektor") ||
        ContainsApproxToken(normalized, "akreditasi") ||
        ContainsApproxToken(normalized, "dosen") ||
        ContainsApproxToken(normalized, "jurusan") ||
        ContainsApproxToken(normalized, "fakultas") ||
        ContainsApproxToken(normalized, "prodi") ||
        ContainsApproxToken(normalized, "kampus");
}

bool QueryAsksPendaftaranPmb(const std::string& normalized) {
    return ContainsText(normalized, "pendaftaran") ||
        ContainsText(normalized, "pmb") ||
        ContainsText(normalized, "mahasiswa baru") ||
        ContainsText(normalized, "cara daftar") ||
        ContainsText(normalized, "registrasi") ||
        ContainsText(normalized, "syarat daftar") ||
        ContainsText(normalized, "cara masuk");
}

int ExtractSemesterNumber(const std::string& normalized) {
    for (int semester = 1; semester <= 8; ++semester) {
        std::string phrase = "semester " + std::to_string(semester);
        if (ContainsText(normalized, phrase)) {
            return semester;
        }
    }

    static const std::array<std::pair<const char*, int>, 8> kSemesterWords = {{
        {"semester satu", 1},
        {"semester dua", 2},
        {"semester tiga", 3},
        {"semester empat", 4},
        {"semester lima", 5},
        {"semester enam", 6},
        {"semester tujuh", 7},
        {"semester delapan", 8}
    }};
    for (const auto& semester_word : kSemesterWords) {
        if (ContainsText(normalized, semester_word.first)) {
            return semester_word.second;
        }
    }
    return 0;
}

std::string CollapseRepeatedSpaces(std::string text) {
    ReplaceAll(text, " ,", ",");
    ReplaceAll(text, " .", ".");
    ReplaceAll(text, " :", ":");
    ReplaceAll(text, " ;", ";");
    ReplaceAll(text, "\n ", "\n");
    while (text.find("  ") != std::string::npos) {
        ReplaceAll(text, "  ", " ");
    }
    return text;
}

bool ParseUnsignedNumber(const std::string& digits, uint64_t& value) {
    if (digits.empty()) {
        return false;
    }

    value = 0;
    for (char ch : digits) {
        if (!std::isdigit(static_cast<unsigned char>(ch))) {
            return false;
        }
        value = (value * 10) + static_cast<uint64_t>(ch - '0');
    }
    return true;
}

std::string ConvertNumberToIndonesianWords(uint64_t value) {
    static const std::array<const char*, 12> kBaseNumbers = {{
        "nol", "satu", "dua", "tiga", "empat", "lima",
        "enam", "tujuh", "delapan", "sembilan", "sepuluh", "sebelas"
    }};

    if (value < kBaseNumbers.size()) {
        return kBaseNumbers[value];
    }
    if (value < 20) {
        return ConvertNumberToIndonesianWords(value - 10) + " belas";
    }
    if (value < 100) {
        std::string result = ConvertNumberToIndonesianWords(value / 10) + " puluh";
        if ((value % 10) != 0) {
            result += " " + ConvertNumberToIndonesianWords(value % 10);
        }
        return result;
    }
    if (value < 200) {
        std::string result = "seratus";
        if ((value % 100) != 0) {
            result += " " + ConvertNumberToIndonesianWords(value % 100);
        }
        return result;
    }
    if (value < 1000) {
        std::string result = ConvertNumberToIndonesianWords(value / 100) + " ratus";
        if ((value % 100) != 0) {
            result += " " + ConvertNumberToIndonesianWords(value % 100);
        }
        return result;
    }
    if (value < 2000) {
        std::string result = "seribu";
        if ((value % 1000) != 0) {
            result += " " + ConvertNumberToIndonesianWords(value % 1000);
        }
        return result;
    }
    if (value < 1000000) {
        std::string result = ConvertNumberToIndonesianWords(value / 1000) + " ribu";
        if ((value % 1000) != 0) {
            result += " " + ConvertNumberToIndonesianWords(value % 1000);
        }
        return result;
    }
    if (value < 1000000000ULL) {
        std::string result = ConvertNumberToIndonesianWords(value / 1000000ULL) + " juta";
        if ((value % 1000000ULL) != 0) {
            result += " " + ConvertNumberToIndonesianWords(value % 1000000ULL);
        }
        return result;
    }
    if (value < 1000000000000ULL) {
        std::string result = ConvertNumberToIndonesianWords(value / 1000000000ULL) + " miliar";
        if ((value % 1000000000ULL) != 0) {
            result += " " + ConvertNumberToIndonesianWords(value % 1000000000ULL);
        }
        return result;
    }

    std::string result = ConvertNumberToIndonesianWords(value / 1000000000000ULL) + " triliun";
    if ((value % 1000000000000ULL) != 0) {
        result += " " + ConvertNumberToIndonesianWords(value % 1000000000000ULL);
    }
    return result;
}

std::string ConvertNumericTokenToSpeech(const std::string& token) {
    std::string digits;
    std::string suffix;

    for (char ch : token) {
        if (std::isdigit(static_cast<unsigned char>(ch))) {
            digits.push_back(ch);
        } else if (std::isalpha(static_cast<unsigned char>(ch))) {
            suffix.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(ch))));
        }
    }

    uint64_t value = 0;
    if (!ParseUnsignedNumber(digits, value)) {
        return token;
    }

    std::string spoken = ConvertNumberToIndonesianWords(value);
    if (!suffix.empty()) {
        spoken.push_back(' ');
        for (size_t i = 0; i < suffix.size(); ++i) {
            if (i != 0) {
                spoken.push_back(' ');
            }
            spoken.push_back(suffix[i]);
        }
    }
    return spoken;
}

std::string ExpandCurrencyMentionsForSpeech(const std::string& text) {
    std::string output;
    output.reserve(text.size() + 64);

    for (size_t i = 0; i < text.size();) {
        if (text.compare(i, 2, "Rp") == 0) {
            size_t j = i + 2;
            while (j < text.size() && text[j] == ' ') {
                ++j;
            }

            size_t token_start = j;
            bool has_digit = false;
            while (j < text.size()) {
                char ch = text[j];
                if (std::isdigit(static_cast<unsigned char>(ch))) {
                    has_digit = true;
                    ++j;
                } else if ((ch == '.' || ch == ',') && (j + 1) < text.size() &&
                           std::isdigit(static_cast<unsigned char>(text[j + 1]))) {
                    ++j;
                } else {
                    break;
                }
            }

            if (has_digit) {
                output += ConvertNumericTokenToSpeech(text.substr(token_start, j - token_start));
                output += " rupiah";
                i = j;
                continue;
            }
        }

        output.push_back(text[i]);
        ++i;
    }

    return output;
}

std::string ExpandPrefixedNumbersForSpeech(const std::string& text, const std::string& prefix, const std::string& spoken_prefix) {
    std::string output;
    output.reserve(text.size() + 64);

    for (size_t i = 0; i < text.size();) {
        if (text.compare(i, prefix.size(), prefix) == 0) {
            size_t j = i + prefix.size();
            while (j < text.size() && text[j] == ' ') {
                ++j;
            }

            size_t token_start = j;
            bool has_digit = false;
            while (j < text.size()) {
                char ch = text[j];
                if (std::isdigit(static_cast<unsigned char>(ch))) {
                    has_digit = true;
                    ++j;
                } else if (std::isalpha(static_cast<unsigned char>(ch)) && has_digit) {
                    ++j;
                } else {
                    break;
                }
            }

            if (has_digit) {
                output += spoken_prefix;
                output += ConvertNumericTokenToSpeech(text.substr(token_start, j - token_start));
                i = j;
                continue;
            }
        }

        output.push_back(text[i]);
        ++i;
    }

    return output;
}

std::string ExpandLecturerTitleForSpeech(const std::string& short_title) {
    std::string spoken = short_title;

    static const std::array<std::pair<const char*, const char*>, 28> kReplacements = {{
        {"Assoc. Prof.", "Associate Professor"},
        {"Assoc Prof.", "Associate Professor"},
        {"Prof.", "Professor"},
        {"Dr.", "Doktor"},
        {"S.E.", "Sarjana Ekonomi"},
        {"M.M.", "Magister Manajemen"},
        {"M.TI.", "Magister Teknologi Informasi"},
        {"M.Ak.", "Magister Akuntansi"},
        {"Ak.", "Akuntan"},
        {"CA", "Chartered Accountant"},
        {"S.Kom.", "Sarjana Komputer"},
        {"M.Kom.", "Magister Komputer"},
        {"MMSI.", "Magister Manajemen Sistem Informasi"},
        {"M.M.S.I.", "Magister Manajemen Sistem Informasi"},
        {"S.T.", "Sarjana Teknik"},
        {"M.E.", "Magister Ekonomi"},
        {"S.Ab.", "Sarjana Administrasi Bisnis"},
        {"S.M.", "Sarjana Manajemen"},
        {"S.Ip", "Sarjana Ilmu Politik"},
        {"S.Ip.", "Sarjana Ilmu Politik"},
        {"M.Si.", "Magister Sains"},
        {"M.Pd.", "Magister Pendidikan"},
        {"S.Pd.", "Sarjana Pendidikan"},
        {"M.Psi.", "Magister Psikologi"},
        {"Psi.", "Psikolog"},
        {"S.Si.", "Sarjana Sains"},
        {"M.T.", "Magister Teknik"},
        {"M.Sc.", "Master of Science"}
    }};

    ReplaceAll(spoken, "B.Com.", "Bachelor of Commerce");
    ReplaceAll(spoken, "M.Comp.", "Master of Computing");
    ReplaceAll(spoken, "M.Ds.", "Magister Desain");
    ReplaceAll(spoken, "M.Sn.", "Magister Seni");
    ReplaceAll(spoken, "S.SI", "Sarjana Sains");
    ReplaceAll(spoken, "S.I.", "Sarjana Informatika");

    for (const auto& replacement : kReplacements) {
        ReplaceAll(spoken, replacement.first, replacement.second);
    }

    return CollapseRepeatedSpaces(spoken);
}

bool ShouldPreserveEnglishCourseTerms(const CampusEntry* entry) {
    return entry != nullptr &&
        (strcmp(entry->category, "dosen") == 0 ||
         strcmp(entry->category, "kurikulum") == 0 ||
         strcmp(entry->category, "matakuliah_unggulan") == 0);
}

std::string MakeCampusSpeechFriendly(const std::string& raw_text, bool preserve_english_course_terms = false) {
    std::string spoken = raw_text;

    static const std::array<std::pair<const char*, const char*>, 21> kGeneralReplacements = {{
        {"S1 ", "Sarjana "},
        {"D3 ", "Diploma Tiga "},
        {"AI & Data Analysis Lab", "Laboratorium Kecerdasan Buatan dan Analisis Data"},
        {"Creativity", "Kreativitas"},
        {"Initiative", "Inisiatif"},
        {"Critical Thinking", "Berpikir Kritis"},
        {"Grit", "Ketangguhan"},
        {"Respectful", "Saling Menghargai"},
        {"Entrepreneurship", "Kewirausahaan"},
        {"Accountable", "Akuntabel"},
        {"Teamwork", "Kerja Sama Tim"},
        {"MBKM", "Merdeka Belajar Kampus Merdeka"},
        {"Campus Hub", "Pusat Kampus"},
        {"Convention Hall", "Aula Konvensi"},
        {"Computer Lab", "Laboratorium Komputer"},
        {"Career Ready System", "Sistem Siap Karier"},
        {"Jl.", "Jalan"},
        {"Jl ", "Jalan "},
        {"No.", "Nomor"},
        {"No ", "Nomor "},
        {"TA ", "Tahun Akademik "}
    }};

    static const std::array<std::pair<const char*, const char*>, 32> kCourseReplacements = {{
        {"AI for Accounting", "Kecerdasan Buatan untuk Akuntansi"},
        {"AI for Marketing", "Kecerdasan Buatan untuk Pemasaran"},
        {"AI for Web Design", "Kecerdasan Buatan untuk Desain Web"},
        {"AI Technician", "Teknisi Kecerdasan Buatan"},
        {"AI", "kecerdasan buatan"},
        {"UI/UX", "antarmuka dan pengalaman pengguna"},
        {"UI & UX", "antarmuka dan pengalaman pengguna"},
        {"UI UX", "antarmuka dan pengalaman pengguna"},
        {"Machine Learning", "pembelajaran mesin"},
        {"Deep Learning", "pembelajaran mendalam"},
        {"Data Mining", "penambangan data"},
        {"Data Warehouse", "gudang data"},
        {"Business Intelligence", "intelijen bisnis"},
        {"Financial Technology", "teknologi finansial"},
        {"FinTech", "teknologi finansial"},
        {"Digital Marketing", "pemasaran digital"},
        {"Design Thinking", "berpikir desain"},
        {"Computer Vision and NLP", "penglihatan komputer dan pemrosesan bahasa alami"},
        {"Computer Vision & NLP", "penglihatan komputer dan pemrosesan bahasa alami"},
        {"Web Design", "desain web"},
        {"Web Programming", "pemrograman web"},
        {"Mobile Programming", "pemrograman aplikasi seluler"},
        {"Integrated Marketing Communication", "komunikasi pemasaran terpadu"},
        {"Search Engine Optimization", "optimasi mesin pencari"},
        {"General English", "Bahasa Inggris Umum"},
        {"Intermediate English", "Bahasa Inggris Menengah"},
        {"English for Specific Purpose", "Bahasa Inggris untuk Tujuan Khusus"},
        {"English for Business", "Bahasa Inggris Bisnis"},
        {"Academic Writing", "Penulisan Akademik"},
        {"Creative Content Advertising", "Periklanan Konten Kreatif"},
        {"Student Exchange", "Pertukaran Mahasiswa"},
        {"TOEFL", "Tes Kemampuan Bahasa Inggris"}
    }};

    for (const auto& replacement : kGeneralReplacements) {
        ReplaceAll(spoken, replacement.first, replacement.second);
    }

    if (!preserve_english_course_terms) {
        for (const auto& replacement : kCourseReplacements) {
            ReplaceAll(spoken, replacement.first, replacement.second);
        }
        ReplaceAll(spoken, "Technopreneurship", "Kewirausahaan Teknologi");
        ReplaceAll(spoken, "Startup", "usaha rintisan");
        ReplaceAll(spoken, "E-Commerce", "Perdagangan Elektronik");
        ReplaceAll(spoken, "e-Commerce", "perdagangan elektronik");
        ReplaceAll(spoken, "Enterprise Resource Planning", "Perencanaan Sumber Daya Perusahaan");
        ReplaceAll(spoken, "Management Information System", "Sistem Informasi Manajemen");
        ReplaceAll(spoken, "Cyber Security", "Keamanan Siber");
        ReplaceAll(spoken, "cybersecurity", "keamanan siber");
        ReplaceAll(spoken, "SEO", "optimasi mesin pencari");
        ReplaceAll(spoken, "ERP", "perencanaan sumber daya perusahaan");
        ReplaceAll(spoken, "IMC", "komunikasi pemasaran terpadu");
        ReplaceAll(spoken, " & ", " dan ");
    }

    ReplaceAll(spoken, "S1", "Sarjana");
    ReplaceAll(spoken, "D3", "Diploma Tiga");
    ReplaceAll(spoken, "1/2 Semester", "setengah semester");
    ReplaceAll(spoken, "Opsi 1", "Opsi satu");
    ReplaceAll(spoken, "Opsi 2", "Opsi dua");
    ReplaceAll(spoken, "Opsi 3", "Opsi tiga");
    ReplaceAll(spoken, "3 Bulan", "tiga bulan");
    ReplaceAll(spoken, "3 bulan", "tiga bulan");
    ReplaceAll(spoken, "6 Bulan", "enam bulan");
    ReplaceAll(spoken, "6 bulan", "enam bulan");
    ReplaceAll(spoken, "1 Tahun", "satu tahun");
    ReplaceAll(spoken, "1 tahun", "satu tahun");
    ReplaceAll(spoken, "1 semester", "satu semester");
    ReplaceAll(spoken, "(3 Bulan)", "(tiga bulan)");
    ReplaceAll(spoken, "(6 Bulan)", "(enam bulan)");
    ReplaceAll(spoken, "(1 Semester)", "(satu semester)");
    ReplaceAll(spoken, "(1 Tahun)", "(satu tahun)");
    ReplaceAll(spoken, "3 fakultas", "tiga fakultas");
    ReplaceAll(spoken, "PMB 2026", "PMB tahun 2026");
    ReplaceAll(spoken, "Kampus 1", "Kampus satu");
    ReplaceAll(spoken, "Kampus 2", "Kampus dua");
    ReplaceAll(spoken, "Magister Teknologi Informasi Kampus", "Magister Teknologi Informasi. Kampus");
    ReplaceAll(spoken, "Teknologi Informasi Kampus", "Teknologi Informasi. Kampus");
    ReplaceAll(spoken, "Tahap-1", "Tahap satu");
    ReplaceAll(spoken, "Tahap-2", "Tahap dua");
    ReplaceAll(spoken, "Tahap-3", "Tahap tiga");
    ReplaceAll(spoken, "Tahap-4", "Tahap empat");
    spoken = ExpandLecturerTitleForSpeech(spoken);
    spoken = ExpandCurrencyMentionsForSpeech(spoken);
    spoken = ExpandPrefixedNumbersForSpeech(spoken, "Nomor ", "Nomor ");
    spoken = ExpandPrefixedNumbersForSpeech(spoken, "tahun ", "tahun ");
    spoken = ExpandPrefixedNumbersForSpeech(spoken, "Tahun ", "Tahun ");
    spoken = ExpandPrefixedNumbersForSpeech(spoken, "Gelombang ", "Gelombang ");
    spoken = ExpandPrefixedNumbersForSpeech(spoken, "Tahap ", "Tahap ");
    spoken = ExpandPrefixedNumbersForSpeech(spoken, "Semester ", "Semester ");
    spoken = ExpandPrefixedNumbersForSpeech(spoken, "semester ", "semester ");
    spoken = ExpandPrefixedNumbersForSpeech(spoken, "Kampus ", "Kampus ");
    return CollapseRepeatedSpaces(spoken);
}

bool QueryLooksLikeSpecificLecturer(const CampusEntry* entry, const SearchQuery& query) {
    if (!IsLecturerEntry(entry)) {
        return false;
    }

    std::string normalized_title = NormalizeText(entry->title);
    int matched_tokens = 0;
    for (const auto& token : query.tokens) {
        if (ContainsText(normalized_title, token) || ContainsApproxToken(normalized_title, token)) {
            ++matched_tokens;
        }
    }

    return matched_tokens >= 2 || (matched_tokens >= 1 && query.tokens.size() == 1);
}

std::string StripPossessiveSuffix(const std::string& token) {
    if (token.size() > 4 && token.compare(token.size() - 3, 3, "nya") == 0) {
        return token.substr(0, token.size() - 3);
    }
    return token;
}

bool TokenLooksLikeDosenHelperWord(const std::string& token) {
    std::string stem = StripPossessiveSuffix(token);
    return TokensApproximatelyMatch(stem, "dosen") ||
        TokensApproximatelyMatch(stem, "pengampu") ||
        TokensApproximatelyMatch(stem, "pengajar") ||
        TokensApproximatelyMatch(stem, "mengajar") ||
        TokensApproximatelyMatch(stem, "ngajar");
}

bool QueryAsksDosen(const SearchQuery& query) {
    if (ContainsText(query.normalized, "dosen") ||
        ContainsText(query.normalized, "pengampu") ||
        ContainsText(query.normalized, "mengajar") ||
        ContainsText(query.normalized, "pengajar") ||
        ContainsText(query.normalized, "diampu") ||
        ContainsText(query.normalized, "ngajar") ||
        ContainsText(query.normalized, "diajar") ||
        ContainsText(query.normalized, "ampu") ||
        ContainsApproxToken(query.normalized, "dosen") ||
        ContainsApproxToken(query.normalized, "pengampu") ||
        ContainsApproxToken(query.normalized, "mengajar") ||
        ContainsApproxToken(query.normalized, "pengajar") ||
        ContainsApproxToken(query.normalized, "ngajar")) {
        return true;
    }

    for (const auto& word : SplitWords(query.normalized)) {
        if (TokenLooksLikeDosenHelperWord(word)) {
            return true;
        }
    }
    return false;
}

bool IsLecturerEntry(const CampusEntry* entry) {
    return entry != nullptr && strcmp(entry->category, "dosen") == 0;
}

bool IsLecturerSummaryEntry(const CampusEntry* entry) {
    return entry != nullptr &&
        (strcmp(entry->id, "dosen_fti") == 0 ||
         strcmp(entry->id, "dosen_feb") == 0 ||
         strcmp(entry->id, "dosen_fps") == 0 ||
         strcmp(entry->id, "dosen_mku") == 0);
}

bool QueryAsksLecturerSummary(const SearchQuery& query) {
    std::string padded_query = " " + query.normalized + " ";
    return ContainsText(query.normalized, "fakultas") ||
        ContainsText(query.normalized, "fti") ||
        ContainsText(query.normalized, "feb") ||
        ContainsText(query.normalized, "fps") ||
        ContainsText(query.normalized, "mku") ||
        ContainsText(query.normalized, "mata kuliah umum") ||
        ContainsText(query.normalized, "dosen umum") ||
        ContainsText(padded_query, " fti ") ||
        ContainsText(padded_query, " feb ") ||
        ContainsText(padded_query, " fps ") ||
        ContainsText(padded_query, " mku ");
}

bool QueryLooksLikeLecturerNameRequest(const SearchQuery& query) {
    for (int i = 0; i < CAMPUS_DB_SIZE; ++i) {
        if (!IsLecturerEntry(&CAMPUS_DB[i]) || IsLecturerSummaryEntry(&CAMPUS_DB[i])) {
            continue;
        }
        if (QueryLooksLikeSpecificLecturer(&CAMPUS_DB[i], query)) {
            return true;
        }
    }
    return false;
}

std::string TrimSimple(std::string text) {
    while (!text.empty() && std::isspace(static_cast<unsigned char>(text.front()))) {
        text.erase(text.begin());
    }
    while (!text.empty() && std::isspace(static_cast<unsigned char>(text.back()))) {
        text.pop_back();
    }
    return text;
}

void PushUniqueNumber(std::vector<int>& values, int value) {
    if (std::find(values.begin(), values.end(), value) == values.end()) {
        values.push_back(value);
    }
}

std::string StripTrailingCoursePunctuation(std::string text) {
    while (!text.empty() &&
           (text.back() == '.' || text.back() == ',' || text.back() == ';' || text.back() == ' ')) {
        text.pop_back();
    }
    return text;
}

std::string NormalizeLecturerCourseText(std::string text) {
    ReplaceAll(text, "&", " dan ");
    ReplaceAll(text, "/", " ");
    ReplaceAll(text, "(", " ");
    ReplaceAll(text, ")", " ");
    ReplaceAll(text, "-", " ");
    return NormalizeText(text.c_str());
}

std::vector<int> ExtractNumberHints(const std::string& normalized_text) {
    std::vector<int> numbers;
    for (const auto& token : SplitWords(normalized_text)) {
        if (token == "satu" || token == "1") {
            PushUniqueNumber(numbers, 1);
        } else if (token == "dua" || token == "2") {
            PushUniqueNumber(numbers, 2);
        } else if (token == "tiga" || token == "3") {
            PushUniqueNumber(numbers, 3);
        } else if (token == "empat" || token == "4") {
            PushUniqueNumber(numbers, 4);
        } else if (token == "lima" || token == "5") {
            PushUniqueNumber(numbers, 5);
        } else if (token == "enam" || token == "6") {
            PushUniqueNumber(numbers, 6);
        } else if (token == "tujuh" || token == "7") {
            PushUniqueNumber(numbers, 7);
        } else if (token == "delapan" || token == "8") {
            PushUniqueNumber(numbers, 8);
        } else if (token == "sembilan" || token == "9") {
            PushUniqueNumber(numbers, 9);
        } else if (token == "sepuluh" || token == "10") {
            PushUniqueNumber(numbers, 10);
        }
    }
    return numbers;
}

std::vector<std::string> ExtractLecturerCourses(const CampusEntry* entry) {
    std::vector<std::string> courses;
    if (entry == nullptr || entry->content == nullptr) {
        return courses;
    }

    std::string raw = entry->content;
    size_t colon = raw.find(':');
    if (colon != std::string::npos && colon + 1 < raw.size()) {
        raw = raw.substr(colon + 1);
    }
    ReplaceAll(raw, "\n", ",");

    size_t start = 0;
    while (start < raw.size()) {
        size_t end = raw.find(',', start);
        std::string piece = raw.substr(start, end == std::string::npos ? std::string::npos : end - start);
        piece = TrimSimple(StripTrailingCoursePunctuation(piece));
        if (!piece.empty()) {
            courses.push_back(piece);
        }
        if (end == std::string::npos) {
            break;
        }
        start = end + 1;
    }

    if (courses.empty()) {
        raw = TrimSimple(StripTrailingCoursePunctuation(raw));
        if (!raw.empty()) {
            courses.push_back(raw);
        }
    }

    return courses;
}

int ScoreCourseNameMatch(const SearchQuery& query, const std::string& course_name) {
    std::string normalized_course = NormalizeLecturerCourseText(course_name);
    if (normalized_course.empty()) {
        return -9999;
    }

    int score = 0;
    int matched_tokens = 0;

    for (const auto& phrase : query.phrases) {
        if (phrase == query.normalized || phrase.empty()) {
            continue;
        }
        if (SplitWords(phrase).size() < 2) {
            continue;
        }
        if (ContainsText(normalized_course, phrase)) {
            score += 80;
        } else if (phrase.size() >= 8) {
            int percent = TokenMatchPercent(phrase, normalized_course.c_str());
            if (percent >= 72) {
                score += percent / 2;
            }
        }
    }

    for (const auto& token : query.tokens) {
        if (ContainsText(normalized_course, token)) {
            score += 22;
            ++matched_tokens;
        } else if (token.size() >= 4 && ContainsApproxToken(normalized_course, token)) {
            score += 12;
            ++matched_tokens;
        }
    }

    int required_matches = query.tokens.size() >= 2 ? 2 : 1;
    if (matched_tokens < required_matches && score < 80) {
        return -9999;
    }

    std::vector<int> query_numbers = ExtractNumberHints(query.normalized);
    std::vector<int> course_numbers = ExtractNumberHints(normalized_course);
    bool has_number_overlap = false;
    for (int query_number : query_numbers) {
        for (int course_number : course_numbers) {
            if (query_number == course_number) {
                has_number_overlap = true;
                break;
            }
        }
        if (has_number_overlap) {
            break;
        }
    }

    if (!query_numbers.empty()) {
        if (has_number_overlap) {
            score += 28;
        } else {
            score -= course_numbers.empty() ? 44 : 34;
        }
    } else if (!course_numbers.empty()) {
        score -= 60;
    }

    score += matched_tokens * matched_tokens * 10;
    return score;
}

int ScoreWholeLecturerContentMatch(const SearchQuery& query, const std::string& content_text) {
    if (content_text.empty()) {
        return -9999;
    }

    int score = 0;
    int matched_tokens = 0;

    for (const auto& phrase : query.phrases) {
        if (phrase == query.normalized || phrase.empty()) {
            continue;
        }
        if (SplitWords(phrase).size() < 2) {
            continue;
        }
        if (ContainsText(content_text, phrase)) {
            score += 48;
        } else if (phrase.size() >= 8) {
            int percent = TokenMatchPercent(phrase, content_text.c_str());
            if (percent >= 72) {
                score += percent / 3;
            }
        }
    }

    for (const auto& token : query.tokens) {
        if (ContainsText(content_text, token)) {
            score += 14;
            ++matched_tokens;
        } else if (token.size() >= 4 && ContainsApproxToken(content_text, token)) {
            score += 8;
            ++matched_tokens;
        }
    }

    if (matched_tokens == 0 && score < 40) {
        return -9999;
    }

    std::vector<int> query_numbers = ExtractNumberHints(query.normalized);
    std::vector<int> text_numbers = ExtractNumberHints(content_text);
    bool has_number_overlap = false;
    for (int query_number : query_numbers) {
        for (int text_number : text_numbers) {
            if (query_number == text_number) {
                has_number_overlap = true;
                break;
            }
        }
        if (has_number_overlap) {
            break;
        }
    }

    if (!query_numbers.empty()) {
        if (has_number_overlap) {
            score += 18;
        } else if (!text_numbers.empty()) {
            score -= 18;
        }
    }

    score += matched_tokens * matched_tokens * 6;
    return score;
}

int ScoreLecturerTopicMatch(const CampusEntry* entry, const SearchQuery& query, bool lecturer_name_query) {
    if (!IsLecturerEntry(entry)) {
        return -9999;
    }

    bool summary_entry = IsLecturerSummaryEntry(entry);
    bool asks_summary = QueryAsksLecturerSummary(query);
    std::string title = NormalizeText(entry->title);
    std::string keywords = NormalizeText(entry->keywords);
    std::string content = NormalizeText(entry->content);
    int score = 0;
    int matched_tokens = 0;
    int best_course_score = -9999;
    int full_content_score = -9999;
    int positive_course_matches = 0;

    if (summary_entry) {
        score += asks_summary ? 90 : -140;
    } else if (asks_summary) {
        score -= 35;
    }

    if (summary_entry || lecturer_name_query) {
        for (const auto& phrase : query.phrases) {
            if (phrase == query.normalized) {
                continue;
            }
            if (ContainsText(title, phrase)) score += summary_entry ? 18 : 44;
            if (ContainsText(keywords, phrase)) score += summary_entry ? 12 : 34;
            if (ContainsText(content, phrase)) score += summary_entry ? 8 : 28;
        }

        for (const auto& token : query.tokens) {
            bool matched = false;
            if (ContainsText(title, token)) {
                score += summary_entry ? 18 : 56;
                matched = true;
            } else if (token.size() >= 4 && ContainsApproxToken(title, token)) {
                score += summary_entry ? 10 : 34;
                matched = true;
            }

            if (ContainsText(keywords, token)) {
                score += summary_entry ? 14 : 38;
                matched = true;
            } else if (token.size() >= 4 && ContainsApproxToken(keywords, token)) {
                score += summary_entry ? 8 : 24;
                matched = true;
            }

            if (ContainsText(content, token)) {
                score += summary_entry ? 8 : 30;
                matched = true;
            } else if (token.size() >= 4 && ContainsApproxToken(content, token)) {
                score += summary_entry ? 4 : 18;
                matched = true;
            }

            if (matched) {
                ++matched_tokens;
            }
        }
    }

    if (!summary_entry) {
        for (const auto& course_name : ExtractLecturerCourses(entry)) {
            int course_score = ScoreCourseNameMatch(query, course_name);
            if (course_score > 0) {
                ++positive_course_matches;
                if (course_score > best_course_score) {
                    best_course_score = course_score;
                }
            }
        }

        full_content_score = ScoreWholeLecturerContentMatch(query, NormalizeLecturerCourseText(entry->content));

        if (best_course_score > 0) {
            score += best_course_score + std::min(20, positive_course_matches * 6);
        }
        if (full_content_score > 0) {
            score += full_content_score / 2;
        }
    }

    if (!summary_entry && !asks_summary && !lecturer_name_query &&
        best_course_score <= 0 && full_content_score <= 0) {
        return -9999;
    }

    if (matched_tokens == 0) {
        if (summary_entry && asks_summary) {
            return score;
        }
        if (!summary_entry && (best_course_score > 0 || full_content_score > 0)) {
            return score;
        }
        return -9999;
    }

    score += matched_tokens * matched_tokens * (summary_entry ? 4 : 10);
    return score;
}

bool QueryRequiresAlgoritmaPemrograman2(const SearchQuery& query) {
    if (!ContainsApproxToken(query.normalized, "algoritma") ||
        !ContainsApproxToken(query.normalized, "pemrograman")) {
        return false;
    }

    for (int number : ExtractNumberHints(query.normalized)) {
        if (number == 2) {
            return true;
        }
    }
    return false;
}

bool LecturerHasAlgoritmaPemrograman2Course(const CampusEntry* entry) {
    for (const auto& course_name : ExtractLecturerCourses(entry)) {
        std::string normalized_course = NormalizeLecturerCourseText(course_name);
        if (!ContainsApproxToken(normalized_course, "algoritma") ||
            !ContainsApproxToken(normalized_course, "pemrograman")) {
            continue;
        }
        for (int number : ExtractNumberHints(normalized_course)) {
            if (number == 2) {
                return true;
            }
        }
    }
    return false;
}

bool TryBuildLecturerTopicResult(const SearchQuery& query, char* buf, int buf_size) {
    if (!QueryAsksDosen(query)) {
        return false;
    }

    bool lecturer_name_query = QueryLooksLikeLecturerNameRequest(query);
    bool require_algoritma_pemrograman_2 = QueryRequiresAlgoritmaPemrograman2(query);
    Match lecturer_top[8] = {Match(), Match(), Match(), Match(), Match(), Match(), Match(), Match()};
    for (int i = 0; i < CAMPUS_DB_SIZE; ++i) {
        if (require_algoritma_pemrograman_2 && !LecturerHasAlgoritmaPemrograman2Course(&CAMPUS_DB[i])) {
            continue;
        }
        int score = ScoreLecturerTopicMatch(&CAMPUS_DB[i], query, lecturer_name_query);
        if (score > 0) {
            InsertMatch(lecturer_top, i, score);
        }
    }

    if (lecturer_top[0].idx < 0 || lecturer_top[0].score < 24) {
        return false;
    }

    bool summary_result = IsLecturerSummaryEntry(&CAMPUS_DB[lecturer_top[0].idx]);
    bool course_topic_query = !summary_result && !lecturer_name_query;
    int results_to_show = 1;
    if (!summary_result) {
        if (course_topic_query) {
            for (int i = 0; i < static_cast<int>(sizeof(lecturer_top) / sizeof(lecturer_top[0])); ++i) {
                if (lecturer_top[i].idx < 0) {
                    break;
                }
                if (lecturer_top[i].score >= std::max(22, (lecturer_top[0].score * 35) / 100)) {
                    results_to_show = i + 1;
                } else {
                    break;
                }
            }
        } else {
            results_to_show = 3;
            if (lecturer_top[1].idx < 0 || lecturer_top[1].score < std::max(32, (lecturer_top[0].score * 55) / 100)) {
                results_to_show = 1;
            } else if (lecturer_top[2].idx < 0 || lecturer_top[2].score < std::max(36, (lecturer_top[0].score * 60) / 100)) {
                results_to_show = 2;
            }
        }
    }

    int pos = snprintf(buf, buf_size,
        "Data dosen UCIC yang relevan. Jawab dalam bahasa Indonesia penuh. Gunakan nama dosen persis seperti tertulis di bawah ini, jangan menambah sapaan seperti Bapak atau Ibu, dan ikuti baris Penyebutan lisan saat menyebut gelar.\n\n");
    if (pos < 0) {
        return false;
    }

    for (int t = 0; t < results_to_show && lecturer_top[t].idx >= 0; ++t) {
        const CampusEntry* entry = &CAMPUS_DB[lecturer_top[t].idx];
        if (!IsLecturerEntry(entry)) {
            continue;
        }
        if (summary_result != IsLecturerSummaryEntry(entry)) {
            continue;
        }
        if (!AppendLecturerEntry(buf, buf_size, pos, entry)) {
            break;
        }
    }

    return pos > 0;
}

bool IsIgnoredToken(const std::string& token) {
    static const char* kIgnoredTokens[] = {
        "apa", "siapa", "berapa", "bagaimana", "dimana", "di", "ke", "dari", "dan", "yang",
        "untuk", "dengan", "atau", "pada", "mohon", "tolong", "minta", "ingin", "mau", "saya",
        "kami", "aku", "kamu", "tentang", "info", "informasi", "jelaskan", "jelasin", "dong",
        "nih", "ya", "kah", "nya", "apakah", "ada", "itu", "ini", "the", "is", "are",
        "of", "for", "please", "tell", "me", "give", "show", "ucic", "cic", "kampus",
        "universitas", "campus", "university", "mata", "kuliah", "mengajar", "pengajar",
        "pengampu", "dosen", "diampu", "ngajar", "diajar", "ampu", "detail", "tentang",
        "berapa", "opsi", "kelas", "gelombang", "tahun", "sapa", "siapakah", "dosennya"
    };
    static const char* kShortAllowedTokens[] = {
        "ti", "si", "mi", "mb", "ui", "ux", "ai", "vr",
        "ar", "wa", "ti", "dkv", "akt", "mjn", "seo", "erp"
    };

    if (token.size() <= 2) {
        for (const char* allowed : kShortAllowedTokens) {
            if (token == allowed) {
                return false;
            }
        }
        return true;
    }

    for (const char* ignored : kIgnoredTokens) {
        if (token == ignored) {
            return true;
        }
    }
    std::string stem = StripPossessiveSuffix(token);
    return TokensApproximatelyMatch(stem, "siapa") ||
        TokenLooksLikeDosenHelperWord(stem);
}

void AddAliases(const std::string& token, std::vector<std::string>& phrases) {
    if (token == "ucic" || token == "cic" || token == "uci") {
        PushUnique(phrases, "universitas catur insan cendekia");
        PushUnique(phrases, "kampus ucic");
        PushUnique(phrases, "data ringkas ucic");
    } else if (token == "catur" || token == "insan" || token == "cendekia" || token == "cendikia" ||
               token == "sendikia" || token == "sendekia" || token == "statur") {
        PushUnique(phrases, "universitas catur insan cendekia");
        PushUnique(phrases, "ucic");
    } else if (token == "kampus" || token == "universitas" || token == "cendekia") {
        PushUnique(phrases, "universitas catur insan cendekia");
        PushUnique(phrases, "kampus ucic");
    } else if (token == "jurusan" || token == "fakultas" || token == "prodi") {
        PushUnique(phrases, "fakultas dan program studi ucic");
        PushUnique(phrases, "3 fakultas");
    } else if (token == "great" || token == "nilai" || token == "karakter" || token == "budaya") {
        PushUnique(phrases, "cic great");
        PushUnique(phrases, "nilai dasar ucic");
        PushUnique(phrases, "nilai kampus");
    } else if (token == "fti") {
        PushUnique(phrases, "fakultas teknologi informasi");
    } else if (token == "feb") {
        PushUnique(phrases, "fakultas ekonomi dan bisnis");
    } else if (token == "fps") {
        PushUnique(phrases, "fakultas pendidikan dan sains");
    } else if (token == "ti") {
        PushUnique(phrases, "teknik informatika");
        PushUnique(phrases, "detail s1 teknik informatika");
    } else if (token == "si") {
        PushUnique(phrases, "sistem informasi");
        PushUnique(phrases, "detail s1 sistem informasi");
    } else if (token == "dkv") {
        PushUnique(phrases, "desain komunikasi visual");
        PushUnique(phrases, "detail s1 desain komunikasi visual");
    } else if (token == "bisdi" || token == "bisdig" || token == "bisdigital") {
        PushUnique(phrases, "bisnis digital");
        PushUnique(phrases, "detail s1 bisnis digital");
    } else if (token == "akt") {
        PushUnique(phrases, "akuntansi");
        PushUnique(phrases, "detail s1 akuntansi");
    } else if (token == "mjn") {
        PushUnique(phrases, "manajemen");
        PushUnique(phrases, "detail s1 manajemen");
    } else if (token == "pkor" || token == "pikor") {
        PushUnique(phrases, "pendidikan kepelatihan keolahragaan");
        PushUnique(phrases, "pendidikan kepelatihan olahraga");
        PushUnique(phrases, "pkor");
        PushUnique(phrases, "detail s1 pendidikan kepelatihan keolahragaan");
    } else if (token == "mi") {
        PushUnique(phrases, "manajemen informatika");
        PushUnique(phrases, "d3 manajemen informatika");
        PushUnique(phrases, "detail d3 manajemen informatika");
    } else if (token == "mb" || token == "d3mb" || token == "manbis") {
        PushUnique(phrases, "manajemen bisnis");
        PushUnique(phrases, "d3 manajemen bisnis");
        PushUnique(phrases, "detail d3 manajemen bisnis");
    } else if (token == "pmb") {
        PushUnique(phrases, "pendaftaran mahasiswa baru");
        PushUnique(phrases, "biaya pmb");
    } else if (token == "wa") {
        PushUnique(phrases, "whatsapp");
        PushUnique(phrases, "kontak");
    } else if (token == "ukt" || token == "spp" || token == "biaya" || token == "harga" || token == "tarif") {
        PushUnique(phrases, "biaya kuliah");
    } else if (token == "kip") {
        PushUnique(phrases, "kip kuliah");
    } else if (token == "karir" || token == "career" || token == "lulusan") {
        PushUnique(phrases, "prospek karir");
    } else if (token == "unggulan") {
        PushUnique(phrases, "mata kuliah unggulan");
        PushUnique(phrases, "matakuliah unggulan");
    } else if (token == "rpi") {
        PushUnique(phrases, "kelas rpi");
    } else if (token == "seo") {
        PushUnique(phrases, "search engine optimization");
    } else if (token == "erp") {
        PushUnique(phrases, "enterprise resource planning");
    } else if (token == "imc") {
        PushUnique(phrases, "integrated marketing communication");
    }
}

SearchQuery BuildSearchQuery(const char* raw_query) {
    SearchQuery query;
    query.normalized = NormalizeText(raw_query);
    if (query.normalized.empty()) {
        return query;
    }

    PushUnique(query.phrases, query.normalized);
    size_t start = 0;
    while (start < query.normalized.size()) {
        size_t end = query.normalized.find(' ', start);
        std::string token = query.normalized.substr(start, end == std::string::npos ? std::string::npos : end - start);
        AddAliases(token, query.phrases);
        if (!IsIgnoredToken(token)) {
            PushUnique(query.tokens, token);
        }
        if (end == std::string::npos) {
            break;
        }
        start = end + 1;
    }

    if (ContainsText(query.normalized, "uang kuliah")) {
        PushUnique(query.phrases, "biaya kuliah");
    }
    if (ContainsText(query.normalized, "nomor whatsapp") || ContainsText(query.normalized, "nomor wa")) {
        PushUnique(query.phrases, "kontak");
    }
    if (ContainsText(query.normalized, "prospek kerja")) {
        PushUnique(query.phrases, "prospek karir");
    }
    if (ContainsText(query.normalized, "mata kuliah unggulan") || ContainsText(query.normalized, "matkul unggulan")) {
        PushUnique(query.phrases, "mata kuliah unggulan");
        PushUnique(query.phrases, "matakuliah unggulan");
    }
    if (ContainsText(query.normalized, "biaya pmb 2026")) {
        PushUnique(query.phrases, "pmb 2026");
    }
    std::string padded_query = " " + query.normalized + " ";
    bool asks_course_list = ContainsText(query.normalized, "mata kuliah") ||
        ContainsText(query.normalized, "matakuliah") ||
        ContainsText(query.normalized, "matkul");
    if (asks_course_list) {
        if (ContainsText(query.normalized, "teknik informatika") || ContainsText(padded_query, " ti ")) {
            PushUnique(query.phrases, "kurikulum s1 teknik informatika");
            PushUnique(query.phrases, "teknik informatika");
        }
        if (ContainsText(query.normalized, "sistem informasi") || ContainsText(padded_query, " si ")) {
            PushUnique(query.phrases, "kurikulum s1 sistem informasi");
            PushUnique(query.phrases, "sistem informasi");
        }
        if (ContainsText(query.normalized, "desain komunikasi visual") || ContainsText(padded_query, " dkv ")) {
            PushUnique(query.phrases, "kurikulum s1 desain komunikasi visual");
            PushUnique(query.phrases, "desain komunikasi visual");
        }
        if (ContainsText(query.normalized, "akuntansi") || ContainsText(padded_query, " akt ")) {
            PushUnique(query.phrases, "kurikulum s1 akuntansi");
            PushUnique(query.phrases, "akuntansi");
        }
        if (ContainsText(query.normalized, "manajemen") || ContainsText(padded_query, " mjn ")) {
            PushUnique(query.phrases, "kurikulum s1 manajemen");
            PushUnique(query.phrases, "manajemen");
        }
        if (ContainsText(query.normalized, "bisnis digital") || ContainsText(padded_query, " bisdi ")) {
            PushUnique(query.phrases, "kurikulum s1 bisnis digital");
            PushUnique(query.phrases, "bisnis digital");
        }
        if (ContainsText(query.normalized, "pendidikan kepelatihan keolahragaan") ||
            ContainsText(query.normalized, "pendidikan kepelatihan olahraga") ||
            ContainsText(padded_query, " pkor ") || ContainsText(padded_query, " pikor ")) {
            PushUnique(query.phrases, "kurikulum s1 pendidikan kepelatihan keolahragaan");
            PushUnique(query.phrases, "pendidikan kepelatihan keolahragaan");
        }
        if (ContainsText(query.normalized, "manajemen informatika") || ContainsText(padded_query, " mi ")) {
            PushUnique(query.phrases, "kurikulum d3 manajemen informatika");
            PushUnique(query.phrases, "manajemen informatika");
        }
        if (ContainsText(query.normalized, "manajemen bisnis") || ContainsText(padded_query, " mb ")) {
            PushUnique(query.phrases, "kurikulum d3 manajemen bisnis");
            PushUnique(query.phrases, "manajemen bisnis");
        }
    }
    if (ContainsText(query.normalized, "kelas sore")) {
        PushUnique(query.phrases, "sore");
    }
    if (ContainsText(query.normalized, "universitas catur insan cendekia")) {
        PushUnique(query.phrases, "ucic");
        PushUnique(query.phrases, "kampus ucic");
        PushUnique(query.phrases, "data ringkas ucic");
    }
    if (LooksLikeUcicReference(query.normalized)) {
        PushUnique(query.phrases, "ucic");
        PushUnique(query.phrases, "universitas catur insan cendekia");
        PushUnique(query.phrases, "kampus ucic");
        PushUnique(query.phrases, "data ringkas ucic");
        PushUnique(query.phrases, "fakultas dan program studi ucic");
    }
    if (LooksLikeImplicitUcicAcademicQuery(query.normalized)) {
        PushUnique(query.phrases, "ucic");
        PushUnique(query.phrases, "universitas catur insan cendekia");
        PushUnique(query.phrases, "kampus ucic");
    }
    if (ContainsText(query.normalized, "cic great") || ContainsText(query.normalized, "nilai dasar") ||
        ContainsText(query.normalized, "nilai kampus") || ContainsText(query.normalized, "karakter kampus") ||
        ContainsText(query.normalized, "budaya kampus") || ContainsText(query.normalized, "core value")) {
        PushUnique(query.phrases, "cic great");
        PushUnique(query.phrases, "nilai dasar ucic");
    }
    if (ContainsApproxToken(query.normalized, "rektor")) {
        PushUnique(query.phrases, "rektor");
    }
    if (ContainsApproxToken(query.normalized, "pemrograman")) {
        PushUnique(query.phrases, "pemrograman");
    }
    if (ContainsApproxToken(query.normalized, "algoritma") && ContainsApproxToken(query.normalized, "pemrograman")) {
        if (ContainsText(query.normalized, " 2") || ContainsText(query.normalized, "dua")) {
            PushUnique(query.phrases, "algoritma dan pemrograman 2");
        } else {
            PushUnique(query.phrases, "algoritma dan pemrograman");
        }
    }
    if (ContainsText(query.normalized, "jurusan yang cocok") ||
        ContainsText(query.normalized, "rekomendasi jurusan") ||
        ContainsText(query.normalized, "prodi yang cocok") ||
        ContainsText(query.normalized, "cocok buat saya") ||
        ContainsText(query.normalized, "cocok untuk saya")) {
        PushUnique(query.phrases, "faq ucic");
        PushUnique(query.phrases, "rekomendasi jurusan ucic");
    }

    return query;
}

std::vector<std::string> CollectIndexQueryTerms(const SearchQuery& query) {
    std::vector<std::string> terms = query.tokens;

    for (const auto& phrase : query.phrases) {
        for (const auto& token : SplitWords(phrase)) {
            if (!IsIgnoredToken(token)) {
                PushUnique(terms, token);
            }
        }
    }

    return terms;
}

void AddIndexPostingScores(const CampusIndexTokenEntry& token_entry, int match_percent, std::vector<int>& scores) {
    if (match_percent <= 0) {
        return;
    }
    for (uint16_t i = 0; i < token_entry.posting_count; ++i) {
        const CampusIndexPosting& posting = kCampusIndexPostings[token_entry.posting_offset + i];
        if (posting.entry_index < scores.size()) {
            scores[posting.entry_index] += static_cast<int>((posting.weight * match_percent + 19) / 20);
        }
    }
}

std::vector<int> BuildCampusIndexScores(const SearchQuery& query) {
    std::vector<int> scores(CAMPUS_DB_SIZE, 0);
    std::vector<std::string> terms = CollectIndexQueryTerms(query);
    if (terms.empty()) {
        return scores;
    }

    for (const auto& term : terms) {
        int exact_index = -1;
        for (size_t i = 0; i < kCampusIndexTokenCount; ++i) {
            if (term == kCampusIndexTokens[i].token) {
                exact_index = static_cast<int>(i);
                break;
            }
        }

        if (exact_index >= 0) {
            AddIndexPostingScores(kCampusIndexTokens[exact_index], 100, scores);
            continue;
        }

        Match fuzzy_top[4] = {Match(), Match(), Match(), Match()};
        for (size_t i = 0; i < kCampusIndexTokenCount; ++i) {
            int match_percent = TokenMatchPercent(term, kCampusIndexTokens[i].token);
            if (match_percent >= 60) {
                InsertMatch(fuzzy_top, static_cast<int>(i), match_percent);
            }
        }

        for (const auto& fuzzy : fuzzy_top) {
            if (fuzzy.idx < 0 || fuzzy.score < 60) {
                continue;
            }
            AddIndexPostingScores(kCampusIndexTokens[fuzzy.idx], fuzzy.score, scores);
        }
    }

    return scores;
}

const CampusEntry* FindEntryById(const char* id) {
    for (int i = 0; i < CAMPUS_DB_SIZE; ++i) {
        if (strcmp(CAMPUS_DB[i].id, id) == 0) {
            return &CAMPUS_DB[i];
        }
    }
    return nullptr;
}

const CampusEntry* FindEntryByExactLookup(const SearchQuery& query) {
    if (query.normalized.empty()) {
        return nullptr;
    }

    for (int i = 0; i < CAMPUS_DB_SIZE; ++i) {
        const CampusEntry& entry = CAMPUS_DB[i];
        std::string id = NormalizeText(entry.id);
        std::string title = NormalizeText(entry.title);
        std::string spoken_title = NormalizeText(MakeCampusSpeechFriendly(entry.title != nullptr ? entry.title : "", false).c_str());

        if (query.normalized == id ||
            query.normalized == title ||
            query.normalized == spoken_title ||
            query.normalized == title + " ucic" ||
            query.normalized == spoken_title + " ucic" ||
            query.normalized == "ucic " + title ||
            query.normalized == "ucic " + spoken_title) {
            return &entry;
        }
    }

    return nullptr;
}

const char* DetectSpecificProdiId(const std::string& normalized, const std::string& padded_query) {
    if (ContainsText(normalized, "teknik informatika") || ContainsText(padded_query, " ti ")) {
        return "prodi_ti";
    }
    if (ContainsText(normalized, "sistem informasi") || ContainsText(padded_query, " si ")) {
        return "prodi_si";
    }
    if (ContainsText(normalized, "desain komunikasi visual") || ContainsText(padded_query, " dkv ")) {
        return "prodi_dkv";
    }
    if (ContainsText(normalized, "akuntansi") || ContainsText(padded_query, " akt ")) {
        return "prodi_akuntansi";
    }
    if ((ContainsText(normalized, "manajemen") || ContainsText(padded_query, " mjn ")) &&
        !ContainsText(normalized, "manajemen informatika") &&
        !ContainsText(normalized, "manajemen bisnis")) {
        return "prodi_manajemen";
    }
    if (ContainsText(normalized, "bisnis digital") || ContainsText(padded_query, " bisdi ")) {
        return "prodi_bisdi";
    }
    if (ContainsText(normalized, "pendidikan kepelatihan keolahragaan") ||
        ContainsText(normalized, "pendidikan kepelatihan olahraga") ||
        ContainsText(padded_query, " pkor ") || ContainsText(padded_query, " pikor ")) {
        return "prodi_pkor";
    }
    if (ContainsText(normalized, "manajemen informatika") || ContainsText(padded_query, " mi ")) {
        return "prodi_d3mi";
    }
    if (ContainsText(normalized, "manajemen bisnis") || ContainsText(padded_query, " mb ")) {
        return "prodi_d3mb";
    }
    return nullptr;
}

void PushUniqueId(std::vector<const char*>& ids, const char* id) {
    if (id == nullptr) {
        return;
    }
    for (const char* existing : ids) {
        if (strcmp(existing, id) == 0) {
            return;
        }
    }
    ids.push_back(id);
}

void CollectMentionedProdiIds(const std::string& normalized, const std::string& padded_query, std::vector<const char*>& ids) {
    if (ContainsText(normalized, "teknik informatika") || ContainsText(padded_query, " ti ")) {
        PushUniqueId(ids, "prodi_ti");
    }
    if (ContainsText(normalized, "sistem informasi") || ContainsText(padded_query, " si ")) {
        PushUniqueId(ids, "prodi_si");
    }
    if (ContainsText(normalized, "desain komunikasi visual") || ContainsText(padded_query, " dkv ")) {
        PushUniqueId(ids, "prodi_dkv");
    }
    if (ContainsText(normalized, "akuntansi") || ContainsText(padded_query, " akt ")) {
        PushUniqueId(ids, "prodi_akuntansi");
    }
    if (ContainsText(normalized, "bisnis digital") || ContainsText(padded_query, " bisdi ")) {
        PushUniqueId(ids, "prodi_bisdi");
    }
    if (ContainsText(normalized, "pendidikan kepelatihan keolahragaan") ||
        ContainsText(normalized, "pendidikan kepelatihan olahraga") ||
        ContainsText(padded_query, " pikor ") || ContainsText(padded_query, " pkor ")) {
        PushUniqueId(ids, "prodi_pkor");
    }
    if (ContainsText(normalized, "manajemen informatika") || ContainsText(padded_query, " mi ")) {
        PushUniqueId(ids, "prodi_d3mi");
    }
    if (ContainsText(normalized, "manajemen bisnis") || ContainsText(padded_query, " mb ")) {
        PushUniqueId(ids, "prodi_d3mb");
    }
    if ((ContainsText(normalized, "manajemen") || ContainsText(padded_query, " mjn ")) &&
        !ContainsText(normalized, "manajemen informatika") &&
        !ContainsText(normalized, "manajemen bisnis")) {
        PushUniqueId(ids, "prodi_manajemen");
    }
}

bool LooksLikeProgramComparisonQuery(const std::string& normalized) {
    return ContainsText(normalized, "beda") ||
        ContainsText(normalized, "bedanya") ||
        ContainsText(normalized, "perbedaan") ||
        ContainsText(normalized, "bandingkan") ||
        ContainsText(normalized, "vs");
}

std::string GetProdiDisplayName(const CampusEntry* entry) {
    if (entry == nullptr || entry->id == nullptr) {
        return "";
    }
    if (strcmp(entry->id, "prodi_ti") == 0) return "Teknik Informatika";
    if (strcmp(entry->id, "prodi_si") == 0) return "Sistem Informasi";
    if (strcmp(entry->id, "prodi_dkv") == 0) return "Desain Komunikasi Visual";
    if (strcmp(entry->id, "prodi_akuntansi") == 0) return "Akuntansi";
    if (strcmp(entry->id, "prodi_manajemen") == 0) return "Manajemen";
    if (strcmp(entry->id, "prodi_bisdi") == 0) return "Bisnis Digital";
    if (strcmp(entry->id, "prodi_pkor") == 0) return "Pendidikan Kepelatihan Keolahragaan";
    if (strcmp(entry->id, "prodi_d3mi") == 0) return "Manajemen Informatika";
    if (strcmp(entry->id, "prodi_d3mb") == 0) return "Manajemen Bisnis";
    return entry->title != nullptr ? entry->title : "";
}

std::string StripTrailingPunctuation(std::string text) {
    while (!text.empty() && (text.back() == '.' || text.back() == ',' || text.back() == ' ')) {
        text.pop_back();
    }
    return text;
}

std::string BuildProdiComparisonText(const CampusEntry* left, const CampusEntry* right) {
    if (left == nullptr || right == nullptr) {
        return "";
    }

    std::string left_name = GetProdiDisplayName(left);
    std::string right_name = GetProdiDisplayName(right);
    std::string left_content = StripTrailingPunctuation(left->content != nullptr ? left->content : "");
    std::string right_content = StripTrailingPunctuation(right->content != nullptr ? right->content : "");

    return left_name + ": " + left_content + ".\n" +
        right_name + ": " + right_content + ".\n" +
        "Secara ringkas, pilih " + left_name + " jika fokus yang Anda minati lebih dekat dengan penjelasan pertama, dan pilih " +
        right_name + " jika fokus yang Anda minati lebih dekat dengan penjelasan kedua.";
}

int IntentBonus(const CampusEntry* entry, const SearchQuery& query) {
    int score = 0;
    std::string padded_query = " " + query.normalized + " ";
    bool asks_dosen = QueryAsksDosen(query);
    bool asks_biaya = ContainsText(query.normalized, "biaya") || ContainsText(query.normalized, "tuition") ||
        ContainsText(query.normalized, "fee") || ContainsText(query.normalized, "cost") ||
        ContainsText(query.normalized, "dpp") ||
        ContainsText(query.normalized, "gedung") || ContainsText(query.normalized, "perlengkapan") ||
        ContainsText(query.normalized, "tahap") || ContainsText(query.normalized, "opsi");
    bool asks_jurusan = ContainsText(query.normalized, "jurusan") || ContainsText(query.normalized, "prodi") ||
        ContainsText(query.normalized, "fakultas") || ContainsText(query.normalized, "program studi");
    bool asks_matakuliah = (ContainsText(query.normalized, "mata kuliah") ||
        ContainsText(query.normalized, "matakuliah") ||
        ContainsText(query.normalized, "matkul")) && !asks_dosen;
    bool asks_kurikulum = ContainsText(query.normalized, "kurikulum") || ContainsText(query.normalized, "semester") || asks_matakuliah;
    bool asks_unggulan = ContainsText(query.normalized, "mata kuliah unggulan") || ContainsText(query.normalized, "matakuliah unggulan") ||
        ContainsText(query.normalized, "matkul unggulan");
    bool asks_karir = ContainsText(query.normalized, "karir") || ContainsText(query.normalized, "prospek kerja") ||
        ContainsText(query.normalized, "career") || ContainsText(query.normalized, "lulusan");
    bool asks_kontak = ContainsText(query.normalized, "kontak") || ContainsText(query.normalized, "whatsapp") ||
        ContainsText(query.normalized, "telepon") || ContainsText(query.normalized, "email");
    bool asks_lokasi = ContainsText(query.normalized, "alamat") || ContainsText(query.normalized, "lokasi") ||
        ContainsText(query.normalized, "dimana");
    bool asks_visi_misi = ContainsText(query.normalized, "visi") || ContainsText(query.normalized, "misi");
    bool asks_rektor = ContainsText(query.normalized, "rektor") || ContainsApproxToken(query.normalized, "rektor");
    bool asks_beasiswa = ContainsText(query.normalized, "beasiswa");
    bool asks_pmb = QueryAsksPendaftaranPmb(query.normalized);
    bool asks_great_values = ContainsText(query.normalized, "cic great") || ContainsText(query.normalized, "great") ||
        ContainsText(query.normalized, "nilai dasar") || ContainsText(query.normalized, "nilai kampus") ||
        ContainsText(query.normalized, "karakter kampus") || ContainsText(query.normalized, "budaya kampus") ||
        ContainsText(query.normalized, "core value");
    bool asks_fasilitas = ContainsText(query.normalized, "fasilitas") || ContainsText(query.normalized, "lab") ||
        ContainsText(query.normalized, "wifi");
    bool asks_akreditasi = ContainsText(query.normalized, "akreditasi") || ContainsText(query.normalized, "ban pt");
    bool asks_profil = ContainsText(query.normalized, "profil") || ContainsText(query.normalized, "sejarah") ||
        ContainsText(query.normalized, "tentang");
    bool asks_program_identity = ContainsText(query.normalized, "itu apa") ||
        ContainsText(query.normalized, "apa itu") ||
        ContainsText(query.normalized, "kepanjangan") ||
        ContainsText(query.normalized, "singkatan") ||
        (ContainsText(padded_query, " itu ") && ContainsText(query.normalized, "apa"));
    bool asks_rekomendasi_jurusan =
        ContainsText(query.normalized, "jurusan yang cocok") ||
        ContainsText(query.normalized, "rekomendasi jurusan") ||
        ContainsText(query.normalized, "prodi yang cocok") ||
        ContainsText(query.normalized, "cocok buat saya") ||
        ContainsText(query.normalized, "cocok untuk saya");
    bool asks_campus_identity = LooksLikeUcicReference(query.normalized) ||
        LooksLikeImplicitUcicAcademicQuery(query.normalized) ||
        ContainsText(query.normalized, "universitas catur insan cendekia");
    const char* specific_prodi_id = DetectSpecificProdiId(query.normalized, padded_query);

    if (asks_dosen) {
        score += strcmp(entry->category, "dosen") == 0 ? 55 : -12;
        if ((ContainsText(query.normalized, "fti") ||
             ContainsText(query.normalized, "teknik informatika") ||
             ContainsText(query.normalized, "sistem informasi") ||
             ContainsText(query.normalized, "dkv") ||
             ContainsText(query.normalized, "manajemen informatika")) &&
            strcmp(entry->id, "dosen_fti") == 0) {
            score += 72;
        }
        if ((ContainsText(query.normalized, "feb") ||
             ContainsText(query.normalized, "fakultas ekonomi") ||
             ContainsText(query.normalized, "akuntansi") ||
             ContainsText(query.normalized, "manajemen") ||
             ContainsText(query.normalized, "manajemen bisnis") ||
             ContainsText(query.normalized, "bisnis digital")) &&
            strcmp(entry->id, "dosen_feb") == 0) {
            score += 72;
        }
        if ((ContainsText(query.normalized, "fps") ||
             ContainsText(query.normalized, "pkor") ||
             ContainsText(query.normalized, "pikor") ||
             ContainsText(query.normalized, "olahraga")) &&
            strcmp(entry->id, "dosen_fps") == 0) {
            score += 72;
        }
        if ((ContainsText(query.normalized, "mku") ||
             ContainsText(query.normalized, "umum") ||
             ContainsText(query.normalized, "bahasa") ||
             ContainsText(query.normalized, "english")) &&
            strcmp(entry->id, "dosen_mku") == 0) {
            score += 72;
        }
    }

    if (asks_biaya) {
        score += strcmp(entry->category, "biaya") == 0 ? 55 : -10;
        if (ContainsText(query.normalized, "opsi 1") || ContainsText(query.normalized, "3 bulan")) {
            score += strcmp(entry->id, "biaya_pmb_opsi1") == 0 ? 85 : 0;
        }
        if (ContainsText(query.normalized, "opsi 2") || ContainsText(query.normalized, "6 bulan") ||
            ContainsText(query.normalized, "1 semester")) {
            score += strcmp(entry->id, "biaya_pmb_opsi2") == 0 ? 85 : 0;
        }
        if (ContainsText(query.normalized, "opsi 3") || ContainsText(query.normalized, "1 tahun") ||
            ContainsText(query.normalized, "2 semester")) {
            score += strcmp(entry->id, "biaya_pmb_opsi3") == 0 ? 85 : 0;
        }
    }

    if (asks_jurusan) {
        if (strcmp(entry->category, "jurusan") == 0) score += 38;
        else if (strcmp(entry->category, "prospek_karir") == 0 || strcmp(entry->category, "matakuliah_unggulan") == 0) score += 12;
        else score -= 8;
        if (asks_campus_identity && strcmp(entry->id, "jurusan_ucic") == 0) {
            score += 100;
        }
        if (specific_prodi_id != nullptr) {
            if (strcmp(entry->id, specific_prodi_id) == 0) {
                score += 130;
            } else if (strcmp(entry->id, "jurusan_ucic") == 0) {
                score -= 55;
            }
            if (asks_program_identity && strcmp(entry->id, "singkatan_prodi") == 0) {
                score += 70;
            }
            if (asks_program_identity && strcmp(entry->id, specific_prodi_id) == 0) {
                score += 80;
            }
        }
    }
    if (asks_rekomendasi_jurusan) {
        if (strcmp(entry->id, "faq_ucic") == 0) {
            score += 320;
        } else if (strcmp(entry->id, "jurusan_ucic") == 0) {
            score -= 140;
        } else if (strcmp(entry->category, "jurusan") == 0) {
            score -= 25;
        }
    }
    if (specific_prodi_id != nullptr && asks_program_identity) {
        if (strcmp(entry->id, specific_prodi_id) == 0) {
            score += 140;
        } else if (strcmp(entry->id, "singkatan_prodi") == 0) {
            score += 55;
        } else if (strcmp(entry->category, "kurikulum") == 0 ||
                   strcmp(entry->category, "prospek_karir") == 0 ||
                   strcmp(entry->category, "matakuliah_unggulan") == 0) {
            score -= 18;
        }
    }
    if (asks_kurikulum) {
        if (asks_dosen) {
            if (strcmp(entry->category, "kurikulum") == 0) {
                score -= 22;
            }
        } else {
            score += strcmp(entry->category, "kurikulum") == 0 ? 52 : -10;
        }
    }
    if (asks_matakuliah) {
        if (strcmp(entry->category, "kurikulum") == 0) {
            score += 30;
        } else if (strcmp(entry->id, "jurusan_ucic") == 0) {
            score -= 45;
        } else if (strcmp(entry->category, "jurusan") == 0) {
            score -= 18;
        }
    }
    if (asks_karir) score += strcmp(entry->category, "prospek_karir") == 0 ? 58 : -10;
    if (asks_unggulan) score += strcmp(entry->category, "matakuliah_unggulan") == 0 ? 58 : -10;
    if (asks_kontak) score += strcmp(entry->category, "kontak") == 0 ? 50 : -10;
    if (asks_lokasi) {
        if (strcmp(entry->category, "lokasi") == 0) score += 60;
        else if (strcmp(entry->category, "kontak") == 0) score += 10;
        else score -= 12;
    }
    if (asks_visi_misi) score += strcmp(entry->id, "visi_misi") == 0 ? 55 : -10;
    if (asks_rektor) {
        if (strcmp(entry->id, "rektor") == 0) {
            score += 120;
        } else if (strcmp(entry->id, "profil_ucic") == 0 || strcmp(entry->id, "data_lengkap") == 0) {
            score -= 55;
        } else {
            score -= 18;
        }
    }
    if (asks_beasiswa) score += strcmp(entry->id, "beasiswa") == 0 ? 55 : -10;
    if (asks_pmb) score += strcmp(entry->id, "pendaftaran_pmb") == 0 ? 55 : -10;
    if (asks_great_values) score += strcmp(entry->id, "great_values") == 0 ? 95 : -10;
    if (asks_fasilitas) score += strcmp(entry->id, "fasilitas") == 0 ? 42 : -8;
    if (asks_akreditasi) {
        bool asks_specific_prodi_akreditasi =
            ContainsText(query.normalized, "teknik informatika") || ContainsText(padded_query, " ti ") ||
            ContainsText(query.normalized, "sistem informasi") || ContainsText(padded_query, " si ") ||
            ContainsText(query.normalized, "desain komunikasi visual") || ContainsText(padded_query, " dkv ") ||
            ContainsText(query.normalized, "akuntansi") || ContainsText(padded_query, " akt ") ||
            ContainsText(query.normalized, "manajemen") || ContainsText(padded_query, " mjn ") ||
            ContainsText(query.normalized, "bisnis digital") || ContainsText(padded_query, " bisdi ") ||
            ContainsText(query.normalized, "pendidikan kepelatihan keolahragaan") ||
            ContainsText(query.normalized, "pendidikan kepelatihan olahraga") ||
            ContainsText(padded_query, " pikor ") || ContainsText(padded_query, " pkor ") ||
            ContainsText(query.normalized, "manajemen informatika") || ContainsText(padded_query, " mi ") ||
            ContainsText(query.normalized, "manajemen bisnis") || ContainsText(padded_query, " mb ");
        if (asks_specific_prodi_akreditasi) {
            if (strcmp(entry->id, "akreditasi_prodi") == 0) {
                score += 95;
            } else if (strcmp(entry->id, "akreditasi") == 0) {
                score += 18;
            } else {
                score -= 10;
            }
        } else {
            score += strcmp(entry->id, "akreditasi") == 0 ? 45 : -8;
        }
    }
    if (asks_profil) {
        if (strcmp(entry->id, "profil_ucic") == 0) {
            score += 150;
        } else if (strcmp(entry->id, "data_lengkap") == 0 || strcmp(entry->id, "logo_ucic") == 0) {
            score += 20;
        } else if (strcmp(entry->id, "jurusan_ucic") == 0) {
            score -= 100;
        }
    }
    return score;
}

int ScoreEntry(const CampusEntry* entry, int entry_index, const SearchQuery& query, const std::vector<int>& index_scores) {
    std::string title = NormalizeText(entry->title);
    std::string keywords = NormalizeText(entry->keywords);
    std::string category = NormalizeText(entry->category);
    std::string content = NormalizeText(entry->content);
    std::string id = NormalizeText(entry->id);
    std::string raw_content = entry->content != nullptr ? entry->content : "";
    std::string semester_search_space = title + " " + keywords + " " + id;

    int score = IntentBonus(entry, query);
    if (entry_index >= 0 && entry_index < static_cast<int>(index_scores.size())) {
        score += index_scores[entry_index];
    }
    int matched_tokens = 0;
    int query_semester = ExtractSemesterNumber(query.normalized);
    int entry_semester = ExtractSemesterNumber(semester_search_space);

    bool asks_algoritma = ContainsApproxToken(query.normalized, "algoritma") &&
        ContainsApproxToken(query.normalized, "pemrograman");
    bool asks_algoritma_2 = asks_algoritma &&
        (ContainsText(query.normalized, " 2") || ContainsText(query.normalized, "dua"));
    bool has_basic_algoritma = raw_content.find("Algoritma & Pemrograman") != std::string::npos ||
        raw_content.find("Algoritma dan Pemrograman,") != std::string::npos ||
        raw_content.find("Algoritma dan Pemrograman.") != std::string::npos;
    bool has_algoritma_2 = raw_content.find("Algoritma dan Pemrograman 2") != std::string::npos;

    if (asks_algoritma_2) {
        if (has_algoritma_2) {
            score += 84;
        }
        if (has_basic_algoritma && !has_algoritma_2) {
            score -= 96;
        }
    } else if (asks_algoritma) {
        if (has_basic_algoritma) {
            score += 48;
        }
        if (has_algoritma_2 && !has_basic_algoritma) {
            score -= 60;
        }
    }

    if (query_semester > 0) {
        if (entry_semester == query_semester) {
            score += 140;
        } else if (entry_semester > 0) {
            score -= 140;
        }
    }

    if (ContainsText(title, query.normalized)) score += 95;
    if (ContainsText(keywords, query.normalized)) score += 85;
    if (ContainsText(content, query.normalized)) score += 28;
    if (ContainsText(category, query.normalized)) score += 18;
    if (ContainsText(id, query.normalized)) score += 24;

    for (const auto& phrase : query.phrases) {
        if (phrase == query.normalized) {
            continue;
        }
        if (ContainsText(title, phrase)) score += 36;
        if (ContainsText(keywords, phrase)) score += 34;
        if (ContainsText(content, phrase)) score += 12;
    }

    for (const auto& token : query.tokens) {
        bool matched = false;
        if (ContainsText(title, token)) {
            score += 20;
            matched = true;
        }
        if (ContainsText(keywords, token)) {
            score += 18;
            matched = true;
        }
        if (ContainsText(category, token)) {
            score += 10;
            matched = true;
        }
        if (ContainsText(id, token)) {
            score += 12;
            matched = true;
        }
        if (ContainsText(content, token)) {
            score += 6;
            matched = true;
        }
        if (!matched && token.size() >= 4) {
            if (ContainsApproxToken(title, token)) {
                score += 14;
                matched = true;
            }
            if (ContainsApproxToken(keywords, token)) {
                score += 12;
                matched = true;
            }
            if (ContainsApproxToken(category, token)) {
                score += 7;
                matched = true;
            }
            if (ContainsApproxToken(id, token)) {
                score += 8;
                matched = true;
            }
            if (ContainsApproxToken(content, token)) {
                score += 4;
                matched = true;
            }
        }
        if (matched) {
            ++matched_tokens;
        }
    }

    if (matched_tokens > 1) {
        score += matched_tokens * matched_tokens * 5;
    } else if (matched_tokens == 1) {
        score += 4;
    }

    return score;
}

template <size_t N>
void InsertMatch(Match (&top)[N], int idx, int score) {
    for (size_t i = 0; i < N; ++i) {
        if (score > top[i].score) {
            for (size_t j = N - 1; j > i; --j) {
                top[j] = top[j - 1];
            }
            top[i].idx = idx;
            top[i].score = score;
            break;
        }
    }
}

bool AppendEntry(char* buf, int buf_size, int& pos, const CampusEntry* entry) {
    if (entry == nullptr || pos >= buf_size - 1) {
        return false;
    }

    bool preserve_english_course_terms = ShouldPreserveEnglishCourseTerms(entry);
    std::string speech_friendly_title = MakeCampusSpeechFriendly(entry->title != nullptr ? entry->title : "", preserve_english_course_terms);
    std::string speech_friendly_content = MakeCampusSpeechFriendly(entry->content != nullptr ? entry->content : "", preserve_english_course_terms);
    int written = snprintf(buf + pos, buf_size - pos, "[%s]\n%s\n\n", speech_friendly_title.c_str(), speech_friendly_content.c_str());
    if (written <= 0 || pos + written >= buf_size - 1) {
        return false;
    }

    pos += written;
    return true;
}

const char* DetectSpecificBiayaLabel(const SearchQuery& query, const std::string& padded_query) {
    bool asks_sore = ContainsText(query.normalized, "sore");
    bool asks_rpi = ContainsText(query.normalized, "rpi");

    if (ContainsText(query.normalized, "teknik informatika") || ContainsText(padded_query, " ti ")) {
        return asks_sore ? "S1 Teknik Informatika (Sore):" : "S1 Teknik Informatika:";
    }
    if (ContainsText(query.normalized, "sistem informasi") || ContainsText(padded_query, " si ")) {
        return asks_sore ? "S1 Sistem Informasi (Sore):" : "S1 Sistem Informasi:";
    }
    if (ContainsText(query.normalized, "desain komunikasi visual") || ContainsText(padded_query, " dkv ")) {
        return asks_sore ? "S1 DKV (Sore):" : "S1 Desain Komunikasi Visual:";
    }
    if (ContainsText(query.normalized, "akuntansi") || ContainsText(padded_query, " akt ")) {
        return asks_sore ? "S1 Akuntansi (Sore):" : "S1 Akuntansi:";
    }
    if ((ContainsText(query.normalized, "manajemen") || ContainsText(padded_query, " mjn ")) &&
        !ContainsText(query.normalized, "manajemen informatika") &&
        !ContainsText(query.normalized, "manajemen bisnis")) {
        if (asks_rpi) return "S1 Manajemen (Kelas RPI):";
        return asks_sore ? "S1 Manajemen (Sore):" : "S1 Manajemen:";
    }
    if (ContainsText(query.normalized, "bisnis digital") || ContainsText(padded_query, " bisdi ")) {
        return asks_sore ? "S1 Bisnis Digital (Sore):" : "S1 Bisnis Digital:";
    }
    if (ContainsText(query.normalized, "pendidikan kepelatihan keolahragaan") ||
        ContainsText(query.normalized, "pendidikan kepelatihan olahraga") ||
        ContainsText(padded_query, " pikor ") || ContainsText(padded_query, " pkor ")) {
        return "S1 Pendidikan Kepelatihan Keolahragaan:";
    }
    if (ContainsText(query.normalized, "manajemen informatika") || ContainsText(padded_query, " mi ")) {
        return asks_sore ? "D3 Manajemen Informatika (Sore):" : "D3 Manajemen Informatika:";
    }
    if (ContainsText(query.normalized, "manajemen bisnis") || ContainsText(padded_query, " mb ")) {
        return "D3 Manajemen Bisnis:";
    }

    return nullptr;
}

std::string ExtractSpecificBiayaContent(const CampusEntry* entry, const SearchQuery& query, const std::string& padded_query) {
    if (entry == nullptr || entry->content == nullptr) {
        return "";
    }
    if (strcmp(entry->id, "biaya_pmb_opsi1") != 0 &&
        strcmp(entry->id, "biaya_pmb_opsi2") != 0 &&
        strcmp(entry->id, "biaya_pmb_opsi3") != 0) {
        return "";
    }

    const char* label = DetectSpecificBiayaLabel(query, padded_query);
    if (label == nullptr) {
        return "";
    }

    std::string raw = entry->content;
    size_t header_end = raw.find("\n- ");
    std::string header = header_end == std::string::npos ? raw : raw.substr(0, header_end);

    size_t label_pos = raw.find(label);
    if (label_pos == std::string::npos) {
        return "";
    }

    size_t line_start = raw.rfind("- ", label_pos);
    if (line_start == std::string::npos) {
        line_start = label_pos;
    }
    size_t line_end = raw.find('\n', label_pos);
    if (line_end == std::string::npos) {
        line_end = raw.size();
    }

    std::string selected_line = raw.substr(line_start, line_end - line_start);
    return header + "\n" + selected_line;
}

bool AppendLecturerEntry(char* buf, int buf_size, int& pos, const CampusEntry* entry) {
    if (entry == nullptr || pos >= buf_size - 1) {
        return false;
    }

    const char* label = IsLecturerSummaryEntry(entry) ? "Ringkasan dosen" : "Nama dosen";
    std::string spoken_name = ExpandLecturerTitleForSpeech(entry->title != nullptr ? entry->title : "");
    std::string speech_friendly_content = MakeCampusSpeechFriendly(entry->content != nullptr ? entry->content : "", true);
    std::string display_name = IsLecturerSummaryEntry(entry) ? std::string(entry->title != nullptr ? entry->title : "") : spoken_name;
    int written = snprintf(buf + pos, buf_size - pos, "%s: %s\nPenyebutan lisan: %s\n%s\n\n",
        label, display_name.c_str(), spoken_name.c_str(), speech_friendly_content.c_str());
    if (written <= 0 || pos + written >= buf_size - 1) {
        return false;
    }

    pos += written;
    return true;
}

}  // ruang nama lokal

const char* SearchCampusData(const char* query, char* buf, int buf_size) {
    if (!query || !buf || buf_size < 100) return nullptr;

    SearchQuery search_query = BuildSearchQuery(query);
    std::string padded_query = " " + search_query.normalized + " ";
    bool asks_dosen = QueryAsksDosen(search_query);
    bool asks_karir = ContainsText(search_query.normalized, "karir") ||
        ContainsText(search_query.normalized, "prospek kerja") ||
        ContainsText(search_query.normalized, "career") ||
        ContainsText(search_query.normalized, "lulusan");
    bool asks_unggulan = ContainsText(search_query.normalized, "mata kuliah unggulan") ||
        ContainsText(search_query.normalized, "matakuliah unggulan") ||
        ContainsText(search_query.normalized, "matkul unggulan");
    bool asks_biaya = ContainsText(search_query.normalized, "biaya") ||
        ContainsText(search_query.normalized, "dpp") ||
        ContainsText(search_query.normalized, "gedung") ||
        ContainsText(search_query.normalized, "total") ||
        ContainsText(search_query.normalized, "perlengkapan") ||
        ContainsText(search_query.normalized, "tahap") ||
        ContainsText(search_query.normalized, "opsi");
    bool asks_rektor = ContainsText(search_query.normalized, "rektor") ||
        ContainsApproxToken(search_query.normalized, "rektor");
    bool asks_akreditasi = ContainsText(search_query.normalized, "akreditasi") ||
        ContainsApproxToken(search_query.normalized, "akreditasi");
    bool asks_matakuliah = (ContainsText(search_query.normalized, "mata kuliah") ||
        ContainsText(search_query.normalized, "matakuliah") ||
        ContainsText(search_query.normalized, "matkul")) && !asks_dosen;
    bool asks_fasilitas = ContainsText(search_query.normalized, "fasilitas") ||
        ContainsText(search_query.normalized, "lab") ||
        ContainsText(search_query.normalized, "wifi");
    bool asks_kontak = ContainsText(search_query.normalized, "kontak") ||
        ContainsText(search_query.normalized, "whatsapp") ||
        ContainsText(search_query.normalized, "telepon") ||
        ContainsText(search_query.normalized, "email");
    bool asks_lokasi = ContainsText(search_query.normalized, "alamat") ||
        ContainsText(search_query.normalized, "lokasi") ||
        ContainsText(search_query.normalized, "dimana");
    bool asks_visi_misi = ContainsText(search_query.normalized, "visi") ||
        ContainsText(search_query.normalized, "misi");
    bool asks_beasiswa = ContainsText(search_query.normalized, "beasiswa");
    bool asks_pmb = QueryAsksPendaftaranPmb(search_query.normalized);
    bool asks_profil = ContainsText(search_query.normalized, "profil") ||
        ContainsText(search_query.normalized, "sejarah") ||
        ContainsText(search_query.normalized, "tentang");
    bool asks_logo = ContainsText(search_query.normalized, "logo") ||
        ContainsText(search_query.normalized, "lambang") ||
        ContainsText(search_query.normalized, "filosofi");
    bool asks_magang = ContainsText(search_query.normalized, "magang") ||
        ContainsText(search_query.normalized, "internship");
    bool asks_great_values = ContainsText(search_query.normalized, "cic great") ||
        ContainsText(search_query.normalized, "great") ||
        ContainsText(search_query.normalized, "nilai dasar") ||
        ContainsText(search_query.normalized, "nilai kampus");
    bool asks_singkatan_prodi = ContainsText(search_query.normalized, "singkatan") ||
        ContainsText(search_query.normalized, "kepanjangan");
    bool asks_program_identity = ContainsText(search_query.normalized, "itu apa") ||
        ContainsText(search_query.normalized, "apa itu") ||
        ContainsText(search_query.normalized, "kepanjangan") ||
        ContainsText(search_query.normalized, "singkatan") ||
        (ContainsText(padded_query, " itu ") && ContainsText(search_query.normalized, "apa"));
    bool asks_rekomendasi_jurusan =
        ContainsText(search_query.normalized, "jurusan yang cocok") ||
        ContainsText(search_query.normalized, "rekomendasi jurusan") ||
        ContainsText(search_query.normalized, "prodi yang cocok") ||
        ContainsText(search_query.normalized, "cocok buat saya") ||
        ContainsText(search_query.normalized, "cocok untuk saya");
    bool asks_algoritma_2 = QueryAsksDosen(search_query) &&
        ContainsApproxToken(search_query.normalized, "algoritma") &&
        ContainsApproxToken(search_query.normalized, "pemrograman") &&
        (ContainsText(search_query.normalized, " 2") || ContainsText(search_query.normalized, "dua"));
    bool has_specific_semester = ExtractSemesterNumber(search_query.normalized) > 0;
    bool has_specific_biaya_option = asks_biaya &&
        (ContainsText(search_query.normalized, "opsi 1") ||
         ContainsText(search_query.normalized, "opsi 2") ||
         ContainsText(search_query.normalized, "opsi 3") ||
         ContainsText(search_query.normalized, "3 bulan") ||
         ContainsText(search_query.normalized, "6 bulan") ||
         ContainsText(search_query.normalized, "1 semester") ||
         ContainsText(search_query.normalized, "1 tahun") ||
         ContainsText(search_query.normalized, "2 semester"));
    bool asks_generic_jurusan_ucic =
        (ContainsText(search_query.normalized, "jurusan") ||
         ContainsText(search_query.normalized, "fakultas") ||
         ContainsText(search_query.normalized, "program studi")) &&
        (LooksLikeUcicReference(search_query.normalized) ||
         ContainsText(search_query.normalized, "universitas catur insan cendekia"));

    auto write_single_entry = [&](const char* entry_id) -> bool {
        const CampusEntry* entry = FindEntryById(entry_id);
        if (entry == nullptr) {
            return false;
        }
        int pos = snprintf(buf, buf_size,
            "Data kampus UCIC yang paling relevan. Jawab dalam bahasa Indonesia penuh. Untuk penyebutan lisan, bacakan S1 sebagai sarjana, D3 sebagai diploma tiga, gelar dan nama mata kuliah yang memang berbahasa Inggris tetap dalam bahasa Inggris, gelar berbahasa Indonesia dengan bentuk lengkap, dan angka biaya atau alamat sebagai angka utuh dalam bahasa Indonesia.\n\n");
        if (pos < 0) {
            return false;
        }
        return AppendEntry(buf, buf_size, pos, entry);
    };

    if (const CampusEntry* exact_entry = FindEntryByExactLookup(search_query)) {
        int pos = snprintf(buf, buf_size,
            "Data kampus UCIC yang paling relevan. Jawab dalam bahasa Indonesia penuh. Untuk penyebutan lisan, bacakan S1 sebagai sarjana, D3 sebagai diploma tiga, gelar dan nama mata kuliah yang memang berbahasa Inggris tetap dalam bahasa Inggris, gelar berbahasa Indonesia dengan bentuk lengkap, dan angka biaya atau alamat sebagai angka utuh dalam bahasa Indonesia.\n\n");
        if (pos > 0 && AppendEntry(buf, buf_size, pos, exact_entry)) {
            return buf;
        }
    }

    // Guardrail intent spesifik: jangan biarkan token umum seperti UCIC/kampus mengalahkan pertanyaan identitas yang jelas.
    if (asks_rektor && write_single_entry("rektor")) {
        return buf;
    }

    if (asks_dosen && TryBuildLecturerTopicResult(search_query, buf, buf_size)) {
        return buf;
    }

    if (asks_visi_misi && write_single_entry("visi_misi")) {
        return buf;
    }

    if (asks_logo && write_single_entry("logo_ucic")) {
        return buf;
    }

    if (asks_lokasi && write_single_entry("lokasi_kampus")) {
        return buf;
    }

    if (asks_kontak && write_single_entry("kontak_ucic")) {
        return buf;
    }

    if (asks_fasilitas && write_single_entry("fasilitas")) {
        return buf;
    }

    if (asks_beasiswa && write_single_entry("beasiswa")) {
        return buf;
    }

    if (asks_magang && write_single_entry("magang")) {
        return buf;
    }

    if (asks_singkatan_prodi && write_single_entry("singkatan_prodi")) {
        return buf;
    }

    if (asks_akreditasi) {
        const char* akreditasi_entry_id =
            (DetectSpecificProdiId(search_query.normalized, padded_query) != nullptr ||
             ContainsText(search_query.normalized, "program studi") ||
             ContainsText(search_query.normalized, "prodi") ||
             ContainsText(search_query.normalized, "jurusan"))
                ? "akreditasi_prodi"
                : "akreditasi";
        if (write_single_entry(akreditasi_entry_id)) {
            return buf;
        }
    }

    if (asks_pmb && !asks_biaya && write_single_entry("pendaftaran_pmb")) {
        return buf;
    }

    if (asks_profil && write_single_entry("profil_ucic")) {
        return buf;
    }

    if (asks_great_values && write_single_entry("great_values")) {
        return buf;
    }

    if (asks_rekomendasi_jurusan) {
        const CampusEntry* faq_entry = FindEntryById("faq_ucic");
        if (faq_entry != nullptr) {
            int pos = snprintf(buf, buf_size,
                "Data kampus UCIC yang paling relevan. Jawab dalam bahasa Indonesia penuh. Untuk penyebutan lisan, bacakan S1 sebagai sarjana, D3 sebagai diploma tiga, gelar dan nama mata kuliah yang memang berbahasa Inggris tetap dalam bahasa Inggris, gelar berbahasa Indonesia dengan bentuk lengkap, dan angka biaya atau alamat sebagai angka utuh dalam bahasa Indonesia.\n\n");
            if (pos > 0) {
                AppendEntry(buf, buf_size, pos, faq_entry);
                return buf;
            }
        }
    }

    std::vector<const char*> compared_prodi_ids;
    CollectMentionedProdiIds(search_query.normalized, padded_query, compared_prodi_ids);
    bool asks_prodi_comparison = LooksLikeProgramComparisonQuery(search_query.normalized) &&
        compared_prodi_ids.size() >= 2;

    if (asks_prodi_comparison) {
        const CampusEntry* left_entry = FindEntryById(compared_prodi_ids[0]);
        const CampusEntry* right_entry = FindEntryById(compared_prodi_ids[1]);
        std::string title = "Perbandingan " + GetProdiDisplayName(left_entry) + " dan " +
            GetProdiDisplayName(right_entry) + " UCIC";
        std::string content = BuildProdiComparisonText(left_entry, right_entry);
        std::string speech_friendly_title = MakeCampusSpeechFriendly(title, false);
        std::string speech_friendly_content = MakeCampusSpeechFriendly(content, false);
        int written = snprintf(buf, buf_size,
            "Data kampus UCIC yang paling relevan. Jawab dalam bahasa Indonesia penuh. Untuk penyebutan lisan, bacakan S1 sebagai sarjana, D3 sebagai diploma tiga, gelar dan nama mata kuliah yang memang berbahasa Inggris tetap dalam bahasa Inggris, gelar berbahasa Indonesia dengan bentuk lengkap, dan angka biaya atau alamat sebagai angka utuh dalam bahasa Indonesia.\n\n[%s]\n%s\n\n",
            speech_friendly_title.c_str(), speech_friendly_content.c_str());
        if (written > 0 && written < buf_size) {
            return buf;
        }
    }

    if (asks_generic_jurusan_ucic && write_single_entry("jurusan_ucic")) {
        return buf;
    }

    Match top[4] = {Match(), Match(), Match(), Match()};
    std::vector<int> index_scores = BuildCampusIndexScores(search_query);

    for (int i = 0; i < CAMPUS_DB_SIZE; ++i) {
        int s = ScoreEntry(&CAMPUS_DB[i], i, search_query, index_scores);
        if (s > 0) {
            InsertMatch(top, i, s);
        }
    }

    if (top[0].idx < 0 || top[0].score < 18) {
        const CampusEntry* fallback = nullptr;
        if (LooksLikeUcicReference(search_query.normalized) ||
            LooksLikeImplicitUcicAcademicQuery(search_query.normalized) ||
            ContainsText(search_query.normalized, "kampus") || ContainsText(search_query.normalized, "universitas")) {
            fallback = FindEntryById("data_lengkap");
        }

        if (fallback != nullptr) {
            int pos = snprintf(buf, buf_size,
                "Saya tidak menemukan kecocokan yang sangat spesifik, jadi saya tampilkan ringkasan kampus UCIC yang paling relevan terlebih dahulu.\n\n");
            if (pos < 0) {
                return nullptr;
            }
            AppendEntry(buf, buf_size, pos, fallback);
            const CampusEntry* contact = FindEntryById("kontak_ucic");
            if (contact != nullptr) {
                AppendEntry(buf, buf_size, pos, contact);
            }
            return buf;
        }

        snprintf(buf, buf_size,
            "Data kampus UCIC yang cocok belum ditemukan untuk '%s'. Coba gunakan kata kunci yang lebih spesifik seperti rektor, biaya kuliah, PMB, jurusan, prodi, dosen, prospek karir, mata kuliah unggulan, kurikulum, alamat, atau beasiswa.",
            query);
        return buf;
    }

    int results_to_show = 1;
    bool prefer_single_best = asks_karir || asks_unggulan || has_specific_semester || has_specific_biaya_option ||
        asks_generic_jurusan_ucic || asks_rektor || asks_akreditasi || asks_matakuliah ||
        asks_fasilitas || asks_kontak || asks_lokasi || asks_visi_misi || asks_beasiswa ||
        asks_pmb || asks_profil || asks_great_values || asks_program_identity ||
        asks_rekomendasi_jurusan || asks_prodi_comparison;
    if (!prefer_single_best) {
        if (top[1].idx >= 0 && top[1].score >= std::max(28, (top[0].score * 65) / 100)) {
            results_to_show = 2;
        }
        if (results_to_show == 2 && top[2].idx >= 0 && top[2].score >= std::max(32, (top[0].score * 55) / 100)) {
            results_to_show = 3;
        }
        if (results_to_show == 3 && top[3].idx >= 0 && top[3].score >= std::max(34, (top[0].score * 50) / 100)) {
            results_to_show = 4;
        }
    }

    int pos = snprintf(buf, buf_size,
        "Data kampus UCIC yang paling relevan. Jawab dalam bahasa Indonesia penuh. Untuk penyebutan lisan, bacakan S1 sebagai sarjana, D3 sebagai diploma tiga, gelar dan nama mata kuliah yang memang berbahasa Inggris tetap dalam bahasa Inggris, gelar berbahasa Indonesia dengan bentuk lengkap, dan angka biaya atau alamat sebagai angka utuh dalam bahasa Indonesia.\n\n");
    if (pos < 0) {
        return nullptr;
    }

    bool should_format_as_lecturer =
        IsLecturerEntry(&CAMPUS_DB[top[0].idx]) &&
        (asks_dosen || QueryLooksLikeSpecificLecturer(&CAMPUS_DB[top[0].idx], search_query));

    if (should_format_as_lecturer) {
        if (results_to_show > 3) {
            results_to_show = 3;
        }
        if (asks_algoritma_2 && results_to_show > 2) {
            results_to_show = 2;
        }
        pos = snprintf(buf, buf_size,
            "Data dosen UCIC yang relevan. Jawab dalam bahasa Indonesia penuh. Gunakan nama dosen persis seperti tertulis di bawah ini, jangan menambah sapaan seperti Bapak atau Ibu, dan ikuti baris Penyebutan lisan saat menyebut gelar.\n\n");
        if (pos < 0) {
            return nullptr;
        }

        for (int t = 0; t < results_to_show && top[t].idx >= 0; ++t) {
            const CampusEntry* entry = &CAMPUS_DB[top[t].idx];
            if (!IsLecturerEntry(entry)) {
                continue;
            }
            if (IsLecturerSummaryEntry(entry) != IsLecturerSummaryEntry(&CAMPUS_DB[top[0].idx])) {
                continue;
            }
            if (!AppendLecturerEntry(buf, buf_size, pos, entry)) {
                break;
            }
        }
        return buf;
    }

    if (asks_biaya && has_specific_biaya_option) {
        const CampusEntry* top_entry = &CAMPUS_DB[top[0].idx];
        std::string filtered_biaya = ExtractSpecificBiayaContent(top_entry, search_query, padded_query);
        if (!filtered_biaya.empty()) {
            std::string speech_friendly_title = MakeCampusSpeechFriendly(top_entry->title != nullptr ? top_entry->title : "", false);
            std::string speech_friendly_content = MakeCampusSpeechFriendly(filtered_biaya, false);
            int written = snprintf(buf, buf_size,
                "Data kampus UCIC yang paling relevan. Jawab dalam bahasa Indonesia penuh. Untuk penyebutan lisan, bacakan S1 sebagai sarjana, D3 sebagai diploma tiga, gelar dan nama mata kuliah yang memang berbahasa Inggris tetap dalam bahasa Inggris, gelar berbahasa Indonesia dengan bentuk lengkap, dan angka biaya atau alamat sebagai angka utuh dalam bahasa Indonesia.\n\n[%s]\n%s\n\n",
                speech_friendly_title.c_str(), speech_friendly_content.c_str());
            if (written > 0 && written < buf_size) {
                return buf;
            }
            // snprintf di atas sudah menimpa isi buf. Tulis ulang header umum
            // supaya jalur cadangan di bawah tidak menyambung entri ke sisa
            // teks biaya yang terpotong.
            pos = snprintf(buf, buf_size,
                "Data kampus UCIC yang paling relevan. Jawab dalam bahasa Indonesia penuh. Untuk penyebutan lisan, bacakan S1 sebagai sarjana, D3 sebagai diploma tiga, gelar dan nama mata kuliah yang memang berbahasa Inggris tetap dalam bahasa Inggris, gelar berbahasa Indonesia dengan bentuk lengkap, dan angka biaya atau alamat sebagai angka utuh dalam bahasa Indonesia.\n\n");
            if (pos < 0) {
                return nullptr;
            }
        }
    }

    for (int t = 0; t < results_to_show && top[t].idx >= 0; ++t) {
        const CampusEntry* entry = &CAMPUS_DB[top[t].idx];
        if (!AppendEntry(buf, buf_size, pos, entry)) {
            break;
        }
    }

    return buf;
}
