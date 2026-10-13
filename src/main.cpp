// =====================================================
// FIRMWARE
// ESP32 Environmental / Agriculture Sensor System
// =====================================================
//
// Sensors:
//   - Micro SD Card       -> CS GPIO 5
//   - DS18B20 Waterproof  -> GPIO 4
//   - DS18B20             -> GPIO 2
//   - DHT11/DHT22         -> GPIO 0
//   - pH Sensor           -> GPIO 25
//   - Soil Moisture V2.0  -> GPIO 34
//
// Required Libraries:
//   - OneWire by Paul Stoffregen
//   - DallasTemperature by Miles Burton
//   - DHT sensor library by Adafruit
//   - Adafruit Unified Sensor
//
// =====================================================


// =====================================================
// LIBRARIES
// =====================================================

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>


// =====================================================
// PIN DEFINITIONS
// =====================================================

// SD Card
#define SD_CS_PIN 5

// DS18B20 sensors
#define DS18B20_WATERPROOF_PIN 4
#define DS18B20_PIN 2

// DHT sensor
#define DHT_PIN 0
#define DHT_TYPE DHT11
// Change DHT22 to DHT11 if you're using a DHT11.

// Analog sensors
#define PH_PIN 25
#define SOIL_PIN 34


// =====================================================
// SENSOR OBJECTS
// =====================================================

// DS18B20 #1 - Waterproof
OneWire oneWireWaterproof(DS18B20_WATERPROOF_PIN);
DallasTemperature ds18b20Waterproof(&oneWireWaterproof);

// DS18B20 #2
OneWire oneWireNormal(DS18B20_PIN);
DallasTemperature ds18b20Normal(&oneWireNormal);

// DHT
DHT dht(DHT_PIN, DHT_TYPE);


// =====================================================
// GLOBAL VARIABLES
// =====================================================

File sdFile;

const unsigned long MENU_TIMEOUT = 10000;

char menuOption = 0;
unsigned long time0;


// =====================================================
// pH CALIBRATION
// =====================================================
//
// IMPORTANT:
//
// These values are only starting values.
// Your actual pH module needs calibration.
//
// Formula:
// pH = (PH_SLOPE × voltage) + PH_OFFSET
//
// You should calibrate using known pH buffer solutions.
// =====================================================

float PH_SLOPE = -5.70;
float PH_OFFSET = 21.34;


// =====================================================
// SOIL MOISTURE CALIBRATION
// =====================================================
//
// These values depend on your particular sensor.
//
// First test the sensor while completely dry.
// Then test it in wet soil.
//
// Higher/lower readings depend on the sensor,
// so adjust these values based on your tests.
// =====================================================

int SOIL_DRY_VALUE = 3000;
int SOIL_WET_VALUE = 1200;


// =====================================================
// SETUP
// =====================================================

void setup()
{
    // -------------------------------------------------
    // Serial
    // -------------------------------------------------

    Serial.begin(115200);

    delay(1000);

    Serial.println();
    Serial.println("=================================");
    Serial.println(" ESP32 SENSOR FIRMWARE");
    Serial.println("=================================");


    // -------------------------------------------------
    // SD Card
    // -------------------------------------------------

    pinMode(SD_CS_PIN, OUTPUT);

    Serial.println("Initializing SD card...");

    if (!SD.begin(SD_CS_PIN))
    {
        Serial.println("ERROR: SD card failed or not present!");
    }
    else
    {
        Serial.println("SD card initialized.");
    }


    // -------------------------------------------------
    // DS18B20
    // -------------------------------------------------

    Serial.println("Initializing DS18B20 sensors...");

    ds18b20Waterproof.begin();
    ds18b20Normal.begin();

    Serial.print("Waterproof DS18B20 sensors found: ");
    Serial.println(ds18b20Waterproof.getDeviceCount());

    Serial.print("Normal DS18B20 sensors found: ");
    Serial.println(ds18b20Normal.getDeviceCount());


    // -------------------------------------------------
    // DHT
    // -------------------------------------------------

    Serial.println("Initializing DHT sensor...");

    dht.begin();


    // -------------------------------------------------
    // Analog sensors
    // -------------------------------------------------

    pinMode(PH_PIN, INPUT);
    pinMode(SOIL_PIN, INPUT);

    // ESP32 ADC resolution:
    // 12-bit = values from 0 to 4095
    analogReadResolution(12);


    // -------------------------------------------------
    // Finished
    // -------------------------------------------------

    Serial.println();
    Serial.println("All sensors initialized.");
    Serial.println();


    // Show menu
    menuOption = menu();
}


// =====================================================
// MAIN LOOP
// =====================================================

