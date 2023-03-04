/**
 * @file energy_task.cpp
 * @author Werner (wvanmele.newsletter@gmail.com)
 * @brief Measure Voltage and Current task
 * @version 0.1
 * @date 2023-02-25
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "energy_task.h"
#include "le_filter_kit.h"

/*
https://github.com/espressif/arduino-esp32/issues/102
https://espressif-docs.readthedocs-hosted.com/projects/arduino-esp32/en/latest/api/adc.html
*/
//
EnergyMonitor EMon1;
QueueHandle_t xQxfer;
float vultza = 0.0f;
FKIT_MAVG mavgVoltage;

void Energy::Init()
{
    // Set esp32 max ADC resultion
    analogReadResolution(ADC_BITS); // esp32 12Bit adc = 4096 steps ( 0-4095)
    adc1_config_width(ADC_WIDTH_12Bit);

    if (ADC_VRMS_IN)
    {
        DEBUG_PRINTLN(F("Initialize ADC Input for Voltage measurement."));
        adcAttachPin(ADC_VRMS_IN);
        adc1_config_width(ADC_WIDTH_12Bit);
        // analogSetPinAttenuation(ADC_VRMS_IN, ADC_11db);
        adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_DB_11); // GPIO33

        EMon1.voltage(ADC_VRMS_IN, 573.2, 1.7);
    }

    if (ADC_IRMS_IN)
    { // Check if ADC for current is defined and initialize
        DEBUG_PRINTLN(F("Initialize ADC Input for Current measurement."));
        adcAttachPin(ADC_IRMS_IN);
        // analogSetPinAttenuation(ADC_IRMS_IN, ADC_11db);
        adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_11); // GPIO35

        EMon1.current(ADC_IRMS_IN, 111.1);
    }

    FKIT_MAVG_Init(&mavgVoltage);
}

void Energy::energyMonTask(void *Parameters)
{
    vTaskSuspend(NULL);
    Init();

    energyMsg_u WattStreamz;
    unsigned long prevMillis;

    // Measure every second to keep the sensor warm :-)
    // Publish every minute
    for (;;)
    {
        // FIXME no power detection instead off something like 7.55V
        unsigned long isNow = millis();

        EMon1.calcVI(20, 2000);
#ifdef DEBUG
        EMon1.serialprint();
#endif

        if (ADC_VRMS_IN)
        {
            float avgVolt = FKIT_MAVG_Update(&mavgVoltage, EMon1.Vrms);
            DEBUG_PRINTF("Averaged Vrms: %.2fV.\n", avgVolt);

            // powerloss detection ????
            if (vultza < 110) // arbitrary number
            {
                WattStreamz.PowerBucket.voltage = 0.0f;
            }
            else
            {
                WattStreamz.PowerBucket.voltage = vultza;
            }
        }
        else
        {
            WattStreamz.PowerBucket.voltage = 0.0f;
        }

        if (ADC_IRMS_IN)
        {
            if (vultza < 110)
            {
                WattStreamz.PowerBucket.current = 0.0f;
            }
            else
            {
                WattStreamz.PowerBucket.current = EMon1.Irms;
            }
        }
        else
        {
            WattStreamz.PowerBucket.current = 0.0f;
        }

        if (ADC_VRMS_IN && ADC_IRMS_IN)
        {
            if (vultza < 110)
            {
                WattStreamz.PowerBucket.power = 0.0f;
            }
            else
            {
                WattStreamz.PowerBucket.power = EMon1.apparentPower; // apparent = RMS
            }
        }
        else
        {
            WattStreamz.PowerBucket.power = 0.0f;
        }

        if (millis() - prevMillis >= PUB_INTERVAL)
        {
            xQueueSend(xQxfer, &WattStreamz.WattArray, 0);

            // wake the publish payload task to populate HomeAssistant
            vTaskResume(mqttPublishPayload);
            prevMillis = isNow;
        }

        vTaskDelay(RETRY_1SEC);
    }
}