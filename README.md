# PocketPulse

PocketPulse is an ESP32-based smart desktop assistant that combines an OLED interface, voice AI, weather updates, prayer time tracking, alarms, and a local Wi-Fi setup portal. The device uses an INMP441 microphone for push-to-talk voice input, sends requests to a backend AI server, and displays useful responses on a small SH1106 OLED screen.

## Features

- Voice AI assistant using an INMP441 I2S microphone
- Serial Monitor AI chat support
- OLED page system with button navigation
- Current time and date using NTP
- Karachi weather updates
- Karachi prayer times with next Azan countdown
- Buzzer alert at prayer time
- Custom alarm with web-based setup page
- Local setup portal for saving Wi-Fi credentials
- Modular Arduino/C++ code structure

## Hardware Used

| Component | Purpose |
|---|---|
| ESP32 Dev Module | Main microcontroller |
| SH1106 128x64 OLED | Display output |
| INMP441 I2S Microphone | Voice recording |
| Active Buzzer | Alarm and Azan alerts |
| Push Button | Page navigation |
| Push-to-talk Button | Voice AI recording |
| Pause Button | Stop alarm melody |
| 3.7V LiPo Battery | Portable power |
| TP4056 Charger Module | Battery charging |

## Pin Configuration

| Function | ESP32 Pin |
|---|---:|
| OLED SDA | 21 |
| OLED SCL | 22 |
| INMP441 WS | 15 |
| INMP441 SD | 32 |
| INMP441 SCK | 14 |
| Buzzer | 25 |
| Page Button | 27 |
| Voice Button | 33 |
| Pause Button | 18 |
| Potentiometer, optional | 34 |

## Project Structure

```text
PocketPulse/
├── pocketpulse.ino        # Main Arduino sketch
├── config.h               # Wi-Fi, API, pins, time, and location settings
├── ai_chat.cpp/.h         # Text AI requests through Serial Monitor
├── voice_ai.cpp/.h        # Voice recording and backend upload
├── display_pages.cpp/.h   # OLED display pages and text rendering
├── setup_portal.cpp/.h    # Wi-Fi and alarm web setup portal
├── wifi_time.cpp          # Wi-Fi time sync helpers
├── weather.cpp/.h         # Weather API integration
├── prayer.cpp/.h          # Prayer time API and Azan countdown
├── alarm.cpp/.h           # Custom alarm logic
├── buzzer.cpp/.h          # Buzzer tones and alarm melody
└── pitches.h              # Musical note frequency definitions
```

## Software Requirements

Install the following before uploading the project:

- Arduino IDE or PlatformIO
- ESP32 board package
- ArduinoJson library
- U8g2 library

The ESP32 core already includes libraries such as `WiFi.h`, `HTTPClient.h`, `WebServer.h`, `Preferences.h`, and I2S driver support.

## Setup Instructions

### 1. Clone the repository

```bash
git clone https://github.com/your-username/PocketPulse.git
cd PocketPulse
```

### 2. Create your config file

Before uploading to GitHub, do not commit your real Wi-Fi password. A safer approach is to create a `config.example.h` file and keep the real `config.h` private.

Example:

```cpp
#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// Wi-Fi settings
static const char* WIFI_SSID = "YOUR_WIFI_NAME";
static const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// Backend AI endpoint
static const char* AI_SERVER_URL = "https://your-backend-url.com/ask";

// Hardware pins
static const int POT_PIN = 34;
static const int BUZZER_PIN = 25;
static const int PAGE_BUTTON_PIN = 27;
static const int PAUSE_PIN = 18;

static const int OLED_SDA = 21;
static const int OLED_SCL = 22;

static const int MIC_I2S_WS = 15;
static const int MIC_I2S_SD = 32;
static const int MIC_I2S_SCK = 14;

static const int VOICE_BUTTON_PIN = 33;

// Time settings
static const char* NTP_SERVER = "pool.ntp.org";
static const long GMT_OFFSET_SEC = 3600 * 5;
static const int DAYLIGHT_OFFSET_SEC = 0;

// Location settings
static const float KARACHI_LATITUDE = 24.9270;
static const float KARACHI_LONGITUDE = 67.0539;

#endif
```

Add your private config file to `.gitignore`:

```text
config.h
```

### 3. Open the project

Open `pocketpulse.ino` in Arduino IDE. Make sure all `.cpp` and `.h` files are in the same sketch folder.

### 4. Select the board

In Arduino IDE:

```text
Tools > Board > ESP32 Arduino > ESP32 Dev Module
```

Then select the correct port.

### 5. Upload

Click **Upload**. Open the Serial Monitor at `115200` baud to view logs.

## How It Works

### Voice AI

When the voice button is held, the ESP32 records audio from the INMP441 microphone using I2S. The audio is converted into a WAV buffer and sent to the backend server using an HTTPS POST request. The backend returns a JSON response containing the transcribed question and AI reply, which is shown on the OLED display.

### Text AI

Questions can also be typed through the Serial Monitor. The ESP32 sends the text question to the configured AI server and displays the reply on the OLED.

### OLED Pages

PocketPulse has multiple OLED pages:

1. AI assistant page
2. Current time page
3. Weather page
4. Next Azan countdown page
5. All Azan times page

The page button cycles through these pages.

### Weather

The weather module fetches current Karachi weather data, including temperature, humidity, and weather condition.

### Prayer Times

The prayer module fetches Karachi prayer times, calculates the next prayer, and displays the remaining time. When the current time matches a prayer time, the buzzer plays an Azan alert.

### Alarm

The alarm module stores a custom alarm name, time, and enabled/disabled status. The alarm can be configured through the local web setup page.

### Setup Portal

If no saved Wi-Fi is found, PocketPulse starts a hotspot named:

```text
PocketPulse_Setup
```

Connect to it and open:

```text
192.168.4.1
```

From there, you can save Wi-Fi credentials or open the alarm setup page.


## Future Improvements

- Add battery percentage monitoring
- Add a settings page for city and timezone
- Add support for multiple prayer calculation methods
- Add OTA firmware updates
- Add deep sleep mode for better battery life
- Add a mobile-friendly dashboard
- Add certificate validation for HTTPS requests

## License

This project is for learning and personal use. Add a license file if you plan to publish it as an open-source project.

## Author

Created by Abdul Rehman.
