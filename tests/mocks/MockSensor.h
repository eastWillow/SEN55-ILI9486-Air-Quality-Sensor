#ifndef MOCK_SENSOR_H
#define MOCK_SENSOR_H

#include "SensorIntf.h"
#include <gmock/gmock.h>

class MockSensor : public SensorIntf {
public:
    MOCK_METHOD(uint16_t, begin, (), (override));
    MOCK_METHOD(uint16_t, startMeasurement, (), (override));
    MOCK_METHOD(uint16_t, readMeasuredValues, (
        float &massConcentrationPm1p0,
        float &massConcentrationPm2p5,
        float &massConcentrationPm4p0,
        float &massConcentrationPm10p0,
        float &ambientHumidity,
        float &ambientTemperature,
        float &vocIndex,
        float &noxIndex), (override));
    MOCK_METHOD(uint16_t, deviceReset, (), (override));
    MOCK_METHOD(void, setTemperatureOffsetSimple, (float tempOffset), (override));
    MOCK_METHOD(void, errorToString, (uint16_t error, char* errorMessage, uint16_t errorMessageLen), (override));
};

#endif // MOCK_SENSOR_H
