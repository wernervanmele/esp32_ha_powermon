#ifndef MAIN_H
#define MAIN_H
#include <Arduino.h>
#include "mqtt/tasks.h"
#include "wifi/tasks.h"
#include "sensors/energy_task.h"
#include "ota/tasks.h"
#include "credentials.h"

/* Define Analog Input pins */
#define ADC_VRMS_IN 33 // GPIO33 - zmpt101b input
#define ADC_IRMS_IN -1 // GPIO35 Irms analogRead() input. -1 = nothing connected

// extern SemaphoreHandle_t _wifi_ok;
// extern SemaphoreHandle_t _mqtt_ok;
extern TaskHandle_t wifiConnectivityHandle;
extern TaskHandle_t HassioAutodiscoveryHandle;
extern TaskHandle_t mqttKeepAlive;
extern TaskHandle_t mqttPublishPayload;
extern TaskHandle_t energyMonTask;
extern TaskHandle_t otaFirmkwareUpdate;
extern QueueHandle_t xQxfer;

/* MQTT Settings */
/* obsolute now autodiscovery via mDNS */
#define DEVICE_NAME "esp32_powermonitor_1"
#define HA_MQTT_BROKER "192.168.28.16"
#define HA_MQTT_PORT 1883
#define HA_MQTT_USER MY_HA_MQTT_USER
#define HA_MQTT_PASSWORD MY_HA_MQTT_PASSWORD

/*  WiFi Settings */
// Set your access point network credentials
#define ssid my_ssid
#define password my_password
#define WIFI_TIMEOUT 60000U // 60 Seconds
#define MQTT_CONNECT_DELAY 200U
#define MQTT_CONNECT_TIMEOUT 20000U
#define PUB_INTERVAL 60000U // 60 Sec.

/* OTA Settimgs */
#define OTA_PASSWORD MY_OTA_PASSWORD

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
