#pragma once
#include <EmonLib.h>
#include <driver/adc.h>
#include <EmonLib.h>
#include "../main.h"

#ifdef USE_ADS1115
#include <Adafruit_ADS1X15.h>
#endif

/* **** Struct via byteArray experiment, starring Miss Union **** */
/*
    Any why ?
    Using a byte array in a union can be preferred for transferring values because it allows
    for a more precise control over the memory layout of the data being transferred, and it is
    less affected by issues related to endianness and padding.  In a union, all of the member
    variables share the same memory location, which means that writing to one member can also modify
    the value of another member. By using a byte array, the individual bytes of the data can be
    written to the array in a specific order, and then the union can be used to access the data in a
    more structured way, based on the types of the member variables. When transferring data across
    different systems or architectures, the endianness (the order in which bytes are stored in memory)
    can cause problems if the data is not properly converted. By using a byte array, the data can be
    written and read in a specific order, independent of the system's endianness.  Padding, which is
    used by compilers to align data in memory for performance reasons, can also cause issues when transferring
    data between systems with different padding requirements. Using a byte array can help avoid these issues
    by ensuring that the data is properly aligned in memory.
    Overall, using a byte array in a union can provide a more flexible and reliable way to transfer
    data between systems with different architectures and memory layouts.
*/
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
    int16_t adsPinReaderCurrent(int _channel);
};