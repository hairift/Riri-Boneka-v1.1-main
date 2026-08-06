#include "web_search.h"
#include <esp_log.h>
#include <esp_timer.h>
#include <cJSON.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <cstring>
#include <cstdio>
#include <atomic>
#include <memory>
#include <string>
#include <algorithm>
#include <cctype>
#include <vector>

#include "boards/common/board.h"

#define TAG "WebSearch"

// Server Riri menyediakan pencarian web yang jauh lebih kaya daripada penelusuran
// Wikipedia lokal: hasil DuckDuckGo, penyaring berita per wilayah dan rentang
// waktu, serta singgahan agar jawaban cepat dan tidak kena batas laju. Bila
// server tidak dapat dihubungi, firmware tetap memakai penelusuran Wikipedia
// bawaan supaya pertanyaan pengguna tidak pernah gagal total.
#ifndef CONFIG_RIRI_SERVER_URL
#define CONFIG_RIRI_SERVER_URL ""
#endif

#ifndef CONFIG_RIRI_WEB_SEARCH_SERVER
#define CONFIG_RIRI_WEB_SEARCH_SERVER 0
#endif

namespace {

// Satu panggilan tool tidak boleh menghabiskan waktu tunggu server MCP. Tanpa
// batas ini, satu pencarian dapat menembak belasan permintaan HTTP berturut-turut
// sehingga AI keburu menyerah dan menjawab tanpa data.
constexpr int64_t kSearchBudgetUs = 14LL * 1000LL * 1000LL;
int64_t s_search_deadline_us = 0;

void StartSearchBudget() {
    s_search_deadline_us = esp_timer_get_time() + kSearchBudgetUs;
}

bool SearchBudgetExpired() {
    return s_search_deadline_us != 0 && esp_timer_get_time() >= s_search_deadline_us;
}

}  // namespace

static void url_encode(const char* src, char* dst, int dst_size) {
    int j = 0;
    for (int i = 0; src[i] && j < dst_size - 4; i++) {
        unsigned char c = (unsigned char)src[i];
        if (c == ' ') {
            dst[j++] = '+';
        } else if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
                   (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.') {
            dst[j++] = (char)c;
        } else {
            snprintf(dst + j, 4, "%%%02X", c);
            j += 3;
        }
    }
    dst[j] = '\0';
}

static void title_encode(const char* src, char* dst, int dst_size) {
    int j = 0;
    for (int i = 0; src[i] && j < dst_size - 4; i++) {
        unsigned char c = (unsigned char)src[i];
        if (c == ' ') {
            dst[j++] = '_';
        } else if (c >= 0x80 || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
                   (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.' ||
                   c == '(' || c == ')') {
            dst[j++] = (char)c;
        } else {
            snprintf(dst + j, 4, "%%%02X", c);
            j += 3;
        }
    }
    dst[j] = '\0';
}

static bool has_word(const char* text, const char* word) {
    if (!text || !word) return false;
    int tlen = strlen(text);
    int wlen = strlen(word);
    if (wlen == 0 || tlen < wlen) return false;
    for (int i = 0; i <= tlen - wlen; i++) {
        bool match = true;
        for (int j = 0; j < wlen; j++) {
            if (tolower((unsigned char)text[i + j]) != tolower((unsigned char)word[j])) {
                match = false;
                break;
            }
        }
        if (match) return true;
    }
    return false;
}

static bool is_ascii_word_char(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) != 0;
}

static bool has_token(const char* text, const char* token) {
    if (!text || !token) return false;
    int tlen = strlen(text);
    int wlen = strlen(token);
    if (wlen == 0 || tlen < wlen) return false;
    for (int i = 0; i <= tlen - wlen; i++) {
        bool match = true;
        for (int j = 0; j < wlen; j++) {
            if (tolower((unsigned char)text[i + j]) != tolower((unsigned char)token[j])) {
                match = false;
                break;
            }
        }
        if (!match) continue;
        bool left_ok = (i == 0) || !is_ascii_word_char(text[i - 1]);
        bool right_ok = (i + wlen >= tlen) || !is_ascii_word_char(text[i + wlen]);
        if (left_ok && right_ok) return true;
    }
    return false;
}

