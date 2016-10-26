#include <Arduino_FreeRTOS.h>
#include <Arduino.h>
#include <timers.h>
#include <semphr.h>
#include "RTOSThread.h"

#ifndef __TEMPCONTROL_H__
#define __TEMPCONTROL_H__

class TempControl : public RTOSThread {
	public:
		TempControl (int heat_pin, int heat_vcc, int cool_pin, int cool_vcc, TaskFunction_t task) :
      RTOSThread (task, "AIR", 128, 3), 
			heat_pin_(heat_pin), heat_vcc_(heat_vcc), cool_pin_(cool_pin),
			cool_vcc_(cool_vcc), hysteresis_(1), sample_time_(10), target_temp_(-127.0)
			{
				digitalWrite (heat_pin, LOW);
				pinMode (heat_pin, OUTPUT);

				digitalWrite (cool_pin, LOW);
				pinMode (cool_pin, OUTPUT);

        digitalWrite (cool_vcc, HIGH);
        pinMode (cool_vcc, OUTPUT);

        temp_sem_ = xSemaphoreCreateMutex ();
        xSemaphoreGive (temp_sem_);

        loadTargetTemp();
        loadHysteresis();

			}

		void setMinHeatTime (int min_heat_time);
		void setMinCoolTime (int min_cool_time);
		void setHysteresis (float hysteresis);
    void loadTargetTemp();
    void storeTargetTemp();
    void loadHysteresis();
    void storeHysteresis();
    float getHysteresis ();
		void setSampleTime (int sample_time);
    int getSampleTime ();
    void setTargetTemp (float temp);
    float getTargetTemp ();
		void start(int sample_time);
    void setHeat (bool on);
    void setCool (bool on);
    bool getHeat();
    bool getCool();
		void start();
		void stop();

	private:
		int heat_pin_;
		int cool_pin_;
    int heat_vcc_;
    int cool_vcc_;
		float hysteresis_;
		int sample_time_;
    float target_temp_;
    SemaphoreHandle_t temp_sem_;
};

extern TempControl AirTempControl;
#endif
