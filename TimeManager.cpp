#include "TimeManager.h"

TimeManager *TimeManager::m_pInstance = NULL;

TimeManager* TimeManager::Instance ()
{
	if (!m_pInstance) {
		m_pInstance = new TimeManager;
	}

	return m_pInstance;
}

TimeManager::sync ()
{
	baseTime_ = rtc_.time();
	baseTick_ = xTaskGetTickCount();	
}

Time TimeManager::getTime ()
{
	TickType_t now = xTaskGetTickCount ();

	if (now < baseTick_) {
		this->sync();
		now = xTaskGetTickCount();
	}

	return baseTime_;
}

void TimeManager::workFunction (void *user_data)
{

	while (1){
		vTaskDelay (100);
	}

}