void loop()
{

    // =================================================
    // OPTION 1
    // MICRO SD CARD
    // =================================================

    if (menuOption == '1')
    {
        Serial.println();
        Serial.println("Testing Micro SD card...");

        sdFile = SD.open("/datalog.txt", FILE_WRITE);

        if (sdFile)
        {
            sdFile.println("ESP32 Sensor Test");

            sdFile.close();

            Serial.println("Successfully wrote to datalog.txt");
        }
        else
        {
            Serial.println("ERROR: Could not open datalog.txt");
        }
    }


    // =================================================
    // OPTION 2
    // WATERPROOF DS18B20
    // =================================================

    else if (menuOption == '2')
    {
        ds18b20Waterproof.requestTemperatures();

        float temperature =
            ds18b20Waterproof.getTempCByIndex(0);

        Serial.print("Waterproof DS18B20: ");

        if (temperature == DEVICE_DISCONNECTED_C)
        {
            Serial.println("Sensor disconnected!");
        }
        else
        {
            Serial.print(temperature);
            Serial.println(" °C");
        }
    }


    // =================================================
    // OPTION 3
    // NORMAL DS18B20
    // =================================================

    else if (menuOption == '3')
    {
        ds18b20Normal.requestTemperatures();

        float temperature =
            ds18b20Normal.getTempCByIndex(0);

        Serial.print("DS18B20: ");

        if (temperature == DEVICE_DISCONNECTED_C)
        {
            Serial.println("Sensor disconnected!");
        }
        else
        {
            Serial.print(temperature);
            Serial.println(" °C");
        }
    }


    // =================================================
    // OPTION 4
    // DHT11 / DHT22
    // =================================================

    else if (menuOption == '4')
    {
        float humidity = dht.readHumidity();
        float temperature = dht.readTemperature();


        if (isnan(humidity) || isnan(temperature))
        {
            Serial.println("ERROR: Failed to read DHT sensor!");
        }
        else
        {
            Serial.print("Humidity: ");
            Serial.print(humidity);
            Serial.println(" %");

            Serial.print("Temperature: ");
            Serial.print(temperature);
            Serial.println(" °C");
        }
    }


    // =================================================
    // OPTION 5
    // pH SENSOR
    // =================================================

    else if (menuOption == '5')
    {
        Serial.println();
        Serial.println("Reading pH sensor...");


        // Read ADC
        int rawPH = analogRead(PH_PIN);


        // Convert ADC reading to voltage
        float voltagePH =
            (rawPH / 4095.0) * 3.3;


        // Calculate estimated pH
        float pH =
            (PH_SLOPE * voltagePH) + PH_OFFSET;


        Serial.print("Raw ADC: ");
        Serial.println(rawPH);

        Serial.print("Voltage: ");
        Serial.print(voltagePH, 3);
        Serial.println(" V");

        Serial.print("Estimated pH: ");
        Serial.println(pH, 2);
    }


    // =================================================
    // OPTION 6
    // CAPACITIVE SOIL MOISTURE V2.0
    // =================================================

    else if (menuOption == '6')
    {
        Serial.println();
        Serial.println("Reading soil moisture...");


        // Read analog value
        int rawSoil = analogRead(SOIL_PIN);


        // Convert to percentage
        int soilPercent =
            map(
                rawSoil,
                SOIL_DRY_VALUE,
                SOIL_WET_VALUE,
                0,
                100
            );


        // Keep within 0-100%
        soilPercent =
            constrain(soilPercent, 0, 100);


        Serial.print("Raw ADC: ");
        Serial.println(rawSoil);

        Serial.print("Soil Moisture: ");
        Serial.print(soilPercent);
        Serial.println(" %");
    }


    // =================================================
    // MENU TIMEOUT
    // =================================================

    if (millis() - time0 > MENU_TIMEOUT)
    {
        menuOption = menu();
    }


    delay(500);
}


// =====================================================
// MENU FUNCTION
// =====================================================

char menu()
{
    Serial.println();
    Serial.println("=================================");
    Serial.println("       SENSOR TEST MENU");
    Serial.println("=================================");

    Serial.println("(1) Micro SD module");
    Serial.println("(2) DS18B20 - Waterproof");
    Serial.println("(3) DS18B20");
    Serial.println("(4) DHT11/DHT22");
    Serial.println("(5) pH Sensor");
    Serial.println("(6) Capacitive Soil Moisture V2.0");

    Serial.println();
    Serial.println("Enter selection:");
    Serial.println("=================================");


    // Wait for serial input
    while (!Serial.available())
    {
        delay(10);
    }


    while (Serial.available())
    {
        char c = Serial.read();


        // -------------------------------------------------
        // Micro SD
        // -------------------------------------------------

        if (c == '1')
        {
            Serial.println("Now testing Micro SD module.");

            time0 = millis();

            return '1';
        }


        // -------------------------------------------------
        // Waterproof DS18B20
        // -------------------------------------------------

        else if (c == '2')
        {
            Serial.println(
                "Now testing waterproof DS18B20."
            );

            time0 = millis();

            return '2';
        }


        // -------------------------------------------------
        // Normal DS18B20
        // -------------------------------------------------

        else if (c == '3')
        {
            Serial.println(
                "Now testing DS18B20."
            );

            time0 = millis();

            return '3';
        }


        // -------------------------------------------------
        // DHT
        // -------------------------------------------------

        else if (c == '4')
        {
            Serial.println(
                "Now testing DHT11/DHT22."
            );

            time0 = millis();

            return '4';
        }


        // -------------------------------------------------
        // pH
        // -------------------------------------------------

        else if (c == '5')
        {
            Serial.println(
                "Now testing pH sensor."
            );

            time0 = millis();

            return '5';
        }


        // -------------------------------------------------
        // Soil Moisture
        // -------------------------------------------------

        else if (c == '6')
        {
            Serial.println(
                "Now testing capacitive soil moisture sensor."
            );

            time0 = millis();

            return '6';
        }
    }


    Serial.println("Illegal input!");

    return 0;
}
