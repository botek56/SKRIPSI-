#include <UTFT.h>
#include <URTouch.h>
#include <SdFat.h>
#include <avr/wdt.h>


extern uint8_t SmallFont[];
extern uint8_t BigFont[];

UTFT myGLCD(ITDB32S, 38, 39, 40, 41);
URTouch myTouch(6, 5, 4, 3, 2);

SdFat SD;
File bmpFile;

#define SD_CS 53
#define selesai  13
#define nyala 21

struct Button {
  int x, y, w, h;
  char label[20];
};

Button menuButtons[] = {
  {50, 50, 220, 50, "        Makanan"},
  {50, 110, 220, 50, "       Minuman"},
  {50, 170, 220, 50, "       Selesai"}
};

Button backButton = {20, 200, 80, 30, "Back"};  // Tombol Back
Button nextButton = {180, 200, 80, 30, "Next"};  // Tombol Next

int currentMenu = 0; // 0 = Menu Utama, 1 = Makanan, 2 = Minuman
int makananIndex = 0;
int minumanIndex = 0;

const char *menuMakanan[] = {"Nasi Goreng", "Mie Ayam", "Ayam Bakar"};
const char *menuMinuman[] = {"Teh Manis", "Matcha", "Ice Coffee Curio"};
const int hargaMakanan[] = {21000, 19000, 20000};  // Harga untuk Makanan 1, 2, 3
const int hargaMinuman[] = {8000, 21000, 18000};  // Harga untuk Minuman 1, 2, 3
const int jumlahMakanan = 3;
const int jumlahMinuman = 3;

String formatHarga(int harga) {
  String hargaStr = String(harga);
  int len = hargaStr.length();
  if (len > 3) {
    hargaStr = hargaStr.substring(0, len - 3) + "." + hargaStr.substring(len - 3);
  }
  return hargaStr;
}

void drawButton(Button btn, int r, int g, int b) {
  myGLCD.setColor(r, g, b);
  myGLCD.fillRoundRect(btn.x, btn.y, btn.x + btn.w, btn.y + btn.h);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect(btn.x, btn.y, btn.x + btn.w, btn.y + btn.h);
  myGLCD.setBackColor(r, g, b);
  myGLCD.setFont(SmallFont);
  myGLCD.print(btn.label, btn.x + 20, btn.y + 10);
}

void displayBMP(const char *filename, int x = -1, int y = -1) {
  bmpFile = SD.open(filename);
  if (!bmpFile) {
    Serial.println("Gagal membuka file BMP!");
    return;
  }

  byte header[54];
  bmpFile.read(header, 54);

  int width = header[18] | (header[19] << 8);
  int height = header[22] | (header[23] << 8);
  int rowSize = ((width * 2 + 3) & ~3);  

  int displayWidth = myGLCD.getDisplayXSize();
  int displayHeight = myGLCD.getDisplayYSize();

  if (x == -1) x = (displayWidth - width) / 2;
  if (y == -1) y = (displayHeight - height) / 2;

  Serial.print("Gambar: "); Serial.print(width); Serial.print("x"); Serial.println(height);
  Serial.print("Layar: "); Serial.print(displayWidth); Serial.print("x"); Serial.println(displayHeight);
  Serial.print("Posisi Gambar: x="); Serial.print(x); Serial.print(", y="); Serial.println(y);

  if (x + width > displayWidth || y + height > displayHeight) {
    Serial.println("Gambar terlalu besar untuk layar!");
    bmpFile.close();
    return;
  }

  byte rowBuffer[rowSize];

  for (int row = 0; row < height; row++) {
    bmpFile.seek(54 + row * rowSize);
    bmpFile.read(rowBuffer, rowSize);

    for (int col = 0; col < width; col++) {
      int index = col * 2;
      uint16_t pixelData = rowBuffer[index] | (rowBuffer[index + 1] << 8);

      // Ambil RGB565
      byte r = (pixelData >> 11) & 0x1F;
      byte g = (pixelData >> 5) & 0x3F;
      byte b = pixelData & 0x1F;

      // Konversi ke 8-bit
      r = r * 8;
      g = g * 4;
      b = b * 8;

      myGLCD.setColor(r, g, b);
      myGLCD.drawPixel(x + col, y + (height - row - 1));  
    }
  }

  bmpFile.close();
}

