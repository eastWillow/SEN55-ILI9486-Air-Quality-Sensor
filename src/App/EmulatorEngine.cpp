#include "EmulatorEngine.h"

#ifndef ARDUINO

#include "../App/App.h"
#include "../Display/LCD_Driver.h"
#include "../Display/LCD_Driver_SDL.h"

EmulatorEngine::EmulatorEngine() {}
EmulatorEngine::~EmulatorEngine() {}

void EmulatorEngine::initialize(SensorIntf *sensor) {
  m_sensor = sensor;
  App_Setup(sensor);
}

void EmulatorEngine::stepFrames(int count) {
  for (int i = 0; i < count; ++i) {
    App_Loop(m_sensor);
    LCD_Update();
  }
}

void EmulatorEngine::captureScreenshot(const std::string &path) {
  LCD_SaveScreenshot(path.c_str());
}

void EmulatorEngine::shutdown() { LCD_Quit(); }

#endif
