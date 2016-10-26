#include <avr/power.h>
#include <avr/sleep.h>

#include "SerialTerminal.h"
#include "TimeManager.h"
#include "Temperature.h"
#include "TempControl.h"
#include "Config.h"

void mainThread (void * params)
{
  TickType_t last_wake_time = xTaskGetTickCount ();
  while (1) {
    vTaskDelayUntil (&last_wake_time, configTICK_RATE_HZ);
    digitalWrite (LED_BUILTIN, HIGH);
    vTaskDelayUntil (&last_wake_time, 45 / portTICK_PERIOD_MS);
    digitalWrite (LED_BUILTIN, LOW);
  }
  
}

void setup() {
  power_adc_disable ();
  power_spi_disable ();
  power_twi_disable ();
  power_timer1_disable ();
  power_timer2_disable ();
  cfg.load ();

  xTaskCreate (mainThread, "MAIN",
    configMINIMAL_STACK_SIZE, NULL,
    1, NULL);
}

void serialEvent () {
  terminal.signalInput();
}

void loop() {
  set_sleep_mode (SLEEP_MODE_IDLE);
  sleep_enable ();
  sleep_cpu ();
  sleep_disable ();
}