static void squash_spaces(char* text) {
    if (!text) return;
    int read = 0;
    int write = 0;
    bool prev_space = true;
    while (text[read]) {
        char c = text[read++];
        if (std::isspace(static_cast<unsigned char>(c))) {
            if (!prev_space) text[write++] = ' ';
            prev_space = true;
        } else {
            text[write++] = c;
            prev_space = false;
        }
    }
    if (write > 0 && text[write - 1] == ' ') write--;
    text[write] = '\0';
}

static void replace_all(std::string& text, const char* from, const char* to) {
    if (!from || !*from) return;
    size_t pos = 0;
    size_t from_len = strlen(from);
    while ((pos = text.find(from, pos)) != std::string::npos) {
        text.replace(pos, from_len, to ? to : "");
        pos += strlen(to ? to : "");
    }
}

static void normalize_common_compact_terms(char* text, int text_size) {
    if (!text || text_size <= 0) return;
    std::string value = text;
    replace_all(value, "jawabarat", "jawa barat");
    replace_all(value, "jawatengah", "jawa tengah");
    replace_all(value, "jawatimur", "jawa timur");
    replace_all(value, "dki jakarta", "jakarta");
    replace_all(value, "diyogyakarta", "di yogyakarta");
    replace_all(value, "walikota", "wali kota");
    replace_all(value, "kemenkeu", "kementerian keuangan");
    replace_all(value, "kemendikbud", "kementerian pendidikan kebudayaan");
    strncpy(text, value.c_str(), text_size - 1);
    text[text_size - 1] = '\0';
    squash_spaces(text);
}

static std::string normalize_match_text(const char* raw) {
    std::string text = raw ? raw : "";
    for (char& ch : text) {
        unsigned char value = static_cast<unsigned char>(ch);
        if (std::isalnum(value) || value >= 0x80) {
            ch = static_cast<char>(std::tolower(value));
        } else {
            ch = ' ';
        }
    }
    while (text.find("  ") != std::string::npos) {
        text.replace(text.find("  "), 2, " ");
    }
    while (!text.empty() && text.front() == ' ') text.erase(text.begin());
    while (!text.empty() && text.back() == ' ') text.pop_back();
    return text;
}

static std::vector<std::string> split_terms(const std::string& normalized) {
    std::vector<std::string> terms;
    size_t start = 0;
    while (start < normalized.size()) {
        size_t end = normalized.find(' ', start);
        std::string token = normalized.substr(start, end == std::string::npos ? std::string::npos : end - start);
        if (token.size() > 2) {
            terms.push_back(token);
        }
        if (end == std::string::npos) break;
        start = end + 1;
    }
    return terms;
}

static void push_unique_variant(std::vector<std::string>& variants, const std::string& value) {
    if (value.empty()) return;
    if (std::find(variants.begin(), variants.end(), value) == variants.end()) {
        variants.push_back(value);
    }
}

static std::string strip_html_tags(const char* raw) {
    std::string text = raw ? raw : "";
    std::string clean;
    clean.reserve(text.size());
    bool inside_tag = false;
    for (char ch : text) {
        if (ch == '<') {
            inside_tag = true;
            continue;
        }
        if (ch == '>') {
            inside_tag = false;
            continue;
        }
        if (!inside_tag) {
            clean.push_back(ch);
        }
    }
    return clean;
}

static const char* detect_public_office_role(const char* query) {
    const char* roles[] = {
        "wakil presiden",
        "presiden",
        "gubernur bank indonesia",
        "wakil gubernur",
        "gubernur",
        "wakil ketua",
        "ketua",
        "wakil kepala",
        "kepala",
        "wakil wali kota",
        "wakil walikota",
        "wali kota",
        "walikota",
        "wakil bupati",
        "bupati",
        "wakil menteri",
        "perdana menteri",
        "menteri",
        "kapolri",
        "panglima tni",
        "jaksa agung",
        nullptr
    };
    for (int i = 0; roles[i]; ++i) {
        if (has_token(query, roles[i])) return roles[i];
    }
    return nullptr;
}

