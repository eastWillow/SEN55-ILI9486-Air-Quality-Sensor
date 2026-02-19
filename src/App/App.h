#ifndef APP_H
#define APP_H

#include "../Sensor/SensorIntf.h"

// Define App States
enum AppState { APP_STATE_MAIN, APP_STATE_INFO, APP_STATE_CHART };

// Button Coordinates
#define BTN_INFO_X 400
#define BTN_INFO_Y 10
#define BTN_INFO_W 70
#define BTN_INFO_H 30

#define BTN_BACK_X 10
#define BTN_BACK_Y 280
#define BTN_BACK_W 70
#define BTN_BACK_H 30

// Chart Button (Side by side with Info)
#define BTN_CHART_X 320
#define BTN_CHART_Y 10
#define BTN_CHART_W 70
#define BTN_CHART_H 30

// Chart Configuration
#define CHART_MAX_POINTS 200
#define CHART_UPDATE_INTERVAL 5000

struct SensorValues {
    float massConcentrationPm1p0;
    float massConcentrationPm2p5;
    float massConcentrationPm4p0;
    float massConcentrationPm10p0;
    float ambientHumidity;
    float ambientTemperature;
    float vocIndex;
    float noxIndex;
    bool valid; // To check if read was successful
};

void App_Setup(SensorIntf *sensor);
void App_Loop(SensorIntf *sensor);
AppState App_GetState(void);

// Exposed for testing and internal use
void App_UpdateSensor(SensorIntf *sensor);
const SensorValues* App_GetSensorValues();

// Chart Data Access
void App_AddChartData(float value);
const float* App_GetChartData();
int App_GetChartCount();
void App_ResetChartData();
void App_ResetState();

#endif
