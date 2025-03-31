# ATC Thermo Hygro Meter for Supla

This library simplifies the integration of the XIAOMI LYWSD03MMC Bluetooth temperature and humidity sensor into the Supla environment. It provides an automatic API for scanning, decoding, and retrieving sensor data by nimBLE library.

# Features
- Automatic scanning of XIAOMI LYWSD03MMC devices. 
- Decoding temperature, humidity and battery data. 
- Configurable BLE scan time and intervals.
- Showing BLE bridge RSSI


# Usage
Static configure BLE Scaning, any time
```cpp
ATCThermHygroMeter::configScan(scanTimeMs, scanIntervalMs);
```

Create sensor instance
```cpp
new ATCThermHygroMeter(macString, measValidTimeMs);
new ATCThermHygroMeter(&customParamMac, measValidTimeMs);
```

Library will automtic send data and humidity to cloud when receive new frame above BLE.
