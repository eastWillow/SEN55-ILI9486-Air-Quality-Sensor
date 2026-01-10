#ifndef APP_H
#define APP_H

#include "../Sensor/SensorIntf.h"

// Define App States
enum AppState {
    APP_STATE_MAIN,
    APP_STATE_INFO
};

void App_Setup(SensorIntf* sensor);
void App_Loop(SensorIntf* sensor);

#endif
