#define DEVICE_NAME "ESP Sensor"

// Replace the next variables with your SSID/Password combination
#define WIFI_SSID "<WIFI_SSID>"
#define WIFI_PASSWORD "<WIFI_PASSWORD>"

// Add your MQTT Broker normally one per master tenant or just use your tenant URL:
#define MQTT_SERVER "mqtt.eu-latest.cumulocity.com" // e.g.: "mqtt.eu-latest.cumulocity.com" for all tenants of eu-latest environment

// if set to true, please adjust values within certificate_config.h file
#define USE_SSL true
#if USE_SSL 
  #include "certificate_config.h"
  #define MQTT_PORT 8883
#else
  #define MQTT_PORT 1883
#endif

#define BOOTSTRAP_USER "<BOOTSTRAP_USER>" // ask your admin for those, should be the same for all tenants on a management tenant
#define BOOTSTRAP_PASSWORD "<BOOTSTRAP_PASSWORD>" // ask your admin for those, should be the same for all tenants on a management tenant
// in case you want to skip bootstrap process, you can also just hardcode credentials for this device:
// just replace the BOOTSTRAP_USER and BOOTSTRAP_PASSWORD within the following lines with your credentials, but keep the definition of BOOTSTRAP_USER and BOOTSTRAP_PASSWORD above.
String mqttUser = BOOTSTRAP_USER; // "<tenantId>/<user>"
String mqttPassword = BOOTSTRAP_PASSWORD; // "<password>"

// set to true if BME280 sensor is available
#define HAS_SENSORS false

// LED Pin
#define LEDPIN 2
#ifdef LED_BUILTIN
  #define LEDPIN LED_BUILTIN
#endif
#ifdef BUILTIN_LED
  #define LEDPIN BUILTIN_LED
#endif

// in some cases led behavior might be inverted (e.g. wemos d1 mini uses LEDON = LOW, LEDOFF = HIGH)
#define LEDON HIGH
#define LEDOFF LOW

// HW information displayed within c8y
#ifdef ARDUINO_ARCH_ESP32
// ESP32
  #define HW_MODEL "Node MCU"
  #define HW_REVISION "ESP32"
#else 
// ESP8266
  #define HW_MODEL "Wemods D1 mini"
  #define HW_REVISION "ESP8266"
#endif

#include "mqtt_topics.h"
