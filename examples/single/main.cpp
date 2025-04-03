#include <Arduino.h>
#include <SuplaDevice.h>

#include <supla/network/esp_wifi.h>
#include <supla/control/button.h>
#include <supla/device/status_led.h>
#include <supla/storage/littlefs_config.h>
#include <supla/network/esp_web_server.h>
#include <supla/device/supla_ca_cert.h>

#include <supla/network/html/device_info.h>
#include <supla/network/html/wifi_parameters.h>
#include <supla/network/html/protocol_parameters.h>

#include <supla/network/html/custom_parameter.h>
#include <supla/network/html/custom_text_parameter.h>

#include <SuplaATCThermHygroMeter.hpp>


#define DEV_NAME "ATC2Supla"

#define PIN_CFG GPIO_NUM_0
#define PIN_LED GPIO_NUM_8


Supla::ESPWifi wifi;
Supla::LittleFsConfig configSupla;
Supla::EspWebServer suplaServer;

Supla::Html::DeviceInfo htmlDeviceInfo(&SuplaDevice);
Supla::Html::WifiParameters htmlWifi;
Supla::Html::ProtocolParameters htmlProto(false, false);

Supla::Html::CustomParameter scanTime("scanT", "Scan Time", 5);
Supla::Html::CustomParameter scanInterval("scanI", "Scan interval", 120);
Supla::Html::CustomParameter validTime("validT", "Valid Time", 370);

Supla::Device::StatusLed statusLed(PIN_LED, true);
Supla::Control::Button cfg(PIN_CFG, true, true);

Supla::Html::CustomTextParameter mac("mac", "ATC Mac", 20);

void setup() {
    Serial.begin(115200);
    delay(4000);

    cfg.configureAsConfigButton(&SuplaDevice);

    Supla::Storage::Init();
    

    new ATCThermHygroMeter(&mac, validTime.getParameterValue() * 1000);
    ATCThermHygroMeter::configScan(scanTime.getParameterValue() * 1000, scanInterval.getParameterValue() * 1000);
    
    SuplaDevice.setSuplaCACert(suplaCACert);
    SuplaDevice.setSupla3rdPartyCACert(supla3rdCACert);

    SuplaDevice.setName(DEV_NAME);
    SuplaDevice.begin();
}

void loop() { SuplaDevice.iterate(); }
