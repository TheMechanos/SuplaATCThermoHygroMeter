# ATC Thermo Hygro Meter for Supla

This library simplifies the integration of the XIAOMI LYWSD03MMC Bluetooth temperature and humidity sensor into the Supla environment. It provides an automatic API for scanning, decoding, and retrieving sensor data by nimBLE library.


# Dependencies
- [supla-device#v25.2](https://github.com/SUPLA/supla-device.git#v25.2)
- [NimBLE-Arduino@^2.2.1](https://github.com/h2zero/NimBLE-Arduino.git)


# Features
- Automatic scanning of XIAOMI LYWSD03MMC devices. 
- Decoding temperature, humidity and battery data. 
- Configurable BLE scan time and intervals.
- Showing BLE bridge RSSI


# Instalation
### Platformio
1. Add dependency to platormio.ini
```
lib_deps=
    https://github.com/TheMechanos/SuplaATCThermoHygroMeter.git
```
### Arduino
1. Install dependecy libraries in Arduino Library Manager
2. Download **SuplaATCThermHygroMeter.cpp** and **SuplaATCThermHygroMeter.hpp** files into your sketch directory


# Usage

First you have to include library 
```cpp
#include <SuplaATCThermHygroMeter.hpp>
```


\
Configure BLE Scaning timing, any time in program
```cpp
unsigned long scanTimeMs = 5 * 1000; //5 sec
unsigned long scanIntervalMs = 120 * 1000; //120 sec
ATCThermHygroMeter::configScan(scanTimeMs, scanIntervalMs);
```


\
Create sensor instance from string MAC
```cpp
String macString= "A4:C1:38:58:B3:79";
ATCThermHygroMeter* atc1 = new ATCThermHygroMeter(macString, measValidTimeMs);
```


\
Create sensor instance from custom supla Html parameter
```cpp
Supla::Storage::Init();
Supla::Html::CustomTextParameter customParamMac("mac", "ATC Mac", 17);

ATCThermHygroMeter* atc2 = new ATCThermHygroMeter(&customParamMac, measValidTimeMs);
```