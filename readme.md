# ATC Thermo Hygro Meter for Supla

This library simplifies the integration of the XIAOMI LYWSD03MMC Bluetooth temperature and humidity sensor into the Supla environment. It provides an automatic API for scanning, decoding, and retrieving sensor data by nimBLE library.


# Dependencies
- [NimBLE-Arduino@^2.2.1](https://github.com/h2zero/NimBLE-Arduino.git)


# Features
- Automatic scanning of XIAOMI LYWSD03MMC devices. 
- Decoding temperature, humidity and battery data. 
- Configurable BLE scan time and intervals.
- Showing BLE bridge RSSI


# Usage

Static configure BLE Scaning, any time
```cpp
unsigned long scanTimeMs = 5 * 1000;
unsigned long scanIntervalMs = 120 * 1000;
ATCThermHygroMeter::configScan(scanTimeMs, scanIntervalMs);
```


\
Create sensor instance from string MAC
```cpp
String macString= "A4:C1:38:58:B3:79";
ATCThermHygroMeter* atc1 = new ATCThermHygroMeter(macString, measValidTimeMs);
```


\
Create sensor instance from custom supla parameter
```cpp
Supla::Storage::Init();
Supla::Html::CustomTextParameter customParamMac("mac", "ATC Mac", 17);

ATCThermHygroMeter* atc2 = new ATCThermHygroMeter(&customParamMac, measValidTimeMs);
```