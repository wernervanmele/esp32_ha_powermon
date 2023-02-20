#ifndef MAIN_H
#define MAIN_H
#include <Arduino.h>
#include "mqtt/tasks.h"
#include "wifi/tasks.h"

// extern SemaphoreHandle_t _wifi_ok;
// extern SemaphoreHandle_t _mqtt_ok;
extern TaskHandle_t wifiConnectivityHandle;
extern TaskHandle_t HassioAutodiscoveryHandle;
extern TaskHandle_t mqttKeepAlive;
extern TaskHandle_t mqttPublishPayload;

/* MQTT Settings */
/* obsolute now autodiscovery via mDNS */
#define DEVICE_NAME "esp32_powermonitor_1"
#define HA_MQTT_BROKER "192.168.28.16"
#define HA_MQTT_PORT 1883
#define HA_MQTT_USER "mqtt-user"
#define HA_MQTT_PASSWORD "mqtt-password"

/*  WiFi Settings */
// Set your access point network credentials
#define ssid "JOEPLA_IOT"
#define password "shoarma2burn"
#define WIFI_TIMEOUT 60000U // 60 Seconds
#define MQTT_CONNECT_DELAY  200U
#define MQTT_CONNECT_TIMEOUT 20000U

/* Serial Printing */
#ifdef DEBUG
#define DEBUG_OUT Serial
#define DEBUG_PRINTF(...) DEBUG_OUT.printf(__VA_ARGS__)
#define DEBUG_PRINTLN(...) DEBUG_OUT.println(__VA_ARGS__)
#define DEBUG_PRINT(...) DEBUG_OUT.print(__VA_ARGS__)
#else
#define DEBUG_PRINTF(...)
#define DEBUG_PRINTLN(...)
#define DEBUG_PRINT(...)
#endif

#endif
