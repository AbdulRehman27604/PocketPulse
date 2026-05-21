#include "weather.h"
#include "config.h"

#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

float temperature = 0;
int humidity = 0;
String weatherText = "Loading";

String weatherCodeToText(int code) {
  if (code == 0) return "Clear";
  if (code == 1) return "Mainly Clear";
  if (code == 2) return "Partly Cloudy";
  if (code == 3) return "Cloudy";
  if (code == 45 || code == 48) return "Fog";
  if (code >= 51 && code <= 57) return "Drizzle";
  if (code >= 61 && code <= 67) return "Rain";
  if (code >= 71 && code <= 77) return "Snow";
  if (code >= 80 && code <= 82) return "Rain Showers";
  if (code == 95 || code == 96 || code == 99) return "Thunderstorm";
  return "Unknown";
}

void getWeather() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected");
    return;
  }

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;

  String url = "https://api.open-meteo.com/v1/forecast?";
  url += "latitude=" + String(KARACHI_LATITUDE, 4);
  url += "&longitude=" + String(KARACHI_LONGITUDE, 4);
  url += "&current=temperature_2m,relative_humidity_2m,weather_code";
  url += "&timezone=Asia%2FKarachi";

  Serial.println("Requesting weather...");
  http.begin(client, url);
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

  int httpCode = http.GET();

  Serial.print("Weather HTTP Code: ");
  Serial.println(httpCode);

  if (httpCode == 200) {
    String payload = http.getString();

    StaticJsonDocument<4096> doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
      Serial.print("Weather JSON failed: ");
      Serial.println(error.c_str());
      http.end();
      return;
    }

    temperature = doc["current"]["temperature_2m"];
    humidity = doc["current"]["relative_humidity_2m"];
    int weatherCode = doc["current"]["weather_code"];

    weatherText = weatherCodeToText(weatherCode);

    Serial.println("Karachi Weather");
    Serial.print("Temp: ");
    Serial.println(temperature);
    Serial.print("Humidity: ");
    Serial.println(humidity);
    Serial.print("Weather: ");
    Serial.println(weatherText);
  } else {
    Serial.println("Weather request failed");
  }

  http.end();
}
