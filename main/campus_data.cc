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

// ===== PROFIL DAN IDENTITAS KAMPUS =====
{"profil_ucic","profil","Profil Universitas Catur Insan Cendekia (UCIC)",
"Universitas Catur Insan Cendekia (UCIC) adalah kampus teknologi AI di Kota Cirebon yang berdampak dan mencetak entrepreneur unggul. UCIC menawarkan pendidikan berkualitas tinggi melalui 4 fakultas: Fakultas Teknologi dan Informasi (FTI), Fakultas Ekonomi dan Bisnis (FEB), Fakultas Pendidikan dan Sains (FPS), dan Fakultas Pascasarjana. Program studi UCIC dirancang menjawab kebutuhan industri modern agar lulusan kompeten dan berdaya saing.\nTagline: \"Kampus Teknologi AI\" dan \"Pilihan Tepat Untuk Masa Depan\". Hashtag resmi: #KAMPUSTEKNOLOGIAI #UCICGREAT.",
"ucic,universitas,catur insan,cendekia,cirebon,kampus,info kampus,tentang ucic,apa itu ucic"},
{"sejarah_ucic","profil","Sejarah UCIC",
"Sejarah UCIC dimulai dari lembaga kursus komputer Cirebon Institute of Computer pada tahun 1984. Tahun 1994 berdiri Akademi Perdagangan Widya Dharma (APWD) melalui SK Mendikbud No. 085/U/O/1994. Yayasan Catur Insan Cendekia berdiri 13 Januari 1999, lalu STMIK CIC memperoleh izin pendirian melalui SK Mendikbud RI No. 123/D/O/1999 (15 Juli 1999) untuk S1 Teknik Informatika dan D3 Manajemen Informatika.\nTahun 2001 dibuka S1 Sistem Informasi dan D3 Komputerisasi Akuntansi. Tahun 2006 APWD berubah menjadi Akademi Perdagangan CIC. Tahun 2017 STMIK CIC membuka Program Studi DKV. Tahun 2019 STMIK CIC dan AP CIC bergabung menjadi Universitas Catur Insan Cendekia melalui SK Kemenristekdikti No. 952/KPT/I/2019.\nPerkembangan terbaru: UCIC kini juga membuka Fakultas Pascasarjana (S2 Manajemen) dan program studi baru S1 Pendidikan Matematika di FPS.",
"sejarah ucic,sejarah lengkap,cirebon institute of computer,stmik cic,apwd,yayasan cic,kapan berdiri,tahun berapa berdiri,cic berdiri tahun"},
{"rektor_pimpinan","profil","Rektor dan Pimpinan UCIC",
"Rektor UCIC saat ini adalah Assoc. Prof. Dr. Chandra Lukita, S.E., M.M., M.T.I., yang juga mengajar di Program Magister Manajemen (Data Analytics) dan menjabat sebagai sekretaris APTIKOM. Beliau memimpin universitas dalam mengembangkan pendidikan tinggi berorientasi teknologi dan kewirausahaan.\nCatatan: susunan pimpinan lain (Warek, Dekan, Kaprodi, Kepala BAAK, dsb.) dapat berubah sewaktu-waktu; untuk data terbaru sebaiknya dikonfirmasi ke pihak kampus/BAAK.",
"rektor,pimpinan,ketua,chandra lukita,siapa rektor ucic,warek,dekan,kaprodi"},
{"visi_misi","visi_misi","Visi dan Misi UCIC",
"Visi UCIC: Menjadi universitas yang unggul dan berprestasi, berorientasi pada teknologi dan kewirausahaan (technopreneur), untuk mendukung masyarakat daerah melalui pendidikan yang inovatif, adaptif, dan berdaya saing global.\n\nMisi UCIC:\n1. Menyelenggarakan pendidikan tinggi berkualitas untuk menghasilkan lulusan adaptif, kreatif, dan kompeten berbasis teknologi informasi.\n2. Mendorong lingkungan inovatif dalam teknologi, bisnis, seni, dan sains.\n3. Melaksanakan Tri Dharma Perguruan Tinggi (pendidikan, penelitian, pengabdian masyarakat) secara profesional dan berkualitas.\n4. Menyediakan akses pendidikan yang terjangkau dan relevan dengan perkembangan zaman.\n5. Mengembangkan kerja sama nasional dan internasional, termasuk dengan industri dan UMKM, untuk meningkatkan mutu akademik dan jiwa technopreneurship sivitas akademika.",
"visi,misi,tujuan,orientasi ucic,cita-cita ucic,visi 2030"},
{"nilai_budaya_great_cic","nilai","Nilai Budaya UCIC: CIC GREAT & Commitment-Integrity-Caring",
"CIC GREAT adalah nilai dasar karakter Universitas CIC:\n- Creativity - menciptakan gagasan baru dan inovatif.\n- Initiative - berani bertindak tanpa menunggu instruksi.\n- Critical Thinking - berpikir rasional dan menganalisis fakta.\n- Grit - pantang menyerah dalam mencapai tujuan.\n- Respectful - menghargai keberagaman dan menjunjung etika.\n- Entrepreneurship - berpikir sebagai inovator dan pencipta peluang.\n- Accountable - bertanggung jawab atas setiap tugas.\n- Teamwork - membangun kolaborasi efektif untuk tujuan bersama.\n\nSelain itu, UCIC juga menjunjung nilai budaya CIC: Commitment (dedikasi menjalankan tugas melebihi tuntutan pekerjaan), Integrity (konsistensi dan kesungguhan menjalankan kebijakan akademik), dan Caring (bertanggung jawab dan peduli pada kepentingan bersama).",
"great,nilai,karakter,cic great,ucic great,creativity,grit,commitment,integrity,caring,nilai budaya cic"},
{"logo_ucic","profil","Filosofi Logo UCIC",
"Logo UCIC memuat makna mendalam:\n- Perisai biru: kejujuran dan perlindungan.\n- Emas: optimisme.\n- Obor, buku, pena: semangat akademik.\n- Udang dan Mega Mendung: identitas Cirebon dan nilai budaya lokal.",
"logo,filosofi,lambang,simbol,makna logo"},
{"keunggulan_ucic","profil","Keunggulan / Alasan Memilih UCIC",
"Keunggulan Universitas Catur Insan Cendekia (UCIC):\n1. Berbasis Teknologi - kurikulum dan fasilitas terintegrasi dengan teknologi terkini, termasuk Artificial Intelligence (AI).\n2. Biaya Terjangkau - tersedia skema cicilan (3 bulan, 1 semester, 1 tahun) serta diskon/promo tertentu.\n3. Career Ready System - kurikulum disusun mengikuti kebutuhan industri dengan proyek nyata, studi kasus, dan program magang/MBKM.\n4. Dosen yang Kompeten - dosen praktisi dan akademisi berpengalaman di bidangnya.\n5. Lokasi Strategis - berada di pusat Kota Cirebon, mudah diakses kendaraan umum maupun pribadi.\n6. Belajar Mini Class Gratis - tersedia program belajar singkat gratis sebagai pengenalan sebelum kuliah.\n7. Fleksibel - tersedia kelas pagi, sore/malam (karyawan), dan RPL bagi yang sudah bekerja, dengan sistem perkuliahan hybrid (tatap muka dan daring).\n8. Peluang kerja jelas - program magang, kerja sama industri, dan Career Development Center (CDC) mendukung kesiapan karier lulusan.",
"keunggulan ucic,kelebihan ucic,kenapa pilih ucic,alasan memilih ucic,why ucic,ucic bagus gak,ucic bagus"},
{"lokasi_kampus","lokasi","Lokasi Kampus UCIC",
"UCIC memiliki dua lokasi kampus di Kota Cirebon, Jawa Barat:\n- Kampus 1: Jl. Kesambi No. 58A, Kesambi, Kota Cirebon.\n- Kampus 2: Jl. Kesambi No. 202, Kesambi, Kota Cirebon.\nKedua lokasi berada di pusat Kota Cirebon sehingga tergolong strategis dan mudah diakses kendaraan umum maupun pribadi. Untuk kelas reguler, perkuliahan dapat berpindah (mobile) antara Kampus 1 dan Kampus 2 sesuai jadwal.",
"lokasi,alamat,kampus,kesambi,cirebon,gedung,tempat,kampus 1,kampus 2"},
{"kontak_ucic","kontak","Kontak Resmi UCIC dan PMB",
"Kontak resmi Universitas Catur Insan Cendekia (UCIC):\n- WhatsApp PMB: 0812 1670 0519.\n- Website PMB: https://pmb.cic.ac.id/\n- Link pendaftaran online: https://pmb.cic.ac.id/register\n- Instagram: universitas_cic / pmb_ucic\n- Kampus 1: Jl. Kesambi No. 58A, Kesambi, Kota Cirebon.\n- Kampus 2: Jl. Kesambi No. 202, Kesambi, Kota Cirebon.\n- Email dan Instagram Perpustakaan: perpustakaan@cic.ac.id / @perpustakaan_ucic.\n\nUntuk pertanyaan pendaftaran, verifikasi pembayaran, kendala login PMB, promo/diskon, jadwal kuliah, dan informasi orientasi, calon mahasiswa dapat menghubungi CS/Admin PMB melalui WhatsApp di atas.",
"kontak,email,telepon,nomor,hubungi,whatsapp,wa,hp,pmb,admin pmb,instagram,website,link pendaftaran"},
{"mou_kerjasama","profil","Kerja Sama dan MoU UCIC",
"UCIC menjalin kerja sama dengan institusi pendidikan dalam dan luar negeri, perusahaan, serta lembaga negara untuk meningkatkan kompetensi mahasiswa dan mendukung praktik kerja lapangan/magang. Mitra MoU antara lain: Acharya Institutes, Nitte Meenakshi Institute of Technology (India), Sathyabama Institute of Science and Technology, SRM Institute of Science and Technology, Universitas Raharja, Universitas Al Azhar Medan, PT Propan Raya ICC Cirebon, serta berbagai SMA/SMK mitra di Cirebon dan sekitarnya (SMK Al-Hidayah, SMK Manba'ul Ulum, SMK Presiden, SMAN 4/6 Cirebon, SMKN 2 Cirebon, SMAN 1 Palimanan, SMKN 1 Leuwimunding, SMKN 1 Kadipaten, MA Putri PUI Majalengka), dan UMKM mitra seperti Kecap Merk Damai Purwokerto.",
"mou,kerja sama,mitra ucic,kerjasama industri,mitra sekolah,kerjasama internasional"},
{"faq_orangtua_kampus","profil","FAQ untuk Orang Tua Calon Mahasiswa",
"Q: Apakah kampus sudah terakreditasi? A: Ya, UCIC memiliki akreditasi resmi dari BAN-PT.\nQ: Apakah ada peluang kerja setelah lulus? A: Ada, melalui program magang dan kerja sama industri yang mendukung kesiapan karier mahasiswa (lihat juga layanan CDC).\nQ: Nanti belajarnya di kampus mana? A: Untuk kelas reguler, perkuliahan bisa berpindah (mobile) antara Kampus 1 dan Kampus 2 sesuai jadwal.",
"faq orang tua,orang tua mahasiswa,akreditasi kampus,peluang kerja lulusan,kampus mana,kampus 1 kampus 2,apakah kampus terakreditasi"},

