//SD Card-----------------
#include "FS.h"
#include "SD.h"
#include <SPI.h>
//I2C---------------------
#include <Wire.h>
//RTC--------------------
#include "RTClib.h"
//INA219-----------------
#include <Adafruit_INA219.h>
//LUX------------------
#include <BH1750.h>

//SD Card-----------------
// Define CS pin for the SD card module
#define SD_CS 5
String dataMessage;

//RTC-------------------
RTC_DS3231 rtc;
int tahun = 0;
byte bulan = 0;
byte tanggal = 0;
byte jam = 9;
byte menit = 0;
byte detik = 0;


//INA219-----------------
Adafruit_INA219 INA219_1 (0x40); // -
Adafruit_INA219 INA219_2 (0x41); // A0 short
Adafruit_INA219 INA219_3 (0x44); // A1 short
Adafruit_INA219 INA219_4 (0x45); // A0 dan A1 short

float tegangan_1, arus_1;
float tegangan_2, arus_2;
float tegangan_3, arus_3;
float tegangan_4, arus_4;


//LUX------------------------
BH1750 lightMeter(0x23);
float lux;


//Buzzer-------------------
#define Buzzer_pin 12

//LED----------------------
#define LED 4


void setup() {

  // Start serial communication for debugging purposes
  Serial.begin(9600);

  //buzzer-------------------------------------------
  pinMode (Buzzer_pin, OUTPUT);

  //Led
  pinMode (LED, OUTPUT);

  //SD CARD--------------------------------------------
  // Initialize SD card
  SD.begin(SD_CS);
  if (!SD.begin(SD_CS)) {
    Serial.println("Card Mount Failed");
    digitalWrite (LED, HIGH);
    return;
  }
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    digitalWrite (LED, HIGH);
    return;
  }
  Serial.println("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("ERROR - SD card initialization failed!");
    digitalWrite (LED, HIGH);
    return;    // init failed
  }

  // If the data.txt file doesn't exist
  // Create a file on the SD card and write the data labels
  File file = SD.open("/data.txt");
  if (!file) {
    Serial.println("File doens't exist");
    Serial.println("Creating file...");
    writeFile(SD, "/data.txt", "Tanggal, Waktu, Tegangan_1, Arus_1, Tegangan_2, Arus_2,Tegangan_3, Arus_3,Tegangan_4, Arus_4, Cahaya, \r\n");
  }
  else {
    Serial.println("File already exists");
  }
  file.close();

  //I2C--------------------------------------
  Wire.begin();

  //RTC--------------------------------------
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    digitalWrite (LED, HIGH);
    Serial.flush();
    while (1) delay(10);
  }

  //INA219--------------------------------
  INA219_1.begin();
  INA219_2.begin();
  INA219_3.begin();
  INA219_4.begin();

  //LUX------------------------------------
  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println(F("BH1750 Advanced begin"));
  } else {
    Serial.println(F("Error initialising BH1750"));
    digitalWrite (LED, HIGH);
  }

  delay (3000);
  digitalWrite (LED, LOW);

  //CEK WAKTU SAAT SET UP----------------------------------
  //DateTime now = rtc.now();
  DateTime now = rtc.now() + TimeSpan(0, 0, 0, 16);
  //jika terjadi perbedaan waktu real

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

  Serial.println("");

}

void loop() {

  RTC_Time ();

  if ( jam >= 8 && jam <= 17) {

    if (menit == 0 || menit == 10 || menit == 20 || menit == 30 || menit == 40 || menit == 50) {
      digitalWrite (Buzzer_pin, HIGH);
      ina219_sensor();
      lux_sensor();
      logSDCard();
      Serial.println("DONE");
      delay (3000);
      digitalWrite (Buzzer_pin, LOW);
      delay (60000);
      digitalWrite (LED, LOW);
    }

  }

  delay (5000);
}


//void-void sensor----------------------------------------------------------

void ina219_sensor() {
  tegangan_1 = INA219_1.getBusVoltage_V();
  arus_1 = INA219_1.getCurrent_mA();

  tegangan_2 = INA219_2.getBusVoltage_V();
  arus_2 = INA219_2.getCurrent_mA();

  tegangan_3 = INA219_3.getBusVoltage_V();
  arus_3 = INA219_3.getCurrent_mA();

  tegangan_4 = INA219_4.getBusVoltage_V();
  arus_4 = INA219_4.getCurrent_mA();

  Serial.print("Tegangan 1 : ");  Serial.print(tegangan_1);  Serial.print(" Volt");
  Serial.print("\t\tArus 1 : ");  Serial.print(arus_1);      Serial.print(" mA\n\n");

  Serial.print("Tegangan 2 : ");  Serial.print(tegangan_2);  Serial.print(" Volt");
  Serial.print("\t\tArus 2 : ");  Serial.print(arus_2);      Serial.print(" mA\n\n");

  Serial.print("Tegangan 3 : ");  Serial.print(tegangan_3);  Serial.print(" Volt");
  Serial.print("\t\tArus 3 : ");  Serial.print(arus_3);      Serial.print(" mA\n\n");

  Serial.print("Tegangan 4 : ");  Serial.print(tegangan_4);  Serial.print(" Volt");
  Serial.print("\t\tArus 4 : ");  Serial.print(arus_4);      Serial.print(" mA\n\n");

  if (tegangan_1 == 32.76 || tegangan_2 == 32.76 || tegangan_3 == 32.76 || tegangan_4 == 32.76 ){
    digitalWrite (LED, HIGH);
  } 
}

void lux_sensor() {
  if (lightMeter.measurementReady()) {
    lux = lightMeter.readLightLevel();
    Serial.print("Light: ");
    Serial.print(lux);
    Serial.println(" lx");
  }
}



void RTC_Time () {
  //DateTime now = rtc.now();
  DateTime now = rtc.now() + TimeSpan(0, 0, 0, 16);
  //jika terjadi perbedaan waktu real

  tahun = now.year();
  bulan = now.month();
  tanggal = now.day();

  jam = now.hour();
  menit = now.minute();
  detik = now.second();
}



//VOID SD CARD------------------------------------------------------------------------
// Write the sensor readings on the SD card
void logSDCard() {
  dataMessage = String(tahun) + "/" + String(bulan) + "/" + String(tanggal) + "," +
                String(jam) + ":" + String(menit) + "," +
                String(tegangan_1) + "," + String(arus_1) + "," +
                String(tegangan_2) + "," + String(arus_2) + "," +
                String(tegangan_3) + "," + String(arus_3) + "," +
                String(tegangan_4) + "," + String(arus_4) + "," +
                String(lux) + "," +
                "\r\n";
  Serial.print("Save data: ");
  Serial.println(dataMessage);
  appendFile(SD, "/data.txt", dataMessage.c_str());
}

// Write to the SD card (DON'T MODIFY THIS FUNCTION)
void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    digitalWrite (LED, HIGH);
    return;
  }
  if (file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
    digitalWrite (LED, HIGH);
  }
  file.close();
}

// Append data to the SD card (DON'T MODIFY THIS FUNCTION)
void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for appending");
    digitalWrite (LED, HIGH);
    return;
  }
  if (file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
    digitalWrite (LED, HIGH);
  }
  file.close();
}
