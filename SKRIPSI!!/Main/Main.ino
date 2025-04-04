#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Wire.h>
#include <MPU6050.h>
#include <Adafruit_HMC5883_U.h>
#include <esp_now.h>
#include <Arduino.h>

bool otaMode = false;
const int otapin = 13;
bool ledState = false;
const int otaled = 15;
unsigned long previousMillisOTA = 0;
const long intervalOTA = 100;
bool ledStateOTA = false;
bool otaUploading = false;

const char* ssid = "DASARATA.NET";
const char* password = "12345678";

float Kp = 10;
float Ki = 0;
float Kd = 27;

float error = 0;
float lastError = 0;
float integral = 0;
float derivative = 0;
float pidValue = 0;

const int SS_PIN = 5;
const int RST_PIN = 17;

int targetMeja = 0;
MFRC522 rfid(SS_PIN, RST_PIN);

const int buzz = 4;
const int LED = 2;
const int L_PWM1 = 32;
const int R_PWM1 = 33;
const int L_PWM2 = 25;
const int R_PWM2 = 26;
const int L_PWM3 = 27;
const int R_PWM3 = 14;
const int RODA = 12;

const int PinLine1 = 39;  //kiri
const int PinLine2 = 34;  //kanan
const int PinLine3 = 36;  //ujung kiri
const int PinLine4 = 35;  //ujung kanan

MPU6050 mpu;
Adafruit_HMC5883_Unified compass = Adafruit_HMC5883_Unified(12345);

float gyroZ_offset = 0;
float offset_X = 0, offset_Y = 0;
float yaw = 0;
float alpha = 0.98;
unsigned long last_time;

int Speed = 80;

unsigned long previousMillis = 0;
unsigned long interval = 3000;
unsigned long currentMillis;

const byte UID_TERPILIH[][7] = {
  { 0xFF, 0x0F, 0x59, 0xE5, 0x20, 0x00, 0x00 },
  { 0xFF, 0x0F, 0x4D, 0xE5, 0x20, 0x00, 0x00 },
  { 0xFF, 0x0F, 0x41, 0xE5, 0x20, 0x00, 0x00 },
  { 0xFF, 0x0F, 0x36, 0xE5, 0x20, 0x00, 0x00 },
  { 0xFF, 0x0F, 0x59, 0x11, 0x22, 0x00, 0x00 }
};

const byte UID_KANAN[][4] = {
  { 0x26, 0xC5, 0x23, 0x87 },
  { 0xF2, 0xF2, 0x34, 0x3 }
};

const byte UID_KIRI[][4] = {
  { 0x40, 0x64, 0xB6, 0x1A },
  { 0xCE, 0xC9, 0x31, 0x3 }
};

const byte UID_PUTAR[][7] = {
  { 0xFF, 0x0F, 0x59, 0xE5, 0x20, 0x00, 0x00 }
};

const char* NAMA_TERPILIH[] = {
  "Meja 1",
  "Meja 2",
  "Meja 3",
  "Meja 4",
  "Meja 5"
};

const int JUMLAH_UID = sizeof(UID_TERPILIH) / sizeof(UID_TERPILIH[0]);
const int JUMLAH_UID_KANAN = sizeof(UID_KANAN) / sizeof(UID_KANAN[0]);
const int JUMLAH_UID_KIRI = sizeof(UID_KIRI) / sizeof(UID_KIRI[0]);
const int JUMLAH_UID_PUTAR = sizeof(UID_PUTAR) / sizeof(UID_PUTAR[0]);

typedef struct struct_message {
  int id;
  int data;
} struct_message;

struct_message receivedData;

uint8_t slave1Address[] = { 0xC8, 0xC9, 0xA3, 0xF9, 0xDB, 0x1C };  //KASIR
uint8_t slave2Address[] = { 0x3C, 0xE9, 0xA5, 0x2B, 0xC7, 0x5D };

bool dataReceived = false;
int TargetMejaTerakhir = 0;

unsigned long previousMillisRFID = 0;
const long intervalRFID = 50;

unsigned long previousMillisBuzz = 0;
const long intervalBuzz = 150;

bool stopRobot = false;

bool lastReceived8 = false;
int lastTargetMeja = 0;
unsigned long lastReceiveTime = 0;
bool waitingToContinue = false;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  SPI.begin();
  rfid.PCD_Init();
  initWiFi();
  initESPNow();
  addPeer(slave1Address);
  addPeer(slave2Address);
  Serial.println(F("Master Ready!"));

  pinMode(LED, OUTPUT);
  pinMode(otaled, OUTPUT);
  pinMode(otapin, INPUT_PULLUP);
  pinMode(RODA, OUTPUT);
  pinMode(buzz, OUTPUT);

  pinMode(L_PWM1, OUTPUT);
  pinMode(R_PWM1, OUTPUT);
  pinMode(L_PWM2, OUTPUT);
  pinMode(R_PWM2, OUTPUT);
  pinMode(L_PWM3, OUTPUT);
  pinMode(R_PWM3, OUTPUT);

  pinMode(PinLine1, INPUT);
  pinMode(PinLine2, INPUT);
  pinMode(PinLine3, INPUT);
  pinMode(PinLine4, INPUT);

  mpu.initialize();
  if (!mpu.testConnection()) {
    Serial.println(F("MPU6050 tidak terdeteksi!"));
    //    while (1);/
  }

  // Inisialisasi HMC5883L
  if (!compass.begin()) {
    Serial.println(F("HMC5883L tidak terdeteksi!"));
    //    while (1);/
  }
  //  kalibrasiOtomatis();
  //  last_time = millis();

  resetYaw();
  buzzz();
  digitalWrite(LED, HIGH);
  stopRobot = true;
  berhenti();
}

void loop() {
  OTAtobrut(); // Jangan dihapus!
  unsigned long currentMillis = millis();

  if (stopRobot) {
    berhenti();

    if (waitingToContinue && currentMillis - lastReceiveTime > 1000) {
      stopRobot = false;
      targetMeja = lastTargetMeja;
      waitingToContinue = false;
    }
    return;
  }

  if (!stopRobot && targetMeja >= 1 && targetMeja <= 4) {
    Serial.println(targetMeja);
    LineFollower();
  } else {
    berhenti();
  }

  if (targetMeja == 12) {
    maju();
    delay(1200);
    berhenti();
    delay(1000);
    putarkanan();
    delay(1600);
    stopRobot = false;
  }
  else if (targetMeja == 6) {
    putarkanan();
    delay(3300);
    berhenti();
    stopRobot = true;
    ESP.restart();
  }
  else if (targetMeja == 7) {
    ESP.restart();
  }

  if (currentMillis - previousMillisRFID >= intervalRFID) {
    previousMillisRFID = currentMillis;
    cariRFID(targetMeja);
  }
}

void buzzz() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillisBuzz >= intervalBuzz) {
    previousMillisBuzz = currentMillis;

    static bool buzzState = false;

    if (buzzState) {
      digitalWrite(buzz, HIGH);
    } else {
      digitalWrite(buzz, LOW);
    }

    buzzState = !buzzState;
  }
}
