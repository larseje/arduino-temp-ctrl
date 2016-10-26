#include <Adafruit_GFX.h>
#include <Fonts/FreeSerifBold9pt7b.h>
#include <UTFTGLUE.h>
#include <TouchScreen.h>
#include "RTOSThread.h"

#define DISP_RS A2
#define DISP_WR A1
#define DISP_CS A3
#define DISP_RST A4
#define DISP_RD A0

#define TOUCH_XP 6
#define TOUCH_YP A1
#define TOUCH_XM A2
#define TOUCH_YM 7

#define TITLE_BACK_COLOR 255, 0, 0
#define TITLE_HEIGHT 12

#define MENU_BACK_COLOR 0, 0, 255
#define MENU_WIDTH 40
#define GRAPH_NO_ELM 280
#define GRAPH_POINT_WIDTH 280/GRAPH_NO_ELM

class Display: public RTOSThread {
  public:
    Display (TaskFunction_t workFunction) :
      RTOSThread (workFunction, "DISP", 512, 1),
      UTFT_ (0, DISP_RS, DISP_WR, DISP_CS, DISP_RST, DISP_RD),
      Touch_ (TOUCH_XP, TOUCH_YP, TOUCH_XM, TOUCH_YM, 300),
      air_temps_ptr (0)
      {
        display_sem = xSemaphoreCreateMutex ();
      }
    void init ();
    void setTitle (const char *title);
    void clearTitle ();
    void clearMenu ();
    int pointGetY (float point);
    void airTempGraphDraw (float current_temp);
    void airTempGraphAddPoint (float point);

  private:

    int dispx;
    int dispy;
    UTFTGLUE UTFT_;
    TouchScreen Touch_;
    SemaphoreHandle_t display_sem;
    float air_temps [GRAPH_NO_ELM];
    int air_temps_ptr;
    float air_temps_min;
    float air_temps_max;
};

extern Display disp;
