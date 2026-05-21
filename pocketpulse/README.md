# AiWatch Separated Files

This ESP32 sketch is split into separate files.

## Pages

Turn the potentiometer to switch pages:

1. AI page
2. Current time page
3. Karachi weather page
4. Next azan + time left page
5. All azan times page

## Required Arduino libraries

Install these in Arduino IDE:

- U8g2
- ArduinoJson

## Upload instructions

1. Download and unzip this folder.
2. Open `AiWatch_Separated_Files.ino` in Arduino IDE.
3. Edit `config.h`.
4. Put your WiFi name and password.
5. Select your ESP32 board.
6. Upload.

## Wiring

### OLED

| OLED | ESP32 |
|---|---|
| VCC | 3V3 |
| GND | GND |
| SDA | GPIO 21 |
| SCL | GPIO 22 |

### Potentiometer

| Pot pin | ESP32 |
|---|---|
| Left | 3V3 |
| Middle | GPIO 34 |
| Right | GND |

### Buzzer

| Buzzer | ESP32 |
|---|---|
| + | GPIO 25 |
| - | GND |

## Important

Use 3.3V for the potentiometer. ESP32 GPIO pins are not 5V tolerant.

If your OLED is blank, it may be SSD1306 instead of SH1106.
Check the comment in `display_pages.cpp`.
