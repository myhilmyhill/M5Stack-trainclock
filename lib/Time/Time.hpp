#include <string>
#include <Arduino.h>

class Time {
private:
  static bool isInited;

public:
  tm getTime(uint32_t interval) const;
  static void init();
  static bool getIsInited() {
    return isInited;
  }
};
