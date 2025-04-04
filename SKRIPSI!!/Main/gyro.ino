void kalibrasiOtomatis() {
  int n = 1000;
  Serial.println(F("Mengkalibrasi MPU6050..."));
  int32_t sumZ = 0;
  for (int i = 0; i < n; i++) {
    int16_t gz = mpu.getRotationZ();
    sumZ += gz;
    delay(2);
  }
  gyroZ_offset = sumZ / n;
  Serial.println(F("Kalibrasi MPU6050 selesai!"));

  Serial.println(F("Memutar robot untuk kalibrasi kompas..."));
  float Xmin = 1000, Xmax = -1000;
  float Ymin = 1000, Ymax = -1000;

  putarRobot(true);

  for (int i = 0; i < 2000; i++) {
    sensors_event_t event;
    compass.getEvent(&event);
    float x = event.magnetic.x;
    float y = event.magnetic.y;
    if (x < Xmin) Xmin = x;
    if (x > Xmax) Xmax = x;
    if (y < Ymin) Ymin = y;
    if (y > Ymax) Ymax = y;
    delay(5);
  }

  // Matikan motor setelah kalibrasi selesai
  putarRobot(false);

  offset_X = (Xmax + Xmin) / 2;
  offset_Y = (Ymax + Ymin) / 2;
  Serial.println("Kalibrasi kompas selesai!");
}

void putarRobot(bool aktif) {
  if (aktif) {
    digitalWrite(RODA, HIGH);
    analogWrite(L_PWM1, 0);
    analogWrite(R_PWM1, Speed);
    analogWrite(L_PWM2, Speed);
    analogWrite(R_PWM2, 0);
    analogWrite(L_PWM3, 0);
    analogWrite(R_PWM3, Speed);
  } else {
    digitalWrite(RODA, LOW);
    analogWrite(L_PWM1, 0);
    analogWrite(R_PWM1, 0);
    analogWrite(L_PWM2, 0);
    analogWrite(R_PWM2, 0);
    analogWrite(L_PWM3, 0);
    analogWrite(R_PWM3, 0);
  }
}

void bacaYaw() {
  int16_t gz = mpu.getRotationZ();
  sensors_event_t event;
  float gyroZ = (gz - gyroZ_offset) / 131.0;
  compass.getEvent(&event);
  float magX = (event.magnetic.x - offset_X);
  float magY = (event.magnetic.y - offset_Y);
  float magYaw = atan2(magY, magX) * (180.0 / PI);
  if (magYaw < 0) magYaw += 360;
  unsigned long current_time = millis();
  float dt = (current_time - last_time) / 1000.0;
  yaw = alpha * (yaw + gyroZ * dt) + (1 - alpha) * magYaw;
  if (yaw < -180) yaw += 360;
  if (yaw > 180) yaw -= 360;
  last_time = current_time;
}

void putarKiri90() {
  float targetYaw = yaw - 90;
  if (targetYaw < -180) targetYaw += 360;
  while (yaw > targetYaw) {
    digitalWrite(RODA, HIGH);
    analogWrite(L_PWM1, 0);
    analogWrite(R_PWM1, Speed);
    analogWrite(L_PWM2, Speed);
    analogWrite(R_PWM2, 0);
    analogWrite(L_PWM3, 0);
    analogWrite(R_PWM3, Speed);
    bacaYaw();
  }
  berhenti();
}

void putarKanan90() {
  float targetYaw = yaw + 90;
  if (targetYaw > 180) targetYaw -= 360;
  while (yaw < targetYaw) {
    digitalWrite(RODA, HIGH);
    analogWrite(L_PWM1, Speed);
    analogWrite(R_PWM1, 0);
    analogWrite(L_PWM2, 0);
    analogWrite(R_PWM2, Speed);
    analogWrite(L_PWM3, Speed);
    analogWrite(R_PWM3, 0);
    bacaYaw();
  }
  berhenti();
}

void resetYaw() {
  yaw = 0;
  Serial.println(F("Yaw direset ke 0"));
}
