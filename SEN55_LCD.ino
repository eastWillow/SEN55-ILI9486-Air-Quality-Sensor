#include "src/Display/DEV_Config.h"
#include "src/Display/LCD_Driver.h"
#include "src/Display/LCD_GUI.h"
#include "src/Sensor/SensorReal.h"
#include "src/App/App.h"

// 宣告 SEN55 物件 (Wrapper)
SensorReal sen5x;

void setup()
{
  sen5x.begin();
  App_Setup(&sen5x);
}

void loop()
{
  App_Loop(&sen5x);
}
