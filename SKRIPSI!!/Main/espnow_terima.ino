void initWiFi() {
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  Serial.print(F("Alamat MAC Master: "));

}
void initESPNow() {
  if (esp_now_init() != ESP_OK) {
    Serial.println(F("Gagal menginisialisasi ESP-NOW"));
    return;
  }
  esp_now_register_send_cb(onSent);
  esp_now_register_recv_cb(onReceive);
}

void addPeer(uint8_t *peerAddress) {
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, peerAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println(F("Gagal menambahkan peer"));
    return;
  }
  Serial.println(F("Peer berhasil ditambahkan"));
}

void onSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Data terkirim ");
  Serial.println(F(status == ESP_NOW_SEND_SUCCESS ? "Berhasil" : "Gagal"));
}

void onReceive(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
  if (len == sizeof(struct_message)) {
    memcpy(&receivedData, incomingData, sizeof(receivedData));
    Serial.print(F("Data diterima: "));
    Serial.println(receivedData.data);

    if (receivedData.data == 8) {
      if (!stopRobot) {
      }
      stopRobot = true;
      berhenti();
      lastReceived8 = true;
      lastReceiveTime = millis();
      waitingToContinue = true;
    }
    else {
      if (lastReceived8) {
        lastReceived8 = false;
      }
      stopRobot = false;
      waitingToContinue = false;
      if (receivedData.data >= 1 && receivedData.data <= 4) {
        lastTargetMeja = receivedData.data;
        targetMeja = receivedData.data;
        Serial.println(targetMeja);
      }
      else if (receivedData.data == 5 || receivedData.data == 6 || receivedData.data == 7) {
        targetMeja = receivedData.data;
      }
    }
  } else {
  }
}
