#include "SerialTerminal.h"
#include "Temperature.h"
#include "TempControl.h"
#include <avr/pgmspace.h>

void SerialTerminal::waitTxLock ()
{
	while (xSemaphoreTake (tx_sem_, portMAX_DELAY) != pdTRUE);
}

void SerialTerminal::releaseTxLock ()
{
	xSemaphoreGive (tx_sem_);
}

void SerialTerminal::readInput ()
{
  byte len;
  int buf;
  
  buf = Serial.read();

  if (buf == -1) {
    return;
  }

  switch (buf) {
    case 'm':
      showMenu();
      break;
    case 's':
      showSetTemp();
      break;
    case 'h':
      showSetHysteresis();
      break;
    default:
      break;
  }   
}

void SerialTerminal::showMenu ()
{
  waitTxLock();

  Serial.println (F("Fermentation chamber version 0.1"));
  Serial.println (F("--------------------------------"));
  Serial.println (F("m       - Show menu"));
  Serial.println (F("s       - Set temperature target"));
  Serial.println (F("h       - Set hysteresis"));
  Serial.flush();

  releaseTxLock();
}

void SerialTerminal::showSetTemp()
{
  String buf = "";
  int i;
  float setpoint;
  
  waitTxLock();

  while (Serial.available() > 0)
    Serial.read();

  Serial.println ("");
  Serial.print (F("Current Temperature Setpoint: "));
  Serial.print (AirTempControl.getTargetTemp());
  Serial.println (F(" [C]"));
  Serial.println (F("Input new setpoint"));

  while (Serial.available() == 0);

  setpoint = Serial.parseFloat();

  if (setpoint == 0) {
    Serial.println (F("Invalid entry"));
    releaseTxLock();
    return;
  }
  AirTempControl.setTargetTemp(setpoint);
  AirTempControl.storeTargetTemp();

  Serial.print (F("New Temperature Setpoint: "));
  Serial.print (AirTempControl.getTargetTemp());
  Serial.println (F(" [C]"));
  Serial.flush();

  releaseTxLock();
}

void SerialTerminal::showSetHysteresis()
{
  String buf = "";
  int i;
  float setpoint;

  waitTxLock();

  while (Serial.available() > 0)
    Serial.read();
    
  Serial.println ("");
  Serial.print (F("Current Hysteresis: "));
  Serial.print (AirTempControl.getHysteresis());
  Serial.println (F(" [C]"));
  Serial.println (F("Input new hysteresis"));

  while (Serial.available() == 0);

  setpoint = Serial.parseFloat();

  if (setpoint == 0) {
    Serial.println (F("Invalid entry"));
    releaseTxLock();
    return;
  }
  AirTempControl.setHysteresis(setpoint);
  AirTempControl.storeHysteresis();

  Serial.print (F("New Hysteresis: "));
  Serial.print (AirTempControl.getHysteresis());
  Serial.println (F(" [C]"));
  Serial.flush();

  releaseTxLock();
}

void SerialTerminal::signalInput ()
{
  xSemaphoreGive (rx_sem);
}

void serialThread (void *param)
{
  Serial.begin (9600);
  terminal.releaseTxLock ();
  terminal.showMenu();
  
  while (1) {
    if (xSemaphoreTake (terminal.rx_sem, portMAX_DELAY) == pdTRUE) {
      terminal.readInput ();  
    }
  }
}

SerialTerminal terminal (serialThread);