void displayBMP24(const char *filename, int x, int y) {
  File bmpFile = SD.open(filename);
  if (!bmpFile) {
    Serial.println("Gagal membuka file BMP!");
    return;
  }

  byte header[54];
  bmpFile.read(header, 54);

  if (header[0] != 'B' || header[1] != 'M' || header[28] != 24) {  
    Serial.println("Format BMP tidak didukung!");
    bmpFile.close();
    return;
  }

  int width = header[18] | (header[19] << 8);
  int height = header[22] | (header[23] << 8);
  int rowSize = (width * 3 + 3) & ~3; 

  byte rowBuffer[rowSize];  

  for (int row = 0; row < height; row++) {
    bmpFile.seek(54 + (height - 1 - row) * rowSize);  
    bmpFile.read(rowBuffer, rowSize);  

    for (int col = 0; col < width; col++) {
      int index = col * 3;
      byte b = rowBuffer[index];    
      byte g = rowBuffer[index + 1]; 
      byte r = rowBuffer[index + 2]; 

      myGLCD.setColor(r, g, b); 
      myGLCD.drawPixel(x + col, y + row);  
    }
  }

  bmpFile.close();
}

void drawMenuUtama() {
  myGLCD.fillScr(0, 0, 0);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.setFont(BigFont);
  myGLCD.print("CAKRAFFEE", CENTER, 10);

  for (int i = 0; i < 3; i++) {
    drawButton(menuButtons[i], 0, 0, 255); 
  }
}

void drawMenuMakanan() {
  myGLCD.fillScr(0, 0, 0);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.setFont(BigFont);
  myGLCD.print("MENU MAKANAN", CENTER, 10);

  int gambarY = 300; 
  if (makananIndex == 0) {
    displayBMP24("nasigoreng.bmp", 50, 50);
  } else if (makananIndex == 1) {
    displayBMP24("mie_ayam.bmp", (myGLCD.getDisplayXSize() - 100) / 2, gambarY);
  } else if (makananIndex == 2) {
    displayBMP24("ayam_bakar.bmp", (myGLCD.getDisplayXSize() - 100) / 2, gambarY);
  }

  int textY = gambarY + 100;
  myGLCD.setColor(255, 255, 255);  
  myGLCD.setBackColor(0, 0, 0);    
  myGLCD.print(menuMakanan[makananIndex], CENTER, 160);

  myGLCD.print("Rp " + formatHarga(hargaMakanan[makananIndex]), CENTER, 180);

  backButton.x = 0;
  backButton.y = 200;  
  nextButton.x = 230;
  nextButton.y = 200;  

  drawButton(backButton, 255, 0, 0);  
  drawButton(nextButton, 0, 255, 0);  
}

void drawMenuMinuman() {
  myGLCD.fillScr(0, 0, 0);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.setFont(BigFont);
  myGLCD.print("MENU MINUMAN", CENTER, 10);

  int gambarY = 300; 
  if (minumanIndex == 0) {
    displayBMP24("teh_manis.bmp", (myGLCD.getDisplayXSize() - 100) / 2, gambarY);
  } else if (minumanIndex == 1) {
    displayBMP24("matcha.bmp", (myGLCD.getDisplayXSize() - 100) / 2, gambarY);
  } else if (minumanIndex == 2) {
    displayBMP24("ice_coffee_curio.bmp", (myGLCD.getDisplayXSize() - 100) / 2, gambarY);
  }

  int textY = gambarY + 100;
  myGLCD.setColor(255, 255, 255);  
  myGLCD.setBackColor(0, 0, 0);     
  myGLCD.print(menuMinuman[minumanIndex], CENTER, 160);
  myGLCD.print("Rp " + formatHarga(hargaMinuman[minumanIndex]), CENTER, 180);

  backButton.x = 0;
  backButton.y = 200;  
  nextButton.x = 230;
  nextButton.y = 200;  

  drawButton(backButton, 255, 0, 0);  
  drawButton(nextButton, 0, 255, 0);  
}

