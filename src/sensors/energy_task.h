#pragma once
#include <EmonLib.h>
#include <driver/adc.h>
#include <EmonLib.h>
#include "../main.h"

typedef struct energyMsg
{
    float voltage;
    float current;
    float power;
} energyMsg_t;

namespace Energy
{

    void Init();
    void energyMonTask(void *Parameters);
};