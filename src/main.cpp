#include <ArduinoJson.h>
#include <ForecasterNextVehicle.hpp>
#include <M5Stack.h>
#include <ParserTimetables.hpp>
#include <Time.hpp>
#include <WiFi.h>
#include "settings.hpp"

static Time GLOBAL_TIME;
static std::vector<TimeArrivalVehicle> GLOBAL_TIMETABLE;
static const ForecasterNextVehicle* GLOBAL_FORECASTER;

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

  // TODO: remove test codes
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, 
  "{"
    "\"timetables\":{"
      "\"weekday\": {"
        "\"days\": [1,2,3,4,5],"
        "\"timetable\" :["
          "{\"hour\":22,\"minutes\": [33,40,50,59]},"
          "{\"hour\":24,\"minutes\": [0,10,20,30,40,50]}"
        "]"
      "},"
      "\"holiday\": {"
        "\"days\": [0,6],"
        "\"timetable\" :["
          "{\"hour\":2,\"minutes\": [33,40,50,59]},"
          "{\"hour\":4,\"minutes\": [0,10,20,30,40,50]}"
        "]"
      "}"
    "}"
  "}"
  );
  JsonObject timetables = doc.as<JsonObject>()["timetables"].as<JsonObject>();

  tm now = GLOBAL_TIME.getTime(500);
  JsonString key = ParserTimetables::GetKeyTimetable(timetables, now.tm_wday);
  JsonArray times = timetables[key]["timetable"];

  GLOBAL_TIMETABLE =  ParserTimetables::MakeForecasterFromJsonString(times);
  std::sort(GLOBAL_TIMETABLE.begin(), GLOBAL_TIMETABLE.end());
  GLOBAL_FORECASTER = new ForecasterNextVehicle(GLOBAL_TIMETABLE, now);
}

void loop(){
  if (!Time::getIsInited()) return;

  // TODO: add reloading at changing date

  tm now = GLOBAL_TIME.getTime(500);
  static char strTime[] = "00:00:00";

  snprintf(strTime, sizeof(strTime), "%02d:%02d:%02d", now.tm_hour, now.tm_min, now.tm_sec);
  M5.Lcd.drawString(strTime, 0, 0);

  tm nextTrain = GLOBAL_FORECASTER->getNextTimeFrom(now);
  snprintf(strTime, sizeof(strTime), "%02d:%02d", nextTrain.tm_hour, nextTrain.tm_min);
  M5.Lcd.drawString(strTime, 0, 16);

  M5.update();
  delay(250);
}
