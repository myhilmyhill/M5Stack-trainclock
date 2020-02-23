#include <M5Stack.h>

class GlobalType {
  private:
  String time = "00:00";

  public:
  String getTime() const {
    return time;
  }

  void setTime(String value) {
    time = value;
  }
};

static GlobalType GLOBAL;

void setup() {
  M5.begin();
}

void loop(){
  M5.Lcd.drawString(GLOBAL.getTime(), 0, 0);
  M5.update();
}
