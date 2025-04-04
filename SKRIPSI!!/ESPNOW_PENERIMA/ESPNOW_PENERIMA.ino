#include <esp_now.h>
#include <WiFi.h>

void onReceive(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
    Serial.print("Data diterima: ");
    Serial.println(*incomingData);
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK) {
        Serial.println("Gagal menginisialisasi ESP-NOW!");
        return;
    }
    esp_now_register_recv_cb(onReceive);
}

void loop() {}
