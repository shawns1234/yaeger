/**
 * AutoTunePID.cpp
 * PID controller with automatic tuning support.
 *
 * Copied from: https://github.com/lily-osp/AutoTunePID
 * while an issue is open with them.
 *
 * This is a vendor module — do not edit directly.
 */

#include "AutoTunePID.h"

AutoTunePID::AutoTunePID(float minOutput, float maxOutput, TuningMethod method)
    : _minOutput(minOutput)
    , _maxOutput(maxOutput)
    , _method(method)
    , _operationalMode(OperationalMode::Normal)
    , _oscillationMode(OscillationMode::Normal)
    , _oscillationSteps(10)
    , _setpoint(0)
    , _lambda(0.5f)
    , _manualOutput(0)
    , _overrideOutput(0)
    , _trackReference(0)
    , _kp(0)
    , _ki(0)
    , _kd(0)
    , _error(0)
    , _previousError(0)
    , _integral(0)
    , _output(0)
    , _lastUpdate(0)
    , _ultimateGain(0)
    , _oscillationPeriod(0)
    , _processTimeConstant(0)
    , _deadTime(0)
    , _integralTime(0)
    , _derivativeTime(0)
    , _inputFilterEnabled(false)
    , _outputFilterEnabled(false)
    , _inputFilteredValue(0)
    , _outputFilteredValue(0)
    , _inputFilterAlpha(0.1f)
    , _outputFilterAlpha(0.1f)
    , _antiWindupEnabled(true)
    , _integralWindupThreshold(0.8f * (maxOutput - minOutput))
{
}

void AutoTunePID::setSetpoint(float setpoint)
{
    _setpoint = setpoint;
}

void AutoTunePID::setTuningMethod(TuningMethod method)
{
    _method = method;
}

void AutoTunePID::setManualGains(float kp, float ki, float kd)
{
    _kp = kp;
    _ki = ki;
    _kd = kd;
}

void AutoTunePID::enableInputFilter(float alpha)
{
    _inputFilterEnabled = true;
    _inputFilterAlpha = constrain(alpha, 0.01f, 1.0f);
}

void AutoTunePID::enableOutputFilter(float alpha)
{
    _outputFilterEnabled = true;
    _outputFilterAlpha = constrain(alpha, 0.01f, 1.0f);
}

void AutoTunePID::enableAntiWindup(bool enable, float threshold)
{
    _antiWindupEnabled = enable;
    _integralWindupThreshold = threshold * (_maxOutput - _minOutput);
}

void AutoTunePID::setOperationalMode(OperationalMode mode)
{
    _operationalMode = mode;
    if (mode == OperationalMode::Hold) {
        _integral = 0;
        _previousError = 0;
        _output = 0;
    }
}

void AutoTunePID::setManualOutput(float output)
{
    _manualOutput = constrain(output, 0, 100);
}

void AutoTunePID::setOverrideOutput(float output)
{
    _overrideOutput = constrain(output, _minOutput, _maxOutput);
}

void AutoTunePID::setTrackReference(float reference)
{
    _trackReference = reference;
}

void AutoTunePID::setOscillationMode(OscillationMode mode)
{
    _oscillationMode = mode;
    switch (mode) {
        case OscillationMode::Normal:
            _oscillationSteps = 10;
            break;
        case OscillationMode::Half:
            _oscillationSteps = 20;
            break;
        case OscillationMode::Mild:
            _oscillationSteps = 40;
            break;
    }
}

void AutoTunePID::setOscillationSteps(int steps)
{
    if (steps > 0) {
        _oscillationSteps = steps;
    }
}

void AutoTunePID::setLambda(float lambda)
{
    _lambda = lambda;
}

void AutoTunePID::update(float currentInput)
{
    unsigned long now = millis();
    if (now - _lastUpdate < 100) {
        return;
    }

    float dt = (now - _lastUpdate) / 1000.0f;
    _lastUpdate = now;

    if (_inputFilterEnabled && _operationalMode != OperationalMode::Tune) {
        currentInput = computeFilteredValue(currentInput, _inputFilteredValue, _inputFilterAlpha);
    }
    _input = currentInput;

    if (_operationalMode == OperationalMode::Tune) {
        performAutoTune(currentInput);
    } else if (_operationalMode == OperationalMode::Manual) {
        _output = map(_manualOutput, 0, 100, _minOutput, _maxOutput);
        return;
    } else if (_operationalMode == OperationalMode::Override) {
        _output = _overrideOutput;
        return;
    } else if (_operationalMode == OperationalMode::Track) {
        _output = constrain(_trackReference, _minOutput, _maxOutput);
        return;
    } else if (_operationalMode == OperationalMode::Hold) {
        return;
    } else if (_operationalMode == OperationalMode::Preserve) {
        if (_operationalMode == OperationalMode::Reverse) {
            _error = _input - _setpoint;
        } else {
            _error = _setpoint - _input;
        }
        return;
    } else {
        if (_operationalMode == OperationalMode::Reverse) {
            _error = _input - _setpoint;
        } else {
            _error = _setpoint - _input;
        }

        _integral += _error * dt;

        _derivative = (_error - _previousError) / dt;
        computePID();
        applyAntiWindup();
        _previousError = _error;
    }

    if (_outputFilterEnabled && _operationalMode != OperationalMode::Tune) {
        _output = computeFilteredValue(_output, _outputFilteredValue, _outputFilterAlpha);
    }
}

