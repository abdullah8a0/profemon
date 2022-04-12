#include "util.h"

Button::Button(int p)
{
    flag = 0;
    state = S0;
    this->pin = p;
    S2_start_time = millis();      // init
    button_change_time = millis(); // init
    debounce_duration = DEBOUNCE_DURATION;
    long_press_duration = LONG_PRESS_DURATION;
    button_pressed = 0;
    // d = (char *)malloc(sizeof(char) * 10);
    // sprintf(d, "%d", 1);
}
void Button::read()
{
    Serial.print("here 3");
    int button_val = digitalRead(pin);
    button_pressed = !button_val; // invert button
    Serial.print("pin: ");
    Serial.print(pin);
    Serial.print(" button_pressed: ");
    Serial.println(button_pressed);
}

int Button::update()
{
    Serial.print("d: ");
    Serial.println(d);
    delay(2000);
    read();
    flag = 0;

    if (state == S0)
    {
        if (button_pressed)
        {
            state = S1;
            button_change_time = millis();
        }
    }
    else if (state == S1)
    {
        if (button_pressed)
        {
            if (millis() - button_change_time >= debounce_duration)
            {
                state = S2;
                S2_start_time = millis();
            }
        }
        else
        {
            state = S0;
            button_change_time = millis();
        }
    }
    else if (state == S2)
    {
        if (button_pressed)
        {
            if (millis() - S2_start_time >= long_press_duration)
            {
                state = S3;
            }
        }
        else
        {
            state = S4;
            button_change_time = millis();
        }
    }
    else if (state == S3)
    {
        if (button_pressed)
        {
        }
        else
        {
            state = S4;
            button_change_time = millis();
        }
    }
    else if (state == S4)
    {
        if (button_pressed)
        {
            button_change_time = millis();
            if (millis() - S2_start_time < long_press_duration)
            {
                state = S2;
            }
            else
            {
                state = S3;
            }
        }
        else
        {
            if (millis() - button_change_time >= debounce_duration)
            {
                state = S0;
                if (millis() - S2_start_time < long_press_duration)
                {
                    flag = 1;
                }
                else
                {
                    flag = 2;
                }
            }
        }
    }
    return flag;
}

// Joystick::Joystick(int VRx, int VRy, int Sw, int button_mode)
// {
// }

void Joystick::read()
{
    int raw_VRx_val = analogRead(VRx);
    int raw_VRy_val = analogRead(VRy);
    VRx_val = map(raw_VRx_val, 0, 1023, -512, 512);
    VRy_val = map(raw_VRy_val, 0, 1023, -512, 512);

    if (button_mode == 1)
    {
        int button_pressed = !digitalRead(Sw);
        if (button_pressed > previous_button_pressed)
        {
            Sw_val = 1;
        }
        else if (button_pressed < previous_button_pressed)
        {
            Sw_val = -1;
        }
        else
        {
            Sw_val = 0;
        }
        previous_button_pressed = button_pressed;
    }
    else if (button_mode == 0)
    {
        Serial.print("here 2");
        Sw_val = button.update();
    }
}
/**
 * @brief get the direction of the Joystick, returns NONE if no direction.
 *  The possible Directions are: NONE, UP, DOWN, LEFT, RIGHT
 *  The joystick must be outside of the deadzone for this to return a direction.
 *  VRx_val and VRy_val are the current x and y coordinates of the joystick.
 *

 *  LEFT = x < -JOYSTICK_DEADZONE and abs(y) < JOYSTICK_DEADZONE
 *
 * @return the direction of the Joystick returns nones on corner cases.
 */
joystick_direction Joystick::update()
{
    Serial.print("here 1");
    read();
    if (abs(VRx_val) < JOYSTICK_DEADZONE && abs(VRy_val) < JOYSTICK_DEADZONE)
    {
        return NONE;
    }
    else if (VRx_val < -JOYSTICK_DEADZONE && abs(VRy_val) < JOYSTICK_DEADZONE)
    {
        return LEFT;
    }
    else if (VRx_val > JOYSTICK_DEADZONE && abs(VRy_val) < JOYSTICK_DEADZONE)
    {
        return RIGHT;
    }
    else if (VRy_val < -JOYSTICK_DEADZONE && abs(VRx_val) < JOYSTICK_DEADZONE)
    {
        return DOWN;
    }
    else if (VRy_val > JOYSTICK_DEADZONE && abs(VRx_val) < JOYSTICK_DEADZONE)
    {
        return UP;
    }
    else
    {
        return NONE;
    }
}