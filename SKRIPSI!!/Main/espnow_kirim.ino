void sendDataToSlave1(int data) {
  struct_message message;
  message.id = 1;
  message.data = data;
  esp_now_send(slave1Address, (uint8_t *)&message, sizeof(message));
  Serial.print(F("Mengirim ke SlaveA: "));
  Serial.println(F(data));
}

void sendDataToSlave2(int data) {
  struct_message message;
  message.id = 2;
  message.data = data;
  esp_now_send(slave2Address, (uint8_t *)&message, sizeof(message));
  Serial.print(F("Mengirim ke SlaveB: "));
  Serial.println(F(data));
}
