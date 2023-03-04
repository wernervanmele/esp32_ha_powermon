#ifndef _LE_FILTER_KIT_H
#define _LE_FILTER_KIT_H

#include <Arduino.h>

/*
set of IIR low/high pass filters
*/

/// @brief EMA - Low Pass Exponential moving average Filter
typedef struct
{
    float alpha; // filter coefficient value between 0 and 1.
    float output;
} FKIT_LP_EMA;

void FKIT_LP_EMA_Init(FKIT_LP_EMA *Filter, float alpha);
void FKIT_LP_EMA_SetAlpha(FKIT_LP_EMA *Filter, float alpha);
float FKIT_LP_EMA_Update(FKIT_LP_EMA *Filter, float input);

/// @brief HIGH Pass EMA Filter
typedef struct
{
    /* data */
    float beta; // filter coefficient value between 0 and 1.
    float output;
    float input;
} FKIT_HP_EMA;

void FKIT_HP_EMA_Init(FKIT_HP_EMA *Filter, float beta);
void FKIT_HP_EMA_SetAlpha(FKIT_HP_EMA *Filter, float beta);
float FKIT_HP_EMA_Update(FKIT_HP_EMA *Filter, float input);

//
//
//
/// @brief Regular Moving average
#define MAVG_WINDOW 17

typedef struct Mavg_t
{
    volatile float samples[MAVG_WINDOW];
    uint8_t sampleIndex;
    volatile float sampleSum;
    float output;
    float input;
} FKIT_MAVG;

void FKIT_MAVG_Init(FKIT_MAVG *Filter);
// void FKIT_MAVG_SetWindow(FKIT_MAVG *Filter);
float FKIT_MAVG_Update(FKIT_MAVG *Filter, float input);

#endif