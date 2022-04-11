#ifndef util_h
#define util_h
#include "Arduino.h"

#define LONG_PRESS_DURATION 1000
#define DEBOUNCE_DURATION 10
#define JOYSTICK_DEADZONE 200

enum button_state
{
    S0,
    S1,
    S2,
    S3,
    S4
};

/**
 * @brief Button class
 * @param p pin number, itialize it in setup()
 */
class Button
{
    int S2_start_time;
    int button_change_time;
    int debounce_duration;
    int long_press_duration;
    int pin;
    int button_pressed;
    button_state state;
    void read();
    int update();

public:
    Button(int p);
    int flag;
};

enum joystick_direction
{
    NONE,
    UP,
    DOWN,
    LEFT,
    RIGHT,
    UP_LEFT,
    UP_RIGHT,
    DOWN_LEFT,
    DOWN_RIGHT
};

/**
 * @brief a class for interfacing with a joystick on an arduino, need to initialize the pins yourself
 *        outside the constructor
 *
 * @param VRx the pin the joystick is connected to for the x-axis
 * @param VRy the pin the joystick is connected to for the y-axis
 * @param Sw the pin the joystick is connected to for the button
 * @param button_mode if 1, button will return 1 during rising edge, -1 during falling edge
 *                    if 0, button is a normal button
 */
class Joystick
{
    Joystick(int VRx, int VRy, int Sw, int button_mode);
    int VRx;
    int VRy;
    int Sw;
    int VRx_val;
    int VRy_val;
    int Sw_val;
    int button_mode;
    int previous_button_pressed = 0;

    Button button;
    void read();
    joystick_direction update();

    joystick_direction direction;
};

#endif