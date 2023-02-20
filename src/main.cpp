/*
	https://forum.mhetlive.com/topic/5/mh-et-live-esp-32-devkit-mini-kit-user-guide-updating
	https://github.com/MHEtLive/ESP32-MINI-KIT/tree/master/Shield%20libraries
	https://docs.platformio.org/en/latest/boards/espressif32/mhetesp32minikit.html
*/

/*
	Based on https://github.com/Savjee/home-energy-monitor
*/
#include <Arduino.h>
#include "main.h"

TaskHandle_t wifiConnectivityHandle;
TaskHandle_t HassioAutodiscoveryHandle;
TaskHandle_t mqttKeepAlive;
TaskHandle_t mqttPublishPayload;

void setup()
{

	DEBUG_OUT.begin(115200);
	while (!DEBUG_OUT)
		;

	DEBUG_PRINT("Entering setup func, running on core: ");
	DEBUG_PRINTLN(xPortGetCoreID());

	vTaskDelay(pdMS_TO_TICKS(500));

	xTaskCreate(
		WifiClient::WifiTask,
		"WifiConnectivity",
		2048,
		NULL,
		1,
		&wifiConnectivityHandle);

	vTaskDelay(pdMS_TO_TICKS(333));

	xTaskCreate(
		Hassio::mqttKeepAliveTask,
		"mqttKeepAliveTask",
		2048,
		NULL,
		1,
		&mqttKeepAlive);

	vTaskDelay(pdMS_TO_TICKS(333));

	xTaskCreate(
		Hassio::autoDiscoveryTask,
		"HassIOautoDiscovery",
		4096,
		NULL,
		2,
		&HassioAutodiscoveryHandle);

	vTaskDelay(pdMS_TO_TICKS(333));

	xTaskCreate(
		Hassio::publishPayload,
		"PublishData",
		3072,
		NULL,
		1,
		&mqttPublishPayload);

	vTaskDelete(NULL);
}

void loop()
{
}
