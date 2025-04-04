void readRFID() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return;

  int indeks = -1;

  for (int i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  for (int j = 0; j < JUMLAH_UID; j++) {
    bool match = true;
    for (int i = 0; i < rfid.uid.size; i++) {
      if (rfid.uid.uidByte[i] != UID_TERPILIH[j][i]) {
        match = false;
        break;
      }
    }
    if (match) {
      indeks = j;
      break;
    }
  }

  if (indeks != -1) {
    Serial.print(F("Location, "));
    Serial.println(F(NAMA_TERPILIH[indeks]));
    buzzz();
  } else {
    Serial.println(F("Akses Ditolak!"));
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

void cariRFID(int targetMeja) {
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    for (int j = 0; j < JUMLAH_UID; j++) {
      bool match = true;
      for (int i = 0; i < rfid.uid.size; i++) {
        if (rfid.uid.uidByte[i] != UID_TERPILIH[j][i]) {
          match = false;
          break;
        }
      }
      if (match && j == (targetMeja - 1)) {
        Serial.print(F("Berhasil menemukan "));
        Serial.println(F(NAMA_TERPILIH[j]));
        //        buzzz();
        berhenti();

        if (targetMeja == 1 || targetMeja == 2) { //meja kiri
          delay(1000);
          putarkanan();
          delay(1600);
          berhenti();
          delay(1000);
          mundur();
          delay(1200);
          berhenti();
          delay(800);
        }

        if (targetMeja == 3 || targetMeja == 4) { //meja kanan
          delay(1000);
          putarkiri();
          delay(1600);
          berhenti();
          delay(1000);
          mundur();
          delay(1200);
          berhenti();
          delay(800);
        }

        if (receivedData.data == 5) {
          Serial.println("PULANG");
          maju();
          delay(1200);
          berhenti();
          delay(1000);
          putarkanan();
          delay(1600);
          stopRobot = false;
        }
        stopRobot = true;
        //        ESP.restart();
        //        return;
      }
    }

    for (int j = 0; j < JUMLAH_UID_KANAN; j++) {
      bool match = true;
      for (int i = 0; i < rfid.uid.size; i++) {
        if (rfid.uid.uidByte[i] != UID_KANAN[j][i]) {
          match = false;
          break;
        }
      }
      if (match) {
        buzzz();
        berhenti();
        stopRobot = true;
        delay(1000);
        putarkanan();
        delay(1600);
        berhenti();
        delay(1000);
        stopRobot = false;
      }
    }

    for (int j = 0; j < JUMLAH_UID_KIRI; j++) {
      bool match = true;
      for (int i = 0; i < rfid.uid.size; i++) {
        if (rfid.uid.uidByte[i] != UID_KIRI[j][i]) {
          match = false;
          break;
        }
      }
      if (match) {
        buzzz();
        berhenti();
        stopRobot = true;
        delay(1000);
        putarkiri();
        delay(1600);
        berhenti();
        delay(1000);
        stopRobot = false;
      }
    }

    for (int j = 0; j < JUMLAH_UID_PUTAR; j++) {
      bool match = true;
      for (int i = 0; i < rfid.uid.size; i++) {
        if (rfid.uid.uidByte[i] != UID_PUTAR[j][i]) {
          match = false;
          break;
        }
      }
      if (match) {
        buzzz();
        berhenti();
        stopRobot = true;
        delay(1000);
        putarkanan();
        delay(3300);
        berhenti();
        delay(1000);
        stopRobot = false;
        ESP.restart();
        return;
      }
    }
  }
}
