#include "Command.h"
#include "logging.h"
#include "preferenceKeys.h"

CommandHandler::CommandHandler(Control* controller, WiFiManager* wifiManager)
  : _controller(controller),
    _wifiManager(wifiManager)
{
}

Command CommandHandler::parseCommand(const char* commandJson, size_t length)
{
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, commandJson, length);

  if (error) {
    logf("Failed to parse JSON: %s\n", error.c_str());
    return Command::Unknown;
  }

  const char* command = doc["command"].as<const char*>();

  if (strcmp(command, "setHeater") == 0) {
    return Command::SetHeater;
  }
  if (strcmp(command, "setFan") == 0) {
    return Command::SetFan;
  }
  if (strcmp(command, "setSetpoint") == 0) {
    return Command::SetSetpoint;
  }
  if (strcmp(command, "setMode") == 0) {
    return Command::SetMode;
  }
  if (strcmp(command, "setTarget") == 0) {
    return Command::SetTarget;
  }
  if (strcmp(command, "getWifi") == 0) {
    return Command::GetWifi;
  }
  if (strcmp(command, "setWifi") == 0) {
    return Command::SetWifi;
  }
  if (strcmp(command, "autotune") == 0) {
    return Command::Autotune;
  }
  if (strcmp(command, "getPreferences") == 0) {
    return Command::GetPreferences;
  }
  if (strcmp(command, "setPreferences") == 0) {
    return Command::SetPreferences;
  }

  return Command::Unknown;
}

void CommandHandler::handleCommand(const char* commandJson, size_t length)
{
  Command cmd = parseCommand(commandJson, length);

  switch (cmd) {
    case Command::SetHeater: {
      JsonDocument doc;
      deserializeJson(doc, commandJson, length);
      float value = doc["value"].as<float>();
      _controller->setHeater(value);
      break;
    }
    case Command::SetFan: {
      JsonDocument doc;
      deserializeJson(doc, commandJson, length);
      float value = doc["value"].as<float>();
      _controller->setFan(value);
      break;
    }
    case Command::SetSetpoint: {
      JsonDocument doc;
      deserializeJson(doc, commandJson, length);
      float value = doc["value"].as<float>();
      _controller->setSetpoint(value);
      break;
    }
    case Command::SetMode: {
      JsonDocument doc;
      deserializeJson(doc, commandJson, length);
      const char* modeStr = doc["mode"].as<const char*>();
      if (strcmp(modeStr, "Manual") == 0) {
        _controller->setMode(OperationalMode::Manual);
      } else if (strcmp(modeStr, "PID") == 0) {
        _controller->setMode(OperationalMode::Auto);
      } else if (strcmp(modeStr, "Tuning") == 0) {
        _controller->setMode(OperationalMode::Tune);
      }
      break;
    }
    case Command::SetTarget: {
      JsonDocument doc;
      deserializeJson(doc, commandJson, length);
      const char* targetStr = doc["target"].as<const char*>();
      if (strcmp(targetStr, "BT") == 0) {
        _controller->setTarget(TemperatureTarget::BT);
      } else if (strcmp(targetStr, "ET") == 0) {
        _controller->setTarget(TemperatureTarget::ET);
      } else if (strcmp(targetStr, "MAX") == 0) {
        _controller->setTarget(TemperatureTarget::MAX);
      }
      break;
    }
    case Command::GetWifi: {
      // Return current WiFi state
      break;
    }
    case Command::SetWifi: {
      JsonDocument doc;
      deserializeJson(doc, commandJson, length);
      const char* ssid = doc["ssid"].as<const char*>();
      const char* pass = doc["pass"].as<const char*>();

      if (ssid && pass) {
        _wifiManager->begin();
        _wifiManager->_prefs->putString("ssid", ssid);
        _wifiManager->_prefs->putString("pass", pass);
      }
      break;
    }
    case Command::Autotune: {
      _controller->startAutotune();
      break;
    }
    case Command::GetPreferences: {
      // Return current preferences
      break;
    }
    case Command::SetPreferences: {
      JsonDocument doc;
      deserializeJson(doc, commandJson, length);

      if (doc["pidKp"].is<float>() && doc["pidKi"].is<float>() && doc["pidKd"].is<float>()) {
        float kp = doc["pidKp"].as<float>();
        float ki = doc["pidKi"].as<float>();
        float kd = doc["pidKd"].as<float>();

        _controller->_pid.setManualGains(kp, ki, kd);
        _controller->_controller->_pid.setOperationalMode(OperationalMode::Auto);
      }

      if (doc["cooldownFanSpeed"].is<long>()) {
        _controller->_prefs->putFloat(coolingFanKey, doc["cooldownFanSpeed"].as<long>());
      }

      break;
    }
    case Command::Unknown:
    default:
      break;
  }
}
