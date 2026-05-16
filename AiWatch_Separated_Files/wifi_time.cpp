#include "wifi_time.h"
#include "config.h"
#include "display_pages.h"

#include <WiFi.h>
#include "time.h"

void connectToWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to WiFi");
  showMessage("Connecting", "WiFi...");

  int attempts = 0;

  while (WiFi.status() != WL_CONNECTED && attempts < 40) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected!");
    Serial.print("ESP32 IP: ");
    Serial.println(WiFi.localIP());
    showMessage("WiFi", "Connected");
    delay(1000);
  } else {
    Serial.println("WiFi failed.");
    showMessage("WiFi failed", "Check details");
  }
}

void setupTime() {
  configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);
}

bool waitForTimeSync() {
  struct tm timeinfo;

  Serial.println("Syncing time...");
  showMessage("Syncing", "Time...");

  for (int i = 0; i < 30; i++) {
    if (getLocalTime(&timeinfo)) {
      Serial.println("Time synced!");
      Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
      return true;
    }

    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.println("Time sync failed");
  showMessage("Time Error", "Sync failed");
  return false;
}

int getCurrentMinutes() {
  struct tm timeinfo;

  if (!getLocalTime(&timeinfo)) {
    return -1;
  }

  return (timeinfo.tm_hour * 60) + timeinfo.tm_min;
}

String getCurrentTimeHHMM() {
  struct tm timeinfo;

  if (!getLocalTime(&timeinfo)) {
    return "";
  }

  char currentTime[6];
  strftime(currentTime, sizeof(currentTime), "%H:%M", &timeinfo);

  return String(currentTime);
}
