#ifndef WIFI_TIME_H
#define WIFI_TIME_H

#include <Arduino.h>

void connectToWiFi();
void setupTime();
bool waitForTimeSync();
int getCurrentMinutes();
String getCurrentTimeHHMM();

#endif