static bool has_profile_or_attribute_intent(const char* query) {
    const char* attribute_terms[] = {
        "lulusan", "alumni", "universitas", "kampus", "sekolah", "kuliah", "pendidikan",
        "biografi", "biodata", "profil", "riwayat", "karier", "karir", "lahir",
        "umur", "usia", "agama", "istri", "suami", "anak", "asal", "partai",
        "hobi", "tinggi", "berat", "keluarga", nullptr
    };
    for (int i = 0; attribute_terms[i]; ++i) {
        if (has_token(query, attribute_terms[i])) return true;
    }
    return false;
}

static bool is_public_office_query(const char* query) {
    return detect_public_office_role(query) != nullptr &&
        !has_profile_or_attribute_intent(query);
}

static bool title_matches_public_office_role(const char* query, const char* title) {
    const char* role = detect_public_office_role(query);
    if (!role) return false;
    return has_token(title, role);
}

static bool title_has_specific_office_context(const char* title) {
    const char* office_terms[] = {
        "presiden", "gubernur", "ketua", "kepala", "wali kota", "walikota", "bupati",
        "menteri", "perdana menteri", "kapolri", "panglima", "jaksa agung",
        nullptr
    };
    for (int i = 0; office_terms[i]; ++i) {
        if (has_token(title, office_terms[i])) return true;
    }
    return false;
}

static void clean_query(const char* raw, char* out, int out_size) {
    const char* noise[] = {
        "siapakah", "siapa", "sekarang", "saat ini", "terbaru", "terkini",
        "who is", "current", "the current",
        "2024", "2025", "2026", "2027",
        nullptr
    };

    strncpy(out, raw ? raw : "", out_size - 1);
    out[out_size - 1] = '\0';

    for (int i = 0; out[i]; ++i) {
        unsigned char c = static_cast<unsigned char>(out[i]);
        if (c == '?' || c == '!' || c == ',' || c == '.' || c == ':' || c == ';') {
            out[i] = ' ';
        } else {
            out[i] = static_cast<char>(tolower(c));
        }
    }

    normalize_common_compact_terms(out, out_size);

    // Hapus kata pengisi hanya bila berdiri sebagai kata utuh. Pencocokan
    // substring bebas akan merusak kueri sah seperti "wisuda 2026" atau nama
    // yang kebetulan memuat potongan kata pengisi.
    for (int n = 0; noise[n]; ++n) {
        int nlen = strlen(noise[n]);
        char* pos = out;
        while ((pos = strstr(pos, noise[n])) != nullptr) {
            bool left_ok = (pos == out) || !is_ascii_word_char(pos[-1]);
            bool right_ok = !is_ascii_word_char(pos[nlen]);
            if (left_ok && right_ok) {
                memmove(pos, pos + nlen, strlen(pos + nlen) + 1);
            } else {
                pos += 1;
            }
        }
    }

    char* start = out;
    while (*start == ' ') start++;
    if (start != out) memmove(out, start, strlen(start) + 1);
    int len = strlen(out);
    while (len > 0 && out[len - 1] == ' ') out[--len] = '\0';
    squash_spaces(out);
}

static int score_title(const char* query, const char* title) {
    bool office_query = is_public_office_query(query);

    if (has_token(title, "wakil") && !has_token(query, "wakil")) return -1;
    if (has_token(title, "mantan") && !has_token(query, "mantan")) return -1;
    if (has_token(title, "bekas") && !has_token(query, "bekas")) return -1;

    int score = 0;
    if (has_token(title, "daftar") && !has_token(query, "daftar")) score -= office_query ? 1 : 5;
    if (has_token(title, "pemilihan") && !has_token(query, "pemilihan")) score -= office_query ? 6 : 3;

    if (office_query) {
        if (!title_matches_public_office_role(query, title)) {
            if (!title_has_specific_office_context(title)) return -20;
            score -= 10;
        } else {
            score += 12;
        }
        if (has_word(title, query)) score += 10;
    }

    char qcopy[256];
    strncpy(qcopy, query, sizeof(qcopy) - 1);
    qcopy[sizeof(qcopy) - 1] = '\0';

    char* word = strtok(qcopy, " ");
    while (word) {
        if (strlen(word) > 2 && has_token(title, word)) score += 2;
        word = strtok(nullptr, " ");
    }

    return score;
}

