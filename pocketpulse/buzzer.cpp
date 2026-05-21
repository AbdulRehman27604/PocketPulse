#include "pitches.h"
#include "buzzer.h"
#include "config.h"
#include "display_pages.h"

#ifndef REST
#define REST 0
#endif

const int BUZZER_CHANNEL = 0;

const int star_melody[] PROGMEM = {
  NOTE_AS4, NOTE_AS4, NOTE_AS4,
  NOTE_F5, NOTE_C6,
  NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F6, NOTE_C6,
  NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F6, NOTE_C6,
  NOTE_AS5, NOTE_A5, NOTE_AS5, NOTE_G5, NOTE_C5, NOTE_C5, NOTE_C5,
  NOTE_F5, NOTE_C6,
  NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F6, NOTE_C6,

  NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F6, NOTE_C6,
  NOTE_AS5, NOTE_A5, NOTE_AS5, NOTE_G5, NOTE_C5, NOTE_C5,
  NOTE_D5, NOTE_D5, NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F5,
  NOTE_F5, NOTE_G5, NOTE_A5, NOTE_G5, NOTE_D5, NOTE_E5, NOTE_C5, NOTE_C5,
  NOTE_D5, NOTE_D5, NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F5,

  NOTE_C6, NOTE_G5, NOTE_G5, REST, NOTE_C5,
  NOTE_D5, NOTE_D5, NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F5,
  NOTE_F5, NOTE_G5, NOTE_A5, NOTE_G5, NOTE_D5, NOTE_E5, NOTE_C6, NOTE_C6,
  NOTE_F6, NOTE_DS6, NOTE_CS6, NOTE_C6, NOTE_AS5, NOTE_GS5, NOTE_G5, NOTE_F5,
  NOTE_C6
};

const int star_durations[] PROGMEM = {
  8, 8, 8,
  2, 2,
  8, 8, 8, 2, 4,
  8, 8, 8, 2, 4,
  8, 8, 8, 2, 8, 8, 8,
  2, 2,
  8, 8, 8, 2, 4,

  8, 8, 8, 2, 4,
  8, 8, 8, 2, 8, 16,
  4, 8, 8, 8, 8, 8,
  8, 8, 8, 4, 8, 4, 8, 16,
  4, 8, 8, 8, 8, 8,

  8, 16, 2, 8, 8,
  4, 8, 8, 8, 8, 8,
  8, 8, 8, 4, 8, 4, 8, 16,
  4, 8, 4, 8, 4, 8, 4, 8,
  1
};

void setupBuzzer() {
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  pinMode(PAUSE_PIN, INPUT_PULLUP);

  ledcSetup(BUZZER_CHANNEL, 2000, 8);
  ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);
  ledcWriteTone(BUZZER_CHANNEL, 0);
}

void startTone(int frequency) {
  if (frequency == REST || frequency == 0) {
    ledcWriteTone(BUZZER_CHANNEL, 0);
  } else {
    ledcWriteTone(BUZZER_CHANNEL, frequency);
  }
}

void stopTone() {
  ledcWriteTone(BUZZER_CHANNEL, 0);
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
    startTone(2000);
    delay(200);
    stopTone();
    delay(150);

    startTone(2000);
    delay(200);
    stopTone();

    delay(1000);
  }
}

void buzzForAlarm(String alarmName) {
  Serial.print("ALARM NAME: ");
  Serial.println(alarmName);

  display.clearBuffer();
  display.setFont(u8g2_font_9x15_tf);
  display.drawStr(0, 25, "ALARM");

  display.setFont(u8g2_font_6x12_tf);
  display.drawStr(0, 45, alarmName.c_str());
  display.drawStr(0, 62, "Press button stop");
  display.sendBuffer();

  int melodyLength = sizeof(star_melody) / sizeof(star_melody[0]);

  while (digitalRead(PAUSE_PIN) == HIGH) {
    for (int i = 0; i < melodyLength; i++) {
      if (digitalRead(PAUSE_PIN) == LOW) {
        stopTone();
        Serial.println("Alarm stopped by button.");
        delay(300);
        return;
      }

      int note = pgm_read_word_near(star_melody + i);
      int divider = pgm_read_word_near(star_durations + i);

      int noteDuration = 1000 / divider;

      if (note == REST) {
        stopTone();
      } else {
        startTone(note);
      }

      delay(noteDuration * 1.3);
      stopTone();
    }

    delay(500);
  }

  stopTone();
}