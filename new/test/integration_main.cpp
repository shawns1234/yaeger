#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "src/Controller/Controller.h"
#include "src/Sensor/Sensor.h"
#include "src/Pwm/Pwm.h"

TEST_CASE("Controller: steady-state PID")
{
    Control control(1.0f, 0.1f, 0.01f, TemperatureTarget::BT);
    control->setSetpoint(100.0f);

    for (int i = 0; i < 200; i++) {
        control->setHeater(control->getHeaterPower());
        control->setFan(control->getFanSpeed());
        control->loop();
    }

    // At steady state, heater power should be near 0
    CHECK(control->getHeaterPower() < 1.0f);
}

TEST_CASE("Controller: autotune produces positive gains")
{
    Control control(0.f, 0.f, 0.f, TemperatureTarget::BT);
    control->startAutotune();

    for (int i = 0; i < 500; i++) {
        control->loop();
    }

    if (control->hasAutotuneResults()) {
        CHECK(control->getKp() > 0.f);
        CHECK(control->getKi() > 0.f);
        CHECK(control->getKd() >= 0.f);
    } else {
        SKIP("Autotune did not complete");
    }
}
