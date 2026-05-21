#include "prayer.h"
#include "wifi_time.h"
#include "buzzer.h"

#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

String fajr = "--:--";
String dhuhr = "--:--";
String asr = "--:--";
String maghrib = "--:--";
String isha = "--:--";

String nextPrayerName = "--";
String nextPrayerTime = "--:--";
String timeLeftText = "--h --m";
String lastBuzzedTime = "";

String cleanTime(String timeValue) {
  int spaceIndex = timeValue.indexOf(" ");
  if (spaceIndex != -1) {
    timeValue = timeValue.substring(0, spaceIndex);
  }
  return timeValue;
}

void getPrayerTimes() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected");
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
  http.begin(client, url);
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

  int httpCode = http.GET();

  Serial.print("Prayer HTTP Code: ");
  Serial.println(httpCode);

  if (httpCode == 200) {
    String payload = http.getString();

    if (payload.length() == 0) {
      Serial.println("Prayer API empty reply");
      http.end();
      return;
    }

    StaticJsonDocument<8192> doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
      Serial.print("Prayer JSON failed: ");
      Serial.println(error.c_str());
      http.end();
      return;
    }

    fajr = cleanTime(doc["data"]["timings"]["Fajr"].as<String>());
    dhuhr = cleanTime(doc["data"]["timings"]["Dhuhr"].as<String>());
    asr = cleanTime(doc["data"]["timings"]["Asr"].as<String>());
    maghrib = cleanTime(doc["data"]["timings"]["Maghrib"].as<String>());
    isha = cleanTime(doc["data"]["timings"]["Isha"].as<String>());

    Serial.println("Karachi Azan Times");
    Serial.println("Fajr: " + fajr);
    Serial.println("Dhuhr: " + dhuhr);
    Serial.println("Asr: " + asr);
    Serial.println("Maghrib: " + maghrib);
    Serial.println("Isha: " + isha);
  } else {
    Serial.println("Prayer request failed");
  }

  http.end();
}

int timeStringToMinutes(String timeStr) {
  int colonIndex = timeStr.indexOf(":");

  if (colonIndex == -1) {
    return -1;
  }

  int hours = timeStr.substring(0, colonIndex).toInt();
  int minutes = timeStr.substring(colonIndex + 1).toInt();

  return (hours * 60) + minutes;
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
  } else if (currentMinutes < dhuhrMin) {
    nextPrayerName = "Dhuhr";
    nextPrayerTime = dhuhr;
    nextPrayerMinutes = dhuhrMin;
  } else if (currentMinutes < asrMin) {
    nextPrayerName = "Asr";
    nextPrayerTime = asr;
    nextPrayerMinutes = asrMin;
  } else if (currentMinutes < maghribMin) {
    nextPrayerName = "Maghrib";
    nextPrayerTime = maghrib;
    nextPrayerMinutes = maghribMin;
  } else if (currentMinutes < ishaMin) {
    nextPrayerName = "Isha";
    nextPrayerTime = isha;
    nextPrayerMinutes = ishaMin;
  } else {
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
}

void checkAzanTime() {
  String currentTime = getCurrentTimeHHMM();

  if (currentTime == "") {
    return;
  }

  if (currentTime == lastBuzzedTime) {
    return;
  }

  if (currentTime == fajr) {
    lastBuzzedTime = currentTime;
    buzzForOneMinute("Fajr");
  } else if (currentTime == dhuhr) {
    lastBuzzedTime = currentTime;
    buzzForOneMinute("Dhuhr");
  } else if (currentTime == asr) {
    lastBuzzedTime = currentTime;
    buzzForOneMinute("Asr");
  } else if (currentTime == maghrib) {
    lastBuzzedTime = currentTime;
    buzzForOneMinute("Maghrib");
  } else if (currentTime == isha) {
    lastBuzzedTime = currentTime;
    buzzForOneMinute("Isha");
  }
}
