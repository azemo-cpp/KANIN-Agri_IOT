#include <Arduino.h>
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <SD.h>

/* =====================
   PIN DEFINITIONS
   ===================== */
#define DHT_PIN_DATA     4
#define DS18B20_PIN_DQ   16
#define SDFILE_PIN_CS    5
#define SOILMOISTURE     34

/* =====================
   SENSOR SETUP
   ===================== */
#define DHTTYPE DHT11
DHT dht(DHT_PIN_DATA, DHTTYPE);

OneWire oneWire(DS18B20_PIN_DQ);
DallasTemperature soilTempSensor(&oneWire);

/* =====================
   FILE
   ===================== */
File dataFile;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("K.A.N.I.N. system booting...");

  dht.begin();
  soilTempSensor.begin();

  if (!SD.begin(SDFILE_PIN_CS)) {
    Serial.println("SD card initialization FAILED");
    while (true);
  }

  Serial.println("SD card initialized");

  if (!SD.exists("/data.csv")) {
    dataFile = SD.open("/data.csv", FILE_WRITE);
    if (dataFile) {
      dataFile.println("AirTemp_C,Humidity_pct,SoilTemp_C,SoilRaw");
      dataFile.close();
    }
  }
}

void loop() {
  float airHumidity = dht.readHumidity();
  float airTemp = dht.readTemperature();

  if (isnan(airHumidity) || isnan(airTemp)) {
    Serial.println("DHT11 read failed");
    delay(2000);
    return;
  }

  soilTempSensor.requestTemperatures();
  float soilTemp = soilTempSensor.getTempCByIndex(0);

  int soilRaw = analogRead(SOILMOISTURE);

  Serial.println("------ SENSOR DATA ------");
  Serial.printf("Air Temp: %.2f C\n", airTemp);
  Serial.printf("Humidity: %.2f %%\n", airHumidity);
  Serial.printf("Soil Temp: %.2f C\n", soilTemp);
  Serial.printf("Soil Moisture Raw: %d\n", soilRaw);
  Serial.println("-------------------------");

  dataFile = SD.open("/data.csv", FILE_WRITE);
  if (dataFile) {
    dataFile.printf("%.2f,%.2f,%.2f,%d\n",
                    airTemp, airHumidity, soilTemp, soilRaw);
    dataFile.close();
    Serial.println("Data logged to SD");
  } else {
    Serial.println("SD write failed");
  }

  delay(15000);

  bool sensorError = false;

  // DHT 11
  if (isnan(airTemp) || isnan(airHumidity)) {
    Serial.println("ERROR: DHT11 FAILED TO READ");
    sensorError = true;
  }

  // DS18B20
  soilTempSensor.requestTemperatures();
  if (soilTemp == DEVICE_DISCONNECTED_C) {
    Serial.println("ERROR: SOIL TEMP FAILED TO READ");
    sensorError = true;
  }
  
  // Soil Moisture
  if (soilRaw == 0 || soilRaw == 4095) {
    Serial.println("ERROR: SOIL MOISTURE FAILED TO READ");
  }

  // Final Decision
  if (sensorError) {
    Serial.println("Data skipped due to sensor errors.");
    delay(2000);
    return;
  }
}