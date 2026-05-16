#include <WiFi.h>
#include "time.h"
#include <U8g2lib.h>
#include <Wire.h>

// OLED display
// Most 1.3 inch OLED displays are SH1106 128x64 I2C
U8G2_SH1106_128X64_NONAME_F_HW_I2C display(U8G2_R0, U8X8_PIN_NONE);

// WiFi details
const char* ssid = "Stromfiber Lounge";
const char* password = "53833464";

// Time settings
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600 * 5;   // Pakistan UTC +5
const int daylightOffset_sec = 0;

void showTimeOnDisplay(struct tm timeinfo) {
  char timeText[20];
  char dateText[25];
  char dayText[15];

  // Format time, date, and day
  strftime(timeText, sizeof(timeText), "%I:%M:%S %p", &timeinfo);
  strftime(dateText, sizeof(dateText), "%d %B %Y", &timeinfo);
  strftime(dayText, sizeof(dayText), "%A", &timeinfo);

  display.clearBuffer();

  display.setFont(u8g2_font_6x12_tf);
  display.drawStr(0, 12, "Current Time");

  display.setFont(u8g2_font_9x15_tf);
  display.drawStr(0, 32, timeText);

  display.setFont(u8g2_font_6x12_tf);
  display.drawStr(0, 48, dayText);
  display.drawStr(0, 62, dateText);

  display.sendBuffer();
}

void printLocalTime() {
  struct tm timeinfo;

  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");

    display.clearBuffer();
    display.setFont(u8g2_font_6x12_tf);
    display.drawStr(0, 20, "Failed to get time");
    display.sendBuffer();

    return;
  }

  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");

  showTimeOnDisplay(timeinfo);
}

void setup() {
  Serial.begin(115200);

  // Start OLED
  display.begin();
  display.clearBuffer();
  display.setFont(u8g2_font_6x12_tf);
  display.drawStr(0, 20, "Starting...");
  display.sendBuffer();

  // Connect to WiFi
  Serial.printf("Connecting to %s ", ssid);

  display.clearBuffer();
  display.drawStr(0, 20, "Connecting WiFi...");
  display.sendBuffer();

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println(" CONNECTED");

  display.clearBuffer();
  display.drawStr(0, 20, "WiFi Connected");
  display.sendBuffer();

  // Get time from internet
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  delay(1000);
  printLocalTime();

  // Do NOT turn WiFi off if you also want AI/weather later
  // WiFi.disconnect(true);
  // WiFi.mode(WIFI_OFF);
}

void loop() {
  printLocalTime();
  delay(1000);
}