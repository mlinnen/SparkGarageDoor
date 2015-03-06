#include "application.h"
#define SWITCH_CLOSED false
#define SWITCH_OPENED true

class GarageDoor {
 private:
  int _currentStatus;
  int _lastStatus;
  int _openSensorPin, _closeSensorPin, _relayPin, _openLEDPin, _closeLEDPin;
  boolean _lastDoorOpened;
  boolean _lastDoorClosed;

 public:
  GarageDoor(int openSensorPin, int closeSensorPin, int relayPin, int openLEDPin, int closeLEDPin);
  void begin(void);
  int readStatus();
  boolean Close();
  boolean Open();
  void PushGarageDoorButton();
};
