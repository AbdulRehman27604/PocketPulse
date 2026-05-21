#include "setup_portal.h"
#include "display_pages.h"
#include "alarm.h"

#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

Preferences preferences;
WebServer server(80);

String savedSSID = "";
String savedPassword = "";

bool routesRegistered = false;

void loadSavedSettings() {
  preferences.begin("settings", true);

  savedSSID = preferences.getString("ssid", "");
  savedPassword = preferences.getString("password", "");

  preferences.end();

  Serial.println("Loaded saved settings:");
  Serial.println("SSID: " + savedSSID);
}

void saveSettings(String ssid, String password) {
  preferences.begin("settings", false);

  preferences.putString("ssid", ssid);
  preferences.putString("password", password);

  preferences.end();

  Serial.println("WiFi settings saved.");
}

bool connectToSavedWiFi() {
  if (savedSSID == "") {
    Serial.println("No saved WiFi.");
    return false;
  }

  // AP_STA = connect to home WiFi + keep setup hotspot active
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP("PocketPulse_Setup");

  WiFi.begin(savedSSID.c_str(), savedPassword.c_str());

  Serial.print("Connecting to saved WiFi: ");
  Serial.println(savedSSID);

  showMessage("Connecting", savedSSID);

  unsigned long startAttemptTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 20000) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected to saved WiFi.");

    Serial.print("Home WiFi IP: ");
    Serial.println(WiFi.localIP());

    Serial.print("Setup Hotspot IP: ");
    Serial.println(WiFi.softAPIP());

    showMessage("Setup IP", "192.168.4.1");
    delay(1500);

    return true;
  }

  Serial.println("Failed to connect to saved WiFi.");
  showMessage("WiFi Failed", "Setup mode");
  delay(1000);

  return false;
}

String setupPageHTML() {
  String html = "";

  html += "<!DOCTYPE html>";
  html += "<html>";
  html += "<head>";
  html += "<title>PocketPulse Setup</title>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>";
  html += "body{font-family:Arial;background:#111;color:white;padding:20px;}";
  html += ".box{max-width:400px;margin:auto;background:#222;padding:20px;border-radius:12px;}";
  html += "input{width:100%;padding:12px;margin:8px 0;border-radius:8px;border:none;box-sizing:border-box;}";
  html += "button{width:100%;padding:12px;background:#00c853;color:white;border:none;border-radius:8px;font-size:16px;margin-top:10px;}";
  html += "h2{text-align:center;}";
  html += "a{color:#00c853;}";
  html += ".linkbox{background:#333;padding:12px;border-radius:8px;margin-bottom:15px;text-align:center;}";
  html += "</style>";
  html += "</head>";
  html += "<body>";
  html += "<div class='box'>";
  html += "<h2>PocketPulse Setup</h2>";

  html += "<div class='linkbox'>";
  html += "<a href='/alarm'>Open Alarm Setup</a>";
  html += "</div>";

  html += "<form action='/save' method='POST'>";

  html += "<label>WiFi SSID</label>";
  html += "<input name='ssid' placeholder='Enter WiFi name' required>";

  html += "<label>WiFi Password</label>";
  html += "<input name='password' type='password' placeholder='Enter WiFi password'>";

  html += "<button type='submit'>Save WiFi and Restart</button>";

  html += "</form>";
  html += "</div>";
  html += "</body>";
  html += "</html>";

  return html;
}

