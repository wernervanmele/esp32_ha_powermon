#include "energy_task.h"

/*
https://github.com/espressif/arduino-esp32/issues/102
*/
//
EnergyMonitor EMon1;
QueueHandle_t xQxfer;

void Energy::Init()
{

    // Set esp32 max ADC resultion
    analogReadResolution(ADC_BITS); // esp32 12Bit adc = 4096 steps ( 0-4095)

    if (ADC_VRMS_IN)
    {
        DEBUG_PRINTLN(F("Initialize ADC Input for Voltage measurement."));
        adcAttachPin(ADC_VRMS_IN);
        adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_DB_11); // GPIO33

        EMon1.voltage(ADC_VRMS_IN, 579.5, 1.7);
    }

    if (ADC_IRMS_IN)
    { // Check if ADC for current is defined and initialize
        DEBUG_PRINTLN(F("Initialize ADC Input for Current measurement."));
        adcAttachPin(ADC_IRMS_IN);
        adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_11); // GPIO35

        EMon1.current(ADC_IRMS_IN, 111.1);
    }
}

void Energy::energyMonTask(void *Parameters)
{
    vTaskSuspend(NULL);
    Init();

    // energyMsg_t PowerPacket;
    energyMsg_u WattStreamz;
    unsigned long prevMillis;

    // Measure every second to keep the sensor warm :-)
    // Publish every minute
    for (;;)
    {

        unsigned long isNow = millis();

        EMon1.calcVI(20, 2000);
        EMon1.serialprint();

        if (ADC_VRMS_IN)
        {
            WattStreamz.PowerBucket.voltage = EMon1.Vrms;
        }
        else
        {
            WattStreamz.PowerBucket.voltage = 0.0f;
        }

        if (ADC_IRMS_IN)
        {
            WattStreamz.PowerBucket.current = EMon1.Irms;
        }
        else
        {
            WattStreamz.PowerBucket.current = 0.0f;
        }

        if (ADC_VRMS_IN && ADC_IRMS_IN)
        {
            WattStreamz.PowerBucket.power = EMon1.apparentPower; // apparent = RMS
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