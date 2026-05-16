#ifndef PRAYER_H
#define PRAYER_H

#include <Arduino.h>

extern String fajr;
extern String dhuhr;
extern String asr;
extern String maghrib;
extern String isha;

extern String nextPrayerName;
extern String nextPrayerTime;
extern String timeLeftText;
extern String lastBuzzedTime;

String cleanTime(String timeValue);
void getPrayerTimes();
int timeStringToMinutes(String timeStr);
void calculateNextPrayer();
void checkAzanTime();

#endif
