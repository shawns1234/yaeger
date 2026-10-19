#include <Adafruit_NeoPixel.h>
#include <ESPAsyncWebServer.h>
#include <ElegantOTA.h>
#include <LittleFS.h>
#include <Preferences.h>

#include "AsyncWebSocket.h"
#include "Command/Command.h"
#include "HardwareSerial.h"
#include "WiFiManager/WiFiManager.h"
#include "api.h"
#include "display.h"
#include "logging.h"
#include "wifi_setup.h"
#include "Controller/Controller.h"
#include "preferenceKeys.h"

#define PIN 48
Adafruit_NeoPixel pixels(1, PIN);
Control* control;
Preferences preferences;
CommandHandler* commandHandler;

// for OTA
const char* host = "esp32 Roaster";

// WebSocket
AsyncWebSocket ws("/ws");
AsyncWebServer server(80);

void setupSimulation(AsyncWebSocket* ws);
void updateSimulation();

unsigned long ota_progress_millis = 0;

void onOTAStart()
{
  log("OTA update started!");
  pixels.setPixelColor(0, pixels.Color(255, 50, 0));
  pixels.show();
}

void onOTAProgress(size_t current, size_t final)
{
  if (millis() - ota_progress_millis > 1000) {
    ota_progress_millis = millis();
    logf("OTA Progress: %u / %u bytes\n", current, final);
  }
}

void onOTAEnd(bool success)
{
  if (success) {
    log("OTA update finished successfully!");
  } else {
    log("OTA update failed!");
  }
  pixels.setPixelColor(0, pixels.Color(0, 0, 0));
  pixels.show();
}

void setup()
{
  log("Starting Yaeger setup");
  pixels.begin();
  pixels.clear();
  pixels.setPixelColor(0, pixels.Color(5, 0, 0));
  pixels.show();

  log("Setting up WiFi");
  preferences.begin("preferences-v1");
  setupWifi(&preferences);
  initDisplay();
  setWifiIP();

  log("Initializing LittleFS");
  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS failed!");
  }
  server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
  server.serveStatic("/settings", LittleFS, "/").setDefaultFile("index.html");
  server.serveStatic("/editor", LittleFS, "/").setDefaultFile("index.html");

  log("Setting up HTTP API");
  setupApi(&server, &preferences);

  log("Initializing PID controller");
  control = new Control(
    preferences.getFloat(pidPKey, 1.0f),
    preferences.getFloat(pidIKey, 0.1f),
    preferences.getFloat(pidDKey, 0.01f),
    StringToTarget(preferences.getString(temperatureTargetKey, "ET"))
  );
  control->setFan(30.f);

  log("Setting up WebSocket");
  ws.onEvent([](AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len) {
    switch (type) {
      case WS_EVT_CONNECT:
        logf("[%u] Client connected\n", client->id());
        break;
      case WS_EVT_DISCONNECT:
        logf("[%u] Client disconnected\n", client->id());
        break;
      case WS_EVT_DATA: {
        AwsFrameInfo* info = (AwsFrameInfo*)arg;
        if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
          commandHandler->handleCommand((char*)data, len);
        }
      }
      break;
      default:
        break;
    }
  });

  server.addHandler(&ws);

  log("Starting server");
  server.begin();
  pixels.clear();
  pixels.setPixelColor(0, pixels.Color(0, 5, 0));
  pixels.show();

  log("Yaeger is ready");
}

void loop()
{
  ElegantOTA.loop();
  ws.cleanupClients();
  delay(10);

  // Handle WiFi reconnection
  if (WiFiManager::getState() != WifiState::Connected) {
    WiFiManager::loop();
  }

  // Main control loop
  control->loop();

  // Send WebSocket status update every 100ms
  static unsigned long lastWsUpdate = 0;
  if (millis() - lastWsUpdate >= 100) {
    lastWsUpdate = millis();

    JsonDocument doc;
    JsonObject root = doc.to<JsonObject>();
    JsonObject data = root["data"].to<JsonObject>();

    data["type"] = "status";
    data["ET"] = control->getExhaustTemp();
    data["BT"] = control->getBeanTemp();
    data["Amb"] = control->getAmbientTemp();
    data["BurnerVal"] = control->getHeaterPower();
    data["Setpoint"] = control->getSetpoint();
    data["Target"] = control->getTarget();
    data["Mode"] = modeToChar(control->getMode());
    data["FanVal"] = control->getFanSpeed();
    data["kp"] = control->getKp();
    data["ki"] = control->getKi();
    data["kd"] = control->getKd();
    data["wifiStrength"] = WiFi.RSSI();

    String json;
    serializeJson(doc, json);
    ws.textAll(json);
  }

  // Save PID gains back to preferences on each update
  preferences.putFloat(pidPKey, control->getKp());
  preferences.putFloat(pidIKey, control->getKi());
  preferences.putFloat(pidDKey, control->getKd());

  // Save preferences to WiFiManager
  if (preferences.isKey("ssid")) {
    control->_wifiManager->_prefs->putString("ssid", preferences.getString("ssid"));
  }
  if (preferences.isKey("pass")) {
    control->_wifiManager->_prefs->putString("pass", preferences.getString("pass"));
  }
}