void AutoTunePID::performAutoTune(float currentInput)
{
    static unsigned long lastToggleTime = 0;
    static bool outputState = true;
    static int oscillationCount = 0;
    static unsigned long oscillationStartTime = 0;

    unsigned long currentTime = millis();

    float highOutput, lowOutput;
    switch (_oscillationMode) {
        case OscillationMode::Normal:
            highOutput = _maxOutput;
            lowOutput = _minOutput;
            break;
        case OscillationMode::Half:
            highOutput = (_maxOutput + _minOutput) / 2.0f + (_maxOutput - _minOutput) / 4.0f;
            lowOutput = (_maxOutput + _minOutput) / 2.0f - (_maxOutput - _minOutput) / 4.0f;
            break;
        case OscillationMode::Mild:
            highOutput = (_maxOutput + _minOutput) / 2.0f + (_maxOutput - _minOutput) / 8.0f;
            lowOutput = (_maxOutput + _minOutput) / 2.0f - (_maxOutput - _minOutput) / 8.0f;
            break;
    }

    if (currentTime - lastToggleTime >= 2000) {
        outputState = !outputState;
        _output = outputState ? highOutput : lowOutput;
        lastToggleTime = currentTime;

        if (oscillationCount == 0) {
            oscillationStartTime = currentTime;
        }
        oscillationCount++;

        if (oscillationCount >= _oscillationSteps) {
            _oscillationPeriod = (currentTime - oscillationStartTime) / (float)(_oscillationSteps * 1000);

            float relayAmplitude = (highOutput - lowOutput) / 2.0f;
            float oscillationAmplitude = relayAmplitude;
            _ultimateGain = 4.0f * relayAmplitude / (PI * oscillationAmplitude);

            _processTimeConstant = 0.67f * _oscillationPeriod;
            _deadTime = 0.17f * _oscillationPeriod;

            _integralTime = 2.0f * _deadTime;
            _derivativeTime = _deadTime / 2.0f;

            switch (_method) {
                case TuningMethod::ZieglerNichols:
                    calculateZieglerNicholsGains();
                    break;
                case TuningMethod::CohenCoon:
                    calculateCohenCoonGains();
                    break;
                case TuningMethod::IMC:
                    calculateIMCGains();
                    break;
                case TuningMethod::TyreusLuyben:
                    calculateTyreusLuybenGains();
                    break;
                case TuningMethod::LambdaTuning:
                    calculateLambdaTuningGains();
                    break;
                default:
                    break;
            }

            oscillationCount = 0;
            _operationalMode = OperationalMode::Normal;
        }
    }
}

void AutoTunePID::calculateZieglerNicholsGains()
{
    _kp = 0.6f * _ultimateGain;
    _ki = _kp / _integralTime;
    _kd = _kp * _derivativeTime;
}

void AutoTunePID::calculateCohenCoonGains()
{
    _kp = 0.8f * _ultimateGain;
    _ki = _kp / (0.8f * _oscillationPeriod);
    _kd = 0.194f * _kp * _oscillationPeriod;
}

void AutoTunePID::calculateIMCGains()
{
    float lambda = _lambda;
    if (lambda <= 0) {
        lambda = 0.5f * _processTimeConstant;
    }

    _kp = _processTimeConstant / (lambda + _deadTime);
    _ki = _kp / _processTimeConstant;
    _kd = _kp * _deadTime / 2.0f;
}

void AutoTunePID::calculateTyreusLuybenGains()
{
    _kp = 0.45f * _ultimateGain;
    _ki = _kp / _integralTime;
    _kd = 0.0f;
}

void AutoTunePID::calculateLambdaTuningGains()
{
    if (_lambda <= 0) {
        _lambda = 0.5f * _processTimeConstant;
    }

    _kp = _processTimeConstant / (_ultimateGain * (_lambda + _deadTime));
    _ki = _kp / _processTimeConstant;
    _kd = _kp * 0.5f * _deadTime;
}

void AutoTunePID::resetError()
{
    _error = 0;
}

void AutoTunePID::computePID()
{
    _error = _setpoint - _input;

    if (abs(_error) < 0.001) {
        _error = 0;
    }

    float P = _kp * _error;
    float I = _ki * _integral;
    float D = _kd * _derivative;

    _output = P + I + D;
    _output = constrain(_output, _minOutput, _maxOutput);
}

void AutoTunePID::applyAntiWindup()
{
    if (_antiWindupEnabled && abs(_integral) > _integralWindupThreshold) {
        _integral = constrain(_integral, -_integralWindupThreshold, _integralWindupThreshold);
    }
}

float AutoTunePID::computeFilteredValue(float input, float& filteredValue, float alpha) const
{
    filteredValue = (alpha * input) + ((1.0f - alpha) * filteredValue);
    return filteredValue;
}
