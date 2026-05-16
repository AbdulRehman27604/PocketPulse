#include "ai_chat.h"
#include "config.h"
#include "display_pages.h"

#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

String lastAIAnswer = "Type question in Serial Monitor.";
String lastAIQuestion = "";

String askAI(String question) {
  if (WiFi.status() != WL_CONNECTED) {
    return "WiFi not connected";
  }

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;
  http.setTimeout(30000);

  bool beginOk = http.begin(client, AI_SERVER_URL);

  if (!beginOk) {
    return "HTTP begin failed";
  }

  http.addHeader("Content-Type", "application/json");

  StaticJsonDocument<512> requestDoc;
  requestDoc["message"] = question;

  String requestBody;
  serializeJson(requestDoc, requestBody);

  int httpCode = http.POST(requestBody);

  if (httpCode <= 0) {
    http.end();
    return "HTTP failed";
  }

  String responseBody = http.getString();
  http.end();

  StaticJsonDocument<4096> responseDoc;
  DeserializationError error = deserializeJson(responseDoc, responseBody);

  if (error) {
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

void handleSerialAI() {
  if (Serial.available()) {
    String question = Serial.readStringUntil('\n');
    question.trim();

    if (question.length() > 0) {
      lastAIQuestion = question;
      lastAIAnswer = "Asking AI...";
      showAIPage();

      Serial.print("Question: ");
      Serial.println(question);

      String answer = askAI(question);

      lastAIAnswer = answer;

      Serial.println("AI Answer:");
      Serial.println(answer);
      Serial.println();
      Serial.println("Type another question:");
    }
  }
}