static std::string extract_office_subject(const char* clean_query) {
    std::string subject = clean_query ? clean_query : "";
    const char* role = detect_public_office_role(clean_query);
    if (role && *role) {
        size_t role_pos = subject.find(role);
        if (role_pos != std::string::npos) {
            subject.erase(role_pos, strlen(role));
        }
    }

    const char* noise_terms[] = {
        "indonesia", "provinsi", "kabupaten", "kota", "wilayah", "daerah",
        "sekarang", "saat ini", "terbaru", nullptr
    };
    for (int i = 0; noise_terms[i]; ++i) {
        size_t pos = std::string::npos;
        while ((pos = subject.find(noise_terms[i])) != std::string::npos) {
            subject.erase(pos, strlen(noise_terms[i]));
        }
    }

    char normalized[256];
    strncpy(normalized, subject.c_str(), sizeof(normalized) - 1);
    normalized[sizeof(normalized) - 1] = '\0';
    squash_spaces(normalized);
    return normalized;
}

static void build_search_variants(const char* clean_query, std::vector<std::string>& variants) {
    std::string clean = clean_query ? clean_query : "";
    if (clean.empty()) return;

    push_unique_variant(variants, clean);
    push_unique_variant(variants, std::string("\"") + clean + "\"");
    push_unique_variant(variants, std::string("intitle:\"") + clean + "\"");

    const char* role = is_public_office_query(clean.c_str()) ? detect_public_office_role(clean.c_str()) : nullptr;
    if (role) {
        std::string role_text = role;
        std::string subject = extract_office_subject(clean.c_str());
        std::string combined = subject.empty() ? role_text : role_text + " " + subject;

        push_unique_variant(variants, combined);
        push_unique_variant(variants, std::string("\"") + combined + "\"");
        push_unique_variant(variants, "daftar " + combined);
        push_unique_variant(variants, std::string("\"daftar ") + combined + "\"");
        push_unique_variant(variants, std::string("intitle:\"") + combined + "\"");
        push_unique_variant(variants, std::string("intitle:\"daftar ") + combined + "\"");
        push_unique_variant(variants, combined + " saat ini");
        push_unique_variant(variants, combined + " sekarang");
        push_unique_variant(variants, combined + " petahana");
        push_unique_variant(variants, "petahana " + combined);
        if (!subject.empty()) {
            push_unique_variant(variants, subject + " " + role_text);
        }
    }
}

static int score_text_match_terms(const char* query, const char* text) {
    std::string normalized_query = normalize_match_text(query);
    std::string normalized_text = normalize_match_text(text);
    if (normalized_query.empty() || normalized_text.empty()) {
        return 0;
    }

    int score = 0;
    if (normalized_text.find(normalized_query) != std::string::npos) {
        score += 12;
    }

    for (const auto& token : split_terms(normalized_query)) {
        if (has_token(normalized_text.c_str(), token.c_str())) {
            score += 3;
        }
    }

    return score;
}

static int score_search_hit(const char* query, const char* title, const char* snippet) {
    int score = score_title(query, title);
    score += score_text_match_terms(query, title) * 2;
    score += score_text_match_terms(query, snippet);

    if (is_public_office_query(query)) {
        if (has_token(snippet, "petahana") || has_token(snippet, "incumbent")) {
            score += 10;
        }
        if (title_matches_public_office_role(query, title)) {
            score += 8;
        }
    }

    return score;
}

static int score_summary_candidate(const char* query,
                                   const char* title,
                                   const char* description,
                                   const char* extract) {
    int score = score_title(query, title);
    score += score_text_match_terms(query, title) * 2;
    score += score_text_match_terms(query, description);
    score += score_text_match_terms(query, extract);

    if (is_public_office_query(query)) {
        const char* role = detect_public_office_role(query);
        bool mentions_role = role != nullptr &&
            (has_token(title, role) || has_token(description, role) || has_token(extract, role));
        bool has_office_context = title_has_specific_office_context(title) ||
            title_has_specific_office_context(description) ||
            title_has_specific_office_context(extract);

        if (mentions_role) {
            score += 14;
        } else if (!has_office_context) {
            score -= 24;
        }

        if (has_token(description, "petahana") || has_token(extract, "petahana") ||
            has_token(extract, "incumbent")) {
            score += 12;
        }

        if ((has_token(description, "provinsi") || has_token(description, "kota") || has_token(description, "kabupaten")) &&
            !mentions_role && !has_office_context) {
            score -= 12;
        }
    }

    return score;
}

