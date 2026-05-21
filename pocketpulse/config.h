#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// =====================================================
// WIFI SETTINGS
// =====================================================
// Use static in header files to avoid "multiple definition" linker errors.
static const char* WIFI_SSID = "TELUSWiFi6977";
static const char* WIFI_PASSWORD = "hQef4qxhfP";

// Your Render Flask AI endpoint
static const char* AI_SERVER_URL = "https://aiwatch-i0mk.onrender.com/ask";

// =====================================================
// HARDWARE PINS
// =====================================================
static const int POT_PIN = 34;       // Potentiometer middle pin
static const int BUZZER_PIN = 25;    // Active buzzer + pin
static const int PAGE_BUTTON_PIN = 27; // Button for changing pages
static const int PAUSE_PIN = 26;

// OLED I2C pins
static const int OLED_SDA = 21;
static const int OLED_SCL = 22;

// =====================================================
// TIME SETTINGS
// =====================================================
static const char* NTP_SERVER = "pool.ntp.org";
static const long GMT_OFFSET_SEC = 3600 * 5;  // Pakistan UTC+5
static const int DAYLIGHT_OFFSET_SEC = 0;

// =====================================================
// WEATHER SETTINGS - KARACHI
// =====================================================
static const float KARACHI_LATITUDE = 24.9270;
static const float KARACHI_LONGITUDE = 67.0539;

#endif
