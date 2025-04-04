void setupOTA() {
  ArduinoOTA.setHostname("BOTEK");
  ArduinoOTA.setPasswordHash("9884cc4f19e708f3d83653e2f8b2cfaf");

  ArduinoOTA.onStart([]() {
    Serial.println(F("\n🔹 OTA Update Dimulai..."));
    otaUploading = true;
    previousMillisOTA = millis();
  });

  ArduinoOTA.onEnd([]() {
    Serial.println(F("\n🔹 OTA Update Selesai!"));
    otaUploading = false;
    digitalWrite(otaled, LOW);
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("🔹 OTA Progress: %u%%\n", (progress * 100) / total);

    if (otaUploading) {
      unsigned long currentMillis = millis();
      if (currentMillis - previousMillisOTA >= intervalOTA) {
        previousMillisOTA = currentMillis;
        ledStateOTA = !ledStateOTA;
        digitalWrite(otaled, ledStateOTA);
      }
    }
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("🔹 OTA Error [%u]\n", error);
  });

  ArduinoOTA.setRebootOnSuccess(true);
  ArduinoOTA.begin();
}

void OTAtobrut() {
  if (digitalRead(otapin) == LOW) {
    if (!otaMode) {
      Serial.println(F("\n🔹 OTA Mode Aktif"));
      stopRobot = true;
      setupOTA();
      otaMode = true;
    }
    if (!otaUploading) {
      digitalWrite(otaled, HIGH);
    }
    ArduinoOTA.handle();  
  } else {
    if (otaMode) {
      Serial.println(F("\n🔹 OTA Mode Nonaktif"));
      otaMode = false;
      stopRobot = false;
    }
    digitalWrite(otaled, LOW);
  }
}
