#ifndef APP_H
#define APP_H

#include "../Sensor/SensorIntf.h"

// Define App States
enum AppState { APP_STATE_MAIN, APP_STATE_INFO, APP_STATE_TREND };

// UI Button Structure
struct Button {
  uint16_t x, y, w, h;
  const char *label;

  bool contains(uint16_t test_x, uint16_t test_y) const {
    return (test_x >= x && test_x <= x + w && test_y >= y && test_y <= y + h);
  }
};

// Global Button Definitions
extern const Button btnInfo;
extern const Button btnTrend;
extern const Button btnBack;

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