static bool summary_looks_like_generic_place(const char* title,
                                             const char* description,
                                             const char* extract) {
    const char* generic_terms[] = {
        "provinsi di", "kabupaten di", "kota di", "wilayah di", "pulau di",
        "province of", "regency of", "city in", nullptr
    };
    for (int i = 0; generic_terms[i]; ++i) {
        if (has_word(description, generic_terms[i]) || has_word(extract, generic_terms[i])) {
            if (!title_has_specific_office_context(title) &&
                !title_has_specific_office_context(description) &&
                !title_has_specific_office_context(extract)) {
                return true;
            }
        }
    }
    return false;
}

static std::string HttpGet(const char* url, int timeout_ms = 4000) {
    auto& board = Board::GetInstance();
    auto network = board.GetNetwork();
    if (!network) return "";
    auto http = network->CreateHttp(0);
    if (!http) return "";
    http->SetTimeout(timeout_ms);
    http->SetKeepAlive(false);
    http->SetHeader("User-Agent", "ESP32-Riri-Boneka/1.0");
    if (!http->Open("GET", url)) return "";
    if (http->GetStatusCode() != 200) {
        http->Close();
        return "";
    }
    std::string response;
    response.reserve(4096);
    char chunk[512];
    int total = 0;
    constexpr int kMaxHttpGetBytes = 12288;
    while (total < kMaxHttpGetBytes) {
        int to_read = std::min((int)sizeof(chunk), kMaxHttpGetBytes - total);
        int read = http->Read(chunk, to_read);
        if (read <= 0) break;
        response.append(chunk, read);
        total += read;
    }
    http->Close();
    return response;
}

static std::string HttpGetPartial(const char* url, int max_bytes, int timeout_ms = 4000) {
    auto& board = Board::GetInstance();
    auto network = board.GetNetwork();
    if (!network) return "";
    auto http = network->CreateHttp(0);
    if (!http) return "";
    http->SetTimeout(timeout_ms);
    http->SetKeepAlive(false);
    http->SetHeader("User-Agent", "ESP32-Riri-Boneka/1.0");
    if (!http->Open("GET", url)) return "";
    if (http->GetStatusCode() != 200) {
        http->Close();
        return "";
    }
    std::string result;
    result.reserve(max_bytes);
    char chunk[512];
    int total = 0;
    while (total < max_bytes) {
        int to_read = std::min((int)sizeof(chunk), max_bytes - total);
        int read = http->Read(chunk, to_read);
        if (read <= 0) break;
        result.append(chunk, read);
        total += read;
    }
    http->Close();
    return result;
}

static std::string FindWikiLinkNear(const std::string& raw, size_t from, size_t max_distance) {
    if (from == std::string::npos) return "";
    size_t br = raw.find("[[", from);
    if (br == std::string::npos || br - from > max_distance) return "";
    br += 2;
    size_t end = raw.find("]]", br);
    if (end == std::string::npos || end - br > 100) return "";
    size_t pipe = raw.find('|', br);
    if (pipe != std::string::npos && pipe < end) end = pipe;
    return raw.substr(br, end - br);
}

static std::string FindIncumbent(const std::string& raw) {
    const char* markers[] = {
        "| incumbent", "|incumbent", "| petahana", "|petahana",
        "incumbent", "Incumbent", "petahana", "Petahana",
        nullptr
    };
    for (int i = 0; markers[i]; ++i) {
        size_t pos = raw.find(markers[i]);
        if (pos == std::string::npos) continue;
        std::string name = FindWikiLinkNear(raw, pos, 200);
        if (!name.empty()) return name;
    }
    return "";
}

struct WikiCandidate {
    std::string title;
    std::string host;
    std::string snippet;
    int score = -100;
    bool role_match = false;
};