// ===== FAKULTAS DAN PROGRAM STUDI =====
{"fakultas_prodi_ucic","jurusan","Daftar Fakultas dan Program Studi UCIC",
"UCIC memiliki 4 fakultas dengan program studi sebagai berikut:\n\n1. Fakultas Teknologi dan Informasi (FTI):\n   - S1 Teknik Informatika (S.Kom.) - ada kelas karyawan\n   - S1 Sistem Informasi (S.Kom.) - ada kelas karyawan\n   - S1 Desain Komunikasi Visual / DKV (S.Ds.)\n   - D3 Manajemen Informatika (A.Md.)\n\n2. Fakultas Ekonomi dan Bisnis (FEB):\n   - S1 Manajemen (S.M.) - ada kelas karyawan\n   - S1 Akuntansi (S.Ak.) - ada kelas karyawan\n   - S1 Bisnis Digital (S.Bns.)\n   - D3 Manajemen Bisnis (A.Md.)\n\n3. Fakultas Pendidikan dan Sains (FPS):\n   - S1 Pendidikan Kepelatihan Olahraga/Keolahragaan (S.Pd.)\n   - S1 Pendidikan Matematika (S.Pd.)\n\n4. Fakultas Pascasarjana:\n   - S2 Manajemen / Magister Manajemen (M.M.)\n\nFakultas Teknologi dan Informasi (FTI) adalah wadah bagi inovator muda yang berminat pada teknologi, desain, dan sistem informasi. Fakultas Ekonomi dan Bisnis (FEB) membekali mahasiswa memahami ekonomi, bisnis, manajemen, keuangan, dan bisnis digital. Fakultas Pendidikan dan Sains (FPS) mencetak tenaga pendidik dan pelatih profesional. Fakultas Pascasarjana menjadi wadah pengembangan profesional dan akademisi untuk memperdalam keilmuan, riset, dan kepemimpinan di era transformasi digital.",
"jurusan ucic,fakultas ucic,program studi ucic,prodi ucic,semua jurusan,semua prodi,jurusan apa saja,fakultas apa saja,daftar prodi,ada jurusan apa saja"},
{"prodi_ti","jurusan","S1 Teknik Informatika (FTI)",
"Tagline: \"Jadi Developer Profesional di Era Digital\".\nTeknik Informatika mempelajari pemanfaatan teknologi komputer secara optimal untuk transformasi dan pengolahan data, mulai dari perancangan, pengembangan, pengujian, hingga validasi perangkat lunak. Mahasiswa banyak mengkaji pemrograman, komputasi, AI, IoT, machine learning, mobile programming, UI/UX, dan keamanan siber.\n\nProspek Karir: Software Engineer, Front End/Back End/Full Stack Web Developer, Mobile Application Developer, UI/UX Designer, Data Science/ML Engineer, IoT Engineer, Network Administrator, Digital Entrepreneur, Cyber Security Analyst, Pengembangan Sistem Berbasis AI, IT Officer, Data Scientist, System Architect.\n\nMata Kuliah Unggulan: Pemrograman Internet, Mobile Programming, Artificial Intelligence, Internet of Things, Machine Learning, UI & UX Designer, Computer Vision and NLP.",
"teknik informatika,ti,programming,software,ai,karir ti,prospek kerja ti,computer science,jadi developer"},
{"prodi_si","jurusan","S1 Sistem Informasi (FTI)",
"Tagline: \"Penghubung Teknologi & Bisnis\".\nSistem Informasi membekali mahasiswa dengan ilmu sistem informasi dan keterampilan menggunakan metode/teknik mutakhir untuk menyelesaikan masalah manajemen organisasi berbasis teknologi informasi. Mahasiswa mempelajari konsep sistem enterprise, analisis prospek bisnis, rekayasa sistem informasi, pemrograman web, pengelolaan basis data, etika profesi, dan kewirausahaan.\n\nProspek Karir: Application Engineer, Digital Marketing, Research and Development (RnD), Data Scientist, Data Analyst, Software Developer, System Analyst/Designer, IT Consultant, ERP Specialist, Project Manager IT, Quality Software System, Business Application Developer, e-Business Manager.\n\nMata Kuliah Unggulan: Audit Sistem Informasi, Enterprise Resource Planning (ERP), Proyek Technopreneurship, E-CRM, Agile Development, UI/UX Design, Business Intelligence.",
"sistem informasi,si,karir si,prospek kerja si,bisnis dan teknologi,penghubung teknologi bisnis"},
{"prodi_dkv","jurusan","S1 Desain Komunikasi Visual / DKV (FTI)",
"Tagline: \"Kreativitas Tanpa Batas di Dunia Visual\".\nDKV menggabungkan kreativitas seni dengan pemahaman komunikasi dan media visual. Mahasiswa mengembangkan keterampilan desain grafis, fotografi, ilustrasi, dan multimedia untuk menciptakan pesan visual yang efektif, estetis, dan berdampak, serta terlibat dalam proyek dan kolaborasi tim.\n\nProspek Karir: Desainer Grafis, UI/UX Designer, Ilustrator, Komik Desainer, Fotografer/Videografer, Desainer Kemasan, Desainer Identitas Visual, Spesialis Media Sosial, Periklanan dan Pemasaran Visual, Produser Konten Digital, Pendidik/Instruktur, Editor Grafis, Desainer Seni dan Hiburan, Digital Marketing.\n\nMata Kuliah Unggulan: Ilustrasi, Fotografi, Komik, 3D Animasi, Media Interaktif, Desain Komunikasi Visual Identitas, Creative Content Advertising.\nPeminatan: DIM (Digital Interaktif Media) dan DA (Digital Advertising).",
"dkv,desain komunikasi visual,desain,grafis,animasi,ui ux,karir dkv,kreativitas tanpa batas"},
{"prodi_d3mi","jurusan","D3 Manajemen Informatika (FTI)",
"Tagline: \"Ahli IT Siap Kerja Cepat\".\nD3 Manajemen Informatika mempelajari teori dan penerapan teknologi informasi untuk mendukung kegiatan bisnis dan operasional perusahaan secara efektif dan efisien. Program ini memadukan keahlian manajemen dengan software, media digital, database, jaringan, web, dan konten digital, dengan penekanan pada praktik dan proyek nyata.\n\nProspek Karir: Web Designer, Content Creator, Software Developer, Network Administrator, IT Consultant, IT Support Specialist, Database Administrator, Web Administrator, Junior Programmer.\n\nMata Kuliah Unggulan: Pemrograman Web, Digital Content Marketing, Fotografi Digital, UI & UX Design, Kecerdasan Buatan.",
"d3,manajemen informatika,diploma,media digital,konten digital,ahli it siap kerja cepat"},
{"prodi_manajemen","jurusan","S1 Manajemen (FEB)",
"Tagline: \"Siap Jadi Pebisnis & Profesional di Dunia Kerja\".\nManajemen mempelajari pengelolaan perusahaan/organisasi terkait ilmu ekonomi dan bisnis. Mahasiswa dibekali keterampilan mengelola kebijakan manajemen, strategi bisnis, organisasi, SDM, produksi, pemasaran, administrasi, hingga organisasi nirlaba, dengan penekanan pada kewirausahaan dan technopreneurship.\n\nProspek Karir: Account Officer, Bagian Operasional dan Logistik, Product Manager, Market Researcher, Event Planner/Organizer, Wirausaha, CEO, Direktur, Manajer, Credit Analyst, Treasury Supervisor, Head of Operations Business.\n\nMata Kuliah Unggulan: Manajemen Keuangan, Manajemen Operasional, Manajemen Pemasaran, Manajemen Sumber Daya Manusia.",
"manajemen,bisnis,pemasaran,sdm,karir manajemen,organisasi,siap jadi pebisnis"},
{"prodi_akuntansi","jurusan","S1 Akuntansi (FEB)",
"Akuntansi mempelajari pengelolaan dan analisis keuangan untuk membantu organisasi mencapai tujuan, mencakup Akuntansi Keuangan, Akuntansi Manajemen, Pajak, dan Auditing, serta kemampuan analisis dan interpretasi data keuangan untuk pengambilan keputusan bisnis.\n\nProspek Karir: Auditor Internal, Auditor Eksternal, Accounting, Konsultan Keuangan, Data Analis, Konsultan Perpajakan.\n\nMata Kuliah Unggulan: Akuntansi Keuangan, Auditing, Sistem Informasi, Perpajakan.",
"akuntansi,keuangan,audit,pajak,karir akuntansi,accounting"},
{"prodi_bisdi","jurusan","S1 Bisnis Digital (FEB)",
"Tagline: \"Bangun Bisnis di Era Online\".\nBisnis Digital mempelajari pemanfaatan teknologi digital dalam bisnis modern, berkaitan dengan manajemen, ekonomi, teknologi informasi, dan kewirausahaan. Lulusan berperan mengelola dan mengembangkan bisnis berbasis teknologi: digital marketing, e-commerce, analisis data bisnis, inovasi produk digital, dan manajemen startup.\n\nProspek Karir: Technopreneur, Digital Marketing Manager, Social Media Marketing Manager, Data Intelligence Analyst, e-Commerce Practitioner, Fintech Practitioner, Startup Consultant, Startup Ecosystem Team, Financial Planner.\n\nMata Kuliah Unggulan: Technopreneurship, Financial Technology, Model Bisnis MSMEs, Analisis Bisnis Digital, Startup Business Model.",
"bisnis digital,bisdig,bisdi,e commerce,startup,digital marketing,karir bisnis digital,bangun bisnis online"},
{"prodi_d3mb","jurusan","D3 Manajemen Bisnis (FEB)",
"Tagline: \"Siap Jadi Pebisnis & Profesional\".\nManajemen Bisnis mempelajari perekrutan, pelatihan, koordinasi, dan pengambilan keputusan efektif untuk mengelola dan mencapai tujuan bisnis, termasuk Manajemen Mutu, Manajemen Proyek, komunikasi, dan negosiasi.\n\nProspek Karir: E-Commerce Specialist, Social Media Specialist, Marketing, Staff Marketing, Technopreneurship, Marketing Manager, Human Resource Development (HRD), Business Analyst, Entrepreneur, Operations Manager.\n\nMata Kuliah Unggulan: Manajemen Bisnis Ritel, Manajemen Tataletak & Planogram, E-Commerce, Technopreneurship, Manajemen Logistik dan Supply Chain.",
"d3 manajemen bisnis,d3 mb,manajemen bisnis,diploma bisnis,ritel,supply chain,siap jadi pebisnis profesional"},
{"prodi_pko","jurusan","S1 Pendidikan Kepelatihan Olahraga/Keolahragaan (FPS)",
"Tagline: \"Dari Hobi Jadi Profesi di Dunia Olahraga\".\nProgram ini mempelajari teori dan praktik kepelatihan olahraga secara ilmiah dan profesional, berkaitan dengan ilmu pendidikan, ilmu keolahragaan, biomedik, psikologi olahraga, dan manajemen kepelatihan. Lulusan merancang, melaksanakan, dan mengevaluasi program latihan olahraga secara sistematis, dari atlet pemula hingga profesional.\n\nProspek Karir: Guru Olahraga, Pelatih Profesional, Personal Trainer, Instruktur Fitness, Wasit/Juri Olahraga, Sport Entrepreneur, Sport Event Organizer, Konsultan Kebugaran, Manajer Tim/Klub Olahraga.\n\nMata Kuliah Unggulan: Ilmu Kepelatihan Olahraga, Biomekanika Olahraga, Psikologi Olahraga, Fisiologi Latihan dan Kinerja Atlet, Manajemen Kepelatihan Olahraga.",
"pkor,pko,pikor,pendidikan kepelatihan olahraga,olahraga,kepelatihan,atlet,pelatih,dari hobi jadi profesi"},
{"prodi_pendidikan_matematika","jurusan","S1 Pendidikan Matematika (FPS) - Program Studi Baru",
"Pendidikan Matematika mempelajari konsep matematika, strategi pembelajaran, serta pemanfaatan teknologi dalam pendidikan. Program ini mencetak calon pendidik matematika yang berpikir logis, analitis, kreatif, dan adaptif. Mahasiswa juga belajar merancang pembelajaran menarik berbasis media digital dan Artificial Intelligence.\n\nProspek Karir: Pendidik Matematika Adaptif (AI-Enabled Educator), Peneliti Pendidikan Matematika (Mathematics Education Researcher), Digital Smart MathPreneur (Digital Entrepreneur & Media Developer).\n\nMata Kuliah Unggulan: Pengantar Pendidikan Matematika, Kalkulus, Aljabar Linier, Statistika Dasar, Strategi dan Inovasi Pembelajaran, Media Pembelajaran Digital berbasis Kecerdasan Buatan, Teknologi Kecerdasan Artifisial dalam Pembelajaran STEM, Micro Teaching, E-Learning dan Pembelajaran Jarak Jauh, Literasi Digital dan Big Data, Game Edukasi dan Gamifikasi, Koding dan Kecerdasan Artifisial.",
"pendidikan matematika,matematika,guru matematika,prodi baru,jurusan baru,kalkulus,ai dalam pendidikan"},
{"prodi_s2_manajemen","jurusan","S2 Manajemen / Magister Manajemen (Fakultas Pascasarjana)",
"Magister Manajemen UCIC memiliki keunggulan pada kurikulum berbasis Artificial Intelligence (AI), entrepreneurship, dan leadership untuk menghadapi transformasi bisnis digital. Pembelajaran dilakukan secara aplikatif melalui proyek nyata dan studi kasus industri, dengan sistem perkuliahan hybrid yang fleksibel bagi profesional aktif. Mahasiswa juga berkesempatan mengikuti publikasi ilmiah dan konferensi internasional, serta memilih konsentrasi sesuai kebutuhan karier.\n\nProspek Karir: Strategic Leader/CEO, Corporate Strategy Manager, Business Development Director, Strategic Advisor, Entrepreneur/Startup Founder, Business Development Specialist, SME(s) Consultant, Technopreneur, Digital Business Strategist, Digital Market Analyst, Digital Management Consultant, Digital Business Analyst.\n\nMata Kuliah Unggulan: AI for Business Decision, Digital Marketing Strategy & Innovation, Strategic & Adaptive Leadership, Business Model Innovation, E-Commerce Strategy & Operations, Global Business Strategy, Business Practice Project, Mature Leadership in Digital Era, Business Intelligence, Digital Transformation Frameworks, Digital Supply Chain Management, SME(s) Management & Sustainability, Academic Writing & Publication, International Conference.\n\nKonsentrasi pilihan: Strategic Management & Leadership, Entrepreneurship & Business Development, Digital Transformation & Management Innovation.",
"s2 manajemen,magister manajemen,pascasarjana,mm,kuliah s2,program magister"},
{"faq_rekomendasi_jurusan","jurusan","Rekomendasi Jurusan Sesuai Minat Calon Mahasiswa",
"Semua jurusan di UCIC dapat direkomendasikan sesuai minat dan bakat calon mahasiswa. Jika calon mahasiswa sudah mantap dengan pilihannya, tidak perlu dipaksa memilih jurusan lain. Jika masih bingung, dapat direkomendasikan berdasarkan minat:\n- Suka komputer, coding, aplikasi, AI, teknologi: Teknik Informatika atau Sistem Informasi.\n- Suka desain, ilustrasi, konten visual, multimedia: DKV.\n- Suka bisnis, marketing, organisasi, usaha: Manajemen, Bisnis Digital, atau D3 Manajemen Bisnis.\n- Suka administrasi bisnis, ritel, operasional, pemasaran: D3 Manajemen Bisnis.\n- Suka olahraga, kepelatihan, aktivitas fisik: Pendidikan Kepelatihan Olahraga.\n- Suka matematika, ingin jadi pendidik/inovator pendidikan: Pendidikan Matematika.\n- Ingin jalur IT terapan yang cepat kerja: D3 Manajemen Informatika.\n- Sudah bekerja dan ingin melanjutkan S2 bidang manajemen: Magister Manajemen (S2).",
"rekomendasi jurusan,bingung pilih jurusan,anak komputer cocok jurusan apa,suka desain masuk mana,jurusan buat bisnis apa,simulasi jurusan,minat bakat"},
{"singkatan_prodi","jurusan","Singkatan Program Studi UCIC",
"Singkatan program studi UCIC:\n- Teknik Informatika = TI\n- Sistem Informasi = SI\n- Desain Komunikasi Visual = DKV\n- Akuntansi = Akt\n- Manajemen = Mjn\n- Bisnis Digital = Bisdi\n- Pendidikan Kepelatihan Keolahragaan = Pikor atau PKOR\n- Manajemen Informatika = MI\n- Manajemen Bisnis = MB",
"singkatan prodi,singkatan jurusan,kepanjangan ti,kepanjangan si,kepanjangan dkv,kepanjangan akt,kepanjangan mjn,kepanjangan bisdi,kepanjangan pikor,kepanjangan pkor,kepanjangan mi,kepanjangan mb,ti,si,dkv,akt,mjn,bisdi,pikor,pkor,mi,mb"},

