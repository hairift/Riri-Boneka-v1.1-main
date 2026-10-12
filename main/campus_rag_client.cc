#include "campus_rag_client.h"

#include "boards/common/board.h"
#include "settings.h"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <memory>
#include <string>

#include <cJSON.h>
#include <esp_log.h>
#include <esp_timer.h>
#include <sdkconfig.h>

#ifndef CONFIG_CAMPUS_RAG_ENABLE
#define CONFIG_CAMPUS_RAG_ENABLE 0
#endif

// Server RAG memakai alamat yang sama dengan layanan Riri lainnya, sehingga
// hanya ada satu alamat yang perlu diisi pengguna di menuconfig.
#ifndef CONFIG_RIRI_SERVER_URL
#define CONFIG_RIRI_SERVER_URL ""
#endif

#ifndef CONFIG_CAMPUS_RAG_TIMEOUT_MS
#define CONFIG_CAMPUS_RAG_TIMEOUT_MS 1800
#endif

#define TAG "CampusRAG"

namespace {

constexpr int kCampusRagHttpConnectId = 3;
constexpr int kCampusRagMaxResponseBytes = 12288;
constexpr int64_t kCampusRagFailureCooldownUs = 60LL * 1000LL * 1000LL;
int64_t s_campus_rag_disabled_until_us = 0;

bool CampusRagInCooldown() {
    return esp_timer_get_time() < s_campus_rag_disabled_until_us;
}

void MarkCampusRagFailure() {
    s_campus_rag_disabled_until_us = esp_timer_get_time() + kCampusRagFailureCooldownUs;
}

void MarkCampusRagSuccess() {
    s_campus_rag_disabled_until_us = 0;
}

std::string UrlEncode(const char* input) {
    static const char* hex = "0123456789ABCDEF";
    std::string out;
    if (input == nullptr) {
        return out;
    }
    for (const unsigned char* ptr = reinterpret_cast<const unsigned char*>(input); *ptr; ++ptr) {
        unsigned char c = *ptr;
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
            (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.' || c == '~') {
            out.push_back(static_cast<char>(c));
        } else if (c == ' ') {
            out.push_back('+');
        } else {
            out.push_back('%');
            out.push_back(hex[(c >> 4) & 0x0F]);
            out.push_back(hex[c & 0x0F]);
        }
    }
    return out;
}

std::string TrimTrailingSlash(std::string value) {
    while (!value.empty() && value.back() == '/') {
        value.pop_back();
    }
    return value;
}

std::string GetConfiguredServerUrl() {
    Settings settings("campus_rag", false);
    std::string url = settings.GetString("url", CONFIG_RIRI_SERVER_URL);
    if (url.empty()) {
        url = CONFIG_RIRI_SERVER_URL;
    }
    return TrimTrailingSlash(url);
}

bool JsonStringEquals(cJSON* item, const char* expected) {
    return item != nullptr && cJSON_IsString(item) && item->valuestring != nullptr &&
        strcmp(item->valuestring, expected) == 0;
}

bool ReadHttpLimited(std::unique_ptr<Http>& http, std::string& response) {
    response.clear();
    response.reserve(4096);

    char chunk[512];
    int total = 0;
    while (total < kCampusRagMaxResponseBytes) {
        int to_read = std::min(static_cast<int>(sizeof(chunk)), kCampusRagMaxResponseBytes - total);
        // Pembaruan hulu v2.5.2: Read mengembalikan NetworkResult<int>.
        auto read = http->Read(chunk, to_read);
        if (!read || *read <= 0) {
            break;
        }
        response.append(chunk, *read);
        total += *read;
    }

    if (total >= kCampusRagMaxResponseBytes) {
        ESP_LOGW(TAG, "Respons Campus RAG melebihi batas aman %d byte", kCampusRagMaxResponseBytes);
        response.clear();
        return false;
    }
    return !response.empty();
}

}  // namespace

