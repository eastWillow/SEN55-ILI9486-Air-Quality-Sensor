#ifndef APP_H
#define APP_H

#include "../Sensor/SensorIntf.h"

// Define App States
enum AppState { APP_STATE_MAIN, APP_STATE_INFO, APP_STATE_TREND };

// Button Structure
struct Button {
  uint16_t x;
  uint16_t y;
  uint16_t w;
  uint16_t h;
  const char *label;

  bool contains(uint16_t touchX, uint16_t touchY) const {
    return (touchX >= x && touchX <= x + w && touchY >= y && touchY <= y + h);
  }
};

// Button Definitions
constexpr Button btnInfo = {400, 10, 70, 30, "INFO"};
constexpr Button btnTrend = {320, 10, 75, 30, "Trend"};
constexpr Button btnBack = {10, 280, 70, 30, "BACK"};

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
