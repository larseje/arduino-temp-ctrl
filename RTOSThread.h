#include <Arduino_FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#ifndef __RTOSTHREAD_H__
#define __RTOSTHREAD_H__

class RTOSThread {
	public:
		RTOSThread (TaskFunction_t func, const char *name,
			 	unsigned short stack_depth=128, UBaseType_t priority=2, void* user_data=NULL) :
			stack_depth_(stack_depth), priority_(priority), name_(name),
			func_(func), user_data_(user_data) {
				xTaskCreate (func_, name_, stack_depth_, user_data_, priority_, &taskHandle_);
			}	

	private:
		unsigned short stack_depth_;
		UBaseType_t priority_;
		const char *name_;
		TaskFunction_t func_;
		void* user_data_;
		TaskHandle_t taskHandle_;
};

#endif
