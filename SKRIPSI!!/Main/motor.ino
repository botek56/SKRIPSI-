void maju() {
  digitalWrite(RODA, HIGH);
  analogWrite(L_PWM1, Speed);//Rodakiri
  analogWrite(R_PWM1, 0);
  analogWrite(L_PWM2, Speed);//Rodakanan
  analogWrite(R_PWM2, 0);
}
void mundur() {
  digitalWrite(RODA, HIGH);
  analogWrite(L_PWM1, 0);
  analogWrite(R_PWM1, Speed);
  analogWrite(L_PWM2, 0);
  analogWrite(R_PWM2, Speed);
}
void putarkiri() {
  digitalWrite(RODA, HIGH);
  analogWrite(L_PWM1, 0);
  analogWrite(R_PWM1, Speed);
  analogWrite(L_PWM2, Speed);
  analogWrite(R_PWM2, 0);
  analogWrite(L_PWM3, 0);
  analogWrite(R_PWM3, Speed);
}
void putarkanan() {
  digitalWrite(RODA, HIGH);
  analogWrite(L_PWM1, Speed);
  analogWrite(R_PWM1, 0);
  analogWrite(L_PWM2, 0);
  analogWrite(R_PWM2, Speed);
  analogWrite(L_PWM3, Speed);
  analogWrite(R_PWM3, 0);
}
void berhenti() {
  digitalWrite(RODA, LOW);
  analogWrite(L_PWM1, 0);
  analogWrite(R_PWM1, 0);
  analogWrite(L_PWM2, 0);
  analogWrite(R_PWM2, 0);
  analogWrite(L_PWM3, 0);
  analogWrite(R_PWM3, 0);
}
void tesmotor() {
  maju();
  delay(3000);
  berhenti();
  delay(2000);
  mundur();
  delay(3000);
  berhenti();
  delay(2000);
  putarkiri();
  delay(3000);
  berhenti();
  delay(2000);
  putarkanan();
  delay(3000);
  berhenti();
  return;
}
