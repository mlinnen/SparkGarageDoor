#include "GarageDoor.h"
#include "application.h"
#include "math.h"

const int STATUS_UNKNOWN = 0;
const int STATUS_OPENED = 1;
const int STATUS_CLOSED = 2;
const int STATUS_OPENING = 3;
const int STATUS_CLOSING = 4;

GarageDoor::GarageDoor(int openSensorPin, int closeSensorPin, int relayPin, int openLEDPin, int closeLEDPin) {
  _openSensorPin = openSensorPin;
  _closeSensorPin = closeSensorPin;
  _closeLEDPin = closeLEDPin;
  _openLEDPin = openLEDPin;
  _relayPin = relayPin;
}
void GarageDoor::begin(void) {
  // set up the pins!
  pinMode(_relayPin, OUTPUT);
  digitalWrite(_relayPin,LOW);

  pinMode(_openSensorPin,INPUT_PULLUP);
  pinMode(_closeSensorPin,INPUT_PULLUP);

  pinMode(_openLEDPin, OUTPUT);
  pinMode(_closeLEDPin, OUTPUT);

}

int GarageDoor::readStatus(){
  int s;
  boolean doorClosed, doorOpened;
  // Read the current state of the sensors
  doorClosed = digitalRead(_closeSensorPin);
  doorOpened = digitalRead(_openSensorPin);
  // TODO should this be debounced?

  if (doorClosed==SWITCH_CLOSED && doorOpened==SWITCH_CLOSED)
  {
      // Both sensors are closed and this is an unknown state
      s = STATUS_UNKNOWN;
  }
  else if(doorClosed==SWITCH_CLOSED && doorOpened==SWITCH_OPENED)
  {
      // The door appears to be fully closed
      s = STATUS_CLOSED;
  }
  else if(doorClosed==SWITCH_OPENED && doorOpened==SWITCH_CLOSED)
  {
      // The door appears to be fully opened
      s = STATUS_OPENED;
  }
  else if(doorClosed==SWITCH_OPENED && doorOpened==SWITCH_OPENED)
  {
      if (_lastDoorClosed==SWITCH_CLOSED && _lastDoorOpened==SWITCH_OPENED){
          // The door was previously closed so it must be opening now
          s = STATUS_OPENING;
      }
      else if (_lastDoorClosed==SWITCH_OPENED && _lastDoorOpened==SWITCH_CLOSED){
          // The door was previously opened so it must be closing now
          s = STATUS_CLOSING;
      }
  }
  if (s == STATUS_CLOSED || s == STATUS_OPENED)
  {
      _lastDoorClosed = doorClosed;
      _lastDoorOpened = doorOpened;
  }

  boolean openedLED = false;
  boolean closedLED = false;

  // Determine how to set the LED status
  if (s == STATUS_OPENED){
      openedLED = HIGH;
      closedLED = LOW;
  }
  else if (s == STATUS_CLOSED){
      openedLED = LOW;
      closedLED = HIGH;
  }
  else if (s == STATUS_OPENING){
      openedLED= !digitalRead(_openLEDPin);
      closedLED = LOW;
  }
  else if (s == STATUS_CLOSING){
      openedLED= LOW;
      closedLED = !digitalRead(_closeLEDPin);
  }
  else if (s == STATUS_UNKNOWN){
      openedLED= !digitalRead(_openLEDPin);
      closedLED = !digitalRead(_closeLEDPin);
  }

  digitalWrite(_openLEDPin,openedLED);
  digitalWrite(_closeLEDPin,closedLED);

  return s;
}
boolean GarageDoor::Close(){
  boolean success;
  if (readStatus()!=STATUS_CLOSED)
  {
    PushGarageDoorButton();
    success=true;
  }
  return success;
}
boolean GarageDoor::Open(){
  boolean success;
  if (readStatus()!=STATUS_OPENED)
  {
    PushGarageDoorButton();
    success=true;
  }
  return success;
}
void GarageDoor::PushGarageDoorButton()
{
    digitalWrite(_relayPin,HIGH);
    delay(1000);
    digitalWrite(_relayPin,LOW);
}
