#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <U8g2lib.h>
#include <Wire.h>

// OLED display
// Most 1.3 inch OLEDs are SH1106 128x64 I2C
U8G2_SH1106_128X64_NONAME_F_HW_I2C display(U8G2_R0, U8X8_PIN_NONE);

// WiFi details
const char* ssid = "Stromfiber Lounge";
const char* password = "53833464";

// Karachi coordinates
const float latitude = 24.927024;
const float longitude = 67.053912;

// Weather values
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
  if (code >= 95) return "Thunderstorm";

  return "Unknown";
}

void showMessage(String line1, String line2) {
  display.clearBuffer();

  display.setFont(u8g2_font_6x12_tf);
  display.drawStr(0, 20, line1.c_str());
  display.drawStr(0, 40, line2.c_str());

  display.sendBuffer();
}

void showWeatherOnDisplay() {
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

void getWeather() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected");
    showMessage("WiFi Error", "Not connected");
    return;
  }

  HTTPClient http;

  String url = "https://api.open-meteo.com/v1/forecast?";
  url += "latitude=" + String(latitude, 4);
  url += "&longitude=" + String(longitude, 4);
  url += "&current=temperature_2m,relative_humidity_2m,weather_code";
  url += "&timezone=Asia%2FKarachi";

  Serial.println("Requesting weather...");
  http.begin(url);

  int httpCode = http.GET();

  if (httpCode > 0) {
    String payload = http.getString();

    StaticJsonDocument<2048> doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
      Serial.print("JSON parsing failed: ");
      Serial.println(error.c_str());
      showMessage("JSON Error", "Parse failed");
      http.end();
      return;
    }

    temperature = doc["current"]["temperature_2m"];
    humidity = doc["current"]["relative_humidity_2m"];
    int weatherCode = doc["current"]["weather_code"];

    weatherText = weatherCodeToText(weatherCode);

    Serial.println();
    Serial.println("Karachi Weather");
    Serial.println("----------------");
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" C");

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    Serial.print("Weather: ");
    Serial.println(weatherText);

    showWeatherOnDisplay();
  } 
  else {
    Serial.print("HTTP request failed. Error: ");
    Serial.println(httpCode);
    showMessage("HTTP Error", String(httpCode));
  }

  http.end();
}

void setup() {
  Serial.begin(115200);

  // Start OLED
  display.begin();
  showMessage("Starting...", "Please wait");

  // Connect WiFi
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

  getWeather();
}

void loop() {
  // Update weather every 10 minutes
  delay(10 * 60 * 1000);
  getWeather();
}