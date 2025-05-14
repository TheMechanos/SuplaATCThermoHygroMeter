#include "SuplaATCThermHygroMeter.hpp"

ATCThermHygroMeterController ATCThermHygroMeter::controller;

ATCThermHygroMeter::ATCThermHygroMeter(unsigned long validTimeTicks)
    : validTimeTicks(validTimeTicks)
    , temp(TEMPERATURE_NOT_AVAILABLE)
    , humi(HUMIDITY_NOT_AVAILABLE)
    , lastDataReceiveTick(0) {

    sendNewValue();
    channel.setBatteryPowered(true);
    channel.setStateOffline();

    controller.registerATC(this);
}

ATCThermHygroMeter::ATCThermHygroMeter(String mac, unsigned long validTimeTicks)
    : ATCThermHygroMeter(validTimeTicks) {
    this->mac = mac;

    SUPLA_LOG_INFO("ATCSupla: New ATC_THM created! MAC=|%s|", mac.c_str());
}

ATCThermHygroMeter::ATCThermHygroMeter(Supla::Html::CustomTextParameter* param, unsigned long validTimeTicks)
    : ATCThermHygroMeter(validTimeTicks) {
    char ms[20];
    memset(ms, 0x00, 20 * sizeof(char));

    param->getParameterValue(ms, 20);
    this->mac = String(ms);

    SUPLA_LOG_INFO("ATCSupla: New ATC_THM created! MAC=|%s|", mac.c_str());
}

ATCThermHygroMeter::~ATCThermHygroMeter() { controller.unregisterATC(this); }

void ATCThermHygroMeter::configScan(unsigned long scanTimeMs, unsigned long scanIntervalMs) { controller.configScan(scanTimeMs, scanIntervalMs); }




void ATCThermHygroMeter::onInit() { controller.init(); }

void ATCThermHygroMeter::iterateAlways() {

    controller.iterate();

    if (millis() - lastDataReceiveTick > validTimeTicks && temp != TEMPERATURE_NOT_AVAILABLE && humi != HUMIDITY_NOT_AVAILABLE) {
        temp = TEMPERATURE_NOT_AVAILABLE;
        humi = HUMIDITY_NOT_AVAILABLE;
        channel.setStateOffline();
        sendNewValue();
        SUPLA_LOG_DEBUG("ATCSupla: Sensor |%s| is going offline!", mac.c_str());
    }
}

void ATCThermHygroMeter::sendNewValue() { channel.setNewValue(temp, humi); }

void ATCThermHygroMeter::onBLEResult(String mac, const NimBLEAdvertisedDevice* device) {
    auto data = device->getServiceData(0);

    mac.toUpperCase();
    if (mac == this->mac && data.length() >= 9) {

        int16_t rssiPercent = map(device->getRSSI(), RSSI_MIN, RSSI_MAX, 0, 100);
        if (rssiPercent > 100)
            rssiPercent = 100;
        if (rssiPercent < 0)
            rssiPercent = 0;

        int16_t rawTemperature = (data[7] | (data[6] << 8));

        this->temp = rawTemperature * 0.1;
        this->humi = (float)(data[8]);
        uint8_t bat = data[9];

        channel.setBatteryLevel(bat);
        channel.setBridgeSignalStrength(rssiPercent);
        channel.setStateOnline();
        sendNewValue();

        lastDataReceiveTick = millis();

        SUPLA_LOG_DEBUG("ATCSupla: Received from [%s] = Temp: %0.1f°C, Humi: %0.0f%%, Bat: %u%% RSSI: %d->%u%%", mac.c_str(), temp, humi, bat,
            device->getRSSI(), rssiPercent);
    }
}


double ATCThermHygroMeter::getTemp() { return temp; }

double ATCThermHygroMeter::getHumi() { return humi; }






ATCThermHygroMeterController::ATCThermHygroMeterController()
    : scanTimeMs(5000)
    , scanIntervalMs(120000) {
    for (size_t q = 0; q < SUPLA_ATC_THERMHYGROMETER_MAX_SENSORS; q++)
        atcs[q] = nullptr;
}

void ATCThermHygroMeterController::configScan(unsigned long scanTimeMs, unsigned long scanIntervalMs) {
    this->scanTimeMs = scanTimeMs;
    this->scanIntervalMs = scanIntervalMs;
    SUPLA_LOG_DEBUG("ATCSupla: BLE confgured to scan in %us interval for %us", scanIntervalMs / 1000, scanTimeMs / 1000);
}

void ATCThermHygroMeterController::init() {
    static bool inited = false;
    if (inited)
        return;

    NimBLEDevice::setScanDuplicateCacheSize(200);
    NimBLEDevice::init(Supla::RegisterDevice::getName());
    pBLEScan = NimBLEDevice::getScan();
    pBLEScan->setScanCallbacks(this, false);
    pBLEScan->setActiveScan(true);
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(50);
    pBLEScan->setMaxResults(0);

    inited = true;
}

bool ATCThermHygroMeterController::registerATC(ATCThermHygroMeter* atc) {
    for (size_t q = 0; q < SUPLA_ATC_THERMHYGROMETER_MAX_SENSORS; q++) {
        if (atcs[q] == nullptr) {
            atcs[q] = atc;
            return true;
        }
    }
    return false;
}

bool ATCThermHygroMeterController::unregisterATC(ATCThermHygroMeter* atc) {
    for (size_t q = 0; q < SUPLA_ATC_THERMHYGROMETER_MAX_SENSORS; q++) {
        if (atcs[q] == atc) {
            atcs[q] = nullptr;
            return true;
        }
    }
    return false;
}

void ATCThermHygroMeterController::iterate() {

    if (SuplaDevice.getDeviceMode() == Supla::DeviceMode::DEVICE_MODE_NORMAL) {
        static unsigned long lastScanTime = 0;
        if (millis() - lastScanTime > scanIntervalMs) {
            lastScanTime = millis();
            pBLEScan->start(scanTimeMs, false, true);
        }
    }
}

void ATCThermHygroMeterController::onResult(const NimBLEAdvertisedDevice* advertisedDevice) {

    if (advertisedDevice->haveServiceData()) {
        String macAddress = advertisedDevice->getAddress().toString().c_str();

        for (size_t q = 0; q < SUPLA_ATC_THERMHYGROMETER_MAX_SENSORS; q++)
            if (atcs[q] != nullptr)
                atcs[q]->onBLEResult(macAddress, advertisedDevice);
    }
}
