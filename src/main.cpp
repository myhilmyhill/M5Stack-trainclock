#include <ForecasterNextVehicle.hpp>
#include <M5Stack.h>
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
  GLOBAL_TIMETABLE.push_back(TimeArrivalVehicle(0, 3));
  GLOBAL_TIMETABLE.push_back(TimeArrivalVehicle(0, 5));
  GLOBAL_TIMETABLE.push_back(TimeArrivalVehicle(22, 50));
  GLOBAL_TIMETABLE.push_back(TimeArrivalVehicle(22, 53));
  GLOBAL_TIMETABLE.push_back(TimeArrivalVehicle(22, 55));
  GLOBAL_TIMETABLE.push_back(TimeArrivalVehicle(23, 0));
  GLOBAL_TIMETABLE.push_back(TimeArrivalVehicle(24, 1));
  GLOBAL_FORECASTER = new ForecasterNextVehicle(GLOBAL_TIMETABLE);
}

void loop(){
  if (!Time::getIsInited()) return;

  tm now = GLOBAL_TIME.getTime(500);
  static char strTime[] = "00:00:00";

  snprintf(strTime, sizeof(strTime), "%02d:%02d:%02d", now.tm_hour, now.tm_min, now.tm_sec);
  M5.Lcd.drawString(strTime, 0, 0);

  tm nextTrain = GLOBAL_FORECASTER->getNextTimeFrom(now);
  snprintf(strTime, sizeof(strTime), "%02d:%02d:%02d", nextTrain.tm_hour, nextTrain.tm_min, nextTrain.tm_sec);
  M5.Lcd.drawString(strTime, 0, 16);

  M5.update();
  delay(250);
}
