#ifndef APP_H
#define APP_H

#include "../Sensor/SensorIntf.h"

// Define color constants if not already defined globally or include LCD header
// Since LCD_GUI.h defines these, we might need to include it or forward declare.
// For the App interface, we just need the Setup and Loop.

void App_Setup(SensorIntf* sensor);
void App_Loop(SensorIntf* sensor);

#endif
