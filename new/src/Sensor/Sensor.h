#ifndef SENSORS_H
#define SENSORS_H

#include <Adafruit_MAX31855.h>
#include <cstdint>
#include "MovingAverageFilter.h"

class Sensor
{
private:
    Adafruit_MAX31855 _tc;
    String _sensorName;
    const uint8_t _noUpdateBeforeMs = 5;
    MovingAverageFilter _valueFilter = MovingAverageFilter(5);
    unsigned long _lastUpdate;
    float _value;
    float _filteredValue;
    float _ambient;

    void handleThermocoupleFault();

public:
    Sensor(int tClk, int tCs, int tDo, String sensorName);
    ~Sensor() = default;

    bool initialize();
    void takeReading();
    float getValue() const;
    float getFilteredValue() const;
    float getAmbient() const;
};

#endif // SENSORS_H
