#include <Arduino_FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "RTOSThread.h"

#ifndef __TEMPERATURE_H__
#define __TEMPERATURE_H__

#define AIR_TEMP_SENS 0
#define LIQUID_TEMP_SENS 1

#define AIR_TEMP_DATA 53
#define AIR_TEMP_VDD 51

// Singleton class for temperature acquistion
class Temperature: public RTOSThread {
	public:
    // Use to trigger instantation of object, and initialise HW
		void start ();

    // Get the raw temperature of a certain index
		float getTemp (uint8_t idx);
    DallasTemperature sensors_;
    
    // The measured value
    float air_temp_;
    float liquid_temp_;
    SemaphoreHandle_t value_sem_;

    // Private constructors to ensure only one instance is created
		Temperature(TaskFunction_t workFunction) :
			RTOSThread (workFunction, "TEMP", 256, 3),
     oneWire_(AIR_TEMP_DATA),
     air_temp_(-127.0)
			{
			  value_sem_ = xSemaphoreCreateMutex ();
        xSemaphoreGive (value_sem_);
			}

    // Instances for communication
		OneWire oneWire_;

   
};

extern Temperature tempAcquisition;

#endif
