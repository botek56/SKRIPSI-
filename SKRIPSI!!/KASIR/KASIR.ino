  #include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <esp_now.h>
#include <WiFi.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define CLK_PIN 26
#define DT_PIN 27
#define SW_PIN 14
#define BUZZER_PIN 5
#define led_red 2
#define led_green 15


typedef struct struct_message {
  int id;
  int data;
} struct_message;

struct_message myData;

uint8_t masterAddress[] = {0x94, 0x54, 0xC5, 0xA9, 0xB0, 0xD0}; // MASTER 94:54:c5:a9:b0:d0


void initWiFi() {
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  Serial.print("Alamat MAC Slave: ");
  Serial.println(WiFi.macAddress());
}

void initESPNow() {
  if (esp_now_init() != ESP_OK) {
    Serial.println("Gagal menginisialisasi ESP-NOW");
    return;
  }
  esp_now_register_send_cb(onSent);
  esp_now_register_recv_cb(onReceive);
}

void addMasterPeer() {
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, masterAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Gagal menambahkan Master sebagai peer");
    digitalWrite(led_green,LOW);
    return;
  }
  Serial.println("Master berhasil ditambahkan");
  
}

void onSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Pengiriman data ke Master ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Berhasil" : "Gagal");
}

void onReceive(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
  if (len == sizeof(struct_message)) { // Validasi ukuran data
    memcpy(&myData, incomingData, sizeof(myData));
    Serial.print("Data diterima dari Master: ");
    Serial.println(myData.data);
  } else {
    Serial.println("Error: Ukuran data tidak sesuai!");
  }
}

void sendDataToMaster(int data) {
  struct_message message;
  message.id = 1;
  message.data = data;

  if (!esp_now_is_peer_exist(masterAddress)) {
    Serial.println("Master belum terdaftar sebagai peer. Menambahkan...");
    addMasterPeer();
  }

  esp_err_t result = esp_now_send(masterAddress, (uint8_t *)&message, sizeof(message));

  if (result == ESP_OK) {
    Serial.print("Mengirim data ke Master: ");
    Serial.println(data);
  } else {
    Serial.print("Gagal mengirim data ke Master, kode error: ");
    Serial.println(result);
  }
}


int lastStateCLK;
int currentStateCLK;
int menuIndex = 0;
int arrowPosition = 0;
unsigned long lastRotaryEvent = 0;
unsigned long debounceDelay = 50;
int currentMenu = 0;

const char *menuItems[] = {"Target Meja", "Setting Robot", "Reset Device"};  // Tambah "Reset Device"
int numMenuItems = sizeof(menuItems) / sizeof(menuItems[0]);

const char *submenuTargetMeja[] = {"Meja 1", "Meja 2", "Meja 3", "Meja 4", "Exit"};
int numSubmenuTargetMeja = sizeof(submenuTargetMeja) / sizeof(submenuTargetMeja[0]);

const char *submenuSettingRobot[] = {"Putar kiri", "Putar Kanan", "Reset Robot", "Exit"};
int numSubmenuSettingRobot = sizeof(submenuSettingRobot) / sizeof(submenuSettingRobot[0]);

