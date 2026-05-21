#ifndef ALARM_H
#define ALARM_H

#include <Arduino.h>

extern String alarmName;
extern String alarmTime;
extern bool alarmEnabled;

void loadAlarmSettings();
void saveAlarmSettings(String name, String time, bool enabled);
void clearAlarmSettings();
void checkAlarm();

#endif