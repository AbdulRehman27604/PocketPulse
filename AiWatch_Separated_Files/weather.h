#ifndef WEATHER_H
#define WEATHER_H

#include <Arduino.h>

extern float temperature;
extern int humidity;
extern String weatherText;

String weatherCodeToText(int code);
void getWeather();

#endif
