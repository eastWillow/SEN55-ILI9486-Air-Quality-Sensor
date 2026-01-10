#ifndef SENSOR_INTF_H
#define SENSOR_INTF_H

#include <stdint.h>

class SensorIntf {
public:
    virtual ~SensorIntf() {}
    virtual uint16_t begin() = 0;
    virtual uint16_t startMeasurement() = 0;
    virtual uint16_t readMeasuredValues(
        float &massConcentrationPm1p0,
        float &massConcentrationPm2p5,
        float &massConcentrationPm4p0,
        float &massConcentrationPm10p0,
        float &ambientHumidity,
        float &ambientTemperature,
        float &vocIndex,
        float &noxIndex) = 0;
    virtual uint16_t deviceReset() = 0;
    virtual void setTemperatureOffsetSimple(float tempOffset) = 0;
    virtual void errorToString(uint16_t error, char* errorMessage, uint16_t errorMessageLen) = 0;
};

#endif
