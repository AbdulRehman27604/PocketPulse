#include "alarm.h"
#include "wifi_time.h"
#include "buzzer.h"

#include <Preferences.h>

Preferences alarmPrefs;

String alarmName = "Alarm";
String alarmTime = "";
bool alarmEnabled = false;

String lastAlarmBuzzedTime = "";

void loadAlarmSettings() {
  alarmPrefs.begin("alarm", true);

  alarmName = alarmPrefs.getString("name", "Alarm");
  alarmTime = alarmPrefs.getString("time", "");
  alarmEnabled = alarmPrefs.getBool("enabled", false);

  alarmPrefs.end();

  Serial.println("Loaded alarm settings:");
  Serial.println("Alarm Name: " + alarmName);
  Serial.println("Alarm Time: " + alarmTime);
  Serial.print("Alarm Enabled: ");
  Serial.println(alarmEnabled ? "Yes" : "No");
}

void saveAlarmSettings(String name, String time, bool enabled) {
  alarmPrefs.begin("alarm", false);

  alarmPrefs.putString("name", name);
  alarmPrefs.putString("time", time);
  alarmPrefs.putBool("enabled", enabled);

  alarmPrefs.end();

  alarmName = name;
  alarmTime = time;
  alarmEnabled = enabled;

  Serial.println("Alarm settings saved:");
  Serial.println("Alarm Name: " + alarmName);
  Serial.println("Alarm Time: " + alarmTime);
  Serial.print("Alarm Enabled: ");
  Serial.println(alarmEnabled ? "Yes" : "No");
}

void clearAlarmSettings() {
  alarmPrefs.begin("alarm", false);
  alarmPrefs.clear();
  alarmPrefs.end();

  alarmName = "Alarm";
  alarmTime = "";
  alarmEnabled = false;
  lastAlarmBuzzedTime = "";

  Serial.println("Alarm settings cleared.");
}

void checkAlarm() {
  if (!alarmEnabled) {
    return;
  }

  if (alarmTime == "") {
    return;
  }

  String currentTime = getCurrentTimeHHMM();

  if (currentTime == "") {
    return;
  }

  // Stop repeated alarm in the same minute
  if (currentTime == lastAlarmBuzzedTime) {
    return;
  }

  if (currentTime == alarmTime) {
    lastAlarmBuzzedTime = currentTime;

    Serial.println("ALARM TIME!");
    Serial.println("Alarm: " + alarmName);

    buzzForAlarm(alarmName);
  }
}