#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <U8g2lib.h>
#include <Wire.h>
#include "time.h"

// OLED display
U8G2_SH1106_128X64_NONAME_F_HW_I2C display(U8G2_R0, U8X8_PIN_NONE);

// WiFi
const char* ssid = "Stromfiber Lounge";
const char* password = "53833464";

// NTP time
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600 * 5;   // Pakistan UTC+5
const int daylightOffset_sec = 0;

// Buzzer
const int buzzerPin = 25;

// Prayer times
String fajr = "--:--";
String dhuhr = "--:--";
String asr = "--:--";
String maghrib = "--:--";
String isha = "--:--";

// Next prayer info
String nextPrayerName = "--";
String nextPrayerTime = "--:--";
String timeLeftText = "--h --m";

// Stops repeated buzzer in same minute
String lastBuzzedTime = "";

void showPrayerTimesOnDisplay();

void showMessage(String line1, String line2) {
  display.clearBuffer();
  display.setFont(u8g2_font_6x12_tf);
  display.drawStr(0, 22, line1.c_str());
  display.drawStr(0, 42, line2.c_str());
  display.sendBuffer();
}

String cleanTime(String timeValue) {
  int spaceIndex = timeValue.indexOf(" ");
  if (spaceIndex != -1) {
    timeValue = timeValue.substring(0, spaceIndex);
  }
  return timeValue;
}

bool waitForTimeSync() {
  struct tm timeinfo;

  Serial.println("Syncing time...");
  showMessage("Syncing", "Time...");

  for (int i = 0; i < 20; i++) {
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

int timeStringToMinutes(String timeStr) {
  // Converts "04:27" into total minutes from midnight
  int colonIndex = timeStr.indexOf(":");

  if (colonIndex == -1) {
    return -1;
  }

  int hours = timeStr.substring(0, colonIndex).toInt();
  int minutes = timeStr.substring(colonIndex + 1).toInt();

  return (hours * 60) + minutes;
}

int getCurrentMinutes() {
  struct tm timeinfo;

  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to get current time");
    return -1;
  }

  return (timeinfo.tm_hour * 60) + timeinfo.tm_min;
}

String getCurrentTimeHHMM() {
  struct tm timeinfo;

  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to get current time");
    return "";
  }

  char currentTime[6];
  strftime(currentTime, sizeof(currentTime), "%H:%M", &timeinfo);

  return String(currentTime);
}

void calculateNextPrayer() {
  int currentMinutes = getCurrentMinutes();

  if (currentMinutes == -1) {
    nextPrayerName = "Time Error";
    nextPrayerTime = "--:--";
    timeLeftText = "--h --m";
    return;
  }

  int fajrMin = timeStringToMinutes(fajr);
  int dhuhrMin = timeStringToMinutes(dhuhr);
  int asrMin = timeStringToMinutes(asr);
  int maghribMin = timeStringToMinutes(maghrib);
  int ishaMin = timeStringToMinutes(isha);

  int nextPrayerMinutes = -1;

  if (currentMinutes < fajrMin) {
    nextPrayerName = "Fajr";
    nextPrayerTime = fajr;
    nextPrayerMinutes = fajrMin;
  }
  else if (currentMinutes < dhuhrMin) {
    nextPrayerName = "Dhuhr";
    nextPrayerTime = dhuhr;
    nextPrayerMinutes = dhuhrMin;
  }
  else if (currentMinutes < asrMin) {
    nextPrayerName = "Asr";
    nextPrayerTime = asr;
    nextPrayerMinutes = asrMin;
  }
  else if (currentMinutes < maghribMin) {
    nextPrayerName = "Maghrib";
    nextPrayerTime = maghrib;
    nextPrayerMinutes = maghribMin;
  }
  else if (currentMinutes < ishaMin) {
    nextPrayerName = "Isha";
    nextPrayerTime = isha;
    nextPrayerMinutes = ishaMin;
  }
  else {
    // After Isha, next prayer is tomorrow's Fajr
    nextPrayerName = "Fajr";
    nextPrayerTime = fajr;
    nextPrayerMinutes = fajrMin + 1440;
  }

  int minutesLeft = nextPrayerMinutes - currentMinutes;

  int hoursLeft = minutesLeft / 60;
  int minsLeft = minutesLeft % 60;

  char buffer[20];
  sprintf(buffer, "%02dh %02dm left", hoursLeft, minsLeft);

  timeLeftText = String(buffer);

  Serial.print("Next prayer: ");
  Serial.print(nextPrayerName);
  Serial.print(" at ");
  Serial.print(nextPrayerTime);
  Serial.print(" | ");
  Serial.println(timeLeftText);
}

