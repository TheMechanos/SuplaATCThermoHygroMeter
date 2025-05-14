#include <Arduino.h>
#include <SuplaDevice.h>

#include <supla/events.h>
#include <supla/actions.h>

#include <SuplaATCThermHygroMeter.hpp>

#include <supla/network/esp_wifi.h>
#include <supla/control/button.h>
#include <supla/device/status_led.h>
#include <supla/storage/littlefs_config.h>
#include <supla/storage/eeprom.h>
#include <supla/network/esp_web_server.h>
#include <supla/device/supla_ca_cert.h>

#include <supla/network/html/device_info.h>
#include <supla/network/html/wifi_parameters.h>
#include <supla/network/html/protocol_parameters.h>
#include <supla/network/html/status_led_parameters.h>
#include <supla/network/html/custom_parameter.h>
#include <supla/network/html/custom_text_parameter.h>
#include <supla/network/html/time_parameters.h>
#include <supla/network/html/hvac_parameters.h>

#include <supla/control/internal_pin_output.h>
#include <supla/control/virtual_relay.h>

#include <supla/sensor/virtual_thermometer.h>

#include <supla/control/hvac_base.h>
#include <supla/clock/clock.h>





#define DEV_NAME "HvacATC"

#define PIN_CFG GPIO_NUM_0
#define PIN_LED GPIO_NUM_8
#define PIN_RELAY GPIO_NUM_4



Supla::LittleFsConfig configSupla;
Supla::Eeprom eeprom;

Supla::Clock suplaClock;
Supla::ESPWifi wifi;
Supla::EspWebServer suplaServer;

Supla::Html::DeviceInfo htmlDeviceInfo(&SuplaDevice);
Supla::Html::WifiParameters htmlWifi;
Supla::Html::ProtocolParameters htmlProto(false, false);
Supla::Html::TimeParameters timeParams(&SuplaDevice);

Supla::Html::CustomParameter scanTime("scanT", "Scan Time", 5);
Supla::Html::CustomParameter scanInterval("scanI", "Scan interval", 120);
Supla::Html::CustomParameter validTime("validT", "Valid Time", 370);
Supla::Html::CustomTextParameter mac("atcMac", "ATC MAC", 17);


Supla::Device::StatusLed statusLed(PIN_LED, true);
Supla::Control::Button cfg(PIN_CFG, true, true);

Supla::Control::InternalPinOutput output(PIN_RELAY);
Supla::Control::HvacBase hvac(&output);
Supla::Html::HvacParameters hvacParams(&hvac);


void setup() {
    Serial.begin(115200);
    delay(4000);

    eeprom.setStateSavePeriod(5000);
    Supla::Storage::Init();


    cfg.configureAsConfigButton(&SuplaDevice);

    ATCThermHygroMeter::configScan(scanTime.getParameterValue() * 1000, scanInterval.getParameterValue() * 1000);
    ATCThermHygroMeter* th = new ATCThermHygroMeter(&mac, validTime.getParameterValue() * 1000);

    hvac.setMainThermometerChannelNo(th->getChannelNumber());
    // hvac.setAuxThermometerChannelNo(2);      // Aux Thermometer DS2
    // hvac.setAuxThermometerType(SUPLA_HVAC_AUX_THERMOMETER_TYPE_FLOOR);
    // hvac.setTemperatureAuxMin(500);  // 5 degrees
    // hvac.setTemperatureAuxMax(7500);  // 75 degrees
    hvac.setTemperatureHisteresisMin(20);  // 0.2 degree
    hvac.setTemperatureHisteresisMax(1000);  // 10 degree
    hvac.setTemperatureHisteresis(40);


    SuplaDevice.setSuplaCACert(suplaCACert);
    SuplaDevice.setSupla3rdPartyCACert(supla3rdCACert);

    SuplaDevice.setName(DEV_NAME);
    SuplaDevice.begin();
}

void loop() { SuplaDevice.iterate(); }



