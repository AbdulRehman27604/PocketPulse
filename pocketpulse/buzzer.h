#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>

void setupBuzzer();

void buzzForOneMinute(String prayerName);
void buzzForAlarm(String alarmName);

#endif