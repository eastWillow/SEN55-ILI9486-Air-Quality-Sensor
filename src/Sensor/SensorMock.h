#ifndef SENSOR_MOCK_H
#define SENSOR_MOCK_H

#include "SensorIntf.h"
#include <cstdlib>
#include <cstdio>
#include <cmath>

#ifndef ARDUINO
using std::isnan;
#endif

class SensorMock : public SensorIntf {
public:
    uint16_t begin() override;
    uint16_t startMeasurement() override;
    uint16_t readMeasuredValues(
        float &massConcentrationPm1p0,
        float &massConcentrationPm2p5,
        float &massConcentrationPm4p0,
        float &massConcentrationPm10p0,
        float &ambientHumidity,
        float &ambientTemperature,
        float &vocIndex,
        float &noxIndex) override;
    uint16_t deviceReset() override;
    void setTemperatureOffsetSimple(float tempOffset) override;
    void errorToString(uint16_t error, char* errorMessage, uint16_t errorMessageLen) override;
};

#endif
