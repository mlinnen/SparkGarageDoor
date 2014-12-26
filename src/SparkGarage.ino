
int DOOR_OPENED_LED = D0;
int DOOR_CLOSED_LED = D1;
int DOOR_OPENED_SENSOR = D3;
int DOOR_CLOSED_SENSOR = D4;
int DOOR_RELAY = D5;

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

void setup() {

    currentStatus = 0;
    lastStatus = 0;

    // Setup the pins
    // Door Status LEDs
    pinMode(DOOR_OPENED_LED, OUTPUT);
    pinMode(DOOR_CLOSED_LED, OUTPUT);
    pinMode(DOOR_RELAY, OUTPUT);
    digitalWrite(DOOR_RELAY,LOW);

    // Door Sensors
    pinMode(DOOR_OPENED_SENSOR,INPUT_PULLUP);
    pinMode(DOOR_CLOSED_SENSOR,INPUT_PULLUP);

    // Register functions
    Spark.function("command", garageDoorCommand);

    // Register variables
    Spark.variable("status",&currentStatus,INT);
    Spark.variable("opensensor",&doorClosed,INT);
    Spark.variable("closesensor",&doorOpened,INT);

    // Register subscriptions
    // Look for the good night command

}

void loop() {

    // Read the current state of the sensors
    doorClosed = digitalRead(DOOR_CLOSED_SENSOR);
    doorOpened = digitalRead(DOOR_OPENED_SENSOR);
    // TODO should this be debounced?

    if (doorClosed==0 && doorOpened==0)
    {
        // Both sensors are closed and this is an unknown state
        currentStatus = STATUS_UNKNOWN;
    }
    else if(doorClosed==0 && doorOpened==1)
    {
        // The door appears to be fully closed
        currentStatus = STATUS_CLOSED;
    }
    else if(doorClosed==1 && doorOpened==0)
    {
        // The door appears to be fully opened
        currentStatus = STATUS_OPENED;
    }
    else if(doorClosed==1 && doorOpened==1)
    {
        if (lastDoorClosed==0 && lastDoorOpened==1){
            // The door was previously closed so it must be opening now
            currentStatus = STATUS_OPENING;
        }
        else if (lastDoorClosed==1 && lastDoorOpened==0){
            // The door was previously opened so it must be closing now
            currentStatus = STATUS_CLOSING;
        }
    }

    if (currentStatus != lastStatus){
        // Notify other devices that the door changed state
        String data = String(currentStatus);
        Spark.publish("garage-door-status", data, 60, PRIVATE);
    }

    lastDoorClosed = doorClosed;
    lastDoorOpened = doorOpened;
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
        // Open garage door if it is closed
        if (currentStatus == STATUS_CLOSED)
        {
            PushGarageDoorButton();
            return 1;
        }
        return 0;
    }
    else if(command =="close")
    {
        // Closes garage door if it is open
        if (currentStatus == STATUS_OPENED)
        {
            PushGarageDoorButton();
            return 1;
        }
        return 0;
    }
    else return -1;
}

void PushGarageDoorButton()
{
    digitalWrite(DOOR_RELAY,HIGH);
    delay(1000);
    digitalWrite(DOOR_RELAY,LOW);
}
