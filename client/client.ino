#include "util.h"
int VRx = 5; // 5 pin
int VRy = 1; // 1 pin
int Sw = 12; // 12 pin
Joystick joystick(VRx, VRy, Sw, 0);
void setup()
{
    Serial.begin(9600);
    Serial.println("setup");
    // initialize the joystick
    pinMode(VRx, INPUT);
    pinMode(VRy, INPUT);
    pinMode(Sw, INPUT_PULLUP);
    analogReadResolution(10);
}

// print the direction of the joystick
void loop()
{
    Serial.println("here 0");
    joystick_direction d = joystick.update();
    // Serial.print("direction: ");
    switch (d)
    {
    case NONE:
        // Serial.println("NONE");
        break;
    case UP:
        Serial.println("UP");
        break;
    case DOWN:
        Serial.println("DOWN");
        break;
    case LEFT:
        Serial.println("LEFT");
        break;
    case RIGHT:
        Serial.println("RIGHT");
        break;
    }
    // Serial.print("Sw: ");
    // Serial.println(joystick.Sw_val);
    // delay(500);
}