// ===== KURIKULUM =====
{"kurikulum_ti","kurikulum","Kurikulum S1 Teknik Informatika",
"Semester 1: Pancasila dan Kewarganegaraan, Bahasa Indonesia, Pembangunan Karakter dan Etika Profesi, General English, Pemrograman Internet, Aljabar Linear dan Matriks, Arsitektur dan Organisasi Komputer, Algoritma dan Pemrograman.\nSemester 2: Pendidikan Agama, Intermediate English, Sistem Basis Data, Komunikasi Data dan Jaringan, Rekayasa Aplikasi Internet, Teknik Multimedia Digital, Sistem Operasi, Struktur Data.\nSemester 3: English for Specific Purpose, Statistik for Data Science, Pemrograman Berorientasi Objek, Administrasi Jaringan, Analisis dan Desain Perangkat Lunak, UI & UX Designer, Kecerdasan Buatan, Manajemen Data & Informasi.\nSemester 4: TOEFL Preparation, Mobile Programming, Keamanan Sistem Informasi dan Jaringan, Sistem Digital, Data Mining, Sistem Penunjang Keputusan, Grafika Komputer & Pengolahan Citra Digital, Sistem Pakar (peminatan), Sistem Terdistribusi, Pengujian dan Pemeliharaan Sistem.\nSemester 5: Student Exchange, Technopreneurship, Pengembangan Aplikasi Mobile, Machine Learning, Internet of Things, Computer Vision and NLP, Game Cerdas, Jaminan Kualitas Perangkat Lunak, Manajemen Proyek Perangkat Lunak.\nSemester 6: Student Exchange, Kapita Selekta, Proyek Technopreneurship, Realitas Virtual dan Augmentasi, Metodologi Penelitian & Riset Pustaka, Proyek Teknologi Informasi, KKN Tematik, MBKM (Studi Independen Bersertifikat), Magang (Studi Independen Mandiri).\nSemester 7: MBKM (Magang Bersertifikat), Magang Internship Mandiri.\nSemester 8: Skripsi.\nPeminatan: Intelligence System (IS) - Grafika Komputer & Citra Digital, Sistem Pakar, Sistem Terdistribusi, Pengujian & Pemeliharaan Sistem. Software Engineering (SE) - Computer Vision and NLP, Game Cerdas, Jaminan Kualitas Perangkat Lunak, Manajemen Proyek Perangkat Lunak.",
"kurikulum ti,kurikulum teknik informatika,semester ti,mata kuliah ti"},
{"kurikulum_si","kurikulum","Kurikulum S1 Sistem Informasi",
"Semester 1: Pancasila dan Kewarganegaraan, Bahasa Indonesia, Pembangunan Karakter dan Etika Profesi, General English, Matematika Bisnis, Manajemen dan Organisasi, Sistem dan Teknologi Informasi, Algoritma dan Pemrograman.\nSemester 2: Pendidikan Agama, Teknologi dan Tren E-commerce, Intermediate English, Arsitektur SI/TI Perusahaan, Tata Kelola Teknologi Informasi, Analisa Proses Bisnis, Pemrograman Web (HTML+CSS), Algoritma dan Pemrograman 2.\nSemester 3: Statistik, Sistem Informasi Manajemen, English for Specific Purpose, Interaksi Manusia dan Komputer, Analisa dan Perancangan Sistem Informasi, Sistem Basis Data, Pemrograman Internet 1.\nSemester 4: TOEFL Preparation, UI & UX Design, Rekayasa Perangkat Lunak (Agile Scrum), Analisa Sistem Berorientasi Obyek, Pemrograman Internet Intermediate (Framework, MVC), Mata Kuliah Pilihan 1 & 2.\nSemester 5: Perancangan Aplikasi Mobile, Student Exchange (Agile Implementation), Jaringan Komputer, Technopreneurship, Rekayasa Perangkat Lunak, Pemrograman Internet Intermediate, Mata Kuliah Pilihan 1 & 2.\nSemester 6: Student Exchange (Audit SI/Interaksi Manusia & Komputer/Audio Visual Effects), Metodologi Penelitian dan Riset Pustaka, Proyek Technopreneurship, Proyek Sistem Informasi, Kapita Selekta, Mata Kuliah Pilihan 1.\nSemester 7: Magang Internship Mandiri, Magang MBKM (Magang Bersertifikat), KKN Tematik.\nSemester 8: Skripsi.\nPeminatan Semester 4: Manajemen Data Enterprise, Implementasi Arsitektur Enterprise, E-commerce, Business Intelligence. Peminatan Semester 5: E-Government, Audit Sistem Informasi, Integrasi Sistem Enterprise, Enterprise Resource Planning. Peminatan Semester 6: E-CRM, Sistem Penunjang Keputusan.",
"kurikulum si,kurikulum sistem informasi,semester si,mata kuliah si"},
{"kurikulum_dkv","kurikulum","Kurikulum S1 Desain Komunikasi Visual (DKV)",
"Semester 1: Pancasila dan Kewarganegaraan, Bahasa Indonesia, Pembangunan Karakter dan Etika Profesi, General English, Dasar Desain Grafis, Basic Tipografi, Bahasa Rupa, Computer Graphic for Graphic Design, Estetika Visual Desain.\nSemester 2: Pendidikan Agama, Teknologi dan Tren E-commerce, Intermediate English, Sosiologi Desain, Design Thinking, Kinetic Tipografi, Metoda Produksi Grafika, Komik Digital.\nSemester 3: English for Specific Purpose, Desain Komunikasi Visual Identitas, 3D Artist, Basic Fotografi, Advertising Copywriting, Semiotika Visual, Web Design, Tinjauan dan Kritik Desain.\nSemester 4: TOEFL Preparation, Desain Komunikasi Visual Promosi, Applied Photography, Audio Visual Effects, Packaging Design, Media Interaktif, Animasi 3D, Creative Content Advertising, Inovasi Grafis.\nSemester 5: Technopreneurship, Desain Komunikasi Visual Sosial, Metodologi Penelitian Desain, Manajemen Desain & Creativepreneur, Game Desain, UI/UX Designer, Visual Merchandising, Video Brand Production, Student Exchange (Fotografi).\nSemester 6: Kapita Selekta, Proyek Technopreneurship, Strategi Konten Digital, Psikologi Persepsi, Project DKV, MBKM (Studi Independen Bersertifikat).\nSemester 7: Penguasaan Teknologi, Tata Tulis Karya Ilmiah, Etika Profesi Kerja, Komunikasi dan Teamwork, Ide/Inisiatif/Tanggung Jawab (semua MBKM), KKN Tematik.\nSemester 8: Skripsi.\nPeminatan: DIM (Digital Interaktif Media), DA (Digital Advertising).",
"kurikulum dkv,kurikulum desain komunikasi visual,semester dkv,mata kuliah dkv"},
{"kurikulum_d3mi","kurikulum","Kurikulum D3 Manajemen Informatika",
"Semester 1: Pancasila dan Kewarganegaraan, Bahasa Indonesia, Pembangunan Karakter dan Etika Profesi, General English, Pengantar Media Digital, Komputer Grafis 1, Komunikasi dan Presentasi Dasar, AI Technician.\nSemester 2: Pendidikan Agama, Intermediate English, Design Thinking, Fotografi Digital, Komputer Grafis 2 (Data Visualization), Statistik Informatika, Digital Copywriting.\nSemester 3: English for Specific Purpose, Technopreneurship, Videografi, Brand Identity, Manajemen Media Sosial, Visual Storytelling, Digital Content Marketing.\nSemester 4: TOEFL Preparation, Proyek Technopreneurship, Fotografi dan Videografi Lanjutan, Digital Bisnis (Startup), Integrated Marketing Communication, Digital Content Interaktif, Monetisasi Konten Digital, Mata Kuliah Pilihan (Kecerdasan Buatan/Business Intelligence/Audio Visual Effects).\nSemester 5: Trend dan Eksplorasi Content Digital, E-Commerce, UI & UX Design, Proyek Manajemen Informatika, Etika dan Regulasi Media Digital, Sertifikasi Kompetensi & HaKI Produk Digital, Manajemen Proyek Media Digital.\nSemester 6: Tugas Akhir, Kapita Selekta, Tata Tulis Karya Ilmiah, Influencer Networking & Collaboration, Kampanye Konten Digital, Optimasi Content Digital, Mata Kuliah Pilihan (Pemrograman Internet Advance/Animasi 3D/Realitas Virtual dan Augmentasi).",
"kurikulum d3 manajemen informatika,kurikulum mi,semester mi,mata kuliah d3 mi"},
{"kurikulum_manajemen","kurikulum","Kurikulum S1 Manajemen",
"Semester 1: Pendidikan Agama, Pancasila dan Kewarganegaraan, General English, Bahasa Indonesia, Ekonomi Mikro, Akuntansi Keuangan Dasar, Teknologi Informasi untuk Bisnis, Manajemen & Bisnis Dasar.\nSemester 2: Pengembangan Karakter, Manajemen Pemasaran, Matematika Ekonomi dan Bisnis, Hukum Bisnis, Komunikasi Bisnis dan Negosiasi, Perilaku Konsumen, Ekonomi Makro, Sistem Informasi Manajemen.\nSemester 3: Manajemen Sumber Daya Manusia, Digital Marketing, Manajemen Keuangan, Manajemen Operasi, Technopreneurship, Akuntansi Biaya, Big Data Analytic.\nSemester 4: English for Business, Manajemen Proyek Teknologi Informasi, Manajemen Anggaran, Perilaku Organisasional, Technopreneurship Lanjutan, Manajemen Strategik, Teori Portofolio dan Analisis Investasi.\nSemester 5: Manajemen Inovasi Berbasis Teknologi, Bisnis Digital, Statistika dan Analisis Bisnis, Metodologi Penelitian, Manajemen Transformasi Digital, Analisa Laporan Keuangan (Student Exchange), Mata Kuliah Pilihan.\nSemester 6: E-Commerce & Platform, Etika Profesi Bisnis, Seminar Manajemen, Metode Pengambilan Keputusan (Student Exchange), Kapita Selekta, Mata Kuliah Pilihan.\nSemester 7: Magang Studi Independen, Kepemimpinan Bisnis, Kepemimpinan Entrepreneur, Kemampuan Kerja Tim, Literasi Digital, Literasi Data.\nSemester 8: Skripsi.",
"kurikulum manajemen,semester manajemen,mata kuliah manajemen"},
{"kurikulum_akuntansi","kurikulum","Kurikulum S1 Akuntansi",
"Semester 1: Akuntansi Keuangan Dasar, Ekonomi Mikro, General English, Manajemen & Bisnis Dasar, Pancasila & Kewarganegaraan, Pendidikan Agama, Teknologi Informasi untuk Bisnis, Bahasa Indonesia.\nSemester 2: Ekonomi Makro, Hukum Bisnis, Komunikasi Bisnis & Negosiasi, Matematika Ekonomi dan Bisnis, Pengembangan Karakter, Praktikum Akuntansi Keuangan, Akuntansi Keuangan Menengah, Akuntansi Biaya.\nSemester 3: Akuntansi Manajemen, Hukum Perpajakan, Manajemen Keuangan, Sistem Informasi Akuntansi, Technopreneurship, Akuntansi Sektor Publik, Akuntansi Keuangan Lanjutan.\nSemester 4: Akuntansi Entitas Mikro Kecil dan Menengah, Akuntansi Internasional, Auditing 1, Technopreneurship Lanjutan, Teknologi Keuangan, Akuntansi Perpajakan, English for Business, Bisnis Digital (Student Exchange).\nSemester 5: Metodologi Penelitian, Akuntansi Syariah, Analisis Laporan Keuangan, Auditing 2, Statistika dan Analisis Bisnis, Praktikum Perpajakan, Agile Implementation (Student Exchange).\nSemester 6-7: Seminar Akuntansi, Teori Akuntansi, Kapita Selekta, Teori Portofolio dan Analisis Investasi, Etika Profesi Akuntan, Praktikum Auditing, KKN Tematik, Mata Kuliah Pilihan (Lintas Prodi), Magang.\nSemester 8: Skripsi.\nMata Kuliah Pilihan: Akuntansi Keperilakuan, Akuntansi Sosial dan Lingkungan, Akuntansi Pemerintahan, Akuntansi Desa, Audit Forensik, Audit Investigatif, Audit Sektor Publik, Audit Sistem Informasi, Audit Internal, Perpajakan Internasional.",
"kurikulum akuntansi,semester akuntansi,mata kuliah akuntansi"},
{"kurikulum_bisnisdigital","kurikulum","Kurikulum S1 Bisnis Digital",
"Semester 1: Bahasa Indonesia, Pancasila dan Kewarganegaraan, Pengembangan Karakter & Etika Profesi, General English, Ekonomika Bisnis, Etika dan Komunikasi Bisnis, Konsep dan Ide Bisnis Digital, Manajemen Bisnis.\nSemester 2: Pendidikan Agama, Graphic Design (Video Animation), Financial Management, Web Design, SEO, Statistika Bisnis, Digital Marketing, Management Information System.\nSemester 3: Technopreneurship, Financial Technology, Model Bisnis MSMEs, Analisis Bisnis Digital, Startup Business Model, Analisa Investasi Bisnis & Portfolio, Metodologi Penelitian Bisnis, Studi Kelayakan Bisnis.\nSemester 4: Project Technopreneurship, English for Business, e-Customer Relationship Management, Academic Writing and Publication, Customer Behaviour, Integrated Marketing Communication (IMC).\nSemester 5: Business Intelligence, Digital Business Platform, Design Thinking Method, Supply Chain Management, Student Exchange: Fotografi (DKV).\nSemester 6: Digital Business Development Project (Digital MSMEs-Preneur / Digital Business Consultant / Digital Business Startup), Crowdfunding & Fintech Ecosystem, Enterprise Resources Planning, Data Mining, Kapita Selekta, Writing Business Report.\nSemester 7: Digital Business Development (Project), MBKM (Kewirausahaan/SIB/Penelitian/PKM).\nSemester 8: Skripsi.\nKonsentrasi: Digital MSMEs-Preneur (MSMEs Business Model Development, AI for MSMEs Business), Digital Business Consultant (Data Analytics, Market Research), Digital Business Startup (Business Modelling, Startup Ideation and Validation).",
"kurikulum bisnis digital,semester bisnis digital,mata kuliah bisnis digital"},
{"kurikulum_d3mb","kurikulum","Kurikulum D3 Manajemen Bisnis",
"Semester 1: Pembangunan Karakter dan Etika Profesi, Pancasila, Kewarganegaraan, dan Anti Korupsi, General English, Bahasa Indonesia, Pengantar Akuntansi, Teknologi Informasi untuk Bisnis, Manajemen & Bisnis Dasar, Pengantar Ilmu Ekonomi Bisnis.\nSemester 2: Intermediate English, Pendidikan Agama, Matematika Ekonomi dan Bisnis, Dasar-Dasar Perpajakan, Perilaku Konsumen, Pengelolaan Keuangan Ritel, Manajemen Tataletak & Planogram, Strategi Pemasaran Ritel.\nSemester 3: English for Specific Purpose, Technopreneurship, Sistem Akuntansi, Akuntansi Keuangan Bisnis, Manajemen Sumber Daya Manusia, Praktikum Akuntansi Manual, Manajemen Logistik dan Supply Chain, Manajemen Hubungan Pelanggan, Hukum Bisnis.\nSemester 4: Technopreneurship Lanjutan, TOEFL Preparation, Metodologi Penelitian Bisnis, Penganggaran Perusahaan, Aplikasi Software Akuntansi, Studi Kelayakan Bisnis, Manajemen Kualitas, Manajemen Kas dan Portofolio.\nSemester 5: Kepemimpinan, Kemampuan Bekerjasama, Kemampuan Berkomunikasi, Kreativitas, Kemampuan Administrasi, Kemampuan Merumuskan dan Menyelesaikan Masalah Teknis, Seminar Bisnis.\nSemester 6: Kapita Selekta, Tugas Akhir, Komunikasi Bisnis.",
"kurikulum d3 manajemen bisnis,kurikulum d3 mb,semester d3 mb"},
{"kurikulum_pko","kurikulum","Kurikulum S1 Pendidikan Kepelatihan Olahraga",
"Semester 1: Bahasa Indonesia, Pancasila dan Kewarganegaraan, General English, Pengembangan Karakter, Anatomi, Pembelajaran Motorik, Pelatihan Cabor Atletik, Pelatihan Cabor Renang, Pelatihan Cabor Futsal.\nSemester 2: Pendidikan Agama, Fisiologi Olahraga, Manajemen dan Administrasi Olahraga, Pelatihan Cabor Sepak Bola/Basket/Voli/Taekwondo, English for Education, Landasan Pendidikan, Kurikulum dan Pembelajaran.\nSemester 3: Technopreneurship, English for Sport, Bimbingan Konseling, Teori Belajar dan Pembelajaran, Pelatihan Cabor Pencak Silat/Karate, Pengantar Ilmu Keolahragaan, Perwasitan Olahraga, Teori dan Metodologi Kepelatihan Olahraga.\nSemester 4: Biomekanika Olahraga, Ilmu Gizi Olahraga, Teknologi Kepelatihan Olahraga, Analisis Performance Olahraga berbasis AI, Technopreneurship Lanjutan, Sport Pedagogy.\nSemester 5: Metode Penelitian Pendidikan Olahraga, Manajemen Kepelatihan Olahraga, Didaktik Metodik Olahraga, Psikologi Olahraga, Evaluasi Kepelatihan Olahraga, Kinesiologi dan Massage Olahraga.\nSemester 6: Microteaching, Kapita Selekta, Statistika, Pemandu Bakat Olahraga, Seminar Magang Kepelatihan Olahraga, Evaluasi dan Pengembangan Latihan, Sistem Organisasi dan Pertandingan, Pemasaran Olahraga, Pengembangan Program Latihan Berbasis Data.\nSemester 7 (MBKM): Profesi Kependidikan, Praktek Lapangan Persekolahan, Sarana dan Prasarana Olahraga, Tes dan Pengukuran Olahraga, Strategi dan Taktik Olahraga berbasis Digital, Pencegahan dan Perawatan Cedera Olahraga.\nSemester 8: Skripsi.",
"kurikulum pkor,kurikulum pko,kurikulum pendidikan kepelatihan olahraga,semester pko"},
{"kurikulum_pendidikan_matematika","kurikulum","Kurikulum S1 Pendidikan Matematika",
"Semester 1-2: Pendidikan Pancasila, Pendidikan Kewarganegaraan, Bahasa Indonesia, General English, Fondasi Pendidikan, Perkembangan Peserta Didik, Kalkulus, Aljabar Linier, dasar-dasar keilmuan matematika.\nSemester 3-4: Filsafat Ilmu & Logika, Analisis Real, Teori Bilangan, Geometri Euclides & Non-Euclides, Probabilitas, Statistika Inferensial, English for Specific Purpose, Kewirausahaan Digital, Micro Teaching, Etnomatematika, Etika dalam Penggunaan AI, Strategi dan Inovasi Pembelajaran, Teknologi Kecerdasan Artifisial dalam Pembelajaran STEM.\nSemester 5-6: Analisis Kompleks, Metode Numerik, Persamaan Diferensial, Pembelajaran Matematika Berbahasa Inggris, Evaluasi Pembelajaran, Pengembangan Kurikulum Matematika, Kapita Selekta Matematika, Bisnis Digital dan Start-up Pendidikan Matematika, Literasi Digital dan Big Data, Metodologi Penelitian Pendidikan Matematika, E-Learning dan Pembelajaran Jarak Jauh, Proyek Technopreneurship Matematika. Mata Kuliah Pilihan: Matematika Komputasi, Matematika Diskrit, Aplikasi AI dalam Pendidikan, Desain Konten Digital Pembelajaran, Database dan Sistem Informasi, Videografi Pendidikan Matematika, Pembelajaran Matematika Inklusi dan ABK, Asesmen Alternatif dalam Matematika, Game Edukasi dan Gamifikasi, Koding dan Kecerdasan Artifisial, Kajian Matematika Sekolah Internasional.\nSemester 7 (PPL): Asistensi Mengajar di Satuan Pendidikan, Pengenalan Lapangan Persekolahan, Penyusunan Perangkat Pembelajaran, Pengembangan Media Pembelajaran, Praktik Mengajar, Penelitian Tindakan Kelas, Laporan dan Presentasi Hasil PPL.\nSemester 8: Skripsi.",
"kurikulum pendidikan matematika,semester pendidikan matematika,mata kuliah pendidikan matematika"},
{"kurikulum_s2_manajemen","kurikulum","Kurikulum S2 Manajemen (Magister Manajemen)",
"Semester 1: Human Capital Management, Business Development Strategy, Business Research & Statistics, Digital Marketing Strategy & Innovation, Mata Kuliah Konsentrasi 1.\nSemester 2: Global Business Strategy, Mature Leadership in Digital Era, Business Practice Project, AI for Business Decision, Academic Writing & Publication.\nSemester 3: Strategic Management & Leadership, Entrepreneurship & Business Development, Digital Transformation & Management Innovation (mata kuliah wajib sesuai konsentrasi yang dipilih).\nSemester 4: Thesis, Paper Publication.\nKonsentrasi yang tersedia: Strategic Management & Leadership, Entrepreneurship & Business Development, Digital Transformation & Management Innovation. Pilihan Mata Kuliah Konsentrasi: Advanced Business Strategy, Entrepreneurial Strategy & Business Opportunity, Digital Transformation Frameworks.",
"kurikulum s2 manajemen,kurikulum magister manajemen,semester s2,mata kuliah s2 manajemen"},
{"id_prodi_ti_karir","prospek_karir","Prospek Karir S1 Teknik Informatika",
"Software Engineer, Front End Web Developer, Back End Web Developer, Full Stack Developer, Mobile Application Developer, UI/UX Designer, Data Scientist / ML Engineer, IoT Engineer, Pengembangan Sistem Berbasis AI, IT Officer, Network Administrator, Digital Entrepreneur, Cyber Security Specialist",
"teknik informatika,karir,prospek kerja,lulusan ti,software engineer,data scientist,cyber security"},
{"id_prodi_ti_unggulan","matakuliah_unggulan","Mata Kuliah Unggulan S1 Teknik Informatika",
"Pemrograman Internet, Mobile Programming, Artificial Intelligence, Internet of Things, Machine Learning, UI/UX Designer, Computer Vision and NLP",
"teknik informatika,mata kuliah unggulan,ai,iot,mobile programming"},
{"id_kurikulum_ti_sem1","kurikulum","Kurikulum S1 Teknik Informatika Semester 1",
"Pancasila dan Kewarganegaraan, Bahasa Indonesia, Pembangunan Karakter dan Etika Profesi, General English, Pemrograman Internet, Aljabar Linear dan Matriks, Arsitektur dan Organisasi Komputer, Algoritma dan Pemrograman",
"kurikulum,teknik informatika,semester 1,mata kuliah"},
{"id_kurikulum_ti_sem2","kurikulum","Kurikulum S1 Teknik Informatika Semester 2",
"Pendidikan Agama, Intermediate English, Sistem Basis Data, Komunikasi Data dan Jaringan, Rekayasa Aplikasi Internet, Teknik Multimedia Digital, Sistem Operasi, Struktur Data",
"kurikulum,teknik informatika,semester 2,mata kuliah"},
{"id_kurikulum_ti_sem3","kurikulum","Kurikulum S1 Teknik Informatika Semester 3",
"English for Specific Purpose, Statistik for Data Science, Pemrograman Berorientasi Objek, Administrasi Jaringan, Analisis dan Desain Perangkat Lunak, UI/UX Designer, Kecerdasan Buatan, Manajemen Data & Informasi",
"kurikulum,teknik informatika,semester 3,mata kuliah"},
{"id_kurikulum_ti_sem4","kurikulum","Kurikulum S1 Teknik Informatika Semester 4",
"TOEFL Preparation, Mobile Programming, Keamanan Sistem Informasi dan Jaringan, Sistem Digital, Data Mining, Sistem Penunjang Keputusan, Grafika Komputer & Pengolahan Citra Digital, Sistem Pakar (Mata Kuliah Peminatan), Sistem Terdistribusi, Pengujian dan Pemeliharaan Sistem",
"kurikulum,teknik informatika,semester 4,mata kuliah"},
{"id_kurikulum_ti_sem5","kurikulum","Kurikulum S1 Teknik Informatika Semester 5",
"Student Exchange, Technopreneurship, Pengembangan Aplikasi Mobile, Machine Learning, Internet of Things, Computer Vision and NLP, Game Cerdas, Jaminan Kualitas Perangkat Lunak, Manajemen Proyek Perangkat Lunak",
"kurikulum,teknik informatika,semester 5,mata kuliah"},
{"id_kurikulum_ti_sem6","kurikulum","Kurikulum S1 Teknik Informatika Semester 6",
"Student Exchange, Kapita Selekta, Proyek Technopreneurship, Realitas Virtual dan Augmentasi, Metodologi Penelitian & Riset Pustaka, Proyek Teknologi Informasi, KKN Tematik, MBKM (Studi Independen Bersertifikat), Magang (Studi Independen Mandiri)",
"kurikulum,teknik informatika,semester 6,mata kuliah"},
{"id_kurikulum_ti_sem7","kurikulum","Kurikulum S1 Teknik Informatika Semester 7",
"MBKM (Magang Bersertifikat), Magang Internship Mandiri",
"kurikulum,teknik informatika,semester 7,magang,mbkm"},
{"id_kurikulum_ti_sem8","kurikulum","Kurikulum S1 Teknik Informatika Semester 8",
"Skripsi",
"kurikulum,teknik informatika,semester 8,skripsi"},
{"id_prodi_si_karir","prospek_karir","Prospek Karir S1 Sistem Informasi",
"Application Engineer, Digital Marketing, Research and Development (R&D), Data Scientist, Software Developer, System Analyst, Quality Software System, System Designer, Business Application Developer, e-Business Manager",
"sistem informasi,karir,prospek kerja,system analyst,business application,digital marketing"},
{"id_prodi_si_unggulan","matakuliah_unggulan","Mata Kuliah Unggulan S1 Sistem Informasi",
"Audit Sistem Informasi, Enterprise Resource Planning (ERP), Proyek Technopreneurship, E-CRM, Agile Development, UI/UX Design, Business Intelligence",
"sistem informasi,mata kuliah unggulan,erp,audit si,agile"},
{"id_kurikulum_si_sem1","kurikulum","Kurikulum S1 Sistem Informasi Semester 1",
"Pancasila dan Kewarganegaraan, Bahasa Indonesia, Pembangunan Karakter dan Etika Profesi, General English, Matematika Bisnis, Manajemen dan Organisasi, Sistem dan Teknologi Informasi, Algoritma dan Pemrograman",
"kurikulum,sistem informasi,semester 1,mata kuliah"},
{"id_kurikulum_si_sem2","kurikulum","Kurikulum S1 Sistem Informasi Semester 2",
"Pendidikan Agama, Teknologi dan Tren E-commerce, Intermediate English, Arsitektur SI/TI Perusahaan, Tata Kelola Teknologi Informasi, Analisa Proses Bisnis, Pemrograman Web (HTML-CSS), Algoritma dan Pemrograman 2",
"kurikulum,sistem informasi,semester 2,mata kuliah"},
{"id_kurikulum_si_sem3","kurikulum","Kurikulum S1 Sistem Informasi Semester 3",
"Statistik, Sistem Informasi Manajemen, English for Specific Purpose, Interaksi Manusia dan Komputer, Analisa dan Perancangan Sistem Informasi, Sistem Basis Data, Pemrograman Internet",
"kurikulum,sistem informasi,semester 3,mata kuliah"},
{"id_kurikulum_si_sem4","kurikulum","Kurikulum S1 Sistem Informasi Semester 4",
"TOEFL Preparation, UI/UX Design, Rekayasa Perangkat Lunak: Agile Scrum Introduction, Analisa Sistem Berorientasi Objek, Pemrograman Internet Intermediate (Framework, MVC), Mata Kuliah Pilihan 1, Mata Kuliah Pilihan 2",
"kurikulum,sistem informasi,semester 4,mata kuliah"},
{"id_kurikulum_si_sem5","kurikulum","Kurikulum S1 Sistem Informasi Semester 5",
"Pengembangan Aplikasi Mobile, Student Exchange (Agile Implementation), Jaringan Komputer, Technopreneurship, Rekayasa Perangkat Lunak, Pemrograman Internet Intermediate, Mata Kuliah Pilihan 1, Mata Kuliah Pilihan 2",
"kurikulum,sistem informasi,semester 5,mata kuliah"},
{"id_kurikulum_si_sem6","kurikulum","Kurikulum S1 Sistem Informasi Semester 6",
"Student Exchange (Audit SI), Student Exchange (Interaksi Manusia & Komputer), Student Exchange (Audio Visual Effects), Metodologi Penelitian dan Riset Pustaka, Proyek Technopreneurship, Proyek Sistem Informasi, Kapita Selekta, Mata Kuliah Pilihan 1",
"kurikulum,sistem informasi,semester 6,mata kuliah"},
{"id_kurikulum_si_sem7","kurikulum","Kurikulum S1 Sistem Informasi Semester 7",
"Magang Internship Mandiri, Magang MBKM (Magang Bersertifikat), KKN Tematik",
"kurikulum,sistem informasi,semester 7,magang,kkn"},
{"id_kurikulum_si_sem8","kurikulum","Kurikulum S1 Sistem Informasi Semester 8",
"Skripsi",
"kurikulum,sistem informasi,semester 8,skripsi"},
{"id_prodi_dkv_karir","prospek_karir","Prospek Karir S1 Desain Komunikasi Visual",
"Desainer Grafis, UI/UX Designer, Ilustrator, Desainer Komik, Fotografer/Videografer, Desainer Kemasan, Desainer Identitas Visual, Spesialis Media Sosial, Periklanan dan Pemasaran Visual, Produser Konten Digital, Pendidik dan Instruktur, Kewirausahaan, Editor Grafis, Desainer Seni dan Hiburan, Digital Marketing",
"dkv,desain komunikasi visual,karir,prospek kerja,graphic designer,illustrator"},
{"id_prodi_dkv_unggulan","matakuliah_unggulan","Mata Kuliah Unggulan S1 Desain Komunikasi Visual",
"Ilustrasi, Fotografi, Komik, 3D Animasi, Media Interaktif, Desain Komunikasi Visual Identitas, Creative Content Advertising",
"dkv,mata kuliah unggulan,ilustrasi,fotografi,animasi 3d"},
{"id_kurikulum_dkv_sem1","kurikulum","Kurikulum S1 Desain Komunikasi Visual Semester 1",
"Pancasila dan Kewarganegaraan, Bahasa Indonesia, Pembangunan Karakter dan Etika Profesi, General English, Dasar Desain Grafis, Basic Tipografi, Bahasa Rupa, Computer Graphic for Graphic Design, Estetika Visual Desain",
"kurikulum,dkv,semester 1,mata kuliah"},
{"id_kurikulum_dkv_sem2","kurikulum","Kurikulum S1 Desain Komunikasi Visual Semester 2",
"Pendidikan Agama, Teknologi dan Tren E-commerce, Intermediate English, Sosiologi Desain, Design Thinking, Kinetic Tipografi, Metoda Produksi Grafika, Komik Digital",
"kurikulum,dkv,semester 2,mata kuliah"},
{"id_kurikulum_dkv_sem3","kurikulum","Kurikulum S1 Desain Komunikasi Visual Semester 3",
"English for Specific Purpose, 3D Artist, Desain Komunikasi Visual Identitas, Basic Fotografi, Advertising Copywriting, Semiotika Visual, Web Design, Tinjauan dan Kritik Desain",
"kurikulum,dkv,semester 3,mata kuliah"},
{"id_kurikulum_dkv_sem4","kurikulum","Kurikulum S1 Desain Komunikasi Visual Semester 4",
"TOEFL Preparation, Desain Komunikasi Visual Promosi, Applied Photography, Audio Visual Effects, Packaging Design, Media Interaktif, Animasi 3D, Creative Content Advertising, Inovasi Grafis",
"kurikulum,dkv,semester 4,mata kuliah"},
{"id_kurikulum_dkv_sem5","kurikulum","Kurikulum S1 Desain Komunikasi Visual Semester 5",
"Technopreneurship, Desain Komunikasi Visual Sosial, Metodologi Penelitian Desain, Manajemen Desain & Creativepreneur, Game Desain, UI/UX Designer, Visual Merchandising, Video Brand Production, Student Exchange (Fotografi)",
"kurikulum,dkv,semester 5,mata kuliah"},
{"id_kurikulum_dkv_sem6","kurikulum","Kurikulum S1 Desain Komunikasi Visual Semester 6",
"Kapita Selekta, Proyek Technopreneurship, Strategi Konten Digital, Psikologi Persepsi, Project DKV, MBKM (Studi Independen Bersertifikat)",
"kurikulum,dkv,semester 6,mata kuliah"},
{"id_kurikulum_dkv_sem7","kurikulum","Kurikulum S1 Desain Komunikasi Visual Semester 7",
"Penguasaan Teknologi (MBKM), Tata Tulis Karya Ilmiah (MBKM), Etika Profesi Kerja (MBKM), Komunikasi dan Teamwork (MBKM), Ide, Inisiatif dan Tanggung Jawab (MBKM), KKN Tematik",
"kurikulum,dkv,semester 7,magang,kkn"},
{"id_kurikulum_dkv_sem8","kurikulum","Kurikulum S1 Desain Komunikasi Visual Semester 8",
"Skripsi",
"kurikulum,dkv,semester 8,skripsi"},
{"id_prodi_d3mi_karir","prospek_karir","Prospek Karir D3 Manajemen Informatika",
"Web Designer, Content Creator, Software Developer, Pengajar Teknologi Informasi, Network Administrator, IT Consultant",
"d3 manajemen informatika,karir,prospek kerja,web designer,content creator"},
{"id_prodi_d3mi_unggulan","matakuliah_unggulan","Mata Kuliah Unggulan D3 Manajemen Informatika",
"Pemrograman Web, Digital Content Marketing, Fotografi Digital, UI/UX Design, Kecerdasan Buatan",
"d3 manajemen informatika,mata kuliah unggulan,web programming,digital marketing"},
{"id_kurikulum_d3mi_sem1","kurikulum","Kurikulum D3 Manajemen Informatika Semester 1",
"Pancasila dan Kewarganegaraan, Bahasa Indonesia, Pembangunan Karakter dan Etika Profesi, General English, Pengantar Media Digital, Komputer Grafis 1, Komunikasi dan Presentasi Dasar, AI Technician",
"kurikulum,d3 manajemen informatika,semester 1,mata kuliah"},
{"id_kurikulum_d3mi_sem2","kurikulum","Kurikulum D3 Manajemen Informatika Semester 2",
"Pendidikan Agama, Intermediate English, Design Thinking, Fotografi Digital, Komputer Grafis 2 (Data Visualization), Statistik Informatika, Digital Copywriting",
"kurikulum,d3 manajemen informatika,semester 2,mata kuliah"},
{"id_kurikulum_d3mi_sem3","kurikulum","Kurikulum D3 Manajemen Informatika Semester 3",
"English for Specific Purpose, Technopreneurship, Videografi, Brand Identity (Corporate & Personal Branding), Manajemen Media Sosial, Visual Storytelling, Digital Content Marketing",
"kurikulum,d3 manajemen informatika,semester 3,mata kuliah"},
{"id_kurikulum_d3mi_sem4","kurikulum","Kurikulum D3 Manajemen Informatika Semester 4",
"TOEFL Preparation, Proyek Technopreneurship, Fotografi dan Videografi Lanjutan, Digital Bisnis (Startup), Integrated Marketing Communication, Digital Content Interaktif, Monetisasi Konten Digital, Mata Kuliah Pilihan",
"kurikulum,d3 manajemen informatika,semester 4,mata kuliah"},
{"id_kurikulum_d3mi_sem5","kurikulum","Kurikulum D3 Manajemen Informatika Semester 5",
"Tren dan Eksplorasi Content Digital, E-Commerce, UI/UX Design, Proyek Manajemen Informatika, Etika dan Regulasi Media Digital, Sertifikasi Kompetensi & HAKI Produk Digital, Manajemen Proyek Media Digital",
"kurikulum,d3 manajemen informatika,semester 5,mata kuliah"},
{"id_kurikulum_d3mi_sem6","kurikulum","Kurikulum D3 Manajemen Informatika Semester 6",
"Tugas Akhir, Kapita Selekta, Tata Tulis Karya Ilmiah, Influencer Networking & Collaboration, Kampanye Konten Digital, Optimasi Content Digital, Mata Kuliah Pilihan",
"kurikulum,d3 manajemen informatika,semester 6,mata kuliah"},
{"id_prodi_manajemen_karir","prospek_karir","Prospek Karir S1 Manajemen",
"Account Officer, Bagian Operasional dan Logistik, Product Manager, Market Researcher, Event Planner/Event Organizer, Wirausaha, CEO, Direktur, Manajer, Credit Analyst, Treasury Supervisor, Head of Operations Business",
"manajemen,karir,prospek kerja,manager,entrepreneur,business"},
{"id_prodi_manajemen_unggulan","matakuliah_unggulan","Mata Kuliah Unggulan S1 Manajemen",
"Manajemen Keuangan, Manajemen Operasional, Manajemen Pemasaran, Manajemen Sumber Daya Manusia",
"manajemen,mata kuliah unggulan,keuangan,pemasaran,sdm"},
{"id_kurikulum_manajemen_sem1","kurikulum","Kurikulum S1 Manajemen Semester 1",
"Pendidikan Agama, Pancasila dan Kewarganegaraan, General English, Bahasa Indonesia, Ekonomi Mikro, Akuntansi Keuangan Dasar, Teknologi Informasi untuk Bisnis, Manajemen & Bisnis Dasar",
"kurikulum,manajemen,semester 1,mata kuliah"},
{"id_kurikulum_manajemen_sem2","kurikulum","Kurikulum S1 Manajemen Semester 2",
"Pengembangan Karakter, Manajemen Pemasaran, Matematika Ekonomi dan Bisnis, Hukum Bisnis, Komunikasi Bisnis dan Negosiasi, Perilaku Konsumen, Ekonomi Makro, Sistem Informasi Manajemen",
"kurikulum,manajemen,semester 2,mata kuliah"},
{"id_kurikulum_manajemen_sem3","kurikulum","Kurikulum S1 Manajemen Semester 3",
"Manajemen Sumber Daya Manusia, Digital Marketing, Manajemen Keuangan, Manajemen Operasi, Technopreneurship, Akuntansi Biaya, Big Data Analytics",
"kurikulum,manajemen,semester 3,mata kuliah"},
{"id_kurikulum_manajemen_sem4","kurikulum","Kurikulum S1 Manajemen Semester 4",
"English for Business, Manajemen Proyek Teknologi Informasi, Manajemen Anggaran, Perilaku Organisasional, Technopreneurship Lanjutan, Manajemen Strategik, Teori Portofolio dan Analisis Investasi",
"kurikulum,manajemen,semester 4,mata kuliah"},
{"id_kurikulum_manajemen_sem5","kurikulum","Kurikulum S1 Manajemen Semester 5",
"Manajemen Inovasi Berbasis Teknologi, Bisnis Digital, Statistika dan Analisis Bisnis, Metodologi Penelitian, Manajemen Transformasi Digital, Analisa Laporan Keuangan (Student Exchange), Mata Kuliah Pilihan",
"kurikulum,manajemen,semester 5,mata kuliah"},
{"id_kurikulum_manajemen_sem6","kurikulum","Kurikulum S1 Manajemen Semester 6",
"E-Commerce & Platform, Etika Profesi Bisnis, Seminar Manajemen, Metode Pengambilan Keputusan (Student Exchange), Kapita Selekta, Mata Kuliah Pilihan",
"kurikulum,manajemen,semester 6,mata kuliah"},
{"id_kurikulum_manajemen_sem7","kurikulum","Kurikulum S1 Manajemen Semester 7",
"Magang Studi Independen, Kepemimpinan Bisnis, Kepemimpinan Entrepreneur, Kemampuan Kerja Tim, Literasi Digital, Literasi Data",
"kurikulum,manajemen,semester 7,magang"},
{"id_kurikulum_manajemen_sem8","kurikulum","Kurikulum S1 Manajemen Semester 8",
"Skripsi",
"kurikulum,manajemen,semester 8,skripsi"},
{"id_prodi_bisnisdigital_karir","prospek_karir","Prospek Karir S1 Bisnis Digital",
"Technopreneur, Digital Marketing Manager, Social Media Marketing Manager, Data Intelligence Analyst, e-Commerce Practitioner, Fintech Practitioner, Startup Consultant, Startup Ecosystem Team, Financial Planner",
"bisnis digital,karir,prospek kerja,digital marketing,startup,e commerce"},
{"id_prodi_bisnisdigital_unggulan","matakuliah_unggulan","Mata Kuliah Unggulan S1 Bisnis Digital",
"Technopreneurship, Financial Technology, Model Bisnis UMKM, Analisis Bisnis Digital, Startup Business Model",
"bisnis digital,mata kuliah unggulan,fintech,startup,umkm"},
{"id_kurikulum_bisnisdigital_sem1","kurikulum","Kurikulum S1 Bisnis Digital Semester 1",
"Bahasa Indonesia, Pancasila dan Kewarganegaraan, Pengembangan Karakter & Etika Profesi, General English, Ekonomika Bisnis, Etika dan Komunikasi Bisnis, Konsep dan Ide Bisnis Digital, Manajemen Bisnis",
"kurikulum,bisnis digital,semester 1,mata kuliah"},
{"id_kurikulum_bisnisdigital_sem2","kurikulum","Kurikulum S1 Bisnis Digital Semester 2",
"Pendidikan Agama, Graphic Design (Video Animation), Financial Management, Web Design, Search Engine Optimization (SEO), Statistika Bisnis, Digital Marketing, Management Information System",
"kurikulum,bisnis digital,semester 2,mata kuliah"},
{"id_kurikulum_bisnisdigital_sem3","kurikulum","Kurikulum S1 Bisnis Digital Semester 3",
"Technopreneurship, Financial Technology, Model Bisnis UMKM (K1), Analisis Bisnis Digital (K2), Startup Business Model (K3), Analisa Investasi Bisnis & Portfolio, Metodologi Penelitian Bisnis, Studi Kelayakan Bisnis",
"kurikulum,bisnis digital,semester 3,mata kuliah"},
{"id_kurikulum_bisnisdigital_sem4","kurikulum","Kurikulum S1 Bisnis Digital Semester 4",
"Project Technopreneurship, English for Business, e-Customer Relationship Management, Academic Writing and Publication, Customer Behaviour, Integrated Marketing Communication (IMC)",
"kurikulum,bisnis digital,semester 4,mata kuliah"},
{"id_kurikulum_bisnisdigital_sem5","kurikulum","Kurikulum S1 Bisnis Digital Semester 5",
"Business Intelligence, Digital Business Platform, Design Thinking Method, Supply Chain Management, Student Exchange: Fotografi (DKV)",
"kurikulum,bisnis digital,semester 5,mata kuliah"},
{"id_kurikulum_bisnisdigital_sem6","kurikulum","Kurikulum S1 Bisnis Digital Semester 6",
"Digital Business Development (Project) - Digital MSMEs-Preneur, Digital Business Development (Project) - Digital Business Consultant, Digital Business Development (Project) - Digital Business Startup, Crowdfunding & Fintech Ecosystem, Enterprise Resources Planning, Kapita Selekta, Writing Business Report",
"kurikulum,bisnis digital,semester 6,mata kuliah"},
{"id_kurikulum_bisnisdigital_sem7","kurikulum","Kurikulum S1 Bisnis Digital Semester 7",
"Digital Business Development (Project), MBKM (Kewirausahaan/SIB/Penelitian/PKM)",
"kurikulum,bisnis digital,semester 7,mbkm"},
{"id_kurikulum_bisnisdigital_sem8","kurikulum","Kurikulum S1 Bisnis Digital Semester 8",
"Skripsi",
"kurikulum,bisnis digital,semester 8,skripsi"},
{"id_prodi_akuntansi_karir","prospek_karir","Prospek Karir S1 Akuntansi",
"Auditor Internal, Auditor Eksternal, Accounting Staff, Konsultan Keuangan, Data Analyst, Konsultan Perpajakan",
"akuntansi,karir,prospek kerja,auditor,pajak,keuangan"},
{"id_prodi_akuntansi_unggulan","matakuliah_unggulan","Mata Kuliah Unggulan S1 Akuntansi",
"Akuntansi Keuangan, Auditing, Sistem Informasi Akuntansi, Perpajakan",
"akuntansi,mata kuliah unggulan,auditing,pajak,sistem informasi akuntansi"},
{"id_kurikulum_akuntansi_sem1","kurikulum","Kurikulum S1 Akuntansi Semester 1",
"Akuntansi Keuangan Dasar, Ekonomi Mikro, General English, Manajemen & Bisnis Dasar, Pancasila & Kewarganegaraan, Pendidikan Agama, Teknologi Informasi untuk Bisnis, Bahasa Indonesia",
"kurikulum,akuntansi,semester 1,mata kuliah"},
{"id_kurikulum_akuntansi_sem2","kurikulum","Kurikulum S1 Akuntansi Semester 2",
"Ekonomi Makro, Hukum Bisnis, Komunikasi Bisnis & Negosiasi, Matematika Ekonomi dan Bisnis, Pengembangan Karakter, Praktikum Akuntansi Keuangan, Akuntansi Keuangan Menengah, Akuntansi Biaya",
"kurikulum,akuntansi,semester 2,mata kuliah"},
{"id_kurikulum_akuntansi_sem3","kurikulum","Kurikulum S1 Akuntansi Semester 3",
"Akuntansi Manajemen, Hukum Perpajakan, Manajemen Keuangan, Sistem Informasi Akuntansi, Technopreneurship, Akuntansi Sektor Publik, Akuntansi Keuangan Lanjutan",
"kurikulum,akuntansi,semester 3,mata kuliah"},
{"id_kurikulum_akuntansi_sem4","kurikulum","Kurikulum S1 Akuntansi Semester 4",
"Akuntansi Entitas Mikro, Kecil dan Menengah, Akuntansi Internasional, Auditing 1, Technopreneurship Lanjutan, Teknologi Keuangan, Akuntansi Perpajakan, English for Business, Bisnis Digital (Student Exchange Prodi Manajemen)",
"kurikulum,akuntansi,semester 4,mata kuliah"},
{"id_kurikulum_akuntansi_sem5","kurikulum","Kurikulum S1 Akuntansi Semester 5",
"Metodologi Penelitian, Akuntansi Syariah, Analisis Laporan Keuangan, Auditing 2, Statistika dan Analisis Bisnis, Praktikum Perpajakan, Agile Implementation (Student Exchange Prodi SI)",
"kurikulum,akuntansi,semester 5,mata kuliah"},
{"id_kurikulum_akuntansi_sem6","kurikulum","Kurikulum S1 Akuntansi Semester 6",
"Seminar Akuntansi, Teori Akuntansi, Kapita Selekta, Teori Portofolio dan Analisis Investasi, Etika Profesi Akuntan, Praktikum Auditing, Mata Kuliah Pilihan 1 (Lintas Prodi UCIC)",
"kurikulum,akuntansi,semester 6,mata kuliah"},
{"id_kurikulum_akuntansi_sem7","kurikulum","Kurikulum S1 Akuntansi Semester 7",
"KKN Tematik, Penatausahaan Dokumen Perkantoran, Pengoperasian Aplikasi dan Alat Bantu Perkantoran, Mengolah Data Perkantoran, Kreativitas dan Inisiatif, Etika Profesi Kerja, Komunikasi dan Teamwork, Magang",
"kurikulum,akuntansi,semester 7,kkn,magang"},
{"id_kurikulum_akuntansi_sem8","kurikulum","Kurikulum S1 Akuntansi Semester 8",
"Skripsi",
"kurikulum,akuntansi,semester 8,skripsi"},
{"id_prodi_pkor_karir","prospek_karir","Prospek Karir S1 Pendidikan Kepelatihan Keolahragaan",
"Guru Olahraga, Pelatih Profesional, Wasit/Juri Profesional, Sport Entrepreneur",
"pkor,pikor,pendidikan kepelatihan keolahragaan,pendidikan kepelatihan olahraga,karir,prospek kerja,pelatih,guru olahraga"},
{"id_prodi_pkor_unggulan","matakuliah_unggulan","Mata Kuliah Unggulan S1 Pendidikan Kepelatihan Keolahragaan",
"Ilmu Kepelatihan Olahraga, Biomekanika Olahraga, Psikologi Olahraga, Fisiologi Latihan dan Kinerja Atlet, Manajemen Kepelatihan Olahraga",
"pkor,pikor,pendidikan kepelatihan keolahragaan,mata kuliah unggulan,kepelatihan,biomekanika,psikologi olahraga"},
{"id_kurikulum_pkor_sem1","kurikulum","Kurikulum S1 Pendidikan Kepelatihan Keolahragaan Semester 1",
"Bahasa Indonesia, Pancasila dan Kewarganegaraan, General English, Pengembangan Karakter, Anatomi, Pembelajaran Motorik, Pelatihan Cabor Atletik, Pelatihan Cabor Renang, Pelatihan Cabor Futsal",
"kurikulum,pkor,pikor,semester 1,mata kuliah"},
{"id_kurikulum_pkor_sem2","kurikulum","Kurikulum S1 Pendidikan Kepelatihan Keolahragaan Semester 2",
"Pendidikan Agama, Fisiologi Olahraga, Manajemen dan Administrasi Olahraga, Pelatihan Cabor Sepak Bola, Pelatihan Cabor Basket, Pelatihan Cabor Voli, Pelatihan Cabor Taekwondo, English for Education, Landasan Pendidikan, Kurikulum dan Pembelajaran",
"kurikulum,pkor,pikor,semester 2,mata kuliah"},
{"id_kurikulum_pkor_sem3","kurikulum","Kurikulum S1 Pendidikan Kepelatihan Keolahragaan Semester 3",
"Technopreneurship, English for Sport, Bimbingan Konseling, Teori Belajar dan Pembelajaran, Pelatihan Cabor Pencak Silat, Pelatihan Cabor Karate, Pengantar Ilmu Keolahragaan, Perwasitan Olahraga, Teori dan Metodologi Kepelatihan Olahraga, Manajemen Pertandingan/Perlombaan",
"kurikulum,pkor,pikor,semester 3,mata kuliah"},
{"id_kurikulum_pkor_sem4","kurikulum","Kurikulum S1 Pendidikan Kepelatihan Keolahragaan Semester 4",
"Biomekanika Olahraga, Ilmu Gizi Olahraga, Teknologi Kepelatihan Olahraga, Analisis Performa Olahraga Berbasis AI, Technopreneurship Lanjutan, Kurikulum dan Pembelajaran, Sport Pedagogy",
"kurikulum,pkor,pikor,semester 4,mata kuliah"},
{"id_kurikulum_pkor_sem5","kurikulum","Kurikulum S1 Pendidikan Kepelatihan Keolahragaan Semester 5",
"Metode Penelitian Pendidikan Olahraga, Manajemen Kepelatihan Olahraga, Didaktik Metodik Olahraga, Psikologi Olahraga, Evaluasi Kepelatihan Olahraga, Kinesiologi dan Massage Olahraga",
"kurikulum,pkor,pikor,semester 5,mata kuliah"},
{"id_kurikulum_pkor_sem6","kurikulum","Kurikulum S1 Pendidikan Kepelatihan Keolahragaan Semester 6",
"Microteaching, Kapita Selekta, Statistika, Pemandu Bakat Olahraga, Seminar Magang Kepelatihan Olahraga, Evaluasi dan Sistem Organisasi Pertandingan, Pemasaran Olahraga, Pengembangan Program Latihan Berbasis Data",
"kurikulum,pkor,pikor,semester 6,mata kuliah"},
{"id_kurikulum_pkor_sem7","kurikulum","Kurikulum S1 Pendidikan Kepelatihan Keolahragaan Semester 7",
"MBKM, Profesi Kependidikan, Praktek Lapangan Persekolahan, Sarana dan Prasarana Olahraga, Tes dan Pengukuran Olahraga, Strategi dan Taktik Olahraga Berbasis Digital, Pencegahan dan Perawatan Cedera Olahraga",
"kurikulum,pkor,pikor,semester 7,mbkm"},
{"id_kurikulum_pkor_sem8","kurikulum","Kurikulum S1 Pendidikan Kepelatihan Keolahragaan Semester 8",
"Skripsi",
"kurikulum,pkor,pikor,semester 8,skripsi"},
{"id_prodi_d3mb_karir","prospek_karir","Prospek Karir D3 Manajemen Bisnis",
"E-Commerce Specialist, Media Social Specialist, Marketing Staff, Technopreneur",
"d3 manajemen bisnis,karir,prospek kerja,marketing,e commerce"},
{"id_prodi_d3mb_unggulan","matakuliah_unggulan","Mata Kuliah Unggulan D3 Manajemen Bisnis",
"Manajemen Bisnis Ritel, Manajemen Tataletak & Planogram, E-Commerce, Technopreneurship, Manajemen Logistik dan Supply Chain",
"d3 manajemen bisnis,mata kuliah unggulan,ritel,e commerce,supply chain"},
{"id_kurikulum_d3mb_sem1","kurikulum","Kurikulum D3 Manajemen Bisnis Semester 1",
"Pembangunan Karakter dan Etika Profesi, Pancasila, Kewarganegaraan, dan Anti Korupsi, General English, Bahasa Indonesia, Pengantar Akuntansi, Teknologi Informasi untuk Bisnis, Manajemen & Bisnis Dasar, Pengantar Ilmu Ekonomi Bisnis",
"kurikulum,d3 manajemen bisnis,semester 1,mata kuliah"},
{"id_kurikulum_d3mb_sem2","kurikulum","Kurikulum D3 Manajemen Bisnis Semester 2",
"Intermediate English, Pendidikan Agama, Matematika Ekonomi dan Bisnis, Dasar-Dasar Perpajakan, Perilaku Konsumen, Manajemen Tataletak & Planogram, Pengelolaan Keuangan Ritel",
"kurikulum,d3 manajemen bisnis,semester 2,mata kuliah"},
{"id_kurikulum_d3mb_sem3","kurikulum","Kurikulum D3 Manajemen Bisnis Semester 3",
"English for Specific Purpose, Technopreneurship, Sistem Akuntansi, Akuntansi Keuangan Bisnis, Manajemen Sumber Daya Manusia, Praktikum Akuntansi Manual, Manajemen Logistik dan Supply Chain, Manajemen Hubungan Pelanggan, Hukum Bisnis",
"kurikulum,d3 manajemen bisnis,semester 3,mata kuliah"},
{"id_kurikulum_d3mb_sem4","kurikulum","Kurikulum D3 Manajemen Bisnis Semester 4",
"Technopreneurship Lanjutan, TOEFL Preparation, Metodologi Penelitian Bisnis, Penganggaran Perusahaan, Aplikasi Software Akuntansi, Studi Kelayakan Bisnis, Manajemen Kas dan Portofolio, Strategi Pemasaran Ritel, Manajemen Kualitas",
"kurikulum,d3 manajemen bisnis,semester 4,mata kuliah"},
{"id_kurikulum_d3mb_sem5","kurikulum","Kurikulum D3 Manajemen Bisnis Semester 5",
"Kepemimpinan, Kemampuan Bekerjasama, Kemampuan Berkomunikasi, Kreativitas, Kemampuan Administrasi, Kemampuan Merumuskan Masalah Teknis, Seminar Bisnis",
"kurikulum,d3 manajemen bisnis,semester 5,mata kuliah"},
{"id_kurikulum_d3mb_sem6","kurikulum","Kurikulum D3 Manajemen Bisnis Semester 6",
"Kapita Selekta, Tugas Akhir, Komunikasi Bisnis",
"kurikulum,d3 manajemen bisnis,semester 6,mata kuliah"},

