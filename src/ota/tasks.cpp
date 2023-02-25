/**
 * @file tasks.cpp
 * @author Werner (wvanmele.newsletter@gmail.com)
 * @brief Over The Air firmware update task
 * @version 0.1
 * @date 2023-02-25
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "tasks.h"

void OTA::Init()
{
    static char buffson[64];

    ArduinoOTA.setHostname(DEVICE_NAME);

#ifdef OTA_PASSWORD
    ArduinoOTA.setPassword(OTA_PASSWORD);
#endif

    ArduinoOTA
        .onStart([]()
                 {
        String type;

        if (ArduinoOTA.getCommand() == U_FLASH)
            type = "sketch";
        else // U_SPIFFS
            type = "filesystem";

        // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
        DEBUG_PRINTLN("Start updating " + type); })
        .onEnd([]()
               { DEBUG_PRINTLN("\nEnd"); })
        .onProgress([](unsigned int progress, unsigned int total)
                    {
        sprintf(buffson, "Progress: %u%%\r", (progress / (total / 100)));
        DEBUG_PRINTLN(buffson); })
        .onError([](ota_error_t error)
                 {

      sprintf(buffson,"Error[%u]: ", error);
      DEBUG_PRINTLN(buffson);

    if (error == OTA_AUTH_ERROR)
          DEBUG_PRINTLN("Auth Failed");
      else if (error == OTA_BEGIN_ERROR)
          DEBUG_PRINTLN("Begin Failed");
      else if (error == OTA_CONNECT_ERROR)
          DEBUG_PRINTLN("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR)
          DEBUG_PRINTLN("Receive Failed");
      else if (error == OTA_END_ERROR)
          DEBUG_PRINTLN("End Failed"); });

    ArduinoOTA.begin();

    DEBUG_PRINTLN("OTA Server ready");
}

void OTA::updateTask(void *Parameters)
{
    // PRevent task being started before WiFI is running
    vTaskSuspend(NULL);

    Init();

    for (;;)
    {
        ArduinoOTA.handle();

        //vTaskDelay(1);
    }

    vTaskDelete(NULL);
}