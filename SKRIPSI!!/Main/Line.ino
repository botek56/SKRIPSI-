int currentSpeed = 0;
unsigned long lastAccelTime = 0;
const int accelStep = 10; // Langkah percepatan
const int accelInterval = 100; // Waktu antar percepatan (ms)

void LineFollower() {
  if (stopRobot) {
    digitalWrite(RODA, LOW);
    return;
  }

  digitalWrite(RODA, HIGH);
  int kiri = analogRead(PinLine1);
  int kanan = analogRead(PinLine2);
  int ujungkiri = analogRead(PinLine3);
  int ujungkanan = analogRead(PinLine4);

  kiri = map(kiri, 0, 3000, 0, 1);
  kanan = map(kanan, 0, 3000, 0, 1);
  ujungkiri = map(ujungkiri, 0, 3000, 0, 1);
  ujungkanan = map(ujungkanan, 0, 3000, 0, 1);

  if (kiri == 1 && kanan == 1) {
    error = 0;
  }
  else if (kanan == 1 && kiri == 0) {
    error = 1;
  }
  else if (kanan == 0 && kiri == 1) {
    error = -1;
  } else {
    error = lastError;
  }

  integral += error;
  derivative = error - lastError;
  pidValue = (Kp * error) + (Ki * integral) + (Kd * derivative);

  lastError = error;

  int baseSpeed = 100;

  // **Soft Start: Tambahkan kecepatan bertahap**
  if (millis() - lastAccelTime >= accelInterval && currentSpeed < baseSpeed) {
    currentSpeed += accelStep;
    if (currentSpeed > baseSpeed) currentSpeed = baseSpeed;
    lastAccelTime = millis();
  }

  int leftSpeed = currentSpeed - pidValue;
  int rightSpeed = currentSpeed + pidValue;

  leftSpeed = constrain(leftSpeed, 0, 110);
  rightSpeed = constrain(rightSpeed, 0, 110);

  analogWrite(L_PWM1, leftSpeed);
  analogWrite(L_PWM2, rightSpeed);
}

void readlf() {
  int kiri = analogRead(PinLine1);
  int kanan = analogRead(PinLine2);
  int ujungkiri = analogRead(PinLine3);
  int ujungkanan = analogRead(PinLine4);

  kiri = map(kiri, 0, 3000, 0, 1);
  kanan = map(kanan, 0, 3000, 0, 1);
  ujungkiri = map(ujungkiri, 0, 3000, 0, 1);
  ujungkanan = map(ujungkanan, 0, 3000, 0, 1);

  Serial.print(F("ujungKiri: "));
  Serial.print(F(ujungkiri));
  Serial.print(F(" | Kiri: "));
  Serial.print(F(kiri));
  Serial.print(F(" | kanan: "));
  Serial.print(F(kanan));
  Serial.print(F(" | ujungKanan: "));
  Serial.println(F(ujungkanan));
}
