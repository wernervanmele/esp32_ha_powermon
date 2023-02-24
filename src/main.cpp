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
TaskHandle_t energyMonTask;
TaskHandle_t otaFirmkwareUpdate;

void setup()
{

	DEBUG_OUT.begin(115200);
	while (!DEBUG_OUT)
		;

	DEBUG_PRINT("Entering setup func, running on core: ");
	DEBUG_PRINTLN(xPortGetCoreID());

	xQxfer = xQueueCreate(8, sizeof(energyMsg_t));

	vTaskDelay(pdMS_TO_TICKS(500));

	xTaskCreate(
		WifiClient::WifiTask,
		"WifiConnectivity",
		2048,
		NULL,
		1,
		&wifiConnectivityHandle);

	xTaskCreate(
		OTA::updateTask,
		"OTAUpdate",
		2048,
		NULL,
		1,
		&otaFirmkwareUpdate);

	xTaskCreate(
		Hassio::mqttKeepAliveTask,
		"mqttKeepAliveTask",
		2048,
		NULL,
		2,
		&mqttKeepAlive);

	xTaskCreate(
		Hassio::autoDiscoveryTask,
		"HassIOautoDiscovery",
		5120,
		NULL,
		2,
		&HassioAutodiscoveryHandle);

	xTaskCreate(
		Energy::energyMonTask,
		"PublishData",
		2048,
		NULL,
		2,
		&energyMonTask);

	xTaskCreate(
		Hassio::publishPayload,
		"PublishData",
		3072,
		NULL,
		2,
		&mqttPublishPayload);

	vTaskDelete(NULL);
}

void loop()
{
}
