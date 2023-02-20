#include "tasks.h"

#define WIFI_DELAY_LOOP pdMS_TO_TICKS(15000) // 15Sec.

WiFiClient espCLient;

bool mqttUnlocked = false;

void WifiClient::WifiTask(void *Parameters)
{

    DEBUG_PRINTF("[WIFI] Starting WiFi task !!, running on core: %d\n", xPortGetCoreID());

    for (;;)
    {
        if (WiFi.status() == WL_CONNECTED)
        { // stay in this loop when connected
            vTaskDelay(WIFI_DELAY_LOOP);
            continue;
        }

        DEBUG_PRINTLN(F("[WIFI] Connecting...."));
        WiFi.disconnect(true); // In case connected,...disconnect
        WiFi.mode(WIFI_STA);   // Station mode
        WiFi.setHostname(DEVICE_NAME);
        DEBUG_PRINTF("[DEBUG] MAC Address: %s\n", WiFi.macAddress().c_str());

        WiFi.begin(ssid, password);
        unsigned long wifiRetryDelay = millis();

        while (WiFi.status() != WL_CONNECTED && (millis() - wifiRetryDelay < WIFI_TIMEOUT))
        {
            //
        }

        if (WiFi.status() != WL_CONNECTED)
        {
            DEBUG_PRINTLN(F("[DEBUG] Wifi still not connected after retry delay, restart device in 5 seconds."));
            vTaskDelay(pdMS_TO_TICKS(5000));
            //   ESP.restart();
        }

        if (WiFi.status() == WL_CONNECTED)
        {
            DEBUG_PRINT(F("[DEBUG] Wifi connected again: "));
            DEBUG_PRINTLN(WiFi.localIP());
            DEBUG_PRINT(F("WiFi CHannel: "));
            DEBUG_PRINTLN(WiFi.channel());
            DEBUG_PRINT(F("Signal Strenght: "));
            DEBUG_PRINTLN(WiFi.RSSI());

            if (!mqttUnlocked)
            {
                // WiFi is connected, release the mqqt task
                vTaskResume(mqttKeepAlive);
                mqttUnlocked = true;
            }
            // Tell mqtt task Wifi connection is ok
            // xTaskNotify(
            //     mqttKeepAlive /* xTaskToNotify */,
            //     0 /* ulValue */,
            //     eNoAction /* eAction */
            // );
        }
    }

    // When we arrive here something is wrong, delete the task
    vTaskDelete(NULL);
}