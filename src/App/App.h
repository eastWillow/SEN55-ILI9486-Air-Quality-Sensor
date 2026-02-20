#ifndef APP_H
#define APP_H

#include "../Sensor/SensorIntf.h"

// Define App States
enum AppState { APP_STATE_MAIN, APP_STATE_INFO };

// Button Coordinates
#define BTN_INFO_X 400
#define BTN_INFO_Y 10
#define BTN_INFO_W 70
#define BTN_INFO_H 30

#define BTN_BACK_X 10
#define BTN_BACK_Y 280
#define BTN_BACK_W 70
#define BTN_BACK_H 30

void App_Setup(SensorIntf *sensor);
void App_Loop(SensorIntf *sensor);
AppState App_GetState(void);
void App_ResetState(void);

#endif
