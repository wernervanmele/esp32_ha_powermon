#pragma once
#include <EmonLib.h>
#include <driver/adc.h>
#include <EmonLib.h>
#include "../main.h"

/* **** Struct via byteArray experiment, starring Miss Union **** */
typedef struct energyMsg
{
    float voltage;
    float current;
    float power;
} energyMsg_t;

const int structSize = sizeof(energyMsg_t);

typedef union CombiPacketBro
{
    energyMsg_t PowerBucket;
    byte WattArray[structSize];
} energyMsg_u;
/* ******************************************* */

namespace Energy
{
    void Init();
    void energyMonTask(void *Parameters);
};