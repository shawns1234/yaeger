#include "api.h"
#include "logging.h"
#include "preferenceKeys.h"

void setupApi(AsyncWebServer* server, Preferences* preferences)
{
  log("Setting up HTTP API");

  server->on("/api/wifi", HTTP_GET, [preferences](AsyncWebServerRequest* request) {
    if (!request->hasParam("ssid") || !request->hasParam("pass")) {
      AsyncWebServerResponse* response = request->beginResponse(400);
      request->send(response);
      return;
    }

    const char* ssid = request->getParam("ssid")->value().c_str();
    const char* pass = request->getParam("pass")->value().c_str();

    preferences->putString(wifiSSIDKey, ssid);
    preferences->putString(wifiPassKey, pass);
    logf("Saved WiFi credentials: SSID=%s\n", ssid);

    request->send(200);
  });

  server->on("/api/pid", HTTP_GET, [preferences](AsyncWebServerRequest* request) {
    float kp = preferences->getFloat(pidPKey, 1.0f);
    float ki = preferences->getFloat(pidIKey, 0.1f);
    float kd = preferences->getFloat(pidDKey, 0.01f);

    AsyncWebResponse* response = request->beginResponse(200, "application/json");
    response->addHeader("Content-Type", "application/json");
    response->setContentType("application/json");
    response->addHeader("Access-Control-Allow-Origin", "*");
    response->addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    response->addHeader("Access-Control-Allow-Headers", "Content-Type");

    String json = String("{\"kp\":") + String(kp, 4) + String(",")
                 + String("\"ki\":") + String(ki, 4) + String(",")
                 + String("\"kd\":") + String(kd, 4) + String("}");

    response->setStatusCode(200);
    response->send(200, "application/json", json.c_str());
  });

  server->on("/api/pid", HTTP_POST, [preferences](AsyncWebServerRequest* request) {
    AsyncWebServerResponse* response = request->beginResponse(200, "application/json");
    response->addHeader("Content-Type", "application/json");

    String json = String("{\"status\":\"ok\",\"message\":\"PID values saved\"}");

    if (request->hasParam("kp") && request->hasParam("ki") && request->hasParam("kd")) {
      float kp = request->getParam("kp")->value().toFloat();
      float ki = request->getParam("ki")->value().toFloat();
      float kd = request->getParam("kd")->value().toFloat();

      preferences->putFloat(pidPKey, kp);
      preferences->putFloat(pidIKey, ki);
      preferences->putFloat(pidDKey, kd);

      logf("PID saved: Kp=%.3f Ki=%.3f Kd=%.3f\n", kp, ki, kd);

      json = String("{\"status\":\"ok\",\"kp\":") + String(kp, 4) + String(",")
            + String("\"ki\":") + String(ki, 4) + String(",")
            + String("\"kd\":") + String(kd, 4) + String("}") + String(",");
    }

    request->send(response);
  });

  server->on("/api/target", HTTP_POST, [preferences](AsyncWebServerRequest* request) {
    AsyncWebServerResponse* response = request->beginResponse(200, "application/json");
    response->addHeader("Content-Type", "application/json");

    if (request->hasParam("target")) {
      String target = request->getParam("target")->value().c_str();
      preferences->putString(temperatureTargetKey, target);

      logf("Temperature target set to: %s\n", target.c_str());

      response->setStatusCode(200);
      response->send(200, "application/json", String("{\"status\":\"ok\",\"target\":\"" + target + "\"}").c_str());
    } else {
      response->setStatusCode(400);
      response->send(400, "application/json", String("{\"status\":\"error\",\"message\":\"Missing target parameter\"}").c_str());
    }
  });

  server->on("/api/fan", HTTP_POST, [preferences](AsyncWebServerRequest* request) {
    AsyncWebServerResponse* response = request->beginResponse(200, "application/json");
    response->addHeader("Content-Type", "application/json");

    if (request->hasParam("speed")) {
      float speed = request->getParam("speed")->value().toFloat();
      preferences->putFloat(coolingFanKey, speed);

      logf("Fan speed set to: %.1f%%\n", speed);

      response->setStatusCode(200);
      response->send(200, "application/json", String("{\"status\":\"ok\",\"fan\":") + String(speed, 1) + String("}").c_str());
    } else {
      response->setStatusCode(400);
      response->send(400, "application/json", String("{\"status\":\"error\",\"message\":\"Missing speed parameter\"}").c_str());
    }
  });

  server->onNotFound([](AsyncWebServerRequest* request) {
    request->send(404, "text/plain", "Not Found");
  });
}
