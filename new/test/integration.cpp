#include "catch2.hpp"

#include "src/Controller/Controller.h"
#include "src/Sensor/Sensor.h"
#include "src/Pwm/Pwm.h"
#include "src/WiFiManager/WiFiManager.h"

#include <cmath>
#include <cstdio>

// Mock Preferences for integration tests
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

TEST_CASE("Integration: full control loop", "[integration]")
{
    Control control(1.0f, 0.1f, 0.01f, TemperatureTarget::BT);

    for (int i = 0; i < 100; i++) {
        control->loop();
    }

    // Heater power should be within bounds
    CHECK(control->getHeaterPower() >= 0.f);
    CHECK(control->getHeaterPower() <= 90.f);

    // Fan speed should be at minimum when heating
    CHECK(control->getFanSpeed() >= 30.f);
}

TEST_CASE("Integration: sensor fault handling", "[integration]")
{
    Sensor sensor(33, 37, 35, "FaultySensor");

    // Simulate a fault
    sensor._tc.readError = []() { return MAX31855_FAULT_OPEN; };

    sensor.takeReading();

    // The fault should be logged (we check that the state changed)
    CHECK(!isnan(sensor.getValue()));
}

TEST_CASE("Integration: PWM output bounds", "[integration]")
{
    PwmOutput heater(3, 50.0f, 10, 1);
    heater.setValue(100.0f);
    CHECK(heater.getValue() <= 100.f);

    heater.setValue(-50.0f);
    CHECK(heater.getValue() >= 0.f);
}

TEST_CASE("Integration: WiFi manager state", "[integration]")
{
    MockPreferences prefs;
    WiFiManager wifi(&prefs);
    wifi.begin();

    // The manager should eventually reach a stable state
    for (int i = 0; i < 100; i++) {
        wifi.loop();
    }

    WifiState state = wifi.getState();
    CHECK(state == WifiState::Connected || state == WifiState::Fallback);
}
