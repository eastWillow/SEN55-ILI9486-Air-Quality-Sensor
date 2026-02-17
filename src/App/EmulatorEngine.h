#ifndef EMULATOR_ENGINE_H
#define EMULATOR_ENGINE_H

#include "../Sensor/SensorIntf.h"

#ifndef ARDUINO
#include <string>

class EmulatorEngine {
private:
  SensorIntf *m_sensor = nullptr;

public:
  EmulatorEngine();
  ~EmulatorEngine();

  // Initialize the emulator with a sensor interface
  void initialize(SensorIntf *sensor);

  // Run the emulator for a specified number of frames
  // Each frame consists of App_Loop() and LCD_Update()
  void stepFrames(int count);

  // Capture the current display state to a file
  void captureScreenshot(const std::string &path);

  // Clean up resources
  void shutdown();
};

#endif

#endif