// ===== DOSEN =====
{"dosen_ti","dosen","Dosen Tetap Program Studi Teknik Informatika (TI)",
"Dosen tetap Program Studi Teknik Informatika UCIC: Marsani Asfi, S.Si., M.Si.; Muhammad Hatta, S.Kom., M.Kom.; Hilman Lesmana, S.Kom., M.SF.; Kusnadi, S.Kom., M.Kom.; Petrus Sokibi Sukanto, S.Kom., M.Kom.; Ridho Taufiq Subagio, S.T., M.Kom.; Rinaldi Adam, S.T., M.Comp.; dr. Thomas Adhi Nugroho C., S.Kom., M.Kom.; Arif Nursetyo, S.Kom., M.Kom.; Victor Asih, S.Si., M.T.; Verina Tita Nabila, S.Si., M.Kom.",
"dosen teknik informatika,dosen ti,pengajar ti,dosen fakultas teknologi informasi ti"},
{"dosen_si","dosen","Dosen Tetap Program Studi Sistem Informasi (SI)",
"Dosen tetap Program Studi Sistem Informasi UCIC: Lena Magdalena, S.Kom., M.MSI.; Mesi Febima, S.Kom., M.Kom.; Moh Umar Fakhrudin, S.Pd., M.Pd.; Unang Solihin, S.Kom., M.Kom.; Bambang Sugianto, S.Kom., M.Kom.",
"dosen sistem informasi,dosen si,pengajar si"},
{"dosen_dkv","dosen","Dosen Tetap Program Studi Desain Komunikasi Visual (DKV)",
"Dosen tetap Program Studi DKV UCIC: Dzulfiqar Fickri Rosyid, S.Ds., M.Ds.; Galih Mandala Putra, S.Sn., M.Ds., CDM.; Ine Rachmawati, S.Sn., M.Sn.; Iwan Kurnia, S.Pd., M.Ds.; Tiara Khodijah, S.Ds., M.Ds.; Yuni Awalaturrohmah Solihah, S.Pd., M.Pd.",
"dosen desain komunikasi visual,dosen dkv,pengajar dkv"},
{"dosen_mi","dosen","Dosen Tetap Program Studi Manajemen Informatika (D3 MI)",
"Dosen tetap Program Studi D3 Manajemen Informatika UCIC: Agus Sevtiana, S.T., M.Kom.; Ricky Perdana Kusuma, S.T., M.Kom.; Muhammad Afif Sulhan, S.K.G., M.Kom.; Wiwiek Nurkomala Dewi, S.T., M.M.",
"dosen manajemen informatika,dosen d3 mi,pengajar mi"},
{"dosen_mgt","dosen","Dosen Tetap Program Studi Manajemen (MGT)",
"Dosen tetap Program Studi Manajemen UCIC: Aditya Kurniawan Chandra, B.Com., M.M. (Warek II Bidang Keuangan); Suwandi, S.E., M.M.; Turini, S.E., M.M.; Dewi Laily Purnamasari, S.T., M.M.; Dr. Taufan Hunneman, S.H., M.H., M.Th., CLA, CTL, CPHRM, CSSS; Fery Fauzi, S.E., M.M.; Mualif Ismail, S.M., M.M.; Sudadi Pranata, S.E., M.Si.",
"dosen manajemen,dosen mgt,pengajar manajemen"},
{"dosen_akt","dosen","Dosen Tetap Program Studi Akuntansi (AKT)",
"Dosen tetap Program Studi Akuntansi UCIC: Aan Kanivia, S.E., M.Ak., CTA., CAP., CTT., CPA; Agung Supriyadi, S.E., M.Ak.; Dessy Kumala Dewi, S.E., M.Ak.; Dewi Anggun, S.E., M.Ak.; Safitri Akbari, S.E., M.Ak., Ak.; Fanny Andriani Setiawan, S.E., M.Ak.",
"dosen akuntansi,dosen akt,pengajar akuntansi"},
{"dosen_bisdi","dosen","Dosen Tetap Program Studi Bisnis Digital (BISDI)",
"Dosen tetap Program Studi Bisnis Digital UCIC: Hartanto, S.E., M.M.; Oryz Agnu Dian Wulandari, S.E., M.M.; Rifqi Fahrudin, S.Kom., M.Kom.; Willy Eka Septian, S.Kom., M.Kom.; Amroni, S.E., M.M.",
"dosen bisnis digital,dosen bisdi,pengajar bisnis digital"},
{"dosen_mb","dosen","Dosen Tetap Program Studi Manajemen Bisnis (D3 MB)",
"Dosen tetap Program Studi D3 Manajemen Bisnis UCIC: Gytha Nurhana Dhea Praadha Gitama, S.E., M.M.; Muthia Fariza, S.IP., M.M.; Sitta Nurhajijah, S.AB., M.Si.; Viar Dwi Kartika, S.Pd., M.Pd.",
"dosen manajemen bisnis,dosen d3 mb,pengajar manajemen bisnis"},
{"dosen_pko","dosen","Dosen Tetap Program Studi Pendidikan Kepelatihan Olahraga (PKO)",
"Dosen tetap Program Studi PKO UCIC: Abdul Rosyid, S.Pd., M.Pd.; Chiang Yulius Darmawan, S.Psi., M.Psi., Psikolog; Helmi Akmal Fauzan, S.Pd., M.Pd.; Nadia Nur Paramitha, S.Pd., M.Pd.; Nur Muhamad Hildan, S.Pd., M.Pd.; Ridwan Afif, S.Pd., M.Pd.; Yogi Slamet, S.Pd., M.Pd.",
"dosen pendidikan kepelatihan olahraga,dosen pko,dosen pkor,pengajar olahraga"},
{"dosen_mm","dosen","Dosen Tetap Program Studi Magister Manajemen (MM/S2)",
"Dosen tetap Program Studi Magister Manajemen UCIC: Assoc. Prof. Dr. Chandra Lukita, S.E., M.M., M.T.I. (Rektor UCIC); Dr. Muhammad Fadheel Djamaly, B.IAM., M.Sc.; Dr. Dian Ismawarni, S.T., M.M.; Dr. Jeammy Nolen, S.T., M.M.",
"dosen magister manajemen,dosen s2 manajemen,dosen mm,pengajar s2"},
{"dosen_mku","dosen","Dosen Mata Kuliah Umum (MKU) UCIC",
"Chiang Yulius Darmawan, Mohammad Umar Fakhrudin, Sudadi Pranata, Dr. Taufan Hunneman, Viar Dwi Kartika, Yuni Awalaturrohmah Solihah.",
"dosen mku,dosen umum,dosen pancasila,dosen bahasa,dosen english,lecturer general,pengajar umum"},
{"dosen_aan_kanivia","dosen","Aan Kanivia, S.E., M.Ak.",
"Mata kuliah: Auditing 1 & 2, Praktikum Auditing, Praktikum Akuntansi Keuangan, Hukum Perpajakan, Audit Internal, Analisis Laporan Keuangan, Bisnis Digital, Penatausahaan Dokumen Perkantoran.",
"aan kanivia,akuntansi,auditing,pajak,dosen feb,dosen akuntansi"},
{"dosen_ade_elza","dosen","Ade Elza Surachman, S.E., M.Ak., Ak.",
"Mata kuliah: Akuntansi Biaya, Akuntansi Manajemen, Akuntansi Sosial & Lingkungan, Sustainability Reporting, Teori Portofolio & Analisis Investasi, Etika Profesi Kinerja, Ekonomi Makro.",
"ade elza surachman,ade eliza s,akuntansi,biaya,investasi,etika,dosen feb"},
{"dosen_dessy_kumala","dosen","Dessy Kumala Dewi, S.E., M.Ak.",
"Mata kuliah: Akuntansi Internasional, Akuntansi Entitas Privat, Analisis Investasi, Statistika & Analisis Bisnis, Metode Kualitatif & Kuantitatif, Mengolah Data Perkantoran.",
"dessy kumala dewi,akuntansi,investasi,statistika,metode penelitian,dosen feb"},
{"dosen_dewi_anggun","dosen","Dewi Anggun Puspitarini, S.E., M.Ak.",
"Mata kuliah: Ekonomi Makro, Etika Profesi Akuntan, Akuntansi Syariah, Akuntansi Keperilakuan.",
"dewi anggun,ekonomi makro,akuntansi syariah,akuntansi keperilakuan,dosen feb"},
{"dosen_fanny_andriani","dosen","Fanny Andriani Setiawan, S.E., M.M., Ak., CA",
"Mata kuliah: Akuntansi Keuangan Menengah, Manajemen Keuangan.",
"fanny andriani,fanny andrian,akuntansi keuangan,manajemen keuangan,dosen feb"},
{"dosen_safitri_akbari","dosen","Safitri Akbari, S.E., M.Ak.",
"Mata kuliah: Akuntansi Keuangan Dasar, Menengah & Lanjutan, Akuntansi Perpajakan, Teori Akuntansi, Manajemen Anggaran, Sistem Akuntansi, Analisis Laporan Keuangan, Pengoperasian Aplikasi dan Alat Bantu Perkantoran.",
"safitri akbari,safiri akbari,akuntansi,anggaran,pajak,sistem akuntansi,pengoperasian aplikasi dan alat bantu perkantoran,dosen feb"},
{"dosen_aditya_kurniawan","dosen","Aditya Kurniawan Chandra, B.Com. (Acc & Fin), M.M.",
"Mata kuliah: Financial Technology, Bisnis Digital, Komunikasi Bisnis & Negosiasi, Sistem Informasi Akuntansi.",
"aditya kurniawan,aditya kurniawan chandra,bcom,acc fin,financial technology,bisnis digital,komunikasi bisnis,dosen feb"},
{"dosen_agung_supriyadi","dosen","Agung Supriyadi, S.E., M.Ak.",
"Mata kuliah: AI for Accounting, Financial Technology, Teknologi Keuangan (FinTech), Manajemen Keuangan, Akuntansi Keuangan Dasar, Praktikum Perpajakan, Teknologi Informasi untuk Bisnis, Ekonomi Makro, Komunikasi & Teamwork.",
"agung supriyadi,aquung supriyadi,ai accounting,fintech,teknologi keuangan,manajemen keuangan,teknologi informasi bisnis,dosen feb"},
{"dosen_amroni","dosen","Amroni, S.E., M.M.",
"Mata kuliah: Aplikasi Software Akuntansi, Pendidikan Agama Islam, Teknologi Keuangan, Akuntansi Syariah, Kemampuan Menyelesaikan Masalah Teknis, Kreativitas dan Inisiatif, Sistem Informasi Akuntansi, Manajemen Bisnis.",
"amroni,aplikasi software akuntansi,pendidikan agama islam,teknologi keuangan,akuntansi syariah,kreativitas,kreatifitas,sistem informasi akuntansi,manajemen bisnis,dosen feb"},
{"dosen_dewi_laily","dosen","Dewi Laily Purnamasari, S.T., M.M.",
"Mata kuliah: Sustainability & ESG in Business, Manajemen Hubungan Pelanggan, Manajemen Tataletak & Planogram, Manajemen & Bisnis Dasar, Kreativitas.",
"dewi laily,sustainability,esg,manajemen bisnis,ritel,kreativitas,kreatifitas,dosen feb"},
{"dosen_feri_fauzi","dosen","Feri Fauzi, S.E., M.M.",
"Mata kuliah: Manajemen Keuangan, Technopreneurship, Technopreneurship Lanjutan, Seminar Strategi Pemasaran, Sustainability & ESG in Business, Ekonomi Makro, Pendidikan Agama Islam, Manajemen & Bisnis Dasar, Kemampuan Kerja Tim.",
"feri fauzi,manajemen keuangan,technopreneurship,technopreneurship lanjutan,pemasaran,agama islam,manajemen bisnis dasar,dosen feb"},
{"dosen_gytha_nurhana","dosen","Gytha N.D.P.G., S.E., M.M.",
"Mata kuliah: Manajemen Anggaran, Manajemen Kas & Portofolio, Matematika Ekonomi & Bisnis, Ekonomi Mikro, Etika Profesi Bisnis, Pengantar Ilmu Ekonomi Bisnis, Kemampuan Merumuskan Masalah Teknis, Manajemen & Bisnis Dasar.",
"gytha nurhana,gytha nurhana dhea,manajemen anggaran,ekonomi mikro,matematika ekonomi,kemampuan merumuskan masalah teknis,manajemen bisnis dasar,dosen feb"},
{"dosen_hartanto","dosen","Hartanto, S.E., M.M.",
"Mata kuliah: Digital Marketing, Ekonomika Bisnis, Academic Writing 2.",
"hartanto,digital marketing,ekonomi bisnis,academic writing,academik writing,dosen feb"},
{"dosen_ika_kartika","dosen","Ika Kartika, S.Kom., M.E.",
"Mata kuliah: Akuntansi Keuangan Bisnis, Penganggaran Perusahaan, Ekspor-Impor, Teknologi Informasi untuk Bisnis, Kepemimpinan Entrepreneur, Kemampuan Administrasi.",
"ika kartika,akuntansi bisnis,ekspor impor,teknologi informasi,kewirausahaan,dosen feb"},
{"dosen_mualif_ismail","dosen","Mualif Ismail, S.M., M.M.",
"Mata kuliah: Perilaku Organisasional, Statistika & Analisis Bisnis, Manajemen Operasi, Manajemen Anggaran.",
"mualif ismail,perilaku organisasi,statistika bisnis,manajemen operasi,dosen feb"},
{"dosen_muthia_fariza","dosen","Muthia Fariza, S.IP., M.M.",
"Mata kuliah: Manajemen Strategis, Manajemen SDM, Manajemen Kualitas, Perilaku Konsumen, Komunikasi Bisnis, Kepemimpinan, Manajemen & Bisnis Dasar, Metode Kualitatif & Kuantitatif.",
"muthia fariza,manajemen strategis,sdm,perilaku konsumen,kepemimpinan,manajemen bisnis dasar,dosen feb"},
{"dosen_ni_wayan","dosen","Ni Wayan Fitriana Ayu Lestari, S.E., M.M.",
"Mata kuliah: Manajemen SDM, Kedisiplinan Kerja, Hukum Bisnis.",
"ni wayan,ni wayan fal,manajemen sdm,hukum bisnis,kedisiplinan,dosen feb"},
{"dosen_oryz_agnu","dosen","Oryz Agnu Dian Wulandari, S.E., M.M.",
"Mata kuliah: Manajemen Pemasaran (AI for Marketing), Digital Marketing, Statistik, Statistika Bisnis, Technopreneurship Lanjutan, Etika & Komunikasi Bisnis, Kreativitas.",
"oryz agnu,uryz agnu,manajemen pemasaran,digital marketing,statistik,technopreneurship,dosen feb"},
{"dosen_siti_nurhajjah","dosen","Siti Nurhajjah, S.Ab., M.Si.",
"Mata kuliah: Strategi Pemasaran Ritel, Metodologi Penelitian Bisnis, Manajemen Operasi, Manajemen Strategik, Hukum Bisnis, Pengantar Akuntansi, Praktikum Akuntansi Manual, Manajemen & Bisnis Dasar, Kemampuan Berkomunikasi, Etika Profesi Bisnis.",
"siti nurhajjah,sitta nurhajjah,pemasaran ritel,metodologi penelitian,manajemen strategik,hukum bisnis,pengantar akuntansi,praktikum akuntansi manual,kemampuan berkomunikasi,dosen feb"},
{"dosen_suwandi","dosen","Suwandi, S.E., M.M.",
"Mata kuliah: Technopreneurship Lanjutan, Financial Management, Manajemen Inovasi Berbasis Teknologi, Manajemen Transformasi Digital, Statistika & Analisis Bisnis, Matematika Ekonomi & Bisnis, Perilaku Organisasional, Kemampuan Komunikasi, Teknologi Informasi untuk Bisnis.",
"suwandi,suwardi,technopreneurship,manajemen keuangan,statistika,matematika ekonomi,perilaku organisasional,kemampuan komunikasi,inovasi teknologi,dosen feb"},
{"dosen_turini","dosen","Turini, S.E., M.M.",
"Mata kuliah: Dasar-Dasar Perpajakan, Studi Kelayakan Bisnis, Teknologi Informasi untuk Bisnis, Pembangunan Karakter & Etika Profesi.",
"turini,y turini,perpajakan,studi kelayakan bisnis,etika profesi,dosen feb"},
{"dosen_agus_sevtiana","dosen","Agus Sevtiana, S.T., M.Kom.",
"Mata kuliah: UI & UX Design, Design Thinking, E-Commerce & Platform, Komputer Grafis & Multimedia, Digital Content Interaktif, E-Government, Computer Animation, Jaringan Komputer.",
"agus sevtiana,agus sevitana,agus seviana,agus sevitana,ui ux,komputer grafis,multimedia,e commerce,design thinking,dosen fti"},
{"dosen_arif_nursetyo","dosen","Arif Nursetyo, S.Kom., M.Kom.",
"Mata kuliah: Kecerdasan Buatan, Computer Vision & NLP, Etika dalam AI, Pengujian & Pemeliharaan Sistem.",
"arif nursetyo,arif nurestyo,software testing,ai,etika ai,computer vision,nlp,dosen fti"},
{"dosen_bambang_sugiarto","dosen","Bambang Sugiarto, S.Kom., M.Kom.",
"Mata kuliah: Rekayasa Aplikasi Internet, Basic Mobile Programming, Mobile Programming, Pemrograman Internet, Pemrograman OOP, Pemrograman Berorientasi Objek, Realitas Virtual dan Augmentasi, VR & AR, Sistem Pakar, Pengembangan Aplikasi Mobile, Student Exchange (AR), Student Exchange (Augmented Reality).",
"bambang sugiarto,rekayasa aplikasi internet,basic mobile programming,mobile programming,pemrograman internet,pemrograman berorientasi objek,realitas virtual,vr ar,augmented reality,sistem pakar,pbo,dosen fti"},
{"dosen_kusnadi","dosen","Kusnadi, S.Kom., M.Kom.",
"Mata kuliah: Algoritma & Pemrograman, Struktur Data, UI & UX Designer, IoT, Proyek Technopreneurship, Komunikasi & Teamwork, Kerja Tim dan Kolaborasi, Keterampilan Presentasi, Keterampilan Interpersonal, Penguasaan Teknologi.",
"kusnadi,struktur data,algoritma,algoritma dan pemrograman,ui ux,iot,technopreneurship,kerja tim,kolaborasi,penguasaan teknologi,dosen fti"},
{"dosen_lena_magdalena","dosen","Lena Magdalena, S.Kom., MMSI.",
"Mata kuliah: Manajemen dan Organisasi, Rekayasa Perangkat Lunak (Agile Scrum & Implementation), Analisa Sistem Berorientasi Objek, Analisa & Perancangan Sistem Informasi, Audit Sistem Informasi, Teknologi dan Tren E-Commerce, Komunikasi & Teamwork, Ide, Inisiatif dan Tanggung Jawab.",
"lena magdalena,rpl,agile scrum,introduction,implementation,audit sistem informasi,analisa sistem,e commerce,manajemen dan organisasi,komunikasi teamwork,dosen fti"},
{"dosen_linda_norhan","dosen","Linda Norhan, S.T., M.M.",
"Mata kuliah: Monetisasi Konten Digital, Digital Copywriting, Brand Identity, Analisa Proses Bisnis, Analisa & Perancangan Sistem Informasi, Matematika Bisnis, Aljabar Linear dan Matriks, Manajemen Periklanan.",
"linda norhan,linda norman,konten digital,copywriting,branding,analisa proses bisnis,matematika bisnis,aljabar linear,dosen fti"},
{"dosen_marsani_asfi","dosen","Marsani Asfi, S.Si., M.Si.",
"Mata kuliah: Algoritma & Pemrograman, Algoritma dan Pemrograman 2, Grafika Komputer & Pengolahan Citra, Machine Learning, Kecerdasan Buatan, Metodologi Penelitian, Metodologi Penelitian dan Riset Pustaka, Matematika Ekonomi & Bisnis, Manajemen Inovasi Berbasis Teknologi.",
"marsani asfi,algoritma,algoritma dan pemrograman 2,grafika komputer,machine learning,ai,metodologi penelitian,riset pustaka,manajemen inovasi,dosen fti"},
{"dosen_mesi_febima","dosen","Mesi Febima, M.Kom.",
"Mata kuliah: Business Intelligence, Manajemen Proyek, Sistem Informasi Manajemen, E-CRM, AI for Web Design, Pemrograman Internet, Pemrograman Internet 1, Analisa Proses Bisnis, Integrasi Sistem Enterprise, Etika Profesi Kerja, Tata Tulis Karya Ilmiah, Penguasaan Teknologi.",
"mesi febima,e crm,business intelligence,manajemen proyek,sistem informasi manajemen,web design,pemrograman internet 1,tata tulis karya ilmiah,penguasaan teknologi,dosen fti"},
{"dosen_muhammad_hatta","dosen","Muhammad Hatta, M.Kom.",
"Mata kuliah: Algoritma dan Pemrograman 2, Machine Learning, Mobile Programming, Keamanan Sistem Informasi & Jaringan, Pemrograman OOP, Pemrograman Berorientasi Objek, Pemrograman Internet (Framework MVC), Pemrograman Internet Intermediate (Framework, MVC), Perancangan Aplikasi Mobile.",
"muhammad hatta,mobile programming,machine learning,algoritma dan pemrograman 2,keamanan sistem,pbo,pemrograman berorientasi objek,framework mvc,pemrograman internet intermediate,dosen fti"},
{"dosen_muhammad_afif","dosen","Muhammad Afif Sulhan, M.Kom.",
"Mata kuliah: Domain & Manajemen Web Server, Keamanan Web.",
"muhammad afif,web server,keamanan web,domain,dosen fti"},
{"dosen_petrus_sokibi","dosen","Petrus Sokibi, S.Kom., M.Kom.",
"Mata kuliah: Algoritma & Pemrograman, Struktur Data, Sistem Basis Data, Manajemen Data & Informasi, Arsitektur SI/IT Perusahaan, Technopreneurship, Sistem dan Teknologi Informasi, Etika Profesi Kerja, Ide, Inisiatif dan Tanggung Jawab, Pancasila & Kewarganegaraan, Pancasila dan Kewarganegaraan dan Anti Korupsi, Pendidikan Agama Katolik, Pendidikan Agama Kristen, Pendidikan Agama Budha, Pendidikan Agama Hindu, Pendidikan Agama Konghucu.",
"petrus sokibi,petrus s,struktur data,basis data,algoritma dan pemrograman,arsitektur si,technopreneurship,sistem dan teknologi informasi,etika profesi kerja,anti korupsi,dosen fti,agama,agama katolik,agama kristen,agama budha,agama hindu,agama konghucu,katolik,kristen,budha,hindu,konghucu"},
{"dosen_prema_adk","dosen","Prema A.D.K., M.Kom.",
"Mata kuliah: Pemrograman Internet (Framework MVC), Pemrograman Internet Intermediate (Framework, MVC), AI for Web Design (HTML, CSS, Copilot).",
"prema adk,pemrograman internet,pemrograman internet intermediate,framework mvc,html,css,copilot,web design,ai,dosen fti"},
{"dosen_ricky_perdana","dosen","Ricky Perdana Kusuma, M.Kom.",
"Mata kuliah: Integrated Marketing Communication, Optimasi Aplikasi Web, IMC, Etika & Regulasi Media Digital, Komunikasi Data, Kewirausahaan Digital, Penguasaan Pemrograman & Design.",
"ricky perdana,integrated marketing communication,imc,optimasi web,etika media digital,komunikasi data,kewirausahaan digital,penguasaan perongruaman design,dosen fti"},
{"dosen_ridho_taufiq","dosen","Ridho Taufiq Subagio, S.T., M.Kom.",
"Mata kuliah: Komunikasi Data & Jaringan, Sistem Digital, IoT, Internet of Things, Sistem Penunjang Keputusan, Jaminan Kualitas Perangkat Lunak, Audit Sistem Informasi, Analisis dan Desain Perangkat Lunak, Metodologi Penelitian, Metodologi Penelitian & Riset Pustaka, Tata Tulis Karya Ilmiah.",
"ridho taufiq,ridho taufiq s,jaringan komputer,sistem digital,iot,internet of things,audit si,analisis dan desain perangkat lunak,metodologi penelitian,riset pustaka,dosen fti"},
{"dosen_rifqi_fahrudin","dosen","Rifqi Fahrudin, S.Kom., M.Kom.",
"Mata kuliah: UI/UX Design, UI/UX Design (Intermediate), UI & UX Designer, Interaksi Manusia & Komputer, Proyek Technopreneurship, Student Exchange (Agile Implementation).",
"rifqi fahrudin,ui ux,ui ux intermediate,ui ux designer,interaksi manusia komputer,technopreneurship,agile,dosen fti"},
{"dosen_rinaldi_adam","dosen","Rinaldi Adam, M.Comp.",
"Mata kuliah: Data Mining & Data Warehouse, Sistem Penunjang Keputusan, Manajemen Proyek Perangkat Lunak.",
"rinaldi adam,kinaidi adam,data mining,data warehouse,sistem penunjang keputusan,manajemen proyek,dosen fti"},
{"dosen_unang_solihin","dosen","Unang Solihin, M.Kom.",
"Mata kuliah: Sistem Terdistribusi, Keamanan Sistem Informasi & Jaringan, Jaringan Komputer, Administrasi Jaringan, Manajemen Proyek.",
"unang solihin,sistem terdistribusi,keamanan jaringan,administrasi jaringan,dosen fti"},
{"dosen_verina_tita","dosen","Verina Tita Nabila, M.Si.",
"Mata kuliah: Data Mining, Machine Learning, Deep Learning, Matematika Ekonomi & Bisnis.",
"verina tita,data mining,machine learning,deep learning,matematika,dosen fti"},
{"dosen_victor_asih","dosen","Victor Asih, S.Si., M.T.",
"Mata kuliah: Sistem Operasi, Arsitektur & Organisasi Komputer.",
"victor asih,sistem operasi,arsitektur komputer,dosen fti"},
{"dosen_willy_eka","dosen","Willy Eka Septian, S.Kom., M.Kom.",
"Mata kuliah: Rekayasa Aplikasi Internet, Web Programming, Web Programming 1 (Konsep OOP PHP), Web Design, Pemrograman Internet, E-Commerce & Platform, Enterprise Resource Planning, Basis Data & Implementasi, Manajemen Proyek Perangkat Lunak, UI/UX Design, UI/UX Design (Dasar), Manajemen Transformasi Digital, Implementasi Arsitektur Enterprise, Management Information System.",
"willy eka,willy septian eka,wilyi eka,web programming,web design,rekayasa aplikasi internet,pemrograman internet,erp,e commerce,basis data implementasi,manajemen proyek,ui ux,dosen fti"},
{"dosen_wiwiek_nurkomala","dosen","Wiwiek Nurkomala Dewi, S.T., M.M.",
"Mata kuliah: Statistik for Data Science, Matematika Informatika, Digital Bisnis Startup, Kewirausahaan Digital, Tata Tulis Karya Ilmiah, Etika Profesi Kerja, Komunikasi Data, Komunikasi & Teamwork, HaKI & Publikasi, Penerapan Teknologi Industri.",
"wiwiek nurkomala,wiwiek nd,statistika data science,startup digital,haki,etika profesi,komunikasi data,kewirausahaan digital,dosen fti"},
{"dosen_dzulfiqar_fickri","dosen","Dzulfiqar Fickri Rosyid, M.Ds.",
"Mata kuliah: Komunikasi Digital, Sosiologi Desain, Desain Komunikasi Visual Sosial, Bahasa Rupa, Semiotika Visual, Intelligent Audio Visual Effect, Eksperimen & Ideasi Visual, Inovasi Grafis, Video Brand Production, Game Cerdas, Game Desain.",
"dzulfiqar fickri,dkv,komunikasi visual,desain komunikasi visual sosial,audio visual,game cerdas,game desain,semiotika,dosen fti"},
{"dosen_galih_mp","dosen","Galih M.P., M.Ds.",
"Mata kuliah: Creative Content Advertising, Desain Komunikasi Visual Promosi, Design Thinking, Estetika Visual Desain, Tinjauan & Kritik Desain, Technopreneurship.",
"galih mp,dkv,advertising,desain promosi,design thinking,estetika,dosen fti"},
{"dosen_ine_rachmawati","dosen","Ine Rachmawati, M.Sn.",
"Mata kuliah: Fotografi Digital, Applied Photography, Basic Fotografi, Videografi, Student Exchange (Fotografi), Dasar Desain Grafis, Metoda Produksi Grafika, Media Interaktif, Psikologi Persepsi, Portofolio DKV, Metodologi Penelitian Desain, Tata Tulis Karya Ilmiah, Ide, Inisiatif dan Tanggung Jawab.",
"ine rachmawati,ine kachmawati,dkv,fotografi,applied photography,basic fotografi,videografi,student exchange fotografi,desain grafis,metodologi penelitian,dosen fti"},
{"dosen_tiara_khodijah","dosen","Tiara Khodijah, M.Ds.",
"Mata kuliah: Graphic Design, Animasi 3D, Tipografi, Kinetic Tipografi, Packaging Design, Desain Komunikasi Visual Identitas, Desain Display, Strategic Konten Digital, Manajemen Desain & Creativepreneur, Komunikasi & Teamwork.",
"tiara khodijah,tiara khodiiah,dkv,animasi 3d,tipografi,kinetic tipografi,branding,packaging,dosen fti"},
{"dosen_helmi_akmal","dosen","Helmi Akmal Fauzan, S.Pd., M.Pd.",
"Mata kuliah: Pelatihan Cabang Renang, Pelatihan Cabang Atletik, Anatomi, Bahasa Indonesia.",
"helmi akmal,pelatihan cabor renang,pelatihan caber renang,pelatihan cabor atletik,pelatihan caber atletik,olahraga,renang,atletik,anatomi,bahasa indonesia,dosen fps"},
{"dosen_kidwan_alif","dosen","Kidwan Alif, S.Pd., M.Pd.",
"Mata kuliah: Fisiologi Olahraga.",
"kidwan alif,fisiologi olahraga,dosen fps"},
{"dosen_nadia_nur","dosen","Nadia Nur Paramitha, S.Pd., M.Pd.",
"Mata kuliah: Ilmu Keolahragaan Dasar, Pembelajaran Motorik.",
"nadia nur,ilmu keolahragaan,pembelajaran motorik,dosen fps"},
{"dosen_nur_hildan","dosen","Nur Muhamad Hildan, S.Pd., M.Pd.",
"Mata kuliah: Pelatihan Cabor Sepak Bola, Pelatihan Cabor Futsal, Bahasa Indonesia.",
"nur hildan,sepak bola,futsal,olahraga,bahasa indonesia,dosen fps"},
{"dosen_yogi_slamet","dosen","Yogi Slamet, S.Pd., M.Pd.",
"Mata kuliah: Kurikulum & Pembelajaran.",
"yogi slamet,kurikulum,pembelajaran,dosen fps"},
{"dosen_chiang_yulius","dosen","Chiang Yulius Darmawan, M.Psi., Psi.",
"Mata kuliah: Perilaku Organisasional, Psikologi Persepsi, Pembangunan Karakter & Etika Profesi.",
"chiang yulius,chiang yulius darmawan,psikologi,perilaku organisasi,etika profesi,dosen umum"},
{"dosen_moh_umar","dosen","Mohammad Umar Fakhrudin, S.Pd., M.Pd.",
"Mata kuliah: General English, Intermediate English, TOEFL Preparation, English for Specific Purpose, Bahasa Indonesia.",
"moh umar,moh umar fahkrudin,mohammad umar fakhrudin,bahasa inggris,toefl,bahasa indonesia,dosen umum"},
{"dosen_sudadi_pranata","dosen","Sudadi Pranata, S.E., M.Si.",
"Mata kuliah: Academic Writing 1 & 2, Pancasila & Kewarganegaraan, Metodologi Penelitian, Kecerdasan Buatan, Data Analytics for Business Decision, Manajemen Operasi, Manajemen SDM, Literasi Digital, Komunikasi Bisnis & Negosiasi, Kemampuan Kerja Tim.",
"sudadi pranata,academic writing,academik writing,pancasila,metodologi penelitian,data analytics,komunikasi bisnis,kemampuan kerja tim,dosen umum"},
{"dosen_taufan_hunneman","dosen","Dr. Taufan Hunneman",
"Mata kuliah: Pancasila & Kewarganegaraan, Hukum Bisnis.",
"taufan hunneman,taufan huneman,pancasila,hukum bisnis,dosen umum"},
{"dosen_viar_dwi","dosen","Viar Dwi Kartika, S.Pd., M.Pd.",
"Mata kuliah: General English, Intermediate English, English for Business, English for Specific Purpose, TOEFL Preparation, Kemampuan Bekerjasama.",
"viar dwi,bahasa inggris,english for business,english for specific purpose,toefl,dosen umum"},
{"dosen_yuni_awalaturrohmah","dosen","Yuni Awalaturrohmah Solihah, M.Pd.",
"Mata kuliah: General English, Intermediate English, English for Business, TOEFL Preparation, English for Specific Purpose, Advertising Copywriting, Etika Profesi Kerja.",
"yuni awalaturrohmah,yuni a s,yuni a.s,bahasa inggris,toefl,copywriting,dosen umum"},
{"dosen_chandra_lukita","dosen","Assoc. Prof. Dr. Chandra Lukita, S.E., M.M., M.TI.",
"Rektor UCIC. Mata kuliah: Technopreneurship Lanjutan, Studi Kelayakan Bisnis, Data Analytics for Business Decision, Academic Writing 2.",
"chandra lukita,rektor,technopreneurship,data analytics,academic writing,academik writing,studi kelayakan bisnis"},
{"dosen_ika_iryanti","dosen","Dr. Ika Iryanti, S.E., M.M.",
"Mata kuliah: Manajemen Strategis, Manajemen Strategik, Analisis Investasi, Seminar Strategi Pemasaran, Metodologi Penelitian, Matematika Ekonomi & Bisnis.",
"ika iryanti,manajemen strategis,investasi,metodologi penelitian,dosen feb"},
{"dosen_muhammad_fadheel","dosen","Dr. Muhammad Fadheel Djamaly, M.Sc.",
"Mata kuliah: Ekspor-Impor.",
"muhammad fadheel,ekspor impor,bisnis internasional"},
{"dosen_winsen_setiawan","dosen","Winsen Setiawan, M.T.",
"Mata kuliah: Ekonomi Makro.",
"winsen setiawan,ekonomi makro"},
{"dosen_team_ai","dosen","Team AI",
"Mata kuliah: Kecerdasan Buatan (untuk berbagai program studi).",
"team ai,kecerdasan buatan,artificial intelligence"},

