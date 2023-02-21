#pragma once
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include <ESPmDNS.h>
#include "../sensors/energy_task.h"
#include "../main.h"

#define HASS_AUTODISC_UPDATE_DELAY_MS pdMS_TO_TICKS(1000)
#define RETRY_1SEC pdMS_TO_TICKS(1000)

extern char tmplVoltage[];
extern char tmplCurrent[];

namespace Hassio
{

    //void Init();
    void autoDiscoveryTask(void *Parameters);
    void mqttKeepAliveTask(void *Parameters);
    void prepJsonVoltage(char *buff);
    void prepJsonCurrent(char *buff);
    bool mqttMdns(void);
    void publishPayload(void *Parameters);

};