static void add_candidate(std::vector<WikiCandidate>& candidates,
                          const char* host,
                          const char* title,
                          const char* snippet,
                          int score,
                          bool role_match) {
    for (auto& candidate : candidates) {
        if (candidate.host == host && candidate.title == title) {
            if (score > candidate.score) {
                candidate.score = score;
                candidate.snippet = snippet ? snippet : "";
            }
            candidate.role_match = candidate.role_match || role_match;
            return;
        }
    }
    WikiCandidate candidate;
    candidate.title = title;
    candidate.host = host;
    candidate.snippet = snippet ? snippet : "";
    candidate.score = score;
    candidate.role_match = role_match;
    candidates.push_back(candidate);
}

static void search_wikipedia_titles(const char* host,
                                    const char* search_term,
                                    const char* clean_query,
                                    std::vector<WikiCandidate>& candidates) {
    char encoded[300];
    url_encode(search_term, encoded, sizeof(encoded));
    char url[700];
    snprintf(url, sizeof(url),
        "https://%s/w/api.php?"
        "action=query&list=search&srsearch=%s&srlimit=5&format=json&utf8=1",
        host, encoded);

    std::string resp = HttpGet(url, 3000);
    if (resp.length() < 50) return;

    cJSON* root = cJSON_Parse(resp.c_str());
    if (!root) return;

    cJSON* query = cJSON_GetObjectItem(root, "query");
    if (query) {
        cJSON* search = cJSON_GetObjectItem(query, "search");
        if (search && cJSON_IsArray(search)) {
            cJSON* item = nullptr;
            cJSON_ArrayForEach(item, search) {
                cJSON* title = cJSON_GetObjectItem(item, "title");
                cJSON* snippet = cJSON_GetObjectItem(item, "snippet");
                if (!title || !cJSON_IsString(title)) continue;
                std::string clean_snippet = (snippet && cJSON_IsString(snippet))
                    ? strip_html_tags(snippet->valuestring) : "";
                int score = score_search_hit(clean_query, title->valuestring, clean_snippet.c_str());
                bool role_match = title_matches_public_office_role(clean_query, title->valuestring);
                ESP_LOGD(TAG, "  [%s] '%s' score=%d role_match=%d",
                    host, title->valuestring, score, role_match ? 1 : 0);
                add_candidate(candidates, host, title->valuestring, clean_snippet.c_str(), score, role_match);
            }
        }
    }

    cJSON_Delete(root);
}

// Meminta hasil pencarian ke server Riri. Mengembalikan false bila server tidak
// diaktifkan, tidak menjawab, atau jawabannya kosong, sehingga pemanggil dapat
// melanjutkan ke penelusuran Wikipedia lokal.
void RiriServerWarmUp() {
    std::string base_url = CONFIG_RIRI_SERVER_URL;
    while (!base_url.empty() && base_url.back() == '/') {
        base_url.pop_back();
    }
    if (base_url.empty()) {
        return;
    }

    static std::atomic<bool> sedang_memanaskan{false};
    bool diharapkan = false;
    if (!sedang_memanaskan.compare_exchange_strong(diharapkan, true)) {
        return;
    }

    auto* alamat = new std::string(base_url + "/health");
    // Dijalankan di tugas tersendiri supaya jalur boot tidak ikut menunggu.
    // Prioritas 1 menjaganya tetap di bawah jalur audio waktu nyata.
    if (xTaskCreate([](void* arg) {
            std::unique_ptr<std::string> url(static_cast<std::string*>(arg));
            std::string balasan = HttpGet(url->c_str(), 60000);
            ESP_LOGI(TAG, "Pemanasan server Riri %s",
                     balasan.empty() ? "gagal, permintaan pertama akan lebih lambat" : "selesai");
            sedang_memanaskan.store(false);
            vTaskDelete(nullptr);
        }, "riri_warm", 6144, alamat, 1, nullptr) != pdPASS) {
        delete alamat;
        sedang_memanaskan.store(false);
        ESP_LOGW(TAG, "Tugas pemanasan server Riri tidak dapat dibuat");
    }
}

