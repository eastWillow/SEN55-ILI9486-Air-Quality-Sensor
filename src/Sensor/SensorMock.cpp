#ifndef ARDUINO

#include "SensorMock.h"

uint16_t SensorMock::begin() {
    return 0;
}

uint16_t SensorMock::startMeasurement() {
    return 0;
}

uint16_t SensorMock::readMeasuredValues(
    float &massConcentrationPm1p0,
    float &massConcentrationPm2p5,
    float &massConcentrationPm4p0,
    float &massConcentrationPm10p0,
    float &ambientHumidity,
    float &ambientTemperature,
    float &vocIndex,
    float &noxIndex) {

    // Return random dummy values
    massConcentrationPm1p0 = (float)(rand() % 50);
    massConcentrationPm2p5 = (float)(rand() % 50);
    massConcentrationPm4p0 = (float)(rand() % 50);
    massConcentrationPm10p0 = (float)(rand() % 50);
    ambientHumidity = 40.0f + (float)(rand() % 20);
    ambientTemperature = 20.0f + (float)(rand() % 10);
    vocIndex = (float)(rand() % 500);
    noxIndex = (float)(rand() % 500);

    return 0;
}

uint16_t SensorMock::deviceReset() {
    return 0;
}

void SensorMock::setTemperatureOffsetSimple(float tempOffset) {
    // No-op
}

void SensorMock::errorToString(uint16_t error, char* errorMessage, uint16_t errorMessageLen) {
    snprintf(errorMessage, errorMessageLen, "Mock Error %d", error);
}
#endif
