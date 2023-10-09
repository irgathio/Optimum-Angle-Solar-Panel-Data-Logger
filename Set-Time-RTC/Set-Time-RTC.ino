/*
  Date and time functions using a DS3231 RTC connected via I2C and Wire lib
  This program only needs to be used once. 
  This program functions to set the RTC module time. 
  The time set follows the time on your PC
*/

#include "RTClib.h"
#include <Wire.h>

RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

byte tahun;
byte bulan;
byte tanggal;

byte jam;
byte menit;
byte detik;

void setup () {
  Serial.begin(9600);

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }



  // When time needs to be re-set on a previously configured device, the
  // following line sets the RTC to the date & time this sketch was compiled
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
}

void loop () {
  //DateTime now = rtc.now();
  DateTime now = rtc.now() + TimeSpan(0, 0, 0, 7);
  //If there is a time difference after setting, you can use TimeSpan to synchronize
  //The TimeSpan value obtained must also be entered into the main code

  tahun = now.year();
  bulan = now.month();
  tanggal = now.day();

  jam = now.hour();
  menit = now.minute();
  detik = now.second();

  Serial.print(tahun, DEC);
  Serial.print('/');
  Serial.print(bulan, DEC);
  Serial.print('/');
  Serial.print(tanggal, DEC);
  Serial.print(" ");

  Serial.print(jam, DEC);
  Serial.print(':');
  Serial.print(menit, DEC);
  Serial.print(':');
  Serial.print(detik, DEC);

  Serial.println();
  delay(3000);
}
