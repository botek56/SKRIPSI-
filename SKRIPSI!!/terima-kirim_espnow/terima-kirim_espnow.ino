#include <esp_now.h>
#include <WiFi.h>

// MAC Address ESP B (GANTI DENGAN MAC STA DARI ESP B)
uint8_t peerAddress[] = {0x94, 0x54, 0xC5, 0xA9, 0xB0, 0xD0};

// Struktur data yang dikirim
typedef struct struct_message {
    int data;
} struct_message;

struct_message myData;
esp_now_peer_info_t peerInfo;

// Callback untuk menerima data
void onReceive(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
    int receivedData = *incomingData;
    Serial.print("ESP A Menerima Data: ");
    Serial.println(receivedData);
}

// Callback untuk status pengiriman
void onSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("ESP A - Status Pengiriman: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Sukses" : "Gagal");
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK) {
        Serial.println("Gagal menginisialisasi ESP-NOW");
        return;
    }

    esp_now_register_recv_cb(onReceive);
    esp_now_register_send_cb(onSent);

    memcpy(peerInfo.peer_addr, peerAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Gagal menambahkan peer ESP-NOW");
        return;
    }
}

void kirimData(int data) {
    myData.data = data;
    esp_now_send(peerAddress, (uint8_t *)&myData, sizeof(myData));
}

void loop() {
    kirimData(69); // Kirim angka 1 ke ESP B
    delay(2000);
}
