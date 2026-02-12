#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <random>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <vector>
#include <numeric>
#include <atomic>
#include <mutex>

using namespace std;

// ===============================
// CONFIGURATION SECTION
// ===============================

struct Config {
    int loggingIntervalMs = 1000;
    int rollingWindowSize = 10;

    int soilMin = 300;
    int soilMax = 800;

    double tempMin = 20.0;
    double tempMax = 35.0;

    double humidityMin = 40.0;
    double humidityMax = 90.0;

    int soilAlertThreshold = 350;
    double tempAlertThreshold = 33.0;
    double humidityAlertThreshold = 85.0;
};

Config config;

// ===============================
// GLOBAL STATE
// ===============================

atomic<bool> running(true);
mutex logMutex;

string DEVICE_ID = "ESP32-SIM-001";

// ===============================
// UTILITY FUNCTIONS
// ===============================

string getTimestamp() {
    auto now = chrono::system_clock::now();
    time_t now_time = chrono::system_clock::to_time_t(now);
    tm *ltm = localtime(&now_time);

    stringstream ss;
    ss << put_time(ltm, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

double computeAverage(const vector<double>& values) {
    if (values.empty()) return 0.0;
    return accumulate(values.begin(), values.end(), 0.0) / values.size();
}

void logCSV(const string& line) {
    lock_guard<mutex> lock(logMutex);
    ofstream file("iot_log.csv", ios::app);
    file << line << endl;
}

void logJSON(const string& json) {
    lock_guard<mutex> lock(logMutex);
    ofstream file("iot_log.json", ios::app);
    file << json << endl;
}

void logAlert(const string& message) {
    lock_guard<mutex> lock(logMutex);
    ofstream file("alerts.log", ios::app);
    file << getTimestamp() << " | " << message << endl;
}

// ===============================
// SENSOR SIMULATION
// ===============================

class SensorSimulator {
private:
    mt19937 gen;
    uniform_real_distribution<> noiseDist;

public:
    SensorSimulator() : gen(random_device{}()), noiseDist(-2.0, 2.0) {}

    int generateSoil() {
        uniform_int_distribution<> dist(config.soilMin, config.soilMax);
        return dist(gen);
    }

    double generateTemperature() {
        uniform_real_distribution<> dist(config.tempMin, config.tempMax);
        return dist(gen) + noiseDist(gen);
    }

    double generateHumidity() {
        uniform_real_distribution<> dist(config.humidityMin, config.humidityMax);
        return dist(gen) + noiseDist(gen);
    }
};

// ===============================
// ALERT SYSTEM
// ===============================

void checkAlerts(int soil, double temp, double humidity) {
    if (soil < config.soilAlertThreshold) {
        logAlert("Low Soil Moisture Detected!");
        cout << "⚠ ALERT: Low Soil Moisture!" << endl;
    }

    if (temp > config.tempAlertThreshold) {
        logAlert("High Temperature Detected!");
        cout << "⚠ ALERT: High Temperature!" << endl;
    }

    if (humidity > config.humidityAlertThreshold) {
        logAlert("High Humidity Detected!");
        cout << "⚠ ALERT: High Humidity!" << endl;
    }
}

// ===============================
// SYSTEM MONITORING
// ===============================

struct SystemStats {
    int totalReadings = 0;
    chrono::time_point<chrono::system_clock> startTime;
};

void printSystemStatus(const SystemStats& stats) {
    auto now = chrono::system_clock::now();
    auto uptime = chrono::duration_cast<chrono::seconds>(now - stats.startTime).count();

    cout << "\n===== SYSTEM STATUS =====" << endl;
    cout << "Device ID: " << DEVICE_ID << endl;
    cout << "Total Readings: " << stats.totalReadings << endl;
    cout << "Uptime: " << uptime << " seconds" << endl;
    cout << "Logging Interval: " << config.loggingIntervalMs << " ms" << endl;
    cout << "=========================\n" << endl;
}

// ===============================
// MAIN LOOP
// ===============================

int main() {

    SensorSimulator sensor;
    SystemStats stats;
    stats.startTime = chrono::system_clock::now();

    vector<double> soilHistory;
    vector<double> tempHistory;
    vector<double> humidityHistory;

    cout << "========================================" << endl;
    cout << "  K.A.N.I.N. SIMULATION SYSTEM   v2.6.1" << endl;
    cout << "  Device: " << DEVICE_ID << endl;
    cout << "========================================" << endl;
    cout << "Press 'q' then Enter to quit.\n" << endl;

    thread inputThread([](){
        string input;
        while (running) {
            cin >> input;
            if (input == "q") {
                running = false;
            }
        }
    });

    while (running) {

        int soil = sensor.generateSoil();
        double temp = sensor.generateTemperature();
        double humidity = sensor.generateHumidity();

        string timestamp = getTimestamp();

        // Maintain rolling history
        soilHistory.push_back(soil);
        tempHistory.push_back(temp);
        humidityHistory.push_back(humidity);

        if (soilHistory.size() > config.rollingWindowSize)
            soilHistory.erase(soilHistory.begin());
        if (tempHistory.size() > config.rollingWindowSize)
            tempHistory.erase(tempHistory.begin());
        if (humidityHistory.size() > config.rollingWindowSize)
            humidityHistory.erase(humidityHistory.begin());

        double soilAvg = computeAverage(soilHistory);
        double tempAvg = computeAverage(tempHistory);
        double humidityAvg = computeAverage(humidityHistory);

        // Console Output
        cout << timestamp
             << " | Soil: " << soil
             << " (Avg: " << soilAvg << ")"
             << " | Temp: " << temp
             << "°C (Avg: " << tempAvg << ")"
             << " | Humidity: " << humidity
             << "% (Avg: " << humidityAvg << ")"
             << endl;

        // CSV Logging
        stringstream csvLine;
        csvLine << timestamp << ","
                << DEVICE_ID << ","
                << soil << ","
                << soilAvg << ","
                << temp << ","
                << tempAvg << ","
                << humidity << ","
                << humidityAvg;

        logCSV(csvLine.str());

        // JSON Logging
        stringstream jsonLine;
        jsonLine << "{"
                 << "\"timestamp\":\"" << timestamp << "\","
                 << "\"device\":\"" << DEVICE_ID << "\","
                 << "\"soil\":" << soil << ","
                 << "\"soil_avg\":" << soilAvg << ","
                 << "\"temperature\":" << temp << ","
                 << "\"temperature_avg\":" << tempAvg << ","
                 << "\"humidity\":" << humidity << ","
                 << "\"humidity_avg\":" << humidityAvg
                 << "}";

        logJSON(jsonLine.str());

        checkAlerts(soil, temp, humidity);

        stats.totalReadings++;

        if (stats.totalReadings % 20 == 0) {
            printSystemStatus(stats);
        }

        this_thread::sleep_for(
            chrono::milliseconds(config.loggingIntervalMs)
        );
    }

    inputThread.join();

    cout << "\nSystem shutting down safely..." << endl;
    printSystemStatus(stats);

    return 0;
}