#include "display_pages.h"
#include "config.h"
#include "weather.h"
#include "prayer.h"
#include "ai_chat.h"

#include <Wire.h>
#include "time.h"

// If your OLED stays blank, your screen may be SSD1306.
// Replace SH1106 with SSD1306 in both display_pages.h and this line.
U8G2_SH1106_128X64_NONAME_F_HW_I2C display(U8G2_R0, U8X8_PIN_NONE);

void setupDisplay() {
  Wire.begin(OLED_SDA, OLED_SCL);
  display.begin();
}

void showMessage(String line1, String line2) {
  display.clearBuffer();
  display.setFont(u8g2_font_6x12_tf);
  display.drawStr(0, 22, line1.c_str());
  display.drawStr(0, 42, line2.c_str());
  display.sendBuffer();
}

void showWrappedText(String text) {
  display.clearBuffer();
  display.setFont(u8g2_font_6x10_tf);

  int x = 0;
  int y = 10;
  int lineHeight = 10;
  int maxWidth = 128;

  String line = "";
  String word = "";

  for (int i = 0; i <= text.length(); i++) {
    char c = text[i];

    if (c == ' ' || c == '\n' || i == text.length()) {
      String testLine;

      if (line.length() == 0) {
        testLine = word;
      } else {
        testLine = line + " " + word;
      }

      if (display.getStrWidth(testLine.c_str()) <= maxWidth) {
        line = testLine;
      } else {
        display.drawStr(x, y, line.c_str());
        y += lineHeight;
        line = word;

        if (y > 64) {
          break;
        }
      }

      word = "";

      if (c == '\n') {
        display.drawStr(x, y, line.c_str());
        y += lineHeight;
        line = "";

        if (y > 64) {
          break;
        }
      }
    } else {
      word += c;
    }
  }

  if (line.length() > 0 && y <= 64) {
    display.drawStr(x, y, line.c_str());
  }

  display.sendBuffer();
}

void updateDisplayPage(int currentPage) {
  if (currentPage == 0) {
    showAIPage();
  } else if (currentPage == 1) {
    showTimePage();
  } else if (currentPage == 2) {
    showWeatherPage();
  } else if (currentPage == 3) {
    showNextAzanPage();
  } else if (currentPage == 4) {
    showAllAzanTimesPage();
  }
}

void showAIPage() {
  String text = "AI Page\n";
  if (lastAIQuestion.length() > 0) {
    text += "Q: " + lastAIQuestion + "\n";
  }
  text += lastAIAnswer;

  showWrappedText(text);
}

void showTimePage() {
  struct tm timeinfo;

  if (!getLocalTime(&timeinfo)) {
    showMessage("Time Error", "No time");
    return;
  }

  char timeText[20];
  char dateText[25];
  char dayText[15];

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

void showWeatherPage() {
  char tempText[20];
  char humText[20];

  sprintf(tempText, "Temp: %.1f C", temperature);
  sprintf(humText, "Hum: %d%%", humidity);

  display.clearBuffer();

  display.setFont(u8g2_font_6x12_tf);
  display.drawStr(0, 10, "Karachi Weather");

  display.setFont(u8g2_font_9x15_tf);
  display.drawStr(0, 30, tempText);

  display.setFont(u8g2_font_6x12_tf);
  display.drawStr(0, 47, humText);
  display.drawStr(0, 62, weatherText.c_str());

  display.sendBuffer();
}

void showNextAzanPage() {
  calculateNextPrayer();

  display.clearBuffer();

  display.setFont(u8g2_font_6x12_tf);
  display.drawStr(0, 10, "Next Azan");

  display.setFont(u8g2_font_9x15_tf);
  display.drawStr(0, 30, nextPrayerName.c_str());

  display.setFont(u8g2_font_6x12_tf);
  display.drawStr(75, 30, nextPrayerTime.c_str());

  display.drawStr(0, 48, "Time Left:");
  display.drawStr(0, 62, timeLeftText.c_str());

  display.sendBuffer();
}

void showAllAzanTimesPage() {
  display.clearBuffer();

  display.setFont(u8g2_font_6x12_tf);
  display.drawStr(0, 10, "Karachi Azan Times");

  display.drawStr(0, 22, ("Fajr:    " + fajr).c_str());
  display.drawStr(0, 32, ("Dhuhr:   " + dhuhr).c_str());
  display.drawStr(0, 42, ("Asr:     " + asr).c_str());
  display.drawStr(0, 52, ("Maghrib: " + maghrib).c_str());
  display.drawStr(0, 62, ("Isha:    " + isha).c_str());

  display.sendBuffer();
}
