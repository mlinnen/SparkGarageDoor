#include "dht.h"
#define DHTPIN D2    // Digital pin D2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#include "GarageDoor.h"
GarageDoor garageDoor(D3,D4,D5,D0,D1);

int currentStatus;
int lastStatus;
double currentTemp;
double currentHumidity;
double currentHeatIndex;

void setup() {

    currentStatus = 0;
    lastStatus = 0;

    dht.begin();

    garageDoor.begin();

    // Register functions
    Spark.function("command", garageDoorCommand);

    // Register variables
    Spark.variable("doorstatus",&currentStatus,INT);
    Spark.variable("temp",&currentTemp,DOUBLE);
    Spark.variable("humidity",&currentHumidity,DOUBLE);
    Spark.variable("heatIndex",&currentHeatIndex,DOUBLE);

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
