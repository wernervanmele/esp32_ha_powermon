#include "energy_task.h"

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

    energyMsg_t PowerPacket;

    for (;;)
    {

        EMon1.calcVI(20, 2000);

        PowerPacket.power = EMon1.Vrms;
        PowerPacket.current = EMon1.Irms;
        PowerPacket.power = EMon1.apparentPower; // apparent = RMS

        xQueueSend(xQxfer, &PowerPacket, 0);

        vTaskDelay(RETRY_1SEC * 60);
    }
}