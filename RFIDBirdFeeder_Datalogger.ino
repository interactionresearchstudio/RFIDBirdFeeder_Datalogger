/*
  SD card datalogger

  This example shows how to log data from three analog sensors
  to an SD card using the SD library.

  The circuit:
   analog sensors on analog ins 0, 1, and 2
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)

  created  24 Nov 2010
  modified 9 Apr 2012
  by Tom Igoe

  This example code is in the public domain.

*/

#include <SPI.h>
#include <SD.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include "RTClib.h"

const int chipSelect = 15;

SoftwareSerial reader = SoftwareSerial(2, 14);

unsigned long lastRx;
byte rfidData[5];
int dataIndex;

#define RXTIMEOUT 1000

RTC_PCF8523 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void setup() {
  Serial.begin(115200);

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
  }

  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1) {
      pinMode(0, OUTPUT);
      digitalWrite(0, HIGH);
      delay(100);
      digitalWrite(0, LOW);
      delay(100);
    }
  }
  Serial.println("card initialized.");

  reader.begin(19200);
}

void loop() {
  // Handle RX event.
  if (reader.available() > 0) {
    lastRx = millis();

    rfidData[dataIndex] = reader.read();
    Serial.print(rfidData[dataIndex], HEX);

    dataIndex++;
    if (dataIndex == 5) {
      Serial.println(" received.");
      saveToSD(rfidData);
      dataIndex = 0;
    }
    else {
      Serial.print(':');
    }
  }

  // Handle RX timeout and rewind index.
  if (millis() - lastRx >= RXTIMEOUT && dataIndex != 0) {
    dataIndex = 0;
    Serial.println("RX timeout.");
  }
}

void saveToSD(byte data[]) {
  File dataFile = SD.open("readings.txt", FILE_WRITE);

  DateTime now = rtc.now();

  if (dataFile) {
    pinMode(0, OUTPUT);
    digitalWrite(0, LOW);
    delay(500);
    digitalWrite(0, HIGH);
    dataFile.print(now.year(), DEC);
    dataFile.print('/');
    dataFile.print(now.month(), DEC);
    dataFile.print('/');
    dataFile.print(now.day(), DEC);
    dataFile.print(" (");
    dataFile.print(daysOfTheWeek[now.dayOfTheWeek()]);
    dataFile.print(") ");
    dataFile.print(now.hour(), DEC);
    dataFile.print(':');
    dataFile.print(now.minute(), DEC);
    dataFile.print(':');
    dataFile.print(now.second(), DEC);
    dataFile.print(", ");
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    for (int i = 0; i < 5; i++) {
      dataFile.print(data[i], HEX);
    }
    dataFile.println();
    dataFile.close();
    Serial.println("Saved reading to SD card.");
  }
  else {
    Serial.println("Error opening readings.txt!");
  }
}
