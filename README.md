# K.A.N.I.N. – Agri IoT System 🌾  
**Knowledge-driven Agriculture for Nurturing and Intelligent Nutrition**

An IoT-based rice (*Oryza sativa*) cultivation monitoring system designed to support
data-driven, sustainable farming through real-time environmental sensing, cloud
visualization, and intelligent analysis.

This project was developed as a quantitative research study at **Naga City Science High School**
and focuses on precision agriculture for small- to medium-scale rice fields.

## 📌 Project Overview

K.A.N.I.N. is a web-enabled crop monitoring system that integrates multiple environmental
sensors with microcontrollers to track critical conditions affecting rice growth.

Unlike basic IoT monitoring systems that rely on static thresholds, K.A.N.I.N. is designed
to support **intelligent decision-making**, helping farmers and researchers optimize
irrigation, soil health, and nutrient management.

**Core goals:**
- Improve rice yield monitoring accuracy  
- Reduce inefficient resource usage  
- Support sustainable, eco-friendly farming  
- Provide real-time and historical data access  

## ⚙️ System Architecture

- **ESP32**  
  Main microcontroller. Handles sensor data acquisition and preprocessing and manages Wi-Fi connectivity and cloud data transmission  

- **IoT Platform**  
  Cloud-based dashboard (e.g., Blynk IoT) for real-time visualization and logging  

- **Power System**  
  Solar-powered with rechargeable Ni-MH battery backup  

## 🌱 Features

- Real-time monitoring of rice field conditions  
- Soil and environmental sensing  
- Cloud-based data visualization  
- Timestamped data logging (15-minute intervals)  
- Portable, field-deployable design  
- Solar-powered and eco-friendly operation  

## 🔬 Sensors Used

- **DS18B20** – Soil temperature  
- **Capacitive Soil Moisture Sensor v2.0** – Soil moisture  
- **Gravity Analog pH Sensor** – Soil acidity/alkalinity  
- **DFRobot NPK Sensor (SEN0605)** – Nitrogen, Phosphorus, Potassium levels  
- **DHT11** – Air temperature and humidity
  
## 🧰 Hardware & Materials

### Main Components 
- ESP32 Wi-Fi/Bluetooth Microcontroller  

### Power & Energy
- 12V 3000mAh Ni-MH Battery Pack  
- 50W 12V Polycrystalline Solar Panel  

## 🛠️ Methodology Summary

1. **Hardware setup & wiring**  
   Sensors connected to Arduino Mega; ESP32 used for wireless communication  

2. **Programming & data handling**  
   - Arduino: sensor reading, calibration, preprocessing  
   - ESP32: cloud transmission and IoT integration  

3. **Calibration & testing**  
   Sensors calibrated using laboratory-grade references  

4. **Data collection**  
   - 15-minute intervals  
   - Timestamped using RTC  
   - Logged locally and to the cloud  

## 📊 Data Collected

- Soil moisture  
- Soil temperature  
- Air temperature  
- Air humidity  
- Soil pH  
- Soil nutrient levels (NPK)  

Data is analyzed for accuracy, reliability, and trends affecting rice growth.

## 🌍 Scope & Limitations

- Focused on **environmental monitoring only**
- Designed for **small to medium rice plots**
- No automated irrigation or fertilization
- No drone or satellite data integration
- Performance may be affected by extreme weather, power loss, or internet instability

## 📄 Documentation

- Full research paper available in:  
  **`K.A.N.I.N (IMRAD).pdf`**

- Includes:
  - Introduction & background  
  - Methodology  
  - System design  
  - Data gathering procedures  
  - Bibliography & appendices  

## 👨‍🔬 Research Team

Group 6 – Naga City Science High School  

- Argete, Zildjan Miles O.  
- Dominguiano, Mikael L.  
- Manalang, Markus Kaelan D.  
- Moskito, Abram Jesse B.  
- Nacario, Vince Ethan C.  
- Panizal, Joshua A.  

Research Adviser: **Sir Mat Atole**

## 📜 License

This project is intended for **educational and research purposes**.  
Please credit the authors if reused or extended.
