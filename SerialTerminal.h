#include <Arduino_FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include "RTOSThread.h"

#ifndef __SERIALTERMINAL_H__
#define __SERIALTERMINAL_H__

// Singleton class for serial communication
class SerialTerminal : public RTOSThread {
	public:
		void waitTxLock();
		void releaseTxLock();
    // Private constructors to ensure only one instance is created
		SerialTerminal(TaskFunction_t task_cb) :
      RTOSThread (task_cb, "SERIAL", 128, 2)
			{
			  tx_sem_ = xSemaphoreCreateMutex ();
        rx_sem = xSemaphoreCreateBinary ();
			}
     void showMenu();
     void showSetTemp();
     void showSetHysteresis();
     void readInput();
     void signalInput();
     SemaphoreHandle_t rx_sem;
private:
		// TX lock
    SemaphoreHandle_t tx_sem_;
   
};

extern SerialTerminal terminal;

#endif