const char* QueryCampusRagServer(const char* query, char* buf, int buf_size) {
    if (!CONFIG_CAMPUS_RAG_ENABLE || query == nullptr || buf == nullptr || buf_size < 256) {
        return nullptr;
    }

    std::string base_url = GetConfiguredServerUrl();
    if (base_url.empty()) {
        return nullptr;
    }
    if (CampusRagInCooldown()) {
        return nullptr;
    }

    auto network = Board::GetInstance().GetNetwork();
    if (!network) {
        return nullptr;
    }

    std::string url = base_url + "/campus/query?q=" + UrlEncode(query);
    auto http = network->CreateHttp(kCampusRagHttpConnectId);
    if (!http) {
        MarkCampusRagFailure();
        return nullptr;
    }

    http->SetTimeout(CONFIG_CAMPUS_RAG_TIMEOUT_MS);
    http->SetKeepAlive(false);
    http->SetHeader("User-Agent", "ESP32-Riri-Boneka-CampusRAG/1.0");
    http->SetHeader("Accept", "application/json");

    if (!http->Open("GET", url)) {
        ESP_LOGW(TAG, "Tidak dapat membuka Campus RAG Server: %s", base_url.c_str());
        MarkCampusRagFailure();
        return nullptr;
    }

    // Pembaruan hulu v2.5.2: GetStatusCode mengembalikan NetworkResult<int>.
    auto status_code = http->GetStatusCode();
    if (!status_code) {
        ESP_LOGW(TAG, "Gagal membaca status HTTP Campus RAG Server");
        http->Close();
        MarkCampusRagFailure();
        return nullptr;
    }
    if (*status_code != 200) {
        ESP_LOGW(TAG, "Campus RAG Server status HTTP %d", *status_code);
        http->Close();
        MarkCampusRagFailure();
        return nullptr;
    }

    std::string response;
    bool read_ok = ReadHttpLimited(http, response);
    http->Close();
    if (!read_ok) {
        MarkCampusRagFailure();
        return nullptr;
    }

    cJSON* root = cJSON_Parse(response.c_str());
    if (root == nullptr) {
        ESP_LOGW(TAG, "Respons Campus RAG Server bukan JSON valid");
        MarkCampusRagFailure();
        return nullptr;
    }

    cJSON* status = cJSON_GetObjectItem(root, "status");
    if (!JsonStringEquals(status, "found")) {
        cJSON_Delete(root);
        // Server menjawab dengan benar, hanya datanya yang tidak ada. Ini bukan
        // kegagalan koneksi, jadi jangan menonaktifkan server selama 60 detik
        // karena pertanyaan berikutnya bisa saja ada datanya.
        MarkCampusRagSuccess();
        return nullptr;
    }

    cJSON* tool_text = cJSON_GetObjectItem(root, "tool_text");
    cJSON* answer = cJSON_GetObjectItem(root, "answer");
    const char* text = nullptr;
    if (tool_text != nullptr && cJSON_IsString(tool_text) && tool_text->valuestring != nullptr) {
        text = tool_text->valuestring;
    } else if (answer != nullptr && cJSON_IsString(answer) && answer->valuestring != nullptr) {
        text = answer->valuestring;
    }

    if (text == nullptr || text[0] == '\0') {
        cJSON_Delete(root);
        MarkCampusRagSuccess();
        return nullptr;
    }

    int written = snprintf(buf, buf_size, "%s", text);
    cJSON_Delete(root);
    if (written <= 0) {
        MarkCampusRagFailure();
        return nullptr;
    }
    if (written >= buf_size) {
        // snprintf sudah memotong dengan aman. Jawaban terpotong tetap lebih
        // berguna daripada tidak ada jawaban sama sekali.
        ESP_LOGW(TAG, "Respons Campus RAG dipotong agar muat di buffer %d byte", buf_size);
    }

    MarkCampusRagSuccess();
    ESP_LOGI(TAG, "Campus RAG Server digunakan untuk query: %s", query);
    return buf;
}
