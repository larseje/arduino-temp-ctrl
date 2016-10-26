#include <DS1302.h>
#include <Arduino_FreeRTOS.h>
#include <task.h>
#include "RTOSThread.h"

#ifndef __TIME_MANAGER_H__
#define __TIME_MANAGER_H__

class TimeManager: public RTOSThread {
	public:
		static TimeManager* Instance();
		// Sync local time keeping against the RTC
		sync ();
		Time getTime ();
	private:
		TimeManager() :
			RTOSThread (workFunction, "TIME"), 
		 	rtc_(3,4,5),
			baseTime_(2000, 1, 1, 0, 0, 0, 1) {
				rtc_.halt (false);
				baseTime_ = rtc_.time();
				baseTick_ = xTaskGetTickCount ();
			}
		TimeManager(TimeManager const&) :
			RTOSThread (workFunction, "TIME"), 
		 	rtc_(3,4,5),
			baseTime_(2000, 1, 1, 0, 0, 0, 1) {
				rtc_.halt (false);
				baseTime_ = rtc_.time();
				baseTick_ = xTaskGetTickCount ();
			}
		TimeManager& operator=(TimeManager const&){};
		static TimeManager* m_pInstance;
		DS1302 rtc_;
		Time baseTime_;
		TickType_t baseTick_;
		static void workFunction (void*);
};

#endif
