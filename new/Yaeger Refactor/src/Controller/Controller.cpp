#include "Controller.h"
#include "roaster_config.h"
#include "logging.h"

const char* modeToChar(OperationalMode mode)
{
  switch (mode) {
    case OperationalMode::Auto:  return "PID";
    case OperationalMode::Tune:  return "Tuning";
    default:                     return "Manual";
  }
}

Controller::Controller(float kp, float ki, float kd, TemperatureTarget target)
  : _pid(0, _maxHeaterPower, TuningMethod::ZieglerNichols),
    _setpoint(0.f),
    _targetTemp(ET),
    _fan(HEATER_PIN, FAN_FREQUENCY, 10, 0),   // fan on channel 0
    _heater(HEATER_PIN, HEATER_FREQUENCY, 10, 1),  // heater on channel 1
    _exhaust(MAX1CLK, MAX1CS, MAX1DO, "Exhaust"),
    _bean(MAX2CLK, MAX2CS, MAX2DO, "Bean"),
    _mode(OperationalMode::Manual),
    _target(ET),
    _lastUpdate(0),
    _tuningEnabled(false),
    _hasAutotuneResults(false),
    _tuneStartTime(0),
    _inFaultState(false),
    _faultStartTime(0),
    _manualOverride(false),
    _manualOutput(0.f)
{
  // Initialize PID with default gains
  _pid.setManualGains(kp, ki, kd);
  _pid.enableAntiWindup(true, 0.8f * (_maxHeaterPower - 0.f));
  _pid.setOscillationMode(OscillationMode::Normal);
  _pid.setSetpoint(0.f);
  _pid.setOperationalMode(OperationalMode::Manual);
  _pid.setManualOutput(0.f);
}

void Controller::begin()
{
  _exhaust.initialize();
  _bean.initialize();
}

void Controller::loop()
{
  unsigned long now = millis();
  unsigned long dt = now - _lastUpdate;

  if (dt < _updateIntervalMs) {
    return;
  }

  _lastUpdate = now;

  // Take sensor readings
  _exhaust.takeReading();
  _bean.takeReading();

  // Handle fault state
  if (_inFaultState) {
    if (millis() - _faultStartTime < _faultTimeoutMs) {
      // Still in fault timeout — keep fault active
    } else {
      // Fault timeout expired — return to normal operation
      _inFaultState = false;
      _mode = OperationalMode::Auto;
      _pid.setOperationalMode(OperationalMode::Auto);
      log("Fault timeout expired, resuming normal operation");
    }
    return;
  }

  // Check for sensor faults
  if (_exhaust._tc.readError()) {
    _inFaultState = true;
    _faultStartTime = millis();
    _pid.setOperationalMode(OperationalMode::Hold);
    log("Fault detected on exhaust sensor, entering fault state");
    return;
  }
  if (_bean._tc.readError()) {
    _inFaultState = true;
    _faultStartTime = millis();
    _pid.setOperationalMode(OperationalMode::Hold);
    log("Fault detected on bean sensor, entering fault state");
    return;
  }

  // Get current temperature based on target
  float currentTemp;
  switch (_target) {
    case TemperatureTarget::BT:   currentTemp = _bean.getFilteredValue();   break;
    case TemperatureTarget::ET:   currentTemp = _exhaust.getFilteredValue(); break;
    case TemperatureTarget::MAX:  currentTemp = std::max(_bean.getFilteredValue(), _exhaust.getFilteredValue()); break;
  }

  // Run PID controller
  _pid.update(currentTemp);

  float heaterOutput = _pid.getOutput();

  // Apply constraints
  heaterOutput = std::clamp(heaterOutput, 0.f, _maxHeaterPower);

  // Set heater and fan
  _heater.setValue(heaterOutput);
  _fan.setValue(_fanMin);

  // If heater is on, ensure fan is at minimum
  if (heaterOutput > 0.f) {
    _fan.setValue(_fanMin);
  }
}

float Controller::getHeaterPower() const
{
  return _pid.getOutput();
}

float Controller::getFanSpeed() const
{
  return _fan.getValue();
}

float Controller::getSetpoint() const
{
  return _pid.getSetpoint();
}

float Controller::getExhaustTemp() const
{
  return _exhaust.getFilteredValue();
}

float Controller::getBeanTemp() const
{
  return _bean.getFilteredValue();
}

float Controller::getAmbientTemp() const
{
  return _exhaust.getAmbient();
}

const char* Controller::getTarget() const
{
  switch (_target) {
    case TemperatureTarget::BT:   return "BT";
    case TemperatureTarget::ET:   return "ET";
    case TemperatureTarget::MAX:  return "MAX";
  }
  return "ET";
}

OperationalMode Controller::getMode() const
{
  return _pid.getOperationalMode();
}

void Controller::setSetpoint(float setpoint)
{
  _setpoint = std::clamp(setpoint, 0.f, 250.f);
  _pid.setSetpoint(_setpoint);
}

void Controller::setMode(OperationalMode mode)
{
  _mode = mode;

  switch (mode) {
    case OperationalMode::Auto:
      _pid.setOperationalMode(OperationalMode::Auto);
      break;
    case OperationalMode::Tune:
      _pid.setOperationalMode(OperationalMode::Tune);
      _tuningEnabled = true;
      log("Autotune started");
      break;
    case OperationalMode::Manual:
      _pid.setOperationalMode(OperationalMode::Manual);
      _tuningEnabled = false;
      break;
  }
}

void Controller::setTarget(TemperatureTarget target)
{
  _target = target;
}

void Controller::setHeater(float power)
{
  _manualOverride = true;
  _manualOutput = std::clamp(power, 0.f, 100.f);
  _pid.setManualOutput(_manualOutput);
}

void Controller::setFan(float speed)
{
  _fan.setValue(std::clamp(speed, 0.f, 100.f));
}

void Controller::startAutotune()
{
  _pid.setOperationalMode(OperationalMode::Tune);
  _tuningEnabled = true;
  _hasAutotuneResults = false;
  _tuneStartTime = millis();
  log("Autotune started");
}

void Controller::stopAutotune()
{
  _pid.setOperationalMode(OperationalMode::Auto);
  _tuningEnabled = false;
  log("Autotune stopped");
}

bool Controller::hasAutotuneResults() const
{
  return _hasAutotuneResults;
}

float Controller::getKp() const
{
  return _pid.getKp();
}

float Controller::getKi() const
{
  return _pid.getKi();
}

float Controller::getKd() const
{
  return _pid.getKd();
}
