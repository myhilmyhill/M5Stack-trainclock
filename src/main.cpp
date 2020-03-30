#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <M5Stack.h>
#include <WiFi.h>

#include <ForecasterNextVehicle.hpp>
#include <ParserTimetables.hpp>
#include <Time.hpp>

#include "settings.hpp"

static Time GLOBAL_TIME;
static JsonObject* GLOBAL_TIMETABLES;
static ForecasterNextVehicle* GLOBAL_FORECASTER;

void setTimetable() {
  static HTTPClient http;
  if (!http.connected()) {
    http.begin(SETTINGS_ADDRESS_TIMETABLE_JSON);
  }

  if (http.GET() < 0)
    return;

  DynamicJsonDocument doc(1024);
  deserializeJson(doc, http.getString());
  JsonObject timetables = doc.as<JsonObject>()["timetables"].as<JsonObject>();
  GLOBAL_TIMETABLES = &timetables;

  tm now = GLOBAL_TIME.getTime(500);

  static ForecasterNextVehicle forecaster = ParserTimetables::makeForecasterInitial(timetables, now);
  GLOBAL_FORECASTER = &forecaster;
}

void setup() {
  M5.begin();

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  if (WiFi.begin(SETTINGS_WIFI_SSID, SETTINGS_WIFI_PASS) != WL_DISCONNECTED) {
    ESP.restart();
  }
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }

  Time::init();
  setTimetable();
}

void loop() {
  if (!Time::getIsInited())
    return;

  // TODO: add reloading
  tm now = GLOBAL_TIME.getTime(500);
  if (!GLOBAL_FORECASTER->isInService(now)) {
    ParserTimetables::makeForecasterCurrent(*GLOBAL_FORECASTER, *GLOBAL_TIMETABLES, now);
  }

  static char strTime[] = "00:00:00";

  snprintf(strTime, sizeof(strTime), "%02d:%02d:%02d", now.tm_hour, now.tm_min,
           now.tm_sec);
  M5.Lcd.drawString(strTime, 0, 0);

  if (GLOBAL_FORECASTER != nullptr) {
    tm nextTrain = GLOBAL_FORECASTER->getNextTimeFrom(now);
    snprintf(strTime, sizeof(strTime), "%02d:%02d", nextTrain.tm_hour,
             nextTrain.tm_min);
    M5.Lcd.drawString(strTime, 0, 16);
  }

  M5.update();
  delay(250);
}
