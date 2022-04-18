#include "util.h"

Button::Button(int p)
{
    flag = 0;
    state = S0;
    pin = p;
    S2_start_time = millis();      // init
    button_change_time = millis(); // init
    debounce_duration = DEBOUNCE_DURATION;
    long_press_duration = LONG_PRESS_DURATION;
    button_pressed = 0;
}
void Button::read()
{
    int button_val = digitalRead(pin);
    button_pressed = !button_val; // invert button
}

int Button::update()
{
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

void Joystick::read()
{
    VRx_val = map(analogRead(_VRx), 0, 1023, -512, 512);
    VRy_val = map(analogRead(_VRy), 0, 1023, -512, 512);
    if (_button_mode == 1)
    {
        int button_pressed = !digitalRead(_Sw);
        if (button_pressed > previous_button_pressed_time)
        {
            Sw_val = 1;
        }
        else if (button_pressed < previous_button_pressed_time)
        {
            Sw_val = -1;
        }
        else
        {
            Sw_val = 0;
        }
        previous_button_pressed_time = button_pressed;
    }
    else if (_button_mode == 0)
    {
        Sw_val = button.update();
    }
}
/**
 * @brief get the direction of the Joystick, returns NONE if no direction.
 *  The possible Directions are: NONE, UP, DOWN, LEFT, RIGHT, UP_LEFT, UP_RIGHT, DOWN_LEFT, DOWN_RIGHT
 *  The joystick must be outside of the deadzone for this to return a direction.
 *  VRx_val and VRy_val are the current x and y coordinates of the joystick.
 *
 *  LEFT = x < -JOYSTICK_DEADZONE  and abs(y) < JOYSTICK_DEADZONE
 *
 * @return the direction of the Joystick
 */
joystick_direction Joystick::update()
{

    read();
    // Serial.print("VRx_val: ");
    // Serial.print(VRx_val);
    // Serial.print(" VRy_val: ");
    // Serial.print(VRy_val);
    // Serial.print(" Sw_val: ");
    // Serial.println(Sw_val);

    joystick_direction dir;

    if (abs(VRx_val) < JOYSTICK_DEADZONE && abs(VRy_val) < JOYSTICK_DEADZONE)
    {
        dir = NONE;
    }
    else if (VRx_val < -JOYSTICK_DEADZONE && abs(VRy_val) < JOYSTICK_DEADZONE)
    {
        dir = JOYSTICK_RIGHT;
    }
    else if (VRx_val > JOYSTICK_DEADZONE && abs(VRy_val) < JOYSTICK_DEADZONE)
    {
        dir = JOYSTICK_LEFT;
    }
    else if (VRy_val < -JOYSTICK_DEADZONE && abs(VRx_val) < JOYSTICK_DEADZONE)
    {
        dir = JOYSTICK_DOWN;
    }
    else if (VRy_val > JOYSTICK_DEADZONE && abs(VRx_val) < JOYSTICK_DEADZONE)
    {
        dir = JOYSTICK_UP;
    }
    else
    {
        dir = NONE;
    }

    if (_joystick_mode == 1)
    {

        if (millis() - previous_joystick_direction_output_time < JOYSTICK_UPDATE_DELAY)
        {
            return NONE;
        }
        else
        {
            previous_joystick_direction_output_time = millis();
            return dir;
        }
    }
    else if (_joystick_mode == 0)
    {
        return dir;
    }
}
