#ifndef DISPLAY_PAGES_H
#define DISPLAY_PAGES_H

#include <Arduino.h>
#include <U8g2lib.h>

extern U8G2_SH1106_128X64_NONAME_F_HW_I2C display;

void setupDisplay();
void showMessage(String line1, String line2);
void showWrappedText(String text);
void updateDisplayPage(int currentPage);

void showAIPage();
void showTimePage();
void showWeatherPage();
void showNextAzanPage();
void showAllAzanTimesPage();

#endif
