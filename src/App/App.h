#ifndef APP_H
#define APP_H

#include "../Sensor/SensorIntf.h"

// Define App States
enum AppState { APP_STATE_MAIN, APP_STATE_INFO, APP_STATE_TREND };

// Button Coordinates
#define BTN_INFO_X 400
#define BTN_INFO_Y 10
#define BTN_INFO_W 70
#define BTN_INFO_H 30

#define BTN_TREND_X 320
#define BTN_TREND_Y 10
#define BTN_TREND_W 75
#define BTN_TREND_H 30

#define BTN_BACK_X 10
#define BTN_BACK_Y 280
#define BTN_BACK_W 70
#define BTN_BACK_H 30

class TimeProvider {
public:
  virtual unsigned long getMillis() = 0;
  virtual ~TimeProvider() = default;
};

class SystemTimeProvider : public TimeProvider {
public:
  unsigned long getMillis() override;
};

class MockTimeProvider : public TimeProvider {
private:
  unsigned long current_time = 0;

public:
  unsigned long getMillis() override;
  void advance(unsigned long ms);
  void set(unsigned long ms);
};

void App_Setup(SensorIntf *sensor, TimeProvider *timeProvider);
void App_Loop(SensorIntf *sensor);
AppState App_GetState(void);
void App_ResetState(void);

#endif
