#include "config.h"
#include "display_pages.h"
#include "wifi_time.h"
#include "weather.h"
#include "prayer.h"
#include "ai_chat.h"
#include "buzzer.h"

// Page system
const int totalPages = 5;
int currentPage = 0;
int lastPage = -1;

unsigned long lastDisplayUpdate = 0;
unsigned long lastWeatherUpdate = 0;
unsigned long lastPrayerUpdate = 0;
unsigned long lastAzanCheck = 0;

const unsigned long displayInterval = 1000;
const unsigned long weatherInterval = 10UL * 60UL * 1000UL;
const unsigned long prayerInterval = 6UL * 60UL * 60UL * 1000UL;
const unsigned long azanCheckInterval = 10000;

int readPageFromPot() {
  int value = analogRead(POT_PIN);
  int page = map(value, 0, 4095, 0, totalPages - 1);

  if (page < 0) page = 0;
  if (page >= totalPages) page = totalPages - 1;

  return page;
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(POT_PIN, INPUT);
  setupBuzzer();

  setupDisplay();
  showMessage("AI Watch", "Starting...");

  connectToWiFi();
  setupTime();
  waitForTimeSync();

  getWeather();
  getPrayerTimes();

  showMessage("Ready", "Turn knob pages");
  delay(1000);

  Serial.println();
  Serial.println("Type a question in Serial Monitor and press Enter:");
}

void loop() {
  handleSerialAI();

  currentPage = readPageFromPot();

  unsigned long now = millis();

  if (now - lastAzanCheck >= azanCheckInterval) {
    lastAzanCheck = now;
    checkAzanTime();
  }

  if (now - lastWeatherUpdate >= weatherInterval) {
    lastWeatherUpdate = now;
    getWeather();
  }

  if (now - lastPrayerUpdate >= prayerInterval) {
    lastPrayerUpdate = now;
    getPrayerTimes();
  }

  if (currentPage != lastPage || now - lastDisplayUpdate >= displayInterval) {
    lastPage = currentPage;
    lastDisplayUpdate = now;
    updateDisplayPage(currentPage);
  }
}
