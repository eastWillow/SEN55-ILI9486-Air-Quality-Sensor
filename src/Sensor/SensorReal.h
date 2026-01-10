#ifndef SENSOR_REAL_H
#define SENSOR_REAL_H

#include "SensorIntf.h"
#include <SensirionI2CSen5x.h>
#include <Wire.h>

class SensorReal : public SensorIntf {
private:
    SensirionI2CSen5x sen5x;

public:
    uint16_t begin() override {
        Wire.begin();
        sen5x.begin(Wire);
        return 0;
    }

    uint16_t startMeasurement() override {
        return sen5x.startMeasurement();
    }

    uint16_t readMeasuredValues(
        float &massConcentrationPm1p0,
        float &massConcentrationPm2p5,
        float &massConcentrationPm4p0,
        float &massConcentrationPm10p0,
        float &ambientHumidity,
        float &ambientTemperature,
        float &vocIndex,
        float &noxIndex) override {
        return sen5x.readMeasuredValues(
            massConcentrationPm1p0, massConcentrationPm2p5, massConcentrationPm4p0,
            massConcentrationPm10p0, ambientHumidity, ambientTemperature, vocIndex,
            noxIndex);
    }

    uint16_t deviceReset() override {
        return sen5x.deviceReset();
    }

    void setTemperatureOffsetSimple(float tempOffset) override {
        sen5x.setTemperatureOffsetSimple(tempOffset);
    }

    void errorToString(uint16_t error, char* errorMessage, uint16_t errorMessageLen) override {
         ::errorToString(error, errorMessage, errorMessageLen);
    }
};

#endif
