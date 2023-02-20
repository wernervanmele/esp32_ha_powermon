#pragma once

#include <WiFi.h>
#include <ESPmDNS.h>
#include "../main.h"

namespace WifiClient
{

    void WifiTask(void *Parameters);
}