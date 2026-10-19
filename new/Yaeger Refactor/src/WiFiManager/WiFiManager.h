#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Preferences.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <Arduino.h>

enum class WifiState {
    Disconnected,
    Scanning,
    Connected,
    Fallback
};

class WiFiManager
{
private:
    Preferences* _prefs;
    WiFiMulti _wifiMulti;
    WifiState _state;
    const char* _ssid;
    const char* _pass;
    unsigned long _failCount;
    const unsigned long _maxFailures = 10;

public:
    WiFiManager(Preferences* prefs);
    ~WiFiManager() = default;

    void begin();
    void loop();
    WifiState getState() const;
    const char* getSSID() const;
    const char* getPass() const;
    bool isConnected() const;
};

#endif // WIFI_MANAGER_H
