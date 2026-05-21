#include "config.h"
#include "setup_portal.h"
#include "alarm.h"
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

// int readPageFromPot() {
//   int value = analogRead(POT_PIN);
//   int page = map(value, 0, 4095, 0, totalPages - 1);

//   if (page < 0) page = 0;
//   if (page >= totalPages) page = totalPages - 1;

//   return page;
// }

void handlePageButton() {
  static int lastButtonState = HIGH;
  static unsigned long lastPressTime = 0;

  int buttonState = digitalRead(PAGE_BUTTON_PIN);

  if (lastButtonState == HIGH && buttonState == LOW) {
    if (millis() - lastPressTime > 250) {
      currentPage++;

      if (currentPage >= totalPages) {
        currentPage = 0;
      }

      lastPressTime = millis();

      Serial.print("Page changed to: ");
      Serial.println(currentPage);
    }
  }

  lastButtonState = buttonState;
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // pinMode(POT_PIN, INPUT);
  pinMode(PAGE_BUTTON_PIN, INPUT_PULLUP);
  setupBuzzer();

  setupDisplay();
  showMessage("PocketPulse", "Starting...");

  loadSavedSettings();

  bool connected = connectToSavedWiFi();

  if (!connected) {
    startSetupPortal();
  }

  setupTime();
  waitForTimeSync();

  loadAlarmSettings();
  startSettingsServer();

  getWeather();
  getPrayerTimes();

  // showMessage("Ready", "Turn knob pages");
  showMessage("Ready", "Press button pages");
  delay(1000);

  Serial.println();
  Serial.println("Type a question in Serial Monitor and press Enter:");
}

void loop() {
  handleSerialAI();
  handleSettingsServer();
  checkAlarm();
  
  // currentPage = readPageFromPot();
  handlePageButton();

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
