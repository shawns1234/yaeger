#ifndef COMMAND_H
#define COMMAND_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "Controller.h"
#include "WiFiManager.h"

enum class Command {
    Unknown,
    SetHeater,
    SetFan,
    SetSetpoint,
    SetMode,
    SetTarget,
    GetStatus,
    SetWifi,
    Autotune,
    GetPreferences,
    SetPreferences
};

class CommandHandler
{
private:
    Control* _controller;
    WiFiManager* _wifiManager;

public:
    CommandHandler(Control* controller, WiFiManager* wifiManager);
    void handleCommand(const char* commandJson, size_t length);
    Command parseCommand(const char* commandJson, size_t length);
};

#endif // COMMAND_H
