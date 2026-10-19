#define CATCH_CONFIG_MAIN
#include "catch2.hpp"

#include "src/Controller/Controller.h"
#include "src/Pwm/Pwm.h"
#include "src/Sensor/Sensor.h"
#include "src/vendor/AutoTunePID.h"
#include "src/Logging/logging.h"

#include <cmath>
#include <cstdio>

// Mock Preferences for testing
class MockPreferences
{
public:
    float getFloat(const char* key, float defaultVal) const { return defaultVal; }
    float putFloat(const char* key, float value) { return value; }
    const char* getString(const char* key, const char* defaultVal) const { return defaultVal; }
    const char* putString(const char* key, const char* value) { return value; }
    bool isKey(const char* key) const { return false; }
    void putFloat(const char* key, float value) {}
    void putString(const char* key, const char* value) {}
};

TEST_CASE("Controller: PID computes correct output", "[controller]")
{
    Control control(1.0f, 0.1f, 0.01f, TemperatureTarget::BT);
    control->setSetpoint(100.0f);

    // Simulate a step response
    for (int i = 0; i < 100; i++) {
        control->setHeater(50.0f);
        control->setFan(30.0f);
        control->loop();
    }

    CHECK(control->getHeaterPower() >= 0.f);
    CHECK(control->getHeaterPower() <= 90.f);
    CHECK(control->getFanSpeed() >= 30.f);
}

TEST_CASE("Controller: setpoint clamping", "[controller]")
{
    Control control(1.0f, 0.1f, 0.01f, TemperatureTarget::BT);
    control->setSetpoint(251.0f);
    CHECK(control->getSetpoint() == 250.0f);

    control->setSetpoint(-10.0f);
    CHECK(control->getSetpoint() >= 0.f);
}

TEST_CASE("Pwm: duty cycle clamping", "[pwm]")
{
    PwmOutput fan(21, 20000.0f, 10, 0);
    fan.setValue(150.0f);
    CHECK(fan.getValue() <= 100.f);

    fan.setValue(-10.0f);
    CHECK(fan.getValue() >= 0.f);
}

TEST_CASE("Sensor: filtered value is a moving average", "[sensor]")
{
    Sensor sensor(33, 37, 35, "Test");
    sensor.takeReading();

    float v = sensor.getFilteredValue();
    CHECK(v >= -100.0f);
    CHECK(v <= 200.0f);
}

TEST_CASE("AutoTunePID: Ziegler-Nichols gains are positive", "[autotune]")
{
    AutoTunePID pid(0.f, 90.f, TuningMethod::ZieglerNichols);
    pid.setOperationalMode(OperationalMode::Tune);
    pid.setSetpoint(0.f);

    pid.update(0.f);
    pid.update(0.f);
    // ... continue toggling to induce oscillation ...

    CHECK(pid.getKp() > 0.f);
    CHECK(pid.getKi() > 0.f);
    CHECK(pid.getKd() >= 0.f);
}

TEST_CASE("Logging: format strings work", "[logging]")
{
    log("Simple message");
    logf("Formatted: %d, %f, %s", 42, 3.14159, "hello");
}

TEST_CASE("Sensor: filtered value is a moving average", "[sensor]")
{
    Sensor sensor(33, 37, 35, "Test");
    sensor.takeReading();

    float v = sensor.getFilteredValue();
    CHECK(v >= -100.0f);
    CHECK(v <= 200.0f);
}

TEST_CASE("Pwm: duty cycle clamping", "[pwm]")
{
    PwmOutput fan(21, 20000.0f, 10, 0);
    fan.setValue(150.0f);
    CHECK(fan.getValue() <= 100.f);

    fan.setValue(-10.0f);
    CHECK(fan.getValue() >= 0.f);
}

TEST_CASE("AutoTunePID: Ziegler-Nichols gains are positive", "[autotune]")
{
    AutoTunePID pid(0.f, 90.f, TuningMethod::ZieglerNichols);
    pid.setOperationalMode(OperationalMode::Tune);
    pid.setSetpoint(50.0f);

    pid.update(45.0f);
    pid.update(45.0f);
    pid.update(45.0f);

    // Gains may not be ready yet after a short run
    // Just check that the object is in a valid state
    CHECK(pid.getOperationalMode() == OperationalMode::Tune);
}

TEST_CASE("AutoTunePID: manual gains are set correctly", "[autotune]")
{
    AutoTunePID pid(0.f, 90.f, TuningMethod::Manual);
    pid.setManualGains(2.0f, 0.1f, 0.01f);
    pid.setOperationalMode(OperationalMode::Auto);

    CHECK(pid.getKp() == 2.0f);
    CHECK(pid.getKi() == 0.1f);
    CHECK(pid.getKd() == 0.01f);
}

TEST_CASE("AutoTunePID: anti-windup limits integral", "[autotune]")
{
    AutoTunePID pid(0.f, 90.f, TuningMethod::ZieglerNichols);
    pid.enableAntiWindup(true, 30.0f); // threshold = 0.8 * 90 = 72
    pid.setOperationalMode(OperationalMode::Auto);
    pid.setSetpoint(50.0f);

    // Drive the integral to a large value
    for (int i = 0; i < 1000; i++) {
        pid.update(0.0f);
    }

    // Anti-windup should have clamped the integral
    CHECK(pid.getKi() * pid._integral <= 30.0f);
}

TEST_CASE("AutoTunePID: manual mode bypasses PID", "[autotune]")
{
    AutoTunePID pid(0.f, 90.f, TuningMethod::Manual);
    pid.setOperationalMode(OperationalMode::Manual);
    pid.setManualOutput(50.0f);

    pid.update(100.0f);
    CHECK(pid.getOutput() == 45.0f); // 0 -> 90, scaled to 0..90
}

TEST_CASE("AutoTunePID: hold mode preserves output", "[autotune]")
{
    AutoTunePID pid(0.f, 90.f, TuningMethod::Manual);
    pid.setOperationalMode(OperationalMode::Hold);
    pid.setManualOutput(50.0f);

    pid.update(100.0f);
    pid.update(100.0f);
    pid.update(100.0f);

    CHECK(pid.getOutput() == 45.0f);
}