static bool SearchViaRiriServer(const char* query, char* buf, int buf_size) {
    if (!CONFIG_RIRI_WEB_SEARCH_SERVER) {
        return false;
    }
    std::string base_url = CONFIG_RIRI_SERVER_URL;
    while (!base_url.empty() && base_url.back() == '/') {
        base_url.pop_back();
    }
    if (base_url.empty()) {
        return false;
    }

    char encoded[512];
    url_encode(query, encoded, sizeof(encoded));
    std::string url = base_url + "/search?q=" + encoded + "&region=id-id";

    // Batas waktunya harus melebihi waktu bangun Cloud Run, bukan sekadar waktu
    // jawab server yang sudah panas. Server yang panas menjawab di bawah satu
    // detik, tetapi instance yang baru dibangunkan perlu memuat dua model
    // pembelajaran mesin lebih dahulu. Dengan batas enam detik sebelumnya,
    // pertanyaan berita selalu gagal padahal servernya sehat.
    std::string body = HttpGet(url.c_str(), 25000);
    if (body.empty()) {
        ESP_LOGW(TAG, "Server pencarian Riri tidak menjawab");
        return false;
    }

    cJSON* root = cJSON_Parse(body.c_str());
    if (root == nullptr) {
        ESP_LOGW(TAG, "Jawaban server pencarian bukan JSON valid");
        return false;
    }

    cJSON* answer = cJSON_GetObjectItem(root, "answer");
    bool ok = false;
    if (cJSON_IsString(answer) && answer->valuestring != nullptr && answer->valuestring[0] != 0) {
        snprintf(buf, buf_size, "%s", answer->valuestring);
        ok = true;
        ESP_LOGI(TAG, "Hasil pencarian diambil dari server Riri");
    }
    cJSON_Delete(root);
    return ok;
}

