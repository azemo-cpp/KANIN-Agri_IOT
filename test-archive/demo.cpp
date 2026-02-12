#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <random>
#include <iomanip>
#include <sstream>
using namespace std;

// time format structure
string getTimeStamp() {
    auto now = chrono::system_clock::now();
    time_t now_time = chrono::system_clock::to_time_t(now);
    chrono::system_clock::to_time_t(now);
    tm *ltm = localtime(&now_time);

    stringstream ss;
    ss << put_time(ltm, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

int main() {
   // Open file in append mode
    ofstream logFile("iot_datalog.txt", ios::app);

    if (!logFile.is_open()) {
        cout << "Error opening file!" << endl;
        return 1;
    }

    cout << "Starting IoT Simulation...\n";
    cout << "Logging data locally to iot_datalog.txt\n\n";

    // Random number generators
    random_device rd;
    mt19937 gen(rd());

    uniform_int_distribution<> soilDist(300, 800);      // Analog soil values
    uniform_real_distribution<> tempDist(20.0, 35.0);   // Temperature in °C
    uniform_real_distribution<> humDist(40.0, 90.0);    // Humidity %

    while (true) {

        int soilMoisture = soilDist(gen);
        double temperature = tempDist(gen);
        double humidity = humDist(gen);

        string timestamp = getTimeStamp();

        // Print to console
        cout << timestamp 
             << " | Soil: " << soilMoisture
             << " | Temp: " << temperature
             << "°C | Humidity: " << humidity << "%" 
             << endl;

        // Log to file
        logFile << timestamp << ","
                << soilMoisture << ","
                << temperature << ","
                << humidity << endl;

        logFile.flush();

        // Simulate 1 second interval
        this_thread::sleep_for(chrono::seconds(1));
    }

    logFile.close();
    return 0;
}