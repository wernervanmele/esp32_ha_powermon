#include "tasks.h"
/*
    Sources:
    https://savjee.be/blog/home-energy-monitor-v2/
    https://github.com/256dpi/arduino-mqtt/releases
    https://www.home-assistant.io/integrations/mqtt/#mqtt-discovery
    https://circuitjournal.com/arduino-force-inline
    https://roelofjanelsinga.com/articles/mqtt-discovery-with-an-arduino/
*/
MQTTClient mqttClient(1024);
//
//  WiFiClientSecure wifiClient;
WiFiClient wifiClient;
MDNSResponder getdns;

IPAddress mqtt_ip;
String mqtt_hostname;
uint16_t mqtt_port = 0;
bool didRun = false;
bool discoveryUnlocked = false;
bool publishUnlocked = false;
byte _macrr[6];
String discoTopicVolt = "homeassistant/sensor/" DEVICE_NAME "/voltage/config";
String discoTopicAmps = "homeassistant/sensor/" DEVICE_NAME "/current/config";
String discoTopicPwr = "homeassistant/sensor/" DEVICE_NAME "/power/config";
String stateTopic = "homeassistant/sensor/" DEVICE_NAME "/state";

bool Hassio::mqttMdns(void)
{
    if (getdns.begin(DEVICE_NAME))
    {
        // DEBUG_PRINTLN(F("mDNS Service started"));

        // query local lan to discover mqtt broker (just experiment);
        int r = getdns.queryService("mqtt", "tcp");
        if (r == 0)
        {
            DEBUG_PRINTLN(F("No local mqtt broker discovered, maybe not register with mDNS ?"));
        }
        else
        {
            // something discovered ??
            for (int broker = 0; broker < r; broker++)
            {
                DEBUG_PRINTF("[mDNS] Index: %d - Hostname: %s - IP: %s - Port: %d\n", r, getdns.hostname(broker), getdns.IP(broker).toString().c_str(), getdns.port(broker));
            }
            mqtt_ip = getdns.IP(0);
            mqtt_hostname = getdns.hostname(0);
            mqtt_port = getdns.port(0);
        }
        return true;
    }
    else
    {
        DEBUG_PRINTLN(F("Starting mDNS Service failed"));
        return false;
    }
}

void Hassio::mqttKeepAliveTask(void *Parameters)
{

    // Suspend ourselves
    vTaskSuspend(NULL);

    // vTaskDelay(RETRY_1SEC * 5); // Wait abit to be sure we're connected to our Broker.
    // xSemaphoreTake(_mqtt_ok, portMAX_DELAY);
    DEBUG_PRINTF("[MQTT] Starting home-assistant KeepAlive task, running on core: %d\n", xPortGetCoreID());

    for (;;)
    {

        if (!didRun)
        {
            didRun = mqttMdns();
        }
        if (didRun)
        {

            // just work
            if (mqttClient.connected())
            {
                if (!discoveryUnlocked)
                { // Resume only once, the autoDiscovery task,
                    vTaskResume(HassioAutodiscoveryHandle);
                    discoveryUnlocked = true;
                }

                mqttClient.loop();
                vTaskDelay(RETRY_1SEC / 4);
                continue;
            }

            if (!WiFi.isConnected())
            {
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                continue;
            }

            mqttClient.setKeepAlive(15);
            mqttClient.begin(mqtt_ip, mqtt_port, wifiClient);
            DEBUG_PRINTLN(F("[MQTT] Connection to broker initiated"));

            long startAttemptTime = millis();
            if (!mqttClient.connect(DEVICE_NAME, HA_MQTT_USER, HA_MQTT_PASSWORD) && (millis() - startAttemptTime < MQTT_CONNECT_TIMEOUT))
            {
                vTaskDelay(MQTT_CONNECT_DELAY / portTICK_PERIOD_MS);
            }

            if (!mqttClient.connected())
            {
                DEBUG_PRINTLN(F("[MQTT] Connection failed, try agin in 30sec"));
                vTaskDelay(RETRY_1SEC * 30);
            }
        }
        else
        {
            DEBUG_PRINTLN(F("[DEBUG] No mqtt server found via mDNS"));
        }
    }

    vTaskDelete(NULL);
}

