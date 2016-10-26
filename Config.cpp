#include <EEPROM.h>
#include "Config.h"

void Config::load ()
{
  EEPROM.get (CFG_ADDR, cfg);
}

void Config::save ()
{
  EEPROM.put (CFG_ADDR, cfg);
}

float Config::getFermentationTempSetPoint ()
{
  return cfg.ferm_temp_set_point;
}

void Config::setFermentationTempSetPoint (float val)
{
  cfg.ferm_temp_set_point = val;
}

Config cfg;
