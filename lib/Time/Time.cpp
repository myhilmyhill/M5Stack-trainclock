#include <ctime>
#include <cstdio>
#include <Arduino.h>
#include "Time.hpp"

bool Time::isInited = false;

tm Time::getTime(uint32_t interval) const {
  tm now;
  if (!getLocalTime(&now, interval)) {
    this->getTime(interval + 10);
  }

  return now;
}

void Time::init() {
  static const char *ntps[] = {
    "ntp.nict.jp",
    "time.google.com",
    "ntp.jst.mfeed.ad.jp",
  };
  configTzTime("JST-9", ntps[0], ntps[1], ntps[2]);
  isInited = true;
}
