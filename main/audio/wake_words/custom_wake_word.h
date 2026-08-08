#ifndef CUSTOM_WAKE_WORD_H
#define CUSTOM_WAKE_WORD_H

#include <esp_attr.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_mn_iface.h>
#include <esp_mn_models.h>
#include <model_path.h>

#include <deque>
#include <string>
#include <vector>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include "audio_codec.h"
#include "wake_word.h"
#include "wake_word_audio_cache.h"

class CustomWakeWord : public WakeWord {
public:
    CustomWakeWord();
    ~CustomWakeWord();

    bool Initialize(AudioCodec* codec, srmodel_list_t* models_list);
    void Feed(const std::vector<int16_t>& data);
    void FeedMono(const int16_t* data, size_t samples);
    void OnWakeWordDetected(std::function<void(const std::string& wake_word)> callback);
    void Start();
    void Stop();
    size_t GetFeedSize();
    void EncodeWakeWordData();
    bool GetWakeWordOpus(std::vector<uint8_t>& opus);
    const std::string& GetLastDetectedWakeWord() const { return last_detected_wake_word_; }

private:
    struct Command {
        std::string command;
        std::string text;
        std::string action;
    };

    // multinet 相关成员变量
    esp_mn_iface_t* multinet_ = nullptr;
    model_iface_data_t* multinet_model_data_ = nullptr;
    srmodel_list_t *models_ = nullptr;
    bool owns_models_ = false;
    char* mn_name_ = nullptr;
    std::string language_ = "cn";
    int duration_ = 3000;
    float threshold_ = 0.2;
    std::deque<Command> commands_;

    // ----------------------------------------------------------------------
    // Gerbang tenaga suara
    //
    // Nilai kemiripan tidak dapat memisahkan kata pemicu asli dari yang palsu.
    // Pada pengukuran di perangkat, kata pemicu asli bernilai 0,139 sampai
    // 0,257 sedangkan yang palsu bernilai 0,162 sampai 0,207, sehingga
    // rentangnya bertumpang tindih seluruhnya. Menaikkan ambang kemiripan
    // hanya akan ikut membuang panggilan yang benar.
    //
    // Yang membedakan keduanya adalah kenyaringan. Pengguna memanggil dari
    // jarak dekat, sedangkan percakapan orang lain di ruangan ramai terdengar
    // jauh lebih pelan. Gerbang ini menolak deteksi yang tenaganya di bawah
    // ambang, sehingga suara latar tidak lagi membangunkan perangkat.
    // ----------------------------------------------------------------------
    void PerbaruiTenaga(const int16_t* data, size_t samples);
    bool TenagaCukup() const;

    // Tenaga tertinggi pada beberapa potongan terakhir, dalam satuan RMS.
    // Riwayat dipakai supaya awal kata yang pelan tidak menggagalkan gerbang
    // ketika bagian tengahnya sudah nyaring.
    static constexpr int kJumlahRiwayatTenaga = 8;
    int16_t riwayat_tenaga_[kJumlahRiwayatTenaga] = {0};
    int indeks_tenaga_ = 0;
    int64_t waktu_tolak_terakhir_us_ = 0;
 
    std::function<void(const std::string& wake_word)> wake_word_detected_callback_;
    AudioCodec* codec_ = nullptr;
    std::string last_detected_wake_word_;
    std::atomic<bool> running_ = false;
    std::vector<int16_t> input_buffer_;
    std::mutex input_buffer_mutex_;

    TaskHandle_t wake_word_encode_task_ = nullptr;
    StaticTask_t* wake_word_encode_task_buffer_ = nullptr;
    StackType_t* wake_word_encode_task_stack_ = nullptr;
    WakeWordAudioCache wake_word_audio_cache_;
    std::deque<std::vector<uint8_t>> wake_word_opus_;
    std::mutex wake_word_mutex_;
    std::condition_variable wake_word_cv_;

    void FeedSamples(const int16_t* data, size_t samples, bool mono);
    void ParseWakenetModelConfig();
};

#endif
