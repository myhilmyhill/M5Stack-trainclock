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
static enum {
  SUCCESS,
  ERROR_NOT_READ,
  ERROR_NOT_PARSE,
  ERROR_NOT_INIT_FORECASTER,
} GLOBAL_ERROR;

void setTimetable(const tm& now) {
  static HTTPClient http;
  if (!http.connected()) {
    http.begin(SETTINGS_ADDRESS_TIMETABLE_JSON);
  }

  if (http.GET() < 0) {
    GLOBAL_ERROR = ERROR_NOT_READ;
    return;
  }

  DynamicJsonDocument doc(16384);
  if (deserializeJson(doc, http.getString()) != DeserializationError::Ok) {
    GLOBAL_ERROR = ERROR_NOT_PARSE;
    return;
  }

  static JsonObject timetables = doc.as<JsonObject>()["timetables"];
  GLOBAL_TIMETABLES = &timetables;

  try {
    static ForecasterNextVehicle forecaster =
        ParserTimetables::makeForecasterInitial(timetables, now);
    GLOBAL_FORECASTER = &forecaster;
  } catch (...) {
    GLOBAL_ERROR = ERROR_NOT_INIT_FORECASTER;
  }
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
  setTimetable(GLOBAL_TIME.getTime(500));
}

void loop() {
  if (!Time::getIsInited())
    return;

  if (GLOBAL_ERROR != SUCCESS) {
    M5.Lcd.clearDisplay(RED);
  }

  tm now = GLOBAL_TIME.getTime(500);
  if (!GLOBAL_FORECASTER) {
    setTimetable(now);
  }
  if (GLOBAL_FORECASTER && !GLOBAL_FORECASTER->isInService(now)) {
    ParserTimetables::makeForecasterCurrent(*GLOBAL_FORECASTER,
                                            *GLOBAL_TIMETABLES, now);
  }

  static char strTime[] = "00:00:00 0000/00/00 0";

  snprintf(strTime, sizeof(strTime), "%02d:%02d:%02d %d/%d/%d %d", now.tm_hour,
           now.tm_min, now.tm_sec, now.tm_year + 1900, now.tm_mon + 1,
           now.tm_mday, now.tm_wday);
  M5.Lcd.drawString(strTime, 0, 0);

  if (GLOBAL_FORECASTER) {
    tm nextTrain = GLOBAL_FORECASTER->getNextTimeFrom(now);
    snprintf(strTime, sizeof(strTime), "%02d:%02d:%02d %d/%d/%d %d",
             nextTrain.tm_hour, nextTrain.tm_min, nextTrain.tm_sec,
             nextTrain.tm_year + 1900, nextTrain.tm_mon + 1, nextTrain.tm_mday,
             nextTrain.tm_wday);
    M5.Lcd.drawString(strTime, 0, 16);
  }

  M5.update();
  delay(250);
}