String alarmPageHTML() {
  String html = "";

  html += "<!DOCTYPE html>";
  html += "<html>";
  html += "<head>";
  html += "<title>PocketPulse Alarm</title>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>";
  html += "body{font-family:Arial;background:#111;color:white;padding:20px;}";
  html += ".box{max-width:400px;margin:auto;background:#222;padding:20px;border-radius:12px;}";
  html += "input,select{width:100%;padding:12px;margin:8px 0 14px 0;border-radius:8px;border:none;box-sizing:border-box;}";
  html += "button{width:100%;padding:12px;background:#00c853;color:white;border:none;border-radius:8px;font-size:16px;margin-top:10px;}";
  html += "h2{text-align:center;}";
  html += ".status{background:#333;padding:12px;border-radius:8px;margin-bottom:14px;}";
  html += "a{color:#00c853;}";
  html += "</style>";
  html += "</head>";
  html += "<body>";
  html += "<div class='box'>";
  html += "<h2>Alarm Setup</h2>";

  html += "<div class='status'>";
  html += "<b>Current Alarm</b><br>";
  html += "Name: " + alarmName + "<br>";
  html += "Time: " + (alarmTime == "" ? String("Not set") : alarmTime) + "<br>";
  html += "Status: " + String(alarmEnabled ? "Enabled" : "Disabled");
  html += "</div>";

  html += "<form action='/saveAlarm' method='POST'>";

  html += "<label>Alarm Name</label>";
  html += "<input name='name' value='" + alarmName + "' placeholder='Wake up' required>";

  html += "<label>Alarm Time</label>";
  html += "<input name='time' type='time' value='" + alarmTime + "' required>";

  html += "<label>Alarm Status</label>";
  html += "<select name='enabled'>";

  if (alarmEnabled) {
    html += "<option value='1' selected>Enabled</option>";
    html += "<option value='0'>Disabled</option>";
  } else {
    html += "<option value='1'>Enabled</option>";
    html += "<option value='0' selected>Disabled</option>";
  }

  html += "</select>";

  html += "<button type='submit'>Save Alarm</button>";
  html += "</form>";

  html += "<p><a href='/'>Back to WiFi Setup</a></p>";

  html += "</div>";
  html += "</body>";
  html += "</html>";

  return html;
}

void registerWebRoutes() {
  if (routesRegistered) {
    return;
  }

  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", setupPageHTML());
  });

  server.on("/alarm", HTTP_GET, []() {
    server.send(200, "text/html", alarmPageHTML());
  });

  server.on("/save", HTTP_POST, []() {
    String ssid = server.arg("ssid");
    String password = server.arg("password");

    saveSettings(ssid, password);

    server.send(200, "text/html",
      "<html>"
      "<body style='font-family:Arial;background:#111;color:white;text-align:center;padding:30px;'>"
      "<h2>WiFi Saved!</h2>"
      "<p>Device will restart now.</p>"
      "</body>"
      "</html>"
    );

    delay(2000);
    ESP.restart();
  });

  server.on("/saveAlarm", HTTP_POST, []() {
    String name = server.arg("name");
    String time = server.arg("time");
    bool enabled = server.arg("enabled") == "1";

    saveAlarmSettings(name, time, enabled);

    server.send(200, "text/html",
      "<html>"
      "<body style='font-family:Arial;background:#111;color:white;text-align:center;padding:30px;'>"
      "<h2>Alarm Saved!</h2>"
      "<p>You can close this page.</p>"
      "<p><a href='/alarm' style='color:#00c853;'>Back to Alarm Setup</a></p>"
      "</body>"
      "</html>"
    );
  });

  routesRegistered = true;
}

void startSetupPortal() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP("PocketPulse_Setup");

  IPAddress ip = WiFi.softAPIP();

  Serial.println("Setup portal started.");
  Serial.print("Connect to WiFi: ");
  Serial.println("PocketPulse_Setup");
  Serial.print("Open: ");
  Serial.println(ip);

  showMessage("Setup Mode", "192.168.4.1");

  registerWebRoutes();
  server.begin();

  while (true) {
    server.handleClient();
    delay(10);
  }
}

void startSettingsServer() {
  registerWebRoutes();
  server.begin();

  Serial.println("Settings server started.");
  Serial.println("Connect to WiFi: PocketPulse_Setup");
  Serial.println("Open: 192.168.4.1");
}

void handleSettingsServer() {
  server.handleClient();
}

void clearSavedSettings() {
  preferences.begin("settings", false);
  preferences.clear();
  preferences.end();

  Serial.println("Saved WiFi settings cleared.");
}