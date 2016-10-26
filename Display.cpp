#include "Display.h"
#include "Temperature.h"
#include "SerialTerminal.h"
#include "TempControl.h"

#include "Config.h"

void Display::init (void)
{
  int i;
  
  digitalWrite (DISP_RD, HIGH);
  pinMode (DISP_RD, OUTPUT);
  UTFT_.InitLCD (LANDSCAPE);
  UTFT_.clrScr ();
  UTFT_.setFont (SmallFont);
  dispx = UTFT_.getDisplayXSize ();
  dispy = UTFT_.getDisplayYSize ();

  setTitle ("Fermentation Chamber");
  clearMenu ();

  for (i = 0; i < GRAPH_NO_ELM; i++) {
    air_temps[i] = -127.0;
  }

  xSemaphoreGive (display_sem);
}

void Display::clearMenu ()
{
  UTFT_.setColor (MENU_BACK_COLOR);
  UTFT_.fillRect (dispx - MENU_WIDTH - 1, 0, dispx - 1, dispy - 1);
  UTFT_.setColor (255, 255, 255);
  UTFT_.drawLine (dispx - MENU_WIDTH - 2, 0, dispx - MENU_WIDTH - 2, dispy - 1);
}

void Display::clearTitle ()
{
  UTFT_.setColor (TITLE_BACK_COLOR);
  UTFT_.fillRect (0, 0, dispx - 2 - MENU_WIDTH, TITLE_HEIGHT);
  UTFT_.setColor (255, 255, 255);
  UTFT_.drawLine (0, TITLE_HEIGHT + 1, dispx - 2 - MENU_WIDTH, TITLE_HEIGHT + 1);
}

void Display::setTitle (const char *title)
{
  int center;
  int16_t x1, y1;
  uint16_t w, h;

  // Font is 6 px wide for text size 1
  center = ((dispx - 2) / 2) - (strlen (title) * 3);
  clearTitle ();
  UTFT_.setTextSize (1);
  UTFT_.setColor (255, 255, 255);
  UTFT_.setBackColor (TITLE_BACK_COLOR);
  UTFT_.getTextBounds (title, 0, 0, &x1, &y1, &w, &h);
  center = (dispx - 2 - MENU_WIDTH - w) / 2;
  UTFT_.print (title, center, 0);
}

int Display::pointGetY (float point)
{
  float ratio;
  float mid;
  
  if (point == -127.0) {
    return -1;
  }

  mid = (air_temps_max - air_temps_min) / 2 + air_temps_min;
  
  ratio = (point - mid) / (((air_temps_max - air_temps_min) * 1.1 + 1)) + 0.5;

  return dispy - (dispy - TITLE_HEIGHT - 2 - 12) * ratio - 1;
}

void Display::airTempGraphDraw (float current_temp)
{
  int i;
  float prev_tmp;
  int prev_pt_y, pt_y;
  int pt_x;
  
  UTFT_.setColor (0,0,0);
  UTFT_.fillRect (0, TITLE_HEIGHT + 2, dispx - 3 - MENU_WIDTH, dispy - 1);
  UTFT_.setColor (0, 255, 0);
  UTFT_.setBackColor (0, 0, 0);

  UTFT_.print ("Set point: ", 50, TITLE_HEIGHT+2);
  UTFT_.printNumF (AirTempControl.getTargetTemp(), 1, 140, TITLE_HEIGHT+2);

  UTFT_.print ("Current: ", 50, TITLE_HEIGHT+22);
  UTFT_.printNumF (current_temp, 1, 140, TITLE_HEIGHT+22);

  air_temps_min = 127.0;
  air_temps_max = -127.0;

  for (i = 0; i < GRAPH_NO_ELM; i++) {
    if (air_temps[i] == -127.0) {
      continue;
    }
    
    if (air_temps[i] > air_temps_max) {
      air_temps_max = air_temps[i];
    }

    if (air_temps[i] < air_temps_min) {
      air_temps_min = air_temps[i];
    }
  }

  for (i = 1; i < GRAPH_NO_ELM; i++) {
    prev_tmp = air_temps[i-1]; 
    prev_pt_y = pointGetY (prev_tmp);
    pt_y = pointGetY (air_temps[i]);

    if (pt_y < 0) {
      continue;
    }

    pt_x = i * GRAPH_POINT_WIDTH;

    if (prev_pt_y < 0) {
      UTFT_.drawLine (pt_x, pt_y, pt_x, pt_y);
      continue;
    }

    UTFT_.drawLine (pt_x - GRAPH_POINT_WIDTH, prev_pt_y, pt_x, pt_y);
  }

  UTFT_.setColor (255, 255, 255);

  for (i = 0; i <= 7; i++) {
    float temp;
    int y;
    
    temp = (((air_temps_max - air_temps_min) * 1.1 + 1)/2 + (air_temps_max - air_temps_min) / 2) + air_temps_min -
      ((air_temps_max - air_temps_min) * 1.1 + 1) * i / 7;
    y = TITLE_HEIGHT + 2 + (dispy - TITLE_HEIGHT - 2 - 12) * i / 7;

    UTFT_.printNumF (temp, 1, 0, y);
  }
}

void Display::airTempGraphAddPoint (float point)
{
  int i;
  
  if (air_temps_ptr == GRAPH_NO_ELM - 1) {
    air_temps[air_temps_ptr] = point;
    
    for (i = 0; i < GRAPH_NO_ELM - 1; i++) {
      air_temps[i] = air_temps[i + 1];  
    }
  } else {
    air_temps[air_temps_ptr++] = point;
  }

  airTempGraphDraw (point);
}

static void displayWorkThread (void *user_data)
{
  TickType_t last_wake_time;
  float temp;
  
  disp.init();

  last_wake_time = xTaskGetTickCount ();

  while (1) {
    temp = tempAcquisition.getTemp (AIR_TEMP_SENS);
    disp.airTempGraphAddPoint (temp);
    vTaskDelayUntil (&last_wake_time, configTICK_RATE_HZ * 60);
  }
  
}

Display disp(displayWorkThread);