// ===== AKREDITASI DAN FASILITAS =====
{"akreditasi_ucic","akreditasi","Akreditasi UCIC",
"UCIC memiliki akreditasi resmi dari BAN-PT dengan status akreditasi institusi \"Baik Sekali\", dan sedang menuju status \"Unggul\". Status akreditasi program studi: S1 Teknik Informatika Baik Sekali, S1 Sistem Informasi Baik Sekali, S1 Desain Komunikasi Visual Baik, D3 Manajemen Informatika Baik Sekali, S1 Manajemen Baik Sekali, S1 Bisnis Digital Baik, S1 Akuntansi Baik, D3 Manajemen Bisnis Baik Sekali, S1 Pendidikan Kepelatihan Olahraga Baik. Untuk status akreditasi program studi baru (S1 Pendidikan Matematika, S2 Manajemen) sebaiknya dikonfirmasi ke kampus karena masih tergolong baru.",
"akreditasi,mutu,kualitas,ban-pt,peringkat,nilai akreditasi,akreditasi jurusan"},
{"fasilitas_kampus_1","fasilitas","Fasilitas Kampus 1 UCIC",
"Fasilitas Kampus 1 UCIC:\n- 9 ruang kelas ber-AC.\n- 1 auditorium berkapasitas sekitar 120 orang.\n- 1 ruang perpustakaan, 1 ruang Inkubator Center, 1 ruang Inkubator Bisnis.\n- 1 ruang Unit Kesehatan, 1 ruang konsultasi mahasiswa.\n- 3 ruang dosen, 1 ruang BAAK, 1 ruang keuangan.\n- Toilet pria dan wanita, 1 mushola.\n- 18 titik CCTV yang beroperasi 24 jam.\n- Area terbuka sebagai kantin mahasiswa sekaligus Student Business Corner (SBC) untuk mendukung kewirausahaan mahasiswa.\n- Area parkir luas.\n- 1 ruang podcast, 1 laboratorium komputer berkapasitas 25 unit PC.",
"fasilitas kampus 1,ruang kelas kampus 1,auditorium kampus 1,perpustakaan kampus 1,inkubator,lab komputer kampus 1,kantin kampus 1,student business corner,sbc"},
{"fasilitas_kampus_2","fasilitas","Fasilitas Kampus 2 UCIC",
"Fasilitas Kampus 2 UCIC (dirancang modern dan berbasis teknologi):\n- 5 ruang kelas modern berdesain full kaca, dilengkapi Smart TV.\n- 2 toilet pria, 2 toilet wanita, 2 mushola.\n- 18 titik CCTV yang beroperasi sekitar 24 jam.\n- Convention Hall berkapasitas sekitar 350 orang, dilengkapi 1 videotron dan 2 unit Smart TV, digunakan untuk seminar, workshop, dan acara besar.\n- 1 ruang marketing (pusat konsultasi) dan 1 ruang pameran karya/journey UCIC.\n- Laboratorium komputer yang mendukung teknologi Artificial Intelligence (AI & Data Analysis Lab).",
"fasilitas kampus 2,kelas modern,smart tv,convention hall,lab ai,laboratorium ai,cctv kampus 2,mushola kampus 2,ruang marketing"},
{"fasilitas_umum_wifi","fasilitas","Fasilitas Umum dan Jaringan Internet UCIC",
"Fasilitas umum UCIC yang tersebar di Kampus 1 dan Kampus 2 antara lain: Campus Hub, Convention Hall, Multimedia/DM/3D Virtual Reality Lab, Ruangan Kelas, AI & Data Analysis Lab, Computer Lab, Ruang Meeting, Perpustakaan, Ruang Podcast, Student Business Corner, dan Auditorium.\n\nKampus 1 dan Kampus 2 dilengkapi jaringan internet nirkabel (WiFi) yang mendukung teknologi WiFi 6 (802.11ax), memberikan akses internet lebih cepat, koneksi lebih stabil untuk banyak pengguna sekaligus, efisiensi bandwidth, serta dukungan optimal untuk pembelajaran digital, streaming, penelitian, dan sistem informasi kampus.",
"fasilitas,fasilitas kampus,lab,laboratorium,gedung,ruangan,sarana,prasarana,apa saja fasilitas,wifi,wifi 6,internet kampus,jaringan kampus,multimedia lab,3d virtual reality,computer lab,ruang meeting"},
{"perpustakaan_umum","fasilitas","Perpustakaan UCIC: Layanan, Keanggotaan, dan Digital",
"Perpustakaan UCIC adalah fasilitas akademik yang menyediakan buku, jurnal, dan layanan literasi untuk mendukung belajar, penelitian, dan pengembangan pengetahuan. Layanan dapat digunakan mahasiswa, dosen, dan tenaga kependidikan UCIC; perpustakaan mitra juga dapat mengakses dengan ketentuan tertentu.\n\nMahasiswa UCIC otomatis terdaftar sebagai anggota perpustakaan; Kartu Mahasiswa digunakan untuk mengakses layanan peminjaman.\n\nLayanan yang tersedia: peminjaman dan pengembalian buku, layanan repository dan e-library, layanan antar buku (khusus dosen), akses internet/WiFi.\n\nLayanan digital: e-catalogue (https://pustaka.cic.ac.id/), e-library (https://uciclibrary.perpustakaan.co.id/home.ks), repository institusi (https://ebook.cic.ac.id/home/daftar). Repository dapat diakses khusus mahasiswa dan dosen melalui portal My CIC > menu E-book, untuk mencari skripsi, jurnal, dan karya ilmiah.\n\nPenggunaan AI untuk membantu tugas di perpustakaan diperbolehkan, namun harus bijak sesuai etika akademik: tetap mencantumkan sumber dan tidak melakukan plagiarisme.\n\nJam layanan (mengikuti kebijakan kampus, dapat berubah): Senin-Jumat sekitar pukul 08.00-16.00 WIB. Kontak: email perpustakaan@cic.ac.id, Instagram @perpustakaan_ucic, atau datang ke Jl. Kesambi No. 202, Kota Cirebon.",
"perpustakaan,apa itu perpustakaan ucic,pengguna perpustakaan,anggota perpustakaan,jam perpustakaan,kontak perpustakaan,e-catalogue,e-library,repository,ai di perpustakaan"},
{"perpustakaan_peminjaman","fasilitas","Perpustakaan UCIC: Aturan Peminjaman dan Bebas Pustaka",
"Masa peminjaman buku umumnya 7-14 hari tergantung jenis koleksi, dan dapat diperpanjang sesuai ketentuan. Denda keterlambatan pengembalian: Rp2.000 per hari per buku.\n\nSurat bebas pustaka menjadi salah satu syarat sidang skripsi/tugas akhir. Persyaratannya antara lain: menunjukkan KTM, surat keterangan dari prodi, surat lunas pembayaran dari BAU, menyerahkan skripsi/tesis yang sudah dijilid dan bebas plagiat, serta CD/PDF skripsi dan jurnal. Buku yang rusak/hilang wajib diganti dengan buku sama atau ganti rugi 2x harga buku.",
"pinjam buku,peminjaman perpustakaan,bebas pustaka,surat bebas pustaka,denda perpustakaan,masa pinjam buku,perpanjang peminjaman"},

// ===== BIAYA, BEASISWA, DAN PENDAFTARAN =====
{"pendaftaran_pmb","pendaftaran","Cara dan Alur Pendaftaran Mahasiswa Baru (PMB) UCIC",
"Pendaftaran mahasiswa baru UCIC dapat dilakukan online melalui https://pmb.cic.ac.id/register atau offline dengan datang langsung ke kampus UCIC.\n\nAlur pendaftaran/daftar ulang:\n1. Calon mahasiswa mengisi registrasi lengkap di https://pmb.cic.ac.id/\n2. Melakukan pembayaran biaya registrasi, perlengkapan, dan pembayaran tahap 1.\n3. Mengirim bukti transfer jika menggunakan rekening/transfer.\n4. Menunggu verifikasi dari tim PMB.\n5. Mengunggah berkas persyaratan (ijazah/SKL, KTP, KK, akta kelahiran, pas foto).\n6. Masuk ke grup CAMABA.\n7. Menunggu informasi lanjutan dari kampus.\n\nSistem perkuliahan bersifat hybrid (kombinasi tatap muka/offline dan daring/online), memberi fleksibilitas menyesuaikan perkembangan teknologi dan kebutuhan pembelajaran.",
"daftar,pendaftaran,pmb,mahasiswa baru,registrasi,cara daftar,masuk,persyaratan,maba,syarat,camaba,daftar online,daftar offline"},
{"pmb_syarat_dokumen","pendaftaran","Syarat dan Berkas Pendaftaran Calon Mahasiswa UCIC",
"Syarat umum pendaftaran: mengisi formulir pendaftaran, fotokopi ijazah/surat keterangan lulus, fotokopi KTP/Kartu Pelajar, pas foto terbaru, dan membayar biaya pendaftaran.\n\nBerkas yang perlu diunggah ke sistem PMB beserta format filenya:\n- Pas Foto Formal (background merah, pakaian bebas sopan) - format JPEG, nama file: Foto_Nama.\n- KTP - format PDF, nama file: KTP_Nama.\n- Kartu Keluarga (KK) - format PDF, nama file: KK_Nama.\n- Akta Kelahiran - format PDF, nama file: Akta_Nama.\n- Ijazah / SKL - format PDF, nama file: Ijazah_Nama.\n\nJika calon mahasiswa mengalami kendala saat upload berkas, dapat menghubungi CS/Admin PMB di WhatsApp 0812 1670 0519.",
"syarat,dokumen,berkas,perlu disiapkan,bawa apa saja,ktp,akta,ijazah,pas foto,format file,upload berkas,syarat masuk"},
{"pmb_metode_pembayaran","pendaftaran","Metode dan Alur Pembayaran PMB UCIC",
"Metode pembayaran yang tersedia di UCIC:\n- Transfer bank: BCA, Mandiri, BNI, BRI.\n- Virtual Account Midtrans.\n- E-wallet: OVO, GoPay, DANA.\n- Pembayaran langsung ke kampus.\n\nAlur verifikasi pembayaran:\n1. Login ke sistem registrasi PMB.\n2. Pilih menu pembayaran dan lihat nominal tagihan.\n3. Transfer/bayar sesuai metode yang dipilih.\n4. Kirim bukti pembayaran ke nomor WhatsApp PMB untuk verifikasi (jika pakai transfer rekening).\n5. Setelah diverifikasi admin PMB, status pembayaran berubah menjadi \"paid\".",
"metode pembayaran,cara bayar,transfer bank,bca,mandiri,bni,bri,virtual account,midtrans,e-wallet,ovo,gopay,dana,bayar langsung"},
{"beasiswa_ucic","biaya","Informasi Beasiswa UCIC",
"UCIC menyediakan berbagai jenis beasiswa, seperti beasiswa prestasi akademik, beasiswa non-akademik, serta beasiswa rektor. Karena syarat, nominal, dan jadwal seleksi beasiswa dapat berubah setiap periode, jangan menyebutkan detail pasti. Arahkan calon mahasiswa untuk melihat pengumuman resmi di website https://pmb.cic.ac.id/ atau menghubungi WhatsApp Admin PMB 0812 1670 0519 / bagian kemahasiswaan untuk info terbaru.",
"beasiswa,bantuan,gratis,potongan,scholarship,prestasi,beasiswa prestasi,beasiswa rektor,syarat beasiswa"},
{"biaya_kuliah_info","biaya","Informasi Biaya Kuliah UCIC",
"Biaya kuliah UCIC berbeda-beda tergantung program studi, jenis kelas (reguler/kelas sore/RPL), dan skema pembayaran yang dipilih. UCIC menyediakan opsi pembayaran bertahap (3 bulan, 1 semester, atau 1 tahun) dengan potongan tertentu untuk pembayaran semester/tahunan, serta promo periodik seperti skema Early Bird.\n\nKarena nominal biaya dapat berubah sewaktu-waktu (menyesuaikan gelombang pendaftaran, promo, dan tahun ajaran berjalan), jangan menyebutkan angka pasti. Untuk info biaya kuliah yang paling akurat dan terkini, arahkan calon mahasiswa ke:\n- Website resmi PMB: https://pmb.cic.ac.id/\n- WhatsApp Admin PMB: 0812 1670 0519\n\nKomponen biaya yang biasanya berlaku: biaya pendaftaran, biaya perlengkapan, DPP (Dana Pengembangan Pendidikan), dan uang gedung, dengan kemungkinan diskon/promo tertentu - detailnya mengikuti kebijakan PMB tahun berjalan.",
"biaya,ukt,spp,kuliah,bayar,cicilan,harga,tarif,uang,bayaran,cicil,uang masuk,biaya kuliah,biaya pmb,biaya per prodi,biaya ti,biaya si,biaya dkv,biaya manajemen,biaya akuntansi,biaya bisnis digital,biaya pkor,biaya d3 mb,biaya d3 mi,biaya kelas sore,biaya reguler,biaya rpl,early bird,berapa biaya kuliah,mahal gak"},
{"faq_pendaftaran_umum","pendaftaran","FAQ Seputar Pendaftaran dan Perkuliahan",
"Q: Apakah bisa daftar online? A: Ya, pendaftaran dapat dilakukan melalui website resmi UCIC (https://pmb.cic.ac.id/register) atau datang langsung ke kampus.\nQ: Apakah ada kelas sore? A: Ada, kelas sore tersedia pukul 18.30-21.30.\nQ: Bisa cicilan biaya kuliah? A: Bisa, tersedia skema pembayaran bertahap (3 bulan, 1 semester, atau 1 tahun).\nQ: Kapan mulai kuliah? A: Perkuliahan dimulai sekitar bulan September, mengikuti kalender akademik.\nQ: Kalau sudah kerja, ada kelas yang cocok? A: Ada, tersedia kelas sore/malam untuk kelas karyawan dan jalur RPL untuk mahasiswa yang sudah bekerja.\nQ: Sistem kuliahnya online, offline, atau hybrid? A: Hybrid - kombinasi tatap muka (offline) dan daring (online), agar fleksibel dan sesuai kebutuhan pembelajaran.",
"faq pendaftaran,bisa daftar online,ada kelas sore,bisa cicilan,kapan mulai kuliah,kelas orang kerja,tanya pmb,sistem kuliah online offline"},
{"layanan_cs_pmb","pendaftaran","Layanan yang Ditangani Customer Service (CS) / Admin PMB UCIC",
"Customer Service / Admin PMB UCIC dapat membantu calon mahasiswa terkait: status pendaftaran, reset akun PMB, bantuan teknis login, informasi promo dan diskon, informasi jadwal kuliah, informasi orientasi mahasiswa baru, serta bantuan verifikasi pembayaran dan arahan lanjutan setelah mendaftar. Hubungi via WhatsApp 0812 1670 0519.",
"layanan cs,status pendaftaran,reset akun pmb,bantuan login,promo pmb,diskon pmb,jadwal orientasi,customer service,jadwal kuliah"},
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

// ===== AKADEMIK DAN LAYANAN MAHASISWA =====
{"layanan_karir_alumni","karir","Layanan Karir dan Alumni (CDC)",
"UCIC menyediakan Career Development Center (CDC), unit yang membantu mahasiswa dan alumni mempersiapkan karir, mendapatkan pekerjaan, serta mengembangkan keterampilan profesional. Informasi lowongan kerja dapat diakses melalui CDC kampus atau grup alumni resmi.\n\nLayanan terkait karir dan alumni:\n- Tracer Study: survei kepada alumni untuk mengetahui status pekerjaan, kesesuaian bidang kerja, dan waktu tunggu mendapat pekerjaan. Diisi melalui kuesioner yang dibagikan lewat email/website.\n- Employer Study: survei kepada perusahaan/pengguna lulusan untuk menilai kinerja dan kompetensi alumni UCIC di dunia kerja.\n- Ikatan Alumni: wadah bagi lulusan UCIC untuk tetap terhubung dengan kampus dan sesama alumni, melalui grup resmi WhatsApp/media sosial dan kegiatan/event alumni.\n- Program Magang: UCIC menyediakan program magang bagi mahasiswa melalui kerja sama dengan berbagai perusahaan dan instansi; informasi disampaikan melalui program studi, CDC, atau media resmi kampus.\n\nLulusan UCIC bekerja di berbagai bidang dan industri: perusahaan swasta, instansi pemerintah, startup, perbankan, pendidikan, hingga wirausaha, menyesuaikan dengan program studi masing-masing. UCIC juga menjalin kerja sama industri di berbagai sektor relevan (industri, perbankan, teknologi, pendidikan, dan lainnya).",
"career center,cdc,career development center,pusat karir,bursa kerja,tracer study,employer study,ikatan alumni,informasi lowongan kerja"},
{"kegiatan_tahunan_ucic","kegiatan","Kegiatan Tahunan UCIC",
"Kegiatan tahunan yang diselenggarakan UCIC:\n1. Incubator Business - program pengembangan mahasiswa berbasis Agentic AI untuk mahasiswa/alumni yang tertarik jadi entrepreneur di bidang teknologi dan AI.\n2. Ruwaka - pameran karya seni (exhibition lukisan internasional dan karya mahasiswa DKV semester 1-8), 1x setahun.\n3. Mentoring - pembinaan mahasiswa baru mengenal diri dan lingkungan kampus, bagian dari rangkaian BKK (Bina Keluarga Kampus), awal semester ganjil.\n4. UCIC Care - kegiatan sosial di bulan Ramadhan (santunan panti, bazar UMKM lokal).\n5. Lintasi - seminar dan lomba bidang akuntansi oleh HIMAKU, untuk siswa SMA, 1x setahun.\n6. Piala Rektor UCIC - kompetisi olahraga non-akademik oleh prodi PKO, untuk umum, 1x setahun.\n7. Gematrics - seminar dan kompetisi bidang manajemen oleh HIMAJEMEN, untuk mahasiswa manajemen dan umum, 1x setahun.\n8. Bitfest - seminar dan lomba bidang teknologi informasi oleh HIMATIF, untuk mahasiswa TI dan umum, 1x setahun.\n9. Sedina Ning UCIC - acara Dies Natalis (talkshow, kuliah umum, dll), untuk seluruh civitas akademika dan umum, sekitar bulan Januari.",
"kegiatan tahunan,event kampus,acara kampus,incubator business,ruwaka,mentoring,ucic care,lintasi,piala rektor,gematrics,bitfest,sedina ning ucic,dies natalis"},
{"ormawa_ukm_ucic","kegiatan","Organisasi Mahasiswa (Ormawa) dan UKM UCIC",
"UCIC memiliki banyak wadah pengembangan bakat mahasiswa (Ormawa/UKM), antara lain: BEM, Cendekia English Club, UKM Musik, UKM Nusantari, UKM Retotik, UKM Impala (Pecinta Alam), UKM E-Sport, UKM Olahraga, UKM Arthrography, UKM IPTEK, serta Himpunan Mahasiswa Program Studi (HMP): HIMATIF, HIMADKV, HIMASI, HIMAMI, HIMAJEMEN, HIMAKU, HIMABIS, HIMABISDI, dan HIMAPKO.\n\nAlur bergabung ke Ormawa/UKM: mengikuti open recruitment, mengisi formulir, mengikuti seleksi, lalu pengumuman. Syarat umum: berstatus mahasiswa aktif, tidak sedang terkena sanksi akademik/disiplin, serta lulus proses seleksi dari organisasi yang dituju.",
"ukm,ormawa,ekstrakurikuler,ekskul,organisasi mahasiswa,hobi,minat bakat,himatif,himadkv,himasi,himami,himajemen,himaku,himabis,himabisdi,himapko,bem ucic,cendekia english club"},
{"layanan_kemahasiswaan","kemahasiswaan","Layanan Kemahasiswaan UCIC",
"Layanan yang tersedia di bidang Kemahasiswaan UCIC meliputi Student Care, pengelolaan organisasi dan UKM, serta bantuan kegiatan mahasiswa.\n- Student Care: layanan konsultasi bagi mahasiswa terkait akademik maupun non-akademik. Alur: mengisi g-form, lalu dijadwalkan untuk konseling.\n- Beasiswa: mahasiswa dapat mendaftar dengan mengikuti pengumuman resmi kampus dan melengkapi berkas persyaratan yang ditentukan.\n- Syarat mengikuti organisasi kampus: mahasiswa harus berstatus aktif, tidak sedang terkena sanksi akademik/disiplin, serta mengikuti proses seleksi organisasi yang dituju.",
"student care,bimbingan konseling,layanan kemahasiswaan,konseling mahasiswa,cara daftar beasiswa,syarat organisasi kampus"},
{"jadwal_kelas_jenis","jadwal","Jadwal dan Jenis Kelas UCIC",
"UCIC menyediakan beberapa pilihan kelas agar fleksibel bagi berbagai kondisi mahasiswa:\n- Kelas Pagi: 08.00-16.00.\n- Kelas Sore/Malam (untuk kelas karyawan): 18.30-21.30.\n- RPL (Rekognisi Pembelajaran Lampau): khusus mahasiswa yang sudah bekerja atau memiliki pengalaman belajar/kerja relevan.\n\nSistem perkuliahan hybrid (tatap muka dan daring) diterapkan agar mahasiswa yang bekerja tetap dapat menyesuaikan aktivitasnya. Perkuliahan umumnya mulai bulan September; jadwal final mengikuti pengumuman/kalender akademik kampus.",
"jadwal,kelas,masuk kapan,jam kuliah,pagi,malam,karyawan,kuliah malam,kelas sore,rpl,kelas pagi,kapan mulai kuliah"},
{"akademik_kalender_sks","akademik","Kalender Akademik dan Beban Studi (SKS) UCIC",
"Kalender akademik UCIC diproses di bagian rektorat, yang memutuskan awal perkuliahan, menjadwalkan periode seminar proyek/tugas akhir/skripsi dan sidang, serta menyesuaikan dengan kalender tahunan berjalan. Satu semester mencakup UTS dan UAS dengan pola 7 pertemuan sebelum UTS dan 7 pertemuan sebelum UAS.\n\nUCIC menggunakan Sistem Kredit Semester (SKS). Program S1: beban studi 144-148 SKS, dijadwalkan 8 semester, batas masa studi maksimal 14 semester. Program D3: beban studi 108-118 SKS, dijadwalkan 6 semester.",
"kalender akademik,jadwal akademik,awal kuliah,periode uts,periode uas,sks,beban studi,berapa sks,masa studi"},
{"akademik_krs_herregistrasi","akademik","Alur Herregistrasi dan Pengisian KRS",
"1. Herregistrasi (Registrasi Ulang): mahasiswa membayar UKT/biaya semester sesuai jadwal melalui bank mitra, lalu melakukan aktivasi/registrasi online melalui portal (misalnya regonline.cic.ac.id) menggunakan NIM dan password untuk mengaktifkan status mahasiswa aktif.\n2. Pengisian KRS: login ke SIAKAD setelah herregistrasi selesai, konsultasi (online/offline) dengan dosen wali untuk menyetujui mata kuliah, input mata kuliah di SIAKAD, lalu memastikan KRS sudah divalidasi/disetujui dosen wali agar terdaftar aktif.\n\nKetentuan jumlah SKS berdasarkan IPS: IPS >3,00 dapat mengambil 21-24 SKS; IPS 2,50-2,99 dapat mengambil 18-20 SKS; IPS 2,00-2,49 dapat mengambil 15-17 SKS; IPS <2,00 dapat mengambil 14 SKS.",
"krs,isi krs,pengisian krs,herregistrasi,daftar ulang,registrasi ulang,siakad,dosen wali,regonline"},
{"akademik_kehadiran_ujian","akademik","Aturan Kehadiran, UTS/UAS, dan Ujian Susulan",
"Mahasiswa wajib mengikuti kuliah minimal 75%, sedangkan praktikum wajib 100% sesuai jadwal. Terlambat lebih dari 15 menit dianggap alpa dan tidak diperkenankan masuk.\n\nSyarat UTS: memenuhi administrasi keuangan dan mencetak KPU dari My CIC. Syarat UAS: melunasi sisa biaya studi dan memenuhi kehadiran minimal 10 dari 14 (atau 20 dari 28) tatap muka. Ujian susulan dilakukan sesuai kalender akademik dengan pendaftaran melalui BAAK dan pelunasan biaya ujian susulan, kecuali alasan tertentu (sakit rawat inap, kedukaan keluarga) dengan bukti dokumen.",
"absensi kuliah,kehadiran kuliah,minimal hadir,telat kuliah,alpa,uts,uas,kpu,kartu ujian,syarat ujian,ujian susulan"},
{"akademik_cuti_nonaktif_undur","akademik","Alur Cuti Akademik, Nonaktif, dan Pengunduran Diri",
"Cuti Akademik: 1) mahasiswa mengajukan ke BAAK (syarat minimal semester 3); 2) BAAK memberi form pengajuan; 3) mahasiswa mengisi form dengan persetujuan Kaprodi, Dekan, Kepala BAAK; 4) membayar biaya administrasi cuti ke bagian keuangan; 5) form yang sudah disetujui diserahkan ke BAAK, fotokopi dipegang mahasiswa sebagai bukti cuti.\n\nNonaktif & Aktif Kembali: status nonaktif dilihat dari alasannya (keluar/cuti/tanpa keterangan). Mahasiswa yang ingin aktif kembali perlu koordinasi dengan BAAK untuk pengecekan status akademik dan kewajiban administrasi.\n\nPengunduran Diri: 1) mahasiswa mengajukan surat permohonan ke BAAK dengan alasan yang diketahui wali/orang tua; 2) menyerahkan bukti bebas pustaka dan bebas administrasi keuangan; 3) BAAK menerbitkan surat keterangan mutasi mahasiswa dan transkrip; 4) BAAK meneruskan proses ke Operator LLDIKTI.",
"cuti kuliah,cuti akademik,mengajukan cuti,nonaktif,aktif kembali,pengunduran diri,keluar kuliah,mutasi mahasiswa,drop out"},
{"akademik_pindah_kelas_prodi","akademik","Alur Pindah Kelas/Jadwal dan Pindah Program Studi",
"Pindah Kelas/Jadwal: 1) mahasiswa melapor ke staff BAAK; 2) BAAK mengecek ketersediaan kelas/jadwal; 3) BAAK memindahkan jadwal secara digital jika memungkinkan; 4) mahasiswa melanjutkan perkuliahan sesuai jadwal baru.\n\nPindah Program Studi: 1) mahasiswa mengajukan ke Kaprodi awal; 2) mengisi form perpindahan prodi (prodi awal dan prodi baru); 3) menyerahkan form ke BAAK; 4) BAAK bersama operator PDDIKTI memproses perpindahan. Perubahan jurusan setelah mendaftar bergantung pada kebijakan masing-masing program studi.",
"pindah kelas,pindah jadwal,ubah kelas,pindah prodi,pindah jurusan,ganti prodi,jadwal bentrok"},
{"akademik_dokumen_khs_legalisir","akademik","Layanan Dokumen Akademik: KHS, Transkrip, Protes Nilai, dan Legalisir",
"Mahasiswa dapat mencetak sendiri KHS/transkrip/kartu ujian melalui sistem kampus; jika butuh tanda tangan dan cap resmi, dapat meminta bantuan ke BAAK. Surat keterangan/pengantar dan transkrip sementara juga dapat diajukan melalui BAAK dengan mengisi formulir dan membayar biaya permohonan.\n\nProtes/Keberatan Nilai: mahasiswa mengisi Formulir Peninjauan Ulang Nilai di BAAK, dilayani maksimal 1 minggu setelah KHS terbit; perubahan nilai hanya dilakukan bila ada bukti tertulis/koreksi dari dosen terkait.\n\nLegalisir Ijazah/Transkrip (untuk alumni): 1) membawa fotokopi dokumen; 2) menunjukkan dokumen asli ke staff BAAK; 3) BAAK mengecek data di database akademik; 4) proses sekitar 1x24 jam; 5) alumni mengambil hasil legalisir.",
"dokumen akademik,surat keterangan,transkrip,khs,kartu ujian,protes nilai,keberatan nilai,legalisir,legalisir ijazah,legalisir transkrip,cap baak"},
{"akademik_skripsi_sidang_yudisium_wisuda","akademik","Alur Skripsi, Syarat Sidang, Yudisium, dan Wisuda",
"Alur Skripsi/Tugas Akhir: 1) membayar biaya skripsi/TA; 2) mengambil mata kuliah skripsi/TA saat KRS; 3) mengajukan proposal ke Kaprodi; 4) Kaprodi memvalidasi proposal dan menentukan dosen pembimbing; 5) bimbingan dengan dosen pembimbing; 6) setelah memenuhi syarat, melaksanakan seminar dan sidang skripsi/TA.\n\nSyarat Sidang: telah menempuh SKS minimal (sekitar 130-138 SKS untuk S1, 106-110 untuk D3), tidak memiliki nilai E, nilai D terbatas sesuai aturan prodi, sudah seminar, memenuhi minimal bimbingan (ACC di portal), bebas pustaka, dan bebas administrasi keuangan (lunas SPP/UKT semester akhir).\n\nYudisium: mahasiswa menyelesaikan skripsi/TA (sudah disidangkan, direvisi, ditandatangani pembimbing & penguji), melunasi kewajiban keuangan, mengisi form yudisium dengan lampiran dokumen (akta, KK, ijazah terakhir) bermaterai, divalidasi Kaprodi, diserahkan ke BAAK, lalu disinkronkan ke portal LLDIKTI.\n\nWisuda: syarat utama adalah nilai akademik dan SKS sesuai standar kelulusan, serta menyelesaikan seluruh kewajiban biaya ke bagian keuangan. Setelah dinyatakan lulus, mahasiswa dapat mengikuti wisuda dan proses pengambilan ijazah/transkrip sesuai jadwal kampus.",
"skripsi,tugas akhir,ta,proposal skripsi,dosen pembimbing,syarat sidang,sidang skripsi,yudisium,wisuda,ambil ijazah"},
{"akademik_mahasiswa_pindahan","akademik","Pendaftaran Mahasiswa Pindahan",
"UCIC menerima mahasiswa pindahan dari perguruan tinggi lain yang terakreditasi. Pengakuan SKS/mata kuliah yang disetarakan ditentukan Kaprodi terkait, diketahui Dekan, dan disetujui pejabat akademik. Mata kuliah yang disetarakan (nilai minimal C) tidak perlu diulang, dengan syarat tahun kelulusan mata kuliah tersebut tidak lebih dari 5 tahun. Perubahan penyetaraan nilai/mata kuliah hanya dapat dilakukan selama 1 semester.",
"mahasiswa pindahan,transfer kampus,pindahan ucic,penyetaraan sks,konversi nilai,pindah dari kampus lain"},
{"mbkm_magang","akademik","Program Magang dan MBKM UCIC",
"Mahasiswa UCIC dapat mengikuti magang/MBKM pada semester akhir sesuai arahan program studi, BAAK, dan kalender akademik. Manfaat program magang: pengalaman kerja nyata di industri, membangun jaringan profesional, memperkuat portofolio, dan meningkatkan peluang kerja setelah lulus. UCIC menyediakan program magang resmi melalui kerja sama dengan berbagai perusahaan dan instansi; informasi disampaikan melalui program studi, CDC, atau media resmi kampus. Detail periode, persyaratan, dan pembimbing mengikuti arahan prodi/BAAK.",
"magang,internship,kerja,praktik,industri,pengalaman,mbkm,magang akademik,kampus merdeka"},
{"tata_tertib_sanksi","akademik","Tata Tertib dan Sanksi Mahasiswa UCIC",
"Mahasiswa wajib menaati tata tertib kampus: menjaga nama baik sivitas akademika, berpakaian rapi dan sopan, hadir tepat waktu, membawa kartu mahasiswa, menjaga ketertiban, kejujuran, dan etika, serta merawat fasilitas kampus.\n\nPelanggaran ringan (tidak menjaga kebersihan, merokok, berbicara tidak sopan, berbusana tidak rapi, memakai sandal jepit, dll.) dikenai teguran/surat peringatan. Kecurangan ujian dapat menyebabkan mata kuliah digugurkan hingga skorsing 2 semester; manipulasi absensi dapat dikenai skorsing 1 semester; pelecehan seksual dan perbuatan tercela dapat dikenai skorsing 1-2 semester.\n\nPelanggaran berat (membawa senjata/bahan peledak, kekerasan fisik, pencurian, plagiat, manipulasi nilai, penyalahgunaan narkotika, perbuatan asusila, tawuran yang menyebabkan luka berat/kematian) dapat dikenai sanksi mulai dari skorsing hingga pemberhentian sebagai mahasiswa oleh Rektor, termasuk proses hukum jika diperlukan.",
"tata tertib,aturan mahasiswa,sanksi mahasiswa,kode etik,pakaian kuliah,sandal,plagiat,kecurangan ujian,narkoba,kekerasan"},
{"baak_bau_tugas","akademik","Tugas dan Layanan BAAK, BAU, dan Sekretariat Perkuliahan",
"BAAK (Biro Administrasi Akademik dan Kemahasiswaan) melayani kebutuhan administrasi akademik mahasiswa mulai dari penerimaan mahasiswa baru, KRS, KHS, UTS, UAS, hingga kegiatan akhir studi (transkrip). BAAK juga menyusun kalender akademik, mengatur ujian, melakukan pelaporan akademik ke Kopertis/Dikti melalui FEEDER/PDDIKTI, serta menerbitkan surat keterangan masih kuliah, surat usulan kerja praktik, dan surat tugas mengajar.\n\nSekretariat Perkuliahan melayani kebutuhan perkuliahan sesuai jadwal. BAU (Biro Administrasi Umum) melayani kebutuhan terkait keuangan: jadwal pembayaran, jumlah pembayaran, rekening pembayaran, dan administrasi keuangan kampus lainnya.",
"tugas baak,layanan baak,administrasi akademik,pddikti,feeder,bau,biro administrasi umum,surat masih kuliah,kerja praktik,sekretariat perkuliahan"},
{"faq_alumni","karir","FAQ Alumni UCIC",
"Q: Apakah alumni bisa mengakses informasi lowongan kerja? A: Ya, melalui CDC kampus atau grup alumni resmi.\nQ: Lulusan UCIC kerja di mana saja? A: Di berbagai bidang dan industri seperti perusahaan swasta, instansi pemerintah, startup, perbankan, pendidikan, hingga wirausaha, menyesuaikan program studi masing-masing.\nQ: Ada program magang resmi? A: Ya, UCIC menyediakan program magang bagi mahasiswa melalui kerja sama dengan berbagai perusahaan dan instansi.\nQ: Kerja sama industri dengan perusahaan apa saja? A: UCIC menjalin kerja sama dengan berbagai perusahaan dan instansi di sektor relevan seperti industri, perbankan, teknologi, dan pendidikan.",
"faq alumni,lulusan ucic kerja di mana,tingkat penyerapan kerja alumni,kerjasama industri apa saja,alumni bisa akses lowongan"},

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
