#include "dht.h"
#define DHTPIN D2    // Digital pin D2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#include "GarageDoor.h"
GarageDoor garageDoor(D3,D4,D5);

#define DOOR_OPENED_LED D0
#define DOOR_CLOSED_LED D1

int lastDoorOpened = 0;
int lastDoorClosed = 0;
int doorClosed = 0;
int doorOpened = 0;

boolean lastOpenedLED = false;
boolean lastClosedLED = false;

const int STATUS_UNKNOWN = 0;
const int STATUS_OPENED = 1;
const int STATUS_CLOSED = 2;
const int STATUS_OPENING = 3;
const int STATUS_CLOSING = 4;

int currentStatus;
int lastStatus;
double currentTemp;
double currentHumidity;
double currentHeatIndex;

void setup() {

    currentStatus = 0;
    lastStatus = 0;

    // Setup the pins
    // Door Status LEDs
    pinMode(DOOR_OPENED_LED, OUTPUT);
    pinMode(DOOR_CLOSED_LED, OUTPUT);

    dht.begin();

    garageDoor.begin();

    // Register functions
    Spark.function("command", garageDoorCommand);

    // Register variables
    Spark.variable("status",&currentStatus,INT);
    Spark.variable("opensensor",&doorClosed,INT);
    Spark.variable("closesensor",&doorOpened,INT);
    Spark.variable("temp",&currentTemp,DOUBLE);
    Spark.variable("humidity",&currentHumidity,DOUBLE);
    Spark.variable("hi",&currentHeatIndex,DOUBLE);

    // Register subscriptions
    // Look for the good night command

}

void loop() {

    currentTemp = dht.readTemperature(true);
    currentHumidity = dht.readHumidity();
    currentHeatIndex = dht.computeHeatIndex(currentTemp, currentHumidity);

    currentStatus = garageDoor.readStatus();

    if (currentStatus != lastStatus){
        // Notify other devices that the door changed state
        String data = String(currentStatus);
        Spark.publish("garage-door-status", data, 60, PRIVATE);
    }

    lastStatus = currentStatus;

    boolean openedLED = false;
    boolean closedLED = false;

    // Determine how to set the LED status
    if (currentStatus == STATUS_OPENED){
        openedLED = LOW;
        closedLED = HIGH;
    }
    else if (currentStatus == STATUS_CLOSED){
        openedLED = HIGH;
        closedLED = LOW;
    }
    else if (currentStatus == STATUS_OPENING){
        openedLED= !lastOpenedLED;
        closedLED = HIGH;
    }
    else if (currentStatus == STATUS_CLOSING){
        openedLED= HIGH;
        closedLED = !lastClosedLED;
    }
    else if (currentStatus == STATUS_UNKNOWN){
        openedLED= !lastOpenedLED;
        closedLED = !lastClosedLED;
    }

    digitalWrite(DOOR_OPENED_LED,openedLED);
    digitalWrite(DOOR_CLOSED_LED,closedLED);

    lastOpenedLED = openedLED;
    lastClosedLED = closedLED;

    delay(1000);
}

int garageDoorCommand(String command)
{
    if(command == "open")
    {
        if (garageDoor.Open()==true)
        {
          return 1;
        }
        return 0;
    }
    else if(command =="close")
    {
        if (garageDoor.Close()==true)
        {
          return 1;
        }
        return 0;
    }
    else return -1;
}
