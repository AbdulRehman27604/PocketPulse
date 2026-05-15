#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <ArduinoJson.h>

// 1.3 inch OLED: usually SH1106
// ESP32 I2C pins: SDA = 21, SCL = 22
U8G2_SH1106_128X64_NONAME_F_HW_I2C oled(U8G2_R0, U8X8_PIN_NONE);

// WiFi details
const char* ssid = "TELUSWiFi6977";
const char* password = "hQef4qxhfP";

// Render Flask endpoint
String serverUrl = "https://aiwatch-i0mk.onrender.com/ask";

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin(21, 22);

  oled.begin();
  showText("ESP32 AI\nStarting...");

  connectToWiFi();

  showText("Ready!\nType question\nin Serial.");
  Serial.println("Type your question and press Enter:");
}

void loop() {
  if (Serial.available()) {
    String question = Serial.readStringUntil('\n');
    question.trim();

    if (question.length() > 0) {
      Serial.print("Question: ");
      Serial.println(question);

      showText("Asking AI...");

      String answer = askAI(question);

      Serial.println("AI Answer:");
      Serial.println(answer);
      Serial.println();

      showText(answer);

      Serial.println("Type another question:");
    }
  }
}

void connectToWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");
  showText("Connecting\nWiFi...");

  int attempts = 0;

  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected!");
    Serial.print("ESP32 IP: ");
    Serial.println(WiFi.localIP());

    showText("WiFi connected!");
    delay(1000);
  } else {
    Serial.println("WiFi failed.");
    showText("WiFi failed.\nCheck WiFi.");
  }
}

String askAI(String question) {
  if (WiFi.status() != WL_CONNECTED) {
    return "WiFi not connected";
  }

  WiFiClientSecure client;
  client.setInsecure();  // needed for HTTPS without certificate setup

  HTTPClient http;
  http.setTimeout(30000); // 30 seconds

  Serial.print("Sending request to: ");
  Serial.println(serverUrl);

  bool beginOk = http.begin(client, serverUrl);

  if (!beginOk) {
    return "HTTP begin failed";
  }

  http.addHeader("Content-Type", "application/json");

  StaticJsonDocument<512> requestDoc;
  requestDoc["message"] = question;

  String requestBody;
  serializeJson(requestDoc, requestBody);

  Serial.print("Sending JSON: ");
  Serial.println(requestBody);

  int httpCode = http.POST(requestBody);

  Serial.print("HTTP Code: ");
  Serial.println(httpCode);

  if (httpCode <= 0) {
    Serial.print("HTTP error: ");
    Serial.println(http.errorToString(httpCode));
    http.end();
    return "HTTP failed";
  }

  String responseBody = http.getString();
  http.end();

  Serial.println("Raw response:");
  Serial.println(responseBody);

  StaticJsonDocument<4096> responseDoc;
  DeserializationError error = deserializeJson(responseDoc, responseBody);

  if (error) {
    Serial.print("JSON error: ");
    Serial.println(error.c_str());
    return "JSON parse error";
  }

  if (responseDoc.containsKey("reply")) {
    return responseDoc["reply"].as<String>();
  }

  if (responseDoc.containsKey("error")) {
    return responseDoc["error"].as<String>();
  }

  return "No reply found";
}

void showText(String text) {
  oled.clearBuffer();
  oled.setFont(u8g2_font_6x10_tf);

  int x = 0;
  int y = 10;
  int lineHeight = 10;
  int maxWidth = 128;   // OLED width in pixels

  String line = "";
  String word = "";

  for (int i = 0; i <= text.length(); i++) {
    char c = text[i];

    // End of word if space, newline, or end of text
    if (c == ' ' || c == '\n' || i == text.length()) {
      String testLine;

      if (line.length() == 0) {
        testLine = word;
      } else {
        testLine = line + " " + word;
      }

      // Check pixel width, not character count
      if (oled.getStrWidth(testLine.c_str()) <= maxWidth) {
        line = testLine;
      } else {
        // Print current line
        oled.drawStr(x, y, line.c_str());
        y += lineHeight;

        // Start new line with current word
        line = word;

        if (y > 64) {
          break;
        }
      }

      word = "";

      // Force new line if original text had \n
      if (c == '\n') {
        oled.drawStr(x, y, line.c_str());
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

  // Print last line
  if (line.length() > 0 && y <= 64) {
    oled.drawStr(x, y, line.c_str());
  }

  oled.sendBuffer();
}