void Hassio::autoDiscoveryTask(void *Parameters)
{
    // Suspend ourselves
    vTaskSuspend(NULL);

    DynamicJsonDocument discoMsgV(1024);
    DynamicJsonDocument discoMsgI(1024);
    DynamicJsonDocument discoMsgP(1024);
    char macStr[18] = {0};
    DEBUG_PRINTF("[MQTT] Starting home-assistant discovery task, running on core: %d\n", xPortGetCoreID());
    String mcu_ident = WiFi.macAddress().c_str();
    WiFi.macAddress(_macrr);
    sprintf(macStr, "%02X%02X%02X%02X%02X%02X", _macrr[0], _macrr[1], _macrr[2], _macrr[3], _macrr[4], _macrr[5]);

    for (;;)
    {

        char tmplVoltage[768] = {0};
        char tmplCurrent[768] = {0};
        char tmplPower[768] = {0};

        if (!mqttClient.connected())
        {
            DEBUG_PRINTF("Not connected to MQTT Broker, retry in 5 seconds\n");
            vTaskDelay(5 * RETRY_1SEC);
            continue;
        }

        DEBUG_PRINTLN("Connected to MQTT, sending auto discovery to Home-Assistant");
        DEBUG_PRINTLN("Register voltage sensor");

        // Build Discovery json's
        discoMsgV["name"] = "Voltage";
        discoMsgV["dev_cla"] = "voltage";
        discoMsgV["unit_of_meas"] = "V";
        discoMsgV["icon"] = "mdi:transmission-tower";
        discoMsgV["stat_topic"] = stateTopic;
        discoMsgV["object_id"] = "voltage";
        discoMsgV["val_tpl"] = "{{ value_json.voltage }}";
        discoMsgV["unique_id"] = String(macStr) + "-volt";
        JsonObject deviceV = discoMsgV.createNestedObject("device");
        deviceV["name"] = DEVICE_NAME;
        deviceV["hw_version"] = "Rev: 0";
        deviceV["model"] = "Powermon Home Energy sensor";
        deviceV["manufacturer"] = "Wespressif";
        JsonArray identV = deviceV.createNestedArray("identifiers");
        identV.add(DEVICE_NAME);
        identV.add(mcu_ident);

        discoMsgI["name"] = "Current";
        discoMsgI["dev_cla"] = "current";
        discoMsgI["unit_of_meas"] = "A";
        discoMsgI["icon"] = "mdi:transmission-tower";
        discoMsgI["stat_topic"] = stateTopic;
        discoMsgI["object_id"] = "current";
        discoMsgI["val_tpl"] = "{{ value_json.current }}";
        discoMsgI["unique_id"] = String(macStr) + "-current";
        JsonObject deviceI = discoMsgI.createNestedObject("device");
        deviceI["name"] = DEVICE_NAME;
        deviceI["hw_version"] = "Rev: 0";
        deviceI["model"] = "Powermon Home Energy sensor";
        deviceI["manufacturer"] = "Wespressif";
        JsonArray identI = deviceI.createNestedArray("identifiers");
        identI.add(DEVICE_NAME);
        identI.add(mcu_ident);

        discoMsgP["name"] = "Power";
        discoMsgP["dev_cla"] = "power";
        discoMsgP["unit_of_meas"] = "W";
        discoMsgP["icon"] = "mdi:transmission-tower";
        discoMsgP["stat_topic"] = stateTopic;
        discoMsgP["object_id"] = "power";
        discoMsgP["val_tpl"] = "{{ value_json.power }}";
        discoMsgP["unique_id"] = String(macStr) + "-power";
        JsonObject deviceP = discoMsgP.createNestedObject("device");
        deviceP["name"] = DEVICE_NAME;
        deviceP["hw_version"] = "Rev: 0";
        deviceP["model"] = "Powermon Home Energy sensor";
        deviceP["manufacturer"] = "Wespressif";
        JsonArray identP = deviceP.createNestedArray("identifiers");
        identP.add(DEVICE_NAME);
        identP.add(mcu_ident);

        serializeJsonPretty(discoMsgI, tmplCurrent);

        DEBUG_PRINT(F("Current Entity discovery message: "));
        DEBUG_PRINTLN(F(tmplCurrent));
        size_t ni = serializeJson(discoMsgI, tmplCurrent);

        DEBUG_PRINTLN("Register current sensor");
        if (mqttClient.publish(discoTopicAmps.c_str(), tmplCurrent, ni))
            DEBUG_PRINTLN("Succeeded !!!");

        serializeJsonPretty(discoMsgV, tmplVoltage);

        DEBUG_PRINT(F("Voltage Entity discovery message: "));
        DEBUG_PRINTLN(F(tmplVoltage));
        size_t nv = serializeJson(discoMsgV, tmplVoltage);

        if (mqttClient.publish(discoTopicVolt.c_str(), tmplVoltage, nv))
            DEBUG_PRINTLN("Succeeded !!!");

        serializeJsonPretty(discoMsgP, tmplPower);

        DEBUG_PRINT(F("Voltage Entity discovery message: "));
        DEBUG_PRINTLN(F(tmplPower));
        size_t nq = serializeJson(discoMsgP, tmplPower);

        if (mqttClient.publish(discoTopicPwr.c_str(), tmplPower, nq))
        {
            DEBUG_PRINTLN("Succeeded !!!");

            if (!publishUnlocked)
            {
                // Resume the payload publish task
                vTaskResume(mqttPublishPayload);
                publishUnlocked = true;
            }
        }

        vTaskDelay(HASS_AUTODISC_UPDATE_DELAY_MS * 60 * 15);
    }

    vTaskDelete(NULL);
}

void Hassio::publishPayload(void *Parameters)
{
    // Suspend ourselves, task will be released by the DiscoveryTask
    vTaskSuspend(NULL);

    DEBUG_PRINTF("[MQTT] Publish payload task!!, running on core: %d\n", xPortGetCoreID());
    char buffer[512];
    energyMsg_t PwrPacket;
    DynamicJsonDocument payload(1024);

    for (;;)
    {

        xQueueReceive(xQxfer, &PwrPacket, portMAX_DELAY);

        // float mainsvolt = random(225, 235);
        // float mainscurrent = random(1, 19);
        payload["voltage"] = PwrPacket.voltage;
        payload["current"] = PwrPacket.current;
        payload["power"] = PwrPacket.power;

        // serializeJsonPretty(payload, buffer);
        size_t x = serializeJson(payload, buffer);
        DEBUG_PRINTLN(F(buffer));
        DEBUG_PRINTLN(F("---------"));

        if (mqttClient.publish(stateTopic.c_str(), buffer, x))
        {
            DEBUG_PRINTLN(F("Bingo"));
        }
        else
        {
            DEBUG_PRINTLN(F("No Bueno ?"));
        }

        vTaskDelay(RETRY_1SEC * 60);
    }
    vTaskDelete(NULL);
}