const char* WebSearchDuckDuckGo(const char* query, char* buf, int buf_size) {
    if (!query || !buf || buf_size < 100) return nullptr;
    ESP_LOGI(TAG, "Search: %s", query);

    // Server didahulukan karena hasilnya lebih segar dan lebih lengkap.
    if (SearchViaRiriServer(query, buf, buf_size)) {
        return buf;
    }

    char clean[256];
    clean_query(query, clean, sizeof(clean));
    ESP_LOGI(TAG, "Cleaned: '%s'", clean);

    bool office_query = is_public_office_query(clean);
    char url[700];
    int pos = 0;

    std::vector<WikiCandidate> candidates;
    std::vector<std::string> search_variants;
    build_search_variants(clean, search_variants);

    StartSearchBudget();

    bool has_role_candidate = false;
    for (const auto& variant : search_variants) {
        if (SearchBudgetExpired() && !candidates.empty()) {
            break;
        }
        search_wikipedia_titles("id.wikipedia.org", variant.c_str(), clean, candidates);
        for (const auto& candidate : candidates) {
            if (candidate.role_match) {
                has_role_candidate = true;
                break;
            }
        }
        if (!candidates.empty() && (!office_query || has_role_candidate || candidates.size() >= 6)) {
            break;
        }
    }

    if (candidates.empty() || (office_query && !has_role_candidate)) {
        for (const auto& variant : search_variants) {
            if (SearchBudgetExpired() && !candidates.empty()) {
                break;
            }
            search_wikipedia_titles("en.wikipedia.org", variant.c_str(), clean, candidates);
            for (const auto& candidate : candidates) {
                if (candidate.role_match) {
                    has_role_candidate = true;
                    break;
                }
            }
            if (!candidates.empty() && (!office_query || has_role_candidate || candidates.size() >= 6)) {
                break;
            }
        }
    }

    if (candidates.empty()) {
        snprintf(buf, buf_size, "Tidak ditemukan informasi untuk '%s'.", query);
        return buf;
    }

    std::stable_sort(candidates.begin(), candidates.end(),
        [](const WikiCandidate& lhs, const WikiCandidate& rhs) {
            if (lhs.score != rhs.score) return lhs.score > rhs.score;
            if (lhs.role_match != rhs.role_match) return lhs.role_match > rhs.role_match;
            return lhs.title.size() < rhs.title.size();
        });

    ESP_LOGI(TAG, "Best: '%s' host=%s score=%d role_match=%d",
        candidates.front().title.c_str(),
        candidates.front().host.c_str(),
        candidates.front().score,
        candidates.front().role_match ? 1 : 0);

    int wikitext_checked = 0;
    for (const auto& candidate : candidates) {
        if (office_query && candidate.score < -5) continue;
        if (wikitext_checked >= 4 || SearchBudgetExpired()) break;
        ++wikitext_checked;
        char encoded_title[256];
        title_encode(candidate.title.c_str(), encoded_title, sizeof(encoded_title));
        snprintf(url, sizeof(url),
            "https://%s/w/api.php?"
            "action=parse&page=%s&prop=wikitext&section=0&format=json&utf8=1",
            candidate.host.c_str(), encoded_title);

        std::string wiki_resp = HttpGetPartial(url, 12288, 3000);
        if (wiki_resp.length() <= 100) continue;

        std::string incumbent = FindIncumbent(wiki_resp);
        if (!incumbent.empty()) {
            pos += snprintf(buf + pos, buf_size - pos,
                ">>> PEJABAT SAAT INI: %s <<<\nSumber: Wikipedia '%s'\n",
                incumbent.c_str(), candidate.title.c_str());
            return buf;
        }
    }

    int best_summary_score = -10000;
    std::string best_summary_text;
    int summaries_checked = 0;
    for (const auto& candidate : candidates) {
        if (summaries_checked >= 5) break;
        if (SearchBudgetExpired() && !best_summary_text.empty()) break;
        if (office_query && candidate.score < -8) continue;
        if (office_query && !candidate.role_match && !title_has_specific_office_context(candidate.title.c_str())) {
            continue;
        }

        char encoded_title[256];
        title_encode(candidate.title.c_str(), encoded_title, sizeof(encoded_title));
        snprintf(url, sizeof(url),
            "https://%s/api/rest_v1/page/summary/%s",
            candidate.host.c_str(), encoded_title);

        std::string sum = HttpGet(url, 3000);
        if (sum.length() <= 50) continue;

        cJSON* root = cJSON_Parse(sum.c_str());
        if (!root) continue;

        cJSON* title = cJSON_GetObjectItem(root, "title");
        cJSON* description = cJSON_GetObjectItem(root, "description");
        cJSON* extract = cJSON_GetObjectItem(root, "extract");
        const char* title_text = (title && cJSON_IsString(title)) ? title->valuestring : candidate.title.c_str();
        const char* description_text = (description && cJSON_IsString(description)) ? description->valuestring : "";
        const char* extract_text = (extract && cJSON_IsString(extract)) ? extract->valuestring : "";

        if (strlen(extract_text) > 20) {
            if (office_query && summary_looks_like_generic_place(title_text, description_text, extract_text)) {
                cJSON_Delete(root);
                ++summaries_checked;
                continue;
            }
            int summary_score = candidate.score +
                score_summary_candidate(clean, title_text, description_text, extract_text);
            if (summary_score > best_summary_score) {
                // Penyangga 4KB di tumpukan tugas MCP terlalu berisiko, jadi
                // ringkasan disusun langsung di heap.
                best_summary_text.clear();
                best_summary_text += "[";
                best_summary_text += title_text;
                if (strlen(description_text) > 3) {
                    best_summary_text += " - ";
                    best_summary_text += description_text;
                }
                best_summary_text += "]\n";
                best_summary_text += extract_text;
                best_summary_text += "\n";
                best_summary_score = summary_score;
            }
        }

        cJSON_Delete(root);
        ++summaries_checked;
    }

    if (!best_summary_text.empty()) {
        pos += snprintf(buf + pos, buf_size - pos, "%s", best_summary_text.c_str());
    }

    if (pos < 10 && office_query) {
        snprintf(buf, buf_size,
            "Belum dapat memastikan pejabat saat ini untuk '%s' dari sumber daring yang tersedia. Coba sebutkan jabatan dan wilayah atau instansinya dengan lebih spesifik.",
            query);
        return buf;
    }

    if (pos < 10) {
        snprintf(buf, buf_size, "Tidak ditemukan informasi untuk '%s'.", query);
    }

    return buf;
}
