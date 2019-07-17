// esp8266binaryclock.ino
//
// Author: Szvetlin Tanyi <svetlintanyi@gmail.com>
//
// To find out easily your LCDs I2C address please use: https://www.instructables.com/id/ESP8266-I2C-PORT-and-Address-Scanner/
// Based on: https://www.instructables.com/id/Simplest-ESP8266-Local-Time-Internet-Clock-With-OL/
//
// Libraries needed:
//  Time.h & TimeLib.h:  https://github.com/PaulStoffregen/Time
//  Timezone.h: https://github.com/JChristensen/Timezone
//  SSD1306.h & SSD1306Wire.h:  https://github.com/squix78/esp8266-oled-ssd1306
//  NTPClient.h: https://github.com/arduino-libraries/NTPClient
//  ESP8266WiFi.h & WifiUDP.h: https://github.com/ekstrand/ESP8266wifi
//
// 128x64 OLED pinout:
// GND goes to ground
// Vin goes to 3.3V
// Data to I2C SDA (GPIO 4)
// Clk to I2C SCL (GPIO 5)

#include <ESP8266WiFi.h>
#include <WifiUDP.h>
#include <String.h>
#include <Wire.h>
#include <SSD1306.h>
#include <SSD1306Wire.h>
#include <NTPClient.h>
#include <Time.h>
#include <TimeLib.h>
#include <Timezone.h>

// Define NTP properties
#define NTP_ADDRESS  "0.hu.pool.ntp.org"  // change this to whatever pool is closest (see ntp.org)

// Set up the NTP UDP client
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS);

// Create a display object
#define SDA_PIN 4
#define SCL_PIN 5
SSD1306  display(0x3D, SDA_PIN, SCL_PIN); //0x3D for the Adafruit 1.3" OLED, 0x3C being the usual address of the OLED

const char* ssid = ""; // insert your own ssid
const char* password = ""; // and password

void setup ()
{
  Serial.begin(115200); // Serial output at 115200 baud rate
  timeClient.begin();   // Start the NTP UDP client

  Wire.pins(SDA_PIN, SCL_PIN);  // Start the OLED with GPIO 4 and 5 on ESP-12E
  Wire.begin(SDA_PIN, SCL_PIN);
  display.init(); // (0,0) is at top left corner (128,64) is at bottom right corner
  display.flipScreenVertically();

  // Connect to wifi
  Serial.println("");
  Serial.print("Connecting to ");
  Serial.print(ssid);
  display.drawString(0, 10, "Connecting to Wifi...");
  display.display();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi at ");
  Serial.print(WiFi.localIP());
  Serial.println("");
  display.drawString(0, 24, "Connected.");
  display.display();
  delay(1000);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) //Check WiFi connection status
  {
    // update the NTP client and get the UNIX UTC timestamp
    timeClient.update();
    unsigned long epochTime =  timeClient.getEpochTime();

    // Convert received time stamp to time_t object
    time_t local, utc;
    utc = epochTime;

    // Then convert the UTC UNIX timestamp to local time
    TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};  // Central European Summer Time
    TimeChangeRule CET = {"CET", Last, Sun, Oct, 3, 60};   // Central European Standard Time
    Timezone CE(CEST, CET);
    local = CE.toLocal(utc);

    // Calculate Hour, Minute, Seconds from local time.
    int H = hour(local);
    int M = minute(local);
    int S = second(local);

    // print the binary clock on the OLED
    display.clear();
    drawClock(H, M, S);
    display.display();
  }
  else // attempt to connect to wifi again if disconnected
  {
    display.clear();
    display.drawString(0, 10, "Connecting to Wifi...");
    display.display();
    WiFi.begin(ssid, password);
    display.drawString(0, 24, "Connected.");
    display.display();
    delay(1000);
  }

  delay(1000);    //Send a request to update every 1 sec (= 1000 ms)
}

// Get the first digit of a given number
int getFirstDigit(int number) {
  if (number < 10) {
    return 0;
  }
  while (number >= 10) {
    number /= 10;
  }

  return number;
}

// Draws the clock
void drawClock(int H, int M, int S) {
  drawHour(H);
  drawMinute(M);
  drawSecond(S);
}

// Draws the columns for the hours
void drawHour(int H) {
  int firstDigit = getFirstDigit(H);
  int secondDigit = H % 10;
  drawColumn(firstDigit, 2, 18, 34);
  drawColumn(secondDigit, 4, 34, 2);
}

// Draws the columns for the minutes
void drawMinute(int M) {
  int firstDigit = getFirstDigit(M);
  int secondDigit = M % 10;
  drawColumn(firstDigit, 3, 50, 18);
  drawColumn(secondDigit, 4, 66, 2);
}

// Draws the columns for the seconds
void drawSecond(int S) {
  int firstDigit = getFirstDigit(S);
  int secondDigit = S % 10;
  drawColumn(firstDigit, 3, 82, 18);
  drawColumn(secondDigit, 4, 98, 2);
}

// Draws a column
void drawColumn(int digit, int columnHeight, int startX, int startY) {
  int boxLength = 12; // length of the sides of the rectangle
  int value;
  //  iterates through a column, if the value is 1 it will draw a filled rectangle, else an empty one
  for (int i = columnHeight - 1; i >= 0; i--) {
    value = digit >> i;
    if ( value & 1) {
      display.fillRect(startX, startY, boxLength, boxLength);
    } else {
      display.drawRect(startX, startY, boxLength, boxLength);
    }
    startY += 16; // shift to next row
  }
}
