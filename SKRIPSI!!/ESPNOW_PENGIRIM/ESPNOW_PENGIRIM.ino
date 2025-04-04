#include <esp_now.h>
#include <WiFi.h>

const int BUTTON1 = 4;
const int BUTTON2 = 18;
const int BUTTON3 = 19;
const int BUTTON4 = 21;
const int Status = 2;

uint8_t broadcastAddress[] = {0x94, 0x54, 0xC5, 0xA9, 0xB0, 0xD0};

esp_now_peer_info_t peerInfo;
void onSent(const uint8_t *macAddr, esp_now_send_status_t status) {
  Serial.print("Pengiriman data: ");
  if (status == ESP_NOW_SEND_SUCCESS) {
    Serial.println("Berhasil");
    digitalWrite(Status, HIGH);
  } else {
    Serial.println("Gagal");
    digitalWrite(Status, LOW);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  pinMode(BUTTON3, INPUT_PULLUP);
  pinMode(BUTTON4, INPUT_PULLUP);
  pinMode(Status, OUTPUT);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Gagal menginisialisasi ESP-NOW");
    return;
  }

  esp_now_register_send_cb(onSent);

  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Gagal menambahkan peer");
    return;
  }
}

void loop() {
  uint8_t data = 1;
  esp_now_send(broadcastAddress, &data, sizeof(data));
  delay(200);
  if (digitalRead(BUTTON1) == LOW) {
    uint8_t data = 1;
    esp_now_send(broadcastAddress, &data, sizeof(data));
    delay(200);
  }
  else if (digitalRead(BUTTON2) == LOW) {
    uint8_t data = 2;
    esp_now_send(broadcastAddress, &data, sizeof(data));
    delay(200);
  }
  else if (digitalRead(BUTTON3) == LOW) {
    uint8_t data = 3;
    esp_now_send(broadcastAddress, &data, sizeof(data));
    delay(200);
  }
  else if (digitalRead(BUTTON4) == LOW) {
    uint8_t data = 4;
    esp_now_send(broadcastAddress, &data, sizeof(data));
    delay(200);
  }
}
