#include "Arduino.h"
#include "Button.h"

Button::Button(int p) {
  flag = 0;
  state = S0;
  pin = p;
  S2_start_time = millis(); //init
  button_change_time = millis(); //init
  debounce_duration = 10;
  long_press_duration = 1000;
  button_pressed = 0;
}

void Button::read() {
  uint8_t button_val = digitalRead(pin);
  button_pressed = !button_val; //invert button
}

int Button::update() {
  Button::read();
  flag = 0;
  if (state==0) {
    if (button_pressed) {
      state = S1;
      button_change_time = millis();
    }

  } else if (state==1) {
    if (button_pressed && millis()-button_change_time >= debounce_duration){
      state = S2;
      S2_start_time = millis();
    } else if (!button_pressed){
      state = S0;
      button_change_time = millis();
    }

  } else if (state==2) {
    if (button_pressed && millis()-S2_start_time >= long_press_duration){
      state = S3;
    } else if (!button_pressed){
      state = S4;
      button_change_time = millis();
    }

  } else if (state==3) {
    if (!button_pressed){
      state = S4;
      button_change_time = millis();
    }
  } else if (state==4) {
    if (!button_pressed && millis()-button_change_time >= debounce_duration){
      if (button_change_time - S2_start_time < long_press_duration) {
        flag = 1; } else {flag = 2;}
      state = S0;
    } else if (button_pressed && millis()-S2_start_time < long_press_duration){
      state = S2;
      button_change_time = millis();
    } else if (button_pressed && millis()-S2_start_time >= long_press_duration){
      state = S3;
      button_change_time = millis();
    }
  }
  return flag;
}