void setup() {
  Serial.begin(115200);
  lcd.begin();
  lcd.backlight();
  showStartupScreen();
  initWiFi();
  initESPNow();
  addMasterPeer();

  pinMode(CLK_PIN, INPUT);
  pinMode(DT_PIN, INPUT);
  pinMode(SW_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(led_red, OUTPUT);
  pinMode(led_green, OUTPUT);


  lastStateCLK = digitalRead(CLK_PIN);
  updateMenu();
  digitalWrite(led_green,HIGH);
}

void loop() {
  currentStateCLK = digitalRead(CLK_PIN);

  if (currentStateCLK != lastStateCLK) {
    if (millis() - lastRotaryEvent > debounceDelay) {
      if (digitalRead(DT_PIN) != currentStateCLK) {
        if (arrowPosition == 0) {
          arrowPosition = 1;
        } else if ((currentMenu == 1 && menuIndex < numSubmenuTargetMeja - 2) ||
                   (currentMenu == 2 && menuIndex < numSubmenuSettingRobot - 2) ||
                   (currentMenu == 0 && menuIndex < numMenuItems - 2)) {
          menuIndex++;
          arrowPosition = 0;
        }
      } else {
        if (arrowPosition == 1) {
          arrowPosition = 0;
        } else if (menuIndex > 0) {
          menuIndex--;
          arrowPosition = 1;
        }
      }
      updateMenu();
      lastRotaryEvent = millis();
    }
  }

  lastStateCLK = currentStateCLK;

  if (digitalRead(SW_PIN) == LOW) {
    buzzerBeep();
    delay(200);
    executeMenuAction();
  }
}

void updateMenu() {
  lcd.clear();

  const char **currentMenuItems;
  int itemCount;

  if (currentMenu == 1) {
    currentMenuItems = submenuTargetMeja;
    itemCount = numSubmenuTargetMeja;
  } else if (currentMenu == 2) {
    currentMenuItems = submenuSettingRobot;
    itemCount = numSubmenuSettingRobot;
  } else {
    currentMenuItems = menuItems;
    itemCount = numMenuItems;
  }

  for (int i = 0; i < 2; i++) {
    int itemIndex = menuIndex + i;
    lcd.setCursor(0, i);

    if (arrowPosition == i) {
      lcd.print("> ");
    } else {
      lcd.print("  ");
    }

    if (itemIndex < itemCount) {
      lcd.print(currentMenuItems[itemIndex]);
    }
  }
}

void executeMenuAction() {
  lcd.clear();
  lcd.setCursor(0, 0);

  if (currentMenu == 1) {
    int selectedIndex = menuIndex + arrowPosition;
    if (selectedIndex == numSubmenuTargetMeja - 1) {
      backToMainMenu();
    } else {
      sendTargetMeja(selectedIndex + 1);
    }
  } else if (currentMenu == 2) { 
    int selectedIndex = menuIndex + arrowPosition;
    if (selectedIndex == numSubmenuSettingRobot - 1) {
      backToMainMenu();
    } else {
      lcd.noBacklight();
      buzzerVariation();
      lcd.backlight();

      if (selectedIndex == 0) {
        sendDataToMaster(5);
        lcd.print("Putar Kiri");
      } else if (selectedIndex == 1) {
        sendDataToMaster(6);
        lcd.print("Putar Kanan");
      } else if (selectedIndex == 2) {
        sendDataToMaster(7);
        lcd.print("Reset Robot");
      }

      delay(500);
      backToMainMenu();
    }
  } else {
    int selectedIndex = menuIndex + arrowPosition;
    if (selectedIndex == 0) {
      currentMenu = 1;
    } else if (selectedIndex == 1) {
      currentMenu = 2;
      menuIndex = 0;
      arrowPosition = 0;
    } else if (selectedIndex == 2) {
      resetDevice();
    }
  }
  updateMenu();
}

void sendTargetMeja(int meja) {
  sendDataToMaster(meja);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Robot Menuju");
  lcd.setCursor(0, 1);
  lcd.print("Meja ");
  lcd.print(meja);
  delay(500);
  lcd.noBacklight();
  buzzerVariation();
  lcd.backlight();
  backToMainMenu();
}

void resetDevice() {
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Resetting...");
  delay(700);
  ESP.restart();
}

void backToMainMenu() {
  currentMenu = 0;
  menuIndex = 0;
  arrowPosition = 0;
  updateMenu();
}

void showStartupScreen() {
  for (int i = 0; i < 10; i++) {
    lcd.noBacklight();
    delay(50);
    lcd.backlight();
    delay(50);
  }
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("ROBOT PENGANTAR");
    lcd.setCursor(5, 1);
    lcd.print("MAKANAN");
    delay(1500);
    lcd.clear();
}

void buzzerBeep() {
  tone(BUZZER_PIN, 1000, 100);
  delay(150);
  noTone(BUZZER_PIN);
}

void buzzerVariation() {
  unsigned long startTime = millis();
  while (millis() - startTime < 1000) {
    tone(BUZZER_PIN, 800, 300);
    delay(400);
    tone(BUZZER_PIN, 1200, 300);
    delay(400);
  }
  noTone(BUZZER_PIN);
}
