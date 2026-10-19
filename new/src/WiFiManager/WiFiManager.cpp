#include "WiFiManager.h"
#include "logging.h"

WiFiManager::WiFiManager(Preferences* prefs)
  : _prefs(prefs),
    _state(WifiState::Disconnected),
    _ssid(nullptr),
    _pass(nullptr),
    _failCount(0),
    _maxFailures(10)
{
}

void WiFiManager::begin()
{
  WiFi.mode(WIFI_STA);
  WiFi.setHostname("yaeger");

  if (_prefs->isKey("ssid")) {
    _ssid = _prefs->getString("ssid", "");
    _pass = _prefs->getString("pass", "");

    log("WiFi credentials found, attempting to connect...");
    _wifiMulti.addAP(_ssid, _pass);
    _wifiMulti.addAP(nullptr, nullptr); // fallback to AP mode
  } else {
    log("No WiFi credentials, starting AP mode");
    _wifiMulti.addAP(nullptr, nullptr);
  }

  _wifiMulti.run();
}

void WiFiManager::loop()
{
  if (_state == WifiState::Connected) {
    return;
  }

  WiFiMultiResult result = _wifiMulti.run(1000);

  if (result == WL_CONNECTED) {
    _state = WifiState::Connected;
    _failCount = 0;
    log("Connected to " + WiFi.SSID());
    log("IP address: " + WiFi.localIP().toString());

    // Save credentials to preferences
    _prefs->putString("ssid", WiFi.SSID());
    _prefs->putString("pass", WiFi.pwd());
  } else if (result == WL_NO_SSID_FOUND || result == WL_IDLE_STATUS) {
    _state = WifiState::Fallback;
    log("No networks found, switching to AP mode");
  } else if (result == WL_CONNECT_FAILED) {
    _failCount++;
    logf("Connect failed (%u/%u), falling back to AP\n", _failCount, _maxFailures);

    if (_failCount >= _maxFailures) {
      _state = WifiState::Fallback;
      log("Max failures reached, switching to AP mode");
    }
  }
}

WifiState WiFiManager::getState() const
{
  return _state;
}

const char* WiFiManager::getSSID() const
{
  return _ssid;
}

const char* WiFiManager::getPass() const
{
  return _pass;
}

bool WiFiManager::isConnected() const
{
  return _state == WifiState::Connected;
}
