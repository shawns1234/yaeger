/**
 * AutoTunePID.h
 * PID controller with automatic tuning support.
 *
 * Copied from: https://github.com/lily-osp/AutoTunePID
 * while an issue is open with them.
 *
 * This is a vendor module — do not edit directly.
 */

#ifndef AUTOTUNEPID_H
#define AUTOTUNEPID_H

#include <Arduino.h>

// Enumeration for different tuning methods
enum class TuningMethod {
    ZieglerNichols,
    CohenCoon,
    IMC,
    TyreusLuyben,
    LambdaTuning,
    Manual
};

// Enumeration for operational modes
enum class OperationalMode {
    Normal,
    Reverse,
    Manual,
    Override,
    Track,
    Hold,
    Preserve,
    Tune,
    Auto
};

// Enumeration for oscillation modes
enum class OscillationMode {
    Normal,
    Half,
    Mild
};

class AutoTunePID
{
public:
    AutoTunePID(float minOutput, float maxOutput, TuningMethod method = TuningMethod::ZieglerNichols);

    void setSetpoint(float setpoint);
    void setTuningMethod(TuningMethod method);
    void setManualGains(float kp, float ki, float kd);
    void enableInputFilter(float alpha);
    void enableOutputFilter(float alpha);
    void enableAntiWindup(bool enable, float threshold = 0.8f);
    void setOperationalMode(OperationalMode mode);
    void setManualOutput(float output);
    void setOverrideOutput(float output);
    void setTrackReference(float reference);
    void setOscillationMode(OscillationMode mode);
    void setOscillationSteps(int steps);
    void setLambda(float lambda);
    void resetError();

    void update(float currentInput);
    float getOutput() const { return _output; }
    float getKp() const { return _kp; }
    float getKi() const { return _ki; }
    float getKd() const { return _kd; }
    float getKu() const { return _ultimateGain; }
    float getTu() const { return _oscillationPeriod; }
    float getSetpoint() const { return _setpoint; }
    OperationalMode getOperationalMode() const { return _operationalMode; }
    float getLambda() const { return _lambda; }

private:
    void computePID();
    void applyAntiWindup();
    float computeFilteredValue(float input, float& filteredValue, float alpha) const;

    void performAutoTune(float currentInput);
    void calculateZieglerNicholsGains();
    void calculateCohenCoonGains();
    void calculateIMCGains();
    void calculateTyreusLuybenGains();
    void calculateLambdaTuningGains();

    const float _minOutput;
    const float _maxOutput;
    TuningMethod _method;
    OperationalMode _operationalMode;
    OscillationMode _oscillationMode;
    int _oscillationSteps;
    float _setpoint;
    float _lambda;
    float _manualOutput;
    float _overrideOutput;
    float _trackReference;
    float _kp, _ki, _kd;
    float _error, _previousError;
    float _integral, _derivative;
    float _output;
    float _input;
    bool _antiWindupEnabled;
    float _integralWindupThreshold;
    unsigned long _lastUpdate;
    float _ultimateGain;
    float _oscillationPeriod;
    float _processTimeConstant;
    float _deadTime;
    float _integralTime;
    float _derivativeTime;
    bool _inputFilterEnabled;
    bool _outputFilterEnabled;
    float _inputFilteredValue;
    float _outputFilteredValue;
    float _inputFilterAlpha;
    float _outputFilterAlpha;
};

#endif
