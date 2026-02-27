#ifndef ARDUINO
#include "SensorMock.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

SensorMock::SensorMock() : m_gen(1) {}

void SensorMock::setSeed(uint32_t seed) { m_gen.seed(seed); }

uint16_t SensorMock::begin() { return 0; }

uint16_t SensorMock::startMeasurement() { return 0; }

uint16_t SensorMock::readMeasuredValues(float &massConcentrationPm1p0,
                                        float &massConcentrationPm2p5,
                                        float &massConcentrationPm4p0,
                                        float &massConcentrationPm10p0,
                                        float &ambientHumidity,
                                        float &ambientTemperature,
                                        float &vocIndex, float &noxIndex) {

  if (m_useFixed) {
    massConcentrationPm1p0 = m_fPm1;
    massConcentrationPm2p5 = m_fPm25;
    massConcentrationPm4p0 = m_fPm4;
    massConcentrationPm10p0 = m_fPm10;
    ambientHumidity = m_fHum;
    ambientTemperature = m_fTemp;
    vocIndex = m_fVoc;
    noxIndex = m_fNox;
  } else {
    // Use secure PRNG instead of rand()
    std::uniform_real_distribution<float> pmDist(0.0f, 50.0f);
    std::uniform_real_distribution<float> humDist(40.0f, 60.0f);
    std::uniform_real_distribution<float> tempDist(20.0f, 30.0f);
    std::uniform_real_distribution<float> indexDist(0.0f, 500.0f);

    massConcentrationPm1p0 = pmDist(m_gen);
    massConcentrationPm2p5 = pmDist(m_gen);
    massConcentrationPm4p0 = pmDist(m_gen);
    massConcentrationPm10p0 = pmDist(m_gen);
    ambientHumidity = humDist(m_gen);
    ambientTemperature = tempDist(m_gen);
    vocIndex = indexDist(m_gen);
    noxIndex = indexDist(m_gen);
  }

  return 0;
}

void SensorMock::setFixedValue(float pm1, float pm25, float pm4, float pm10,
                               float hum, float temp, float voc, float nox) {
  m_useFixed = true;
  m_fPm1 = pm1;
  m_fPm25 = pm25;
  m_fPm4 = pm4;
  m_fPm10 = pm10;
  m_fHum = hum;
  m_fTemp = temp;
  m_fVoc = voc;
  m_fNox = nox;
}

uint16_t SensorMock::deviceReset() { return 0; }

void SensorMock::setTemperatureOffsetSimple(float tempOffset) {
  // No-op
}

void SensorMock::errorToString(uint16_t error, char *errorMessage,
                               uint16_t errorMessageLen) {
  snprintf(errorMessage, errorMessageLen, "Mock Error %d", error);
}
#endif