void checkTouch() {
  if (myTouch.dataAvailable()) {
    myTouch.read();
    int x = myTouch.getX();
    int y = myTouch.getY();

    Serial.print("X: ");
    Serial.print(x);
    Serial.print(" Y: ");
    Serial.println(y);

    if (currentMenu == 0) {
      for (int i = 0; i < 3; i++) {
        if (x >= menuButtons[i].x && x <= menuButtons[i].x + menuButtons[i].w &&
            y >= menuButtons[i].y && y <= menuButtons[i].y + menuButtons[i].h) {

          myGLCD.setColor(255, 0, 0);
          myGLCD.fillRoundRect(menuButtons[i].x, menuButtons[i].y,
                               menuButtons[i].x + menuButtons[i].w, menuButtons[i].y + menuButtons[i].h);
          delay(200);
          drawButton(menuButtons[i], 0, 0, 255);

          if (i == 0) { // Makanan
            currentMenu = 1;
            makananIndex = 0;
            drawMenuMakanan();
          } else if (i == 1) { // Minuman
            currentMenu = 2;
            minumanIndex = 0;
            drawMenuMinuman();
          } else if (i == 2) { // Exit
            myGLCD.fillScr(0, 0, 0);
            myGLCD.print("BACK HOME...", CENTER, 120);
            digitalWrite(selesai, HIGH);
            delay(1000);
            resetdevice();
            digitalWrite(selesai, LOW);
            drawMenuUtama();
          }
        }
      }
    }

    if (currentMenu == 1) {
      if (x >= backButton.x && x <= backButton.x + backButton.w &&
          y >= backButton.y && y <= backButton.y + backButton.h) {
        if (makananIndex == 0) {
          currentMenu = 0;
          drawMenuUtama();
        } else {
          makananIndex = (makananIndex - 1 + jumlahMakanan) % jumlahMakanan;
          drawMenuMakanan();
        }
      }
      if (x >= nextButton.x && x <= nextButton.x + nextButton.w &&
          y >= nextButton.y && y <= nextButton.y + nextButton.h) {
        if (makananIndex == jumlahMakanan - 1) {
          currentMenu = 0; 
          drawMenuUtama();
        } else {
          makananIndex = (makananIndex + 1) % jumlahMakanan;
          drawMenuMakanan();
        }
      }
    }

    if (currentMenu == 2) {
      if (x >= backButton.x && x <= backButton.x + backButton.w &&
          y >= backButton.y && y <= backButton.y + backButton.h) {
        if (minumanIndex == 0) {
          currentMenu = 0; 
          drawMenuUtama();
        } else {
          minumanIndex = (minumanIndex - 1 + jumlahMinuman) % jumlahMinuman;
          drawMenuMinuman();
        }
      }
      if (x >= nextButton.x && x <= nextButton.x + nextButton.w &&
          y >= nextButton.y && y <= nextButton.y + nextButton.h) {
        if (minumanIndex == jumlahMinuman - 1) {
          currentMenu = 0; 
          drawMenuUtama();
        } else {
          minumanIndex = (minumanIndex + 1) % jumlahMinuman;
          drawMenuMinuman();
        }
      }
    }
  }
}

void resetdevice() {
  wdt_enable(WDTO_15MS);
  while (1) {}
}

void setup() {
  Serial.begin(115200);
  myGLCD.InitLCD();
  myGLCD.clrScr();
  myGLCD.setFont(BigFont);
  myTouch.InitTouch();
  myTouch.setPrecision(PREC_MEDIUM);

  // Inisialisasi SD Card
  if (!SD.begin(SD_CS)) {
    Serial.println("SD Card Error!");

    myGLCD.print("SD Error!", CENTER, 10);
    while (1); 
  }
  Serial.println("SD Card OK!");


  pinMode(selesai, OUTPUT);
  pinMode(nyala, INPUT_PULLUP);
  digitalWrite(selesai, LOW);
  digitalWrite(nyala, HIGH);
  displayBMP("BTK.bmp", 0, 0);
  delay(100);
  displayBMP("TRSE.bmp", 0, 0);
  while (digitalRead(nyala) == HIGH) {
    delay(100);
  }
  myGLCD.clrScr();
  drawMenuUtama();

}

void loop() {
  checkTouch();
}
