#pragma once
#include <ArduinoOTA.h>

#include "../main.h"

namespace OTA
{
    void Init();
    void updateTask(void *Parameters);
}