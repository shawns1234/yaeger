#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <Arduino.h>
#include <Preferences.h>
#include <cmath>
#include "Sensor.h"
#include "Pwm.h"
#include "vendor/AutoTunePID.h"

enum class TemperatureTarget {
    BT,   // Bean temp
    ET,   // Exhaust temp
    MAX   // Max of both
};

enum class OperationalMode {
    Manual,
    Auto,
    Tune
};

const char* modeToChar(OperationalMode mode);

class Controller
{
private:
    // PID controller
    AutoTunePID _pid;
    float _setpoint;
    float _targetTemp;

    // Actuators
    PwmOutput _fan;
    PwmOutput _heater;

    // Sensors
    Sensor _exhaust;
    Sensor _bean;

    // State
    OperationalMode _mode;
    TemperatureTarget _target;
    unsigned long _lastUpdate;
    const unsigned long _updateIntervalMs = 20; // ~50 Hz

    // Autotune state
    bool _tuningEnabled;
    bool _hasAutotuneResults;
    unsigned long _tuneStartTime;
    const unsigned long _tuneStepMs = 2000; // toggle every 2s for relay method

    // Fault state
    bool _inFaultState;
    unsigned long _faultStartTime;
    const unsigned long _faultTimeoutMs = 5000;

    // Manual override
    bool _manualOverride;
    float _manualOutput;

    // Constants
    const float _maxHeaterPower = 90.0f;
    const float _fanMin = 30.0f;
    const float _fanMax = 100.0f;

public:
    Controller(float kp, float ki, float kd, TemperatureTarget target);
    ~Controller() = default;

    void begin();
    void loop();

    // Control
    float getHeaterPower() const;
    float getFanSpeed() const;
    float getSetpoint() const;
    float getExhaustTemp() const;
    float getBeanTemp() const;
    float getAmbientTemp() const;
    const char* getTarget() const;
    OperationalMode getMode() const;

    // Setters
    void setSetpoint(float setpoint);
    void setMode(OperationalMode mode);
    void setTarget(TemperatureTarget target);
    void setHeater(float power);
    void setFan(float speed);
    void startAutotune();
    void stopAutotune();
    bool hasAutotuneResults() const;
    float getKp() const;
    float getKi() const;
    float getKd() const;
};

#endif // CONTROLLER_H
