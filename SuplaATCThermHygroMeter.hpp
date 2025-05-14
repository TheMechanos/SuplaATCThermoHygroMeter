#pragma once

#include <Arduino.h>

#include <NimBLEAdvertisedDevice.h>
#include <NimBLEDevice.h>
#include <NimBLEScan.h>

#include <SuplaDevice.h>
#include <supla/device/register_device.h>
#include <supla/network/html/custom_text_parameter.h>
#include <supla/sensor/therm_hygro_meter.h>

#ifndef SUPLA_ATC_THERMHYGROMETER_MAX_SENSORS
#define SUPLA_ATC_THERMHYGROMETER_MAX_SENSORS 16
#endif


class ATCThermHygroMeterController;

class ATCThermHygroMeter : public Supla::Sensor::ThermHygroMeter {
    friend class ATCThermHygroMeterController;

private:
    ATCThermHygroMeter(unsigned long validTimeTicks);

public:
    ATCThermHygroMeter(String mac, unsigned long validTimeTicks);
    ATCThermHygroMeter(Supla::Html::CustomTextParameter* param, unsigned long validTimeTicks);

    ~ATCThermHygroMeter();

    double getTemp();
    double getHumi();

    static void configScan(unsigned long scanTimeMs, unsigned long scanIntervalMs);
    static ATCThermHygroMeterController controller;

private:
    constexpr static const int16_t RSSI_MIN = -110;
    constexpr static const int16_t RSSI_MAX = -35;

    void onBLEResult(String mac, const NimBLEAdvertisedDevice* device);

    void onInit() override;
    void iterateAlways() override;

    void sendNewValue();

    String mac;
    unsigned long validTimeTicks;
    double temp;
    double humi;
    unsigned long lastDataReceiveTick;
};


class ATCThermHygroMeterController : public NimBLEScanCallbacks {
    friend class ATCThermHygroMeter;

private:
    ATCThermHygroMeterController();

    void configScan(unsigned long scanTimeMs, unsigned long scanIntervalMs);

    void init();
    void iterate();

    bool registerATC(ATCThermHygroMeter* atc);
    bool unregisterATC(ATCThermHygroMeter* atc);

    unsigned long scanTimeMs;
    unsigned long scanIntervalMs;

    NimBLEScan* pBLEScan;
    ATCThermHygroMeter* atcs[SUPLA_ATC_THERMHYGROMETER_MAX_SENSORS];

    void onResult(const NimBLEAdvertisedDevice* advertisedDevice) override;
    // void onDiscovered(const NimBLEAdvertisedDevice* advertisedDevice) override { Serial.printf("BLE: onDiscover\n"); }
    // void onScanEnd(const NimBLEScanResults& results, int reason) override { Serial.printf("BLE: Scan ended reason = %d\n", reason); }
};