#ifndef SETUP_PORTAL_H
#define SETUP_PORTAL_H

#include <Arduino.h>

extern String savedSSID;
extern String savedPassword;

void loadSavedSettings();
void saveSettings(String ssid, String password);
bool connectToSavedWiFi();

void startSetupPortal();

void startSettingsServer();
void handleSettingsServer();

void clearSavedSettings();

#endif