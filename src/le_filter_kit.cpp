#include "le_filter_kit.h"
/*
    Low Pass Exponential Moving Average
*/
void FKIT_LP_EMA_Init(FKIT_LP_EMA *Filter, float alpha)
{
    FKIT_LP_EMA_SetAlpha(Filter, alpha);
    Filter->output = 0.0f;
}

void FKIT_LP_EMA_SetAlpha(FKIT_LP_EMA *Filter, float alpha)
{
    /* Keep alpha between 0..1 */
    if (alpha > 1.0f)
    {
        alpha = 1.0f;
    }
    else if (alpha < 0.0f)
    {
        alpha = 0.0f;
    }
    /* and set the filter coefficient */
    Filter->alpha = alpha;
}

float FKIT_LP_EMA_Update(FKIT_LP_EMA *Filter, float input)
{
    Filter->output = Filter->alpha * input + (1.0f - Filter->alpha) * Filter->output;
    return Filter->output;
}

/*
    High Pass Exponential Moving Average
*/

void FKIT_HP_EMA_Init(FKIT_HP_EMA *Filter, float beta)
{
    FKIT_HP_EMA_SetAlpha(Filter, beta);
    Filter->output = 0.0f;
}

void FKIT_HP_EMA_SetAlpha(FKIT_HP_EMA *Filter, float beta)
{
    /* Keep alpha between 0..1 */
    if (beta > 1.0f)
    {
        beta = 1.0f;
    }
    else if (beta < 0.0f)
    {
        beta = 0.0f;
    }
    /* and set the filter coefficient */
    Filter->beta = beta;
}

float FKIT_HP_EMA_Update(FKIT_HP_EMA *Filter, float input)
{
    Filter->output = 0.5f * (2.0f - Filter->beta) * (input - Filter->input) + (1.0f - Filter->beta) * Filter->output;
    Filter->input = input;

    return Filter->output;
}

/*
    Regular Moving Average Filter (default window = 15)
*/

void FKIT_MAVG_Init(FKIT_MAVG *Filter)
{
    Filter->input = 0.0f;
    Filter->output = 0.0f;
    Filter->sampleIndex = 0.0f;
    Filter->sampleSum = 0.0f;
}

// void FKIT_MAVG_SetWindow(FKIT_MAVG *Filter, uint8_t window)
// {
//     /* Let's limit between 0 and 100, because we can */
//     if (window > 100)
//     {
//         window = 100;
//     }
//     else if (window < 1)
//     {
//         window = 1;
//     }
//     Filter->window = window;

// }

float FKIT_MAVG_Update(FKIT_MAVG *Filter, float input)
{
    Filter->sampleSum = Filter->sampleSum - Filter->samples[Filter->sampleIndex];
    Filter->samples[Filter->sampleIndex] = input;
    Filter->sampleSum = Filter->sampleSum + input;
    Filter->sampleIndex = (Filter->sampleIndex + 1) % MAVG_WINDOW;
    Filter->output = Filter->sampleSum / MAVG_WINDOW;

    return (Filter->output);
}