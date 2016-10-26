#include "Temperature.h"
#include "SerialTerminal.h"
#include "Config.h"

float Temperature::getTemp (uint8_t idx)
{
  float ret = -127.0;

  while (xSemaphoreTake (value_sem_, portMAX_DELAY) != pdTRUE);
  
	if (idx == 0)
	  ret = air_temp_;
  else if (idx == 1)
    ret = liquid_temp_;

  xSemaphoreGive (value_sem_);

  return ret;
}

static void workFunction (void *user_data)
{
  TickType_t xLastWakeTime;
  byte test[8];
  
  while (xSemaphoreTake (tempAcquisition.value_sem_, portMAX_DELAY) != pdTRUE);

  /* Turn on supply for sensor */
  digitalWrite(AIR_TEMP_VDD, HIGH);
  pinMode(AIR_TEMP_VDD, OUTPUT);
  vTaskDelay(10);
  
  tempAcquisition.oneWire_.reset();
  tempAcquisition.sensors_.setOneWire (&tempAcquisition.oneWire_);

  tempAcquisition.sensors_.begin ();
  tempAcquisition.sensors_.setResolution (12);
  xSemaphoreGive (tempAcquisition.value_sem_);
   
  xLastWakeTime = xTaskGetTickCount ();
  
  while (1) {
    
    vTaskDelayUntil (&xLastWakeTime, 2000 / portTICK_PERIOD_MS);
    tempAcquisition.sensors_.requestTemperatures();
    while (xSemaphoreTake (tempAcquisition.value_sem_, portMAX_DELAY) != pdTRUE);
		tempAcquisition.air_temp_ = tempAcquisition.sensors_.getTempCByIndex (0);
    tempAcquisition.liquid_temp_ = tempAcquisition.sensors_.getTempCByIndex (1);
    xSemaphoreGive (tempAcquisition.value_sem_);
    terminal.waitTxLock ();
    Serial.print (F("Temperature: "));
    Serial.print (tempAcquisition.air_temp_);
    Serial.print (F(" [C], "));
    Serial.print (tempAcquisition.liquid_temp_);
    Serial.println (F(" [C]"));
    terminal.releaseTxLock ();

  }
}

Temperature tempAcquisition (workFunction);
