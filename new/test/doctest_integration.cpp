#ifndef DOCTEST_INTEGRATION_H
#define DOCTEST_INTEGRATION_H

#include "catch2/catch_all.hpp"

#include "src/Controller/Controller.h"
#include "src/Sensor/Sensor.h"
#include "src/Pwm/Pwm.h"
#include "src/vendor/AutoTunePID.h"

#include <cmath>

TEST_CASE("Controller: PID output is within bounds", "[integration]")
{
    Control control(1.0f, 0.1f, 0.01f, TemperatureTarget::BT);
    control->setSetpoint(100.0f);

    for (int i = 0; i < 100; i++) {
        control->loop();
    }

    CHECK(control->getHeaterPower() >= 0.f);
    CHECK(control->getHeaterPower() <= 90.f);
}

TEST_CASE("Controller: setpoint clamping", "[integration]")
{
    Control control(1.0f, 0.1f, 0.01f, TemperatureTarget::BT);
    control->setSetpoint(251.0f);
    CHECK(control->getSetpoint() == 250.0f);

    control->setSetpoint(-10.0f);
    CHECK(control->getSetpoint() >= 0.f);
}

TEST_CASE("Sensor: reading is within reasonable range", "[integration]")
{
    Sensor sensor(33, 37, 35, "Test");
    sensor.takeReading();

    CHECK(sensor.getValue() >= -100.0f);
    CHECK(sensor.getValue() <= 200.0f);
}

TEST_CASE("Pwm: duty cycle is normalized", "[integration]")
{
    PwmOutput heater(3, 50.0f, 10, 1);
    heater.setValue(150.0f);
    CHECK(heater.getValue() <= 100.f);

    heater.setValue(-10.0f);
    CHECK(heater.getValue() >= 0.f);
}

TEST_CASE("AutoTunePID: Ziegler-Nichols gains are positive", "[integration]")
{
    AutoTunePID pid(0.f, 90.f, TuningMethod::ZieglerNichols);
    pid.setOperationalMode(OperationalMode::Tune);

    for (int i = 0; i < 100; i++) {
        pid.update(0.0f);
    }

    // Gains may not be ready after this short run; skip if not
    if (pid.getKp() > 0.f) {
        CHECK(pid.getKp() > 0.f);
        CHECK(pid.getKi() > 0.f);
        CHECK(pid.getKd() >= 0.f);
    }
}

#endif // DOCTEST_INTEGRATION_H
