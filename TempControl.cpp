#include "TempControl.h"
#include "SerialTerminal.h"
#include "Temperature.h"
#include <EEPROM.h>

#define HEAT_PIN 48
#define HEAT_VCC 46
#define COOL_PIN 24
#define COOL_VCC 22

#define COOL_GRACE_MINUTES 5

void TempControl::loadTargetTemp ()
{
  while (!xSemaphoreTake (temp_sem_, portMAX_DELAY));

  EEPROM.get (4, target_temp_);

  xSemaphoreGive (temp_sem_);
}

void TempControl::storeTargetTemp ()
{
  while (!xSemaphoreTake (temp_sem_, portMAX_DELAY));

  EEPROM.put (4, target_temp_);
    
  xSemaphoreGive (temp_sem_);
}

void TempControl::loadHysteresis ()
{
  while (!xSemaphoreTake (temp_sem_, portMAX_DELAY));

  EEPROM.get (8, hysteresis_);

  xSemaphoreGive (temp_sem_);
}

void TempControl::storeHysteresis ()
{
  while (!xSemaphoreTake (temp_sem_, portMAX_DELAY));

  EEPROM.put (8, hysteresis_);
    
  xSemaphoreGive (temp_sem_);
}


void TempControl::setHysteresis (float hysteresis)
{
  while (!xSemaphoreTake (temp_sem_, portMAX_DELAY));
  
	hysteresis_ = hysteresis;

  xSemaphoreGive (temp_sem_);
}

int TempControl::getSampleTime ()
{
  return sample_time_;
}

void TempControl::setTargetTemp (float temp)
{
  while (!xSemaphoreTake (temp_sem_, portMAX_DELAY));

  target_temp_ = temp;

  xSemaphoreGive (temp_sem_);
}

float TempControl::getTargetTemp ()
{
  float ret;

  while (!xSemaphoreTake (temp_sem_, portMAX_DELAY));

  ret = target_temp_;

  xSemaphoreGive (temp_sem_);

  return ret;
}

float TempControl::getHysteresis ()
{
  float ret;
  
  while (!xSemaphoreTake (temp_sem_, portMAX_DELAY));

  ret = hysteresis_;

  xSemaphoreGive (temp_sem_);

  return ret;
}

void TempControl::setHeat (bool on)
{
  if (on) {
    setCool (false);
    digitalWrite (heat_pin_, HIGH);
  } else {
    digitalWrite (heat_pin_, LOW);
  }
}

void TempControl::setCool (bool on)
{
  if (on) {
    setHeat (false);
    digitalWrite (cool_pin_, HIGH);
  } else {
    digitalWrite (cool_pin_, LOW);
  }
}

bool TempControl::getCool()
{
  return digitalRead (cool_pin_);
}

bool TempControl::getHeat()
{
  return digitalRead (heat_pin_);
}

void air_temp_task (void *param)
{
  TickType_t last_wake_time = xTaskGetTickCount ();
  float air_temp;
  float target_temp;
  int hysteresis;

  while (1) {
    vTaskDelayUntil (&last_wake_time, configTICK_RATE_HZ * 60);
    air_temp = tempAcquisition.getTemp (AIR_TEMP_SENS);

    if (air_temp == -127.0)
      continue;
    
    target_temp = AirTempControl.getTargetTemp ();
    if (target_temp == -127.0)
      continue;

    hysteresis = AirTempControl.getHysteresis ();

    if (air_temp > target_temp) {
      AirTempControl.setHeat (false);
      if (air_temp > (target_temp + hysteresis)){
        if (!AirTempControl.getCool()) { 
          AirTempControl.setCool (true);
          vTaskDelayUntil (&last_wake_time, configTICK_RATE_HZ * 60 * (COOL_GRACE_MINUTES-1));
        }
      }
    } else {
      AirTempControl.setCool (false);
      if (air_temp < (target_temp - hysteresis))
        AirTempControl.setHeat (true);
    }
  }
}

TempControl AirTempControl(HEAT_PIN, HEAT_VCC, COOL_PIN, COOL_VCC, air_temp_task);
