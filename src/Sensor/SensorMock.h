#ifndef SENSOR_MOCK_H
#define SENSOR_MOCK_H

#include "SensorIntf.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>

#ifndef ARDUINO
#include <cmath>
#endif

class SensorMock : public SensorIntf {
public:
  uint16_t begin() override;
  uint16_t startMeasurement() override;
  uint16_t readMeasuredValues(float &massConcentrationPm1p0,
                              float &massConcentrationPm2p5,
                              float &massConcentrationPm4p0,
                              float &massConcentrationPm10p0,
                              float &ambientHumidity, float &ambientTemperature,
                              float &vocIndex, float &noxIndex) override;
  uint16_t deviceReset() override;
  void setTemperatureOffsetSimple(float tempOffset) override;
  void errorToString(uint16_t error, char *errorMessage,
                     uint16_t errorMessageLen) override;

  // For testing
  void setFixedValue(float pm1, float pm25, float pm4, float pm10, float hum,
                     float temp, float voc, float nox);

private:
  bool m_useFixed = false;
  float m_fPm1, m_fPm25, m_fPm4, m_fPm10, m_fHum, m_fTemp, m_fVoc, m_fNox;
};

#endif
