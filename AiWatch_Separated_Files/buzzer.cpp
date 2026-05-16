#include "buzzer.h"
#include "config.h"
#include "display_pages.h"

void setupBuzzer() {
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
}

void buzzForOneMinute(String prayerName) {
  Serial.print("AZAN TIME: ");
  Serial.println(prayerName);

  display.clearBuffer();
  display.setFont(u8g2_font_9x15_tf);
  display.drawStr(0, 25, "AZAN TIME");

  display.setFont(u8g2_font_6x12_tf);
  display.drawStr(0, 45, prayerName.c_str());
  display.sendBuffer();

  unsigned long startTime = millis();

  while (millis() - startTime < 60000) {
    // beep beep
    digitalWrite(BUZZER_PIN, HIGH);
    delay(200);
    digitalWrite(BUZZER_PIN, LOW);
    delay(150);

    digitalWrite(BUZZER_PIN, HIGH);
    delay(200);
    digitalWrite(BUZZER_PIN, LOW);

    // pause
    delay(1000);
  }
}
