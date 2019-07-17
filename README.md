# ESP8266 Binary Clock

This project creates a binary clock, which is shown on a tiny OLED LCD. The ESP connects to an NTP server via Wi-Fi to get the current time.

What do you need to create this project?

1. ESP8266 (ESP-12E)
2. 128*64 I2C OLED LCD
3. 4 jumper wires
4. 1 breadboard
5. 1 Micro USB cable

### Wiring

![ESP8266 Pinout](/images/esp8266_pinout.png)

GND goes to GND

VIN goes to 3.3V

SDA to I2C SDA (D2, GPIO 4)

SCL to I2C SCL (D1, GPIO 5)

Breadboard wiring:

![Breadboard wiring](/images/esp8266_oled_wiring_bb.png)

### Result

![Photo](/images/photo.jpg)
