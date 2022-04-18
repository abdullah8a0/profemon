#include <SPI.h>
#include <TFT_eSPI.h>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include "util.h"

WiFiClientSecure client; // global WiFiClient Secure object
WiFiClient client2;      // global WiFiClient Secure object

const char NETWORK[] = "MIT";
const char PASSWORD[] = "";

uint8_t channel = 1;                                 // network channel on 2.4 GHz
byte bssid[] = {0x04, 0x95, 0xE6, 0xAE, 0xDB, 0x41}; // 6 byte MAC address of AP you're targeting.

int VRx = 5; // 5 pin
int VRy = 1; // 1 pin
int Sw = 12; // 12 pin
Joystick joystick(VRx, VRy, Sw, 0, 1);
void setup()
{
  Serial.begin(115200);
  Serial.println("Starting");
  while (!Serial)
    ;
  pinMode(VRx, INPUT);
  pinMode(VRy, INPUT);
  pinMode(Sw, INPUT_PULLUP);
  analogReadResolution(10);
}

void loop()
{
  joystick_direction dir = joystick.update();
  if (dir == JOYSTICK_UP)
  {
    Serial.println("UP");
  }
  else if (dir == JOYSTICK_DOWN)
  {
    Serial.println("DOWN");
  }
  else if (dir == JOYSTICK_LEFT)
  {
    Serial.println("LEFT");
  }
  else if (dir == JOYSTICK_RIGHT)
  {
    Serial.println("RIGHT");
  }
  else
  {
    Serial.println("NONE");
  }
  delay(100);
}