void showNextAzanOnDisplay() {
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
    digitalWrite(buzzerPin, HIGH);
    delay(200);
    digitalWrite(buzzerPin, LOW);
    delay(150);

    digitalWrite(buzzerPin, HIGH);
    delay(200);
    digitalWrite(buzzerPin, LOW);

    // pause
    delay(1000);
  }

  showNextAzanOnDisplay();
}

void checkAzanTime() {
  String currentTime = getCurrentTimeHHMM();

  if (currentTime == "") {
    return;
  }

  Serial.print("Current time: ");
  Serial.println(currentTime);

  if (currentTime == lastBuzzedTime) {
    return;
  }

  if (currentTime == fajr) {
    lastBuzzedTime = currentTime;
    buzzForOneMinute("Fajr");
  }
  else if (currentTime == dhuhr) {
    lastBuzzedTime = currentTime;
    buzzForOneMinute("Dhuhr");
  }
  else if (currentTime == asr) {
    lastBuzzedTime = currentTime;
    buzzForOneMinute("Asr");
  }
  else if (currentTime == maghrib) {
    lastBuzzedTime = currentTime;
    buzzForOneMinute("Maghrib");
  }
  else if (currentTime == isha) {
    lastBuzzedTime = currentTime;
    buzzForOneMinute("Isha");
  }
}

void getPrayerTimes() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected");
    showMessage("WiFi Error", "Not connected");
    return;
  }

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;

  String url = "https://api.aladhan.com/v1/timingsByCity?";
  url += "city=Karachi";
  url += "&country=Pakistan";
  url += "&method=1";

  Serial.println("Requesting prayer times...");
  Serial.println(url);

  http.begin(client, url);
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

  int httpCode = http.GET();

  Serial.print("HTTP Code: ");
  Serial.println(httpCode);

  if (httpCode == 200) {
    String payload = http.getString();

    if (payload.length() == 0) {
      Serial.println("Empty response from API");
      showMessage("API Error", "Empty reply");
      http.end();
      return;
    }

    StaticJsonDocument<8192> doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
      Serial.print("JSON parsing failed: ");
      Serial.println(error.c_str());
      showMessage("JSON Error", "Parse failed");
      http.end();
      return;
    }

    fajr = cleanTime(doc["data"]["timings"]["Fajr"].as<String>());
    dhuhr = cleanTime(doc["data"]["timings"]["Dhuhr"].as<String>());
    asr = cleanTime(doc["data"]["timings"]["Asr"].as<String>());
    maghrib = cleanTime(doc["data"]["timings"]["Maghrib"].as<String>());
    isha = cleanTime(doc["data"]["timings"]["Isha"].as<String>());

    Serial.println();
    Serial.println("Karachi Azan Times");
    Serial.println("------------------");
    Serial.println("Fajr: " + fajr);
    Serial.println("Dhuhr: " + dhuhr);
    Serial.println("Asr: " + asr);
    Serial.println("Maghrib: " + maghrib);
    Serial.println("Isha: " + isha);

    showNextAzanOnDisplay();
  }
  else {
    Serial.print("HTTP request failed. Code: ");
    Serial.println(httpCode);
    showMessage("HTTP Error", String(httpCode));
  }

  http.end();
}

void setup() {
  Serial.begin(115200);

  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  display.begin();
  showMessage("Starting...", "Please wait");

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  showMessage("Connecting", "WiFi...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
  showMessage("WiFi", "Connected");

  delay(1000);

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  waitForTimeSync();

  getPrayerTimes();
}

void loop() {
  checkAzanTime();

  // Refresh display every 10 seconds so time-left updates
  showNextAzanOnDisplay();

  delay(10000);
}