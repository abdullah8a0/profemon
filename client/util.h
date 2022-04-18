#ifndef util_h
#define util_h
#include "Arduino.h"

#define LONG_PRESS_DURATION 1000
#define DEBOUNCE_DURATION 10
#define JOYSTICK_DEADZONE 400
#define JOYSTICK_UPDATE_DELAY 500

enum joystick_direction
{
    NONE,
    JOYSTICK_UP,
    JOYSTICK_DOWN,
    JOYSTICK_LEFT,
    JOYSTICK_RIGHT,
};

enum button_state
{
    S0,
    S1,
    S2,
    S3,
    S4
};

/**
 * @brief Button class, just call update() to read the button state
 *   the result is stored in the flag variable.
 *  flag = 0 means no reading
 *  flag = 1 means a short press happened
 *  flag = 2 means a long press happened (>1 second)
 *
 *
 * edit
 * edit again
 *  The button is debounced by a 10ms delay.
 *  Most of the time, you only care to check if flag > 0.
 *
 * @param p pin number, itialize its mode in setup()
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
    char *d;

public:
    int update();
    Button(int p);
    int flag;
};

/**
 * @brief a class for interfacing with a joystick on an arduino, need to initialize the pins yourself
 *        outside the constructor
 *
 *  example :
 *
 *  Joystick joystick(VRx, VRy, Sw, 0, 1);
 *
 *  // in setup()
 *
 *  analogReadeResolution(10);
 *  pinMode(VRx, INPUT);
 *  pinMode(VRy, INPUT);
 *  pinMode(Sw, INPUT_PULLUP);
 *
 *  // in loop()
 *
 *  joystick_direction dir = joystick.update();
 *  int flag = joystick.Sw_val;
 *
 *
 *
 * @param VRx the pin the joystick is connected to for the x-axis
 * @param VRy the pin the joystick is connected to for the y-axis
 * @param Sw the pin the joystick is connected to for the button
 * @param button_mode if 1, button will return 1 during rising edge, -1 during falling edge
 *                    if 0, button is a normal button
 * @param joystick_mode if 1, joystick will return direction every JOYSTICK_DELAY ms
 *                     if 0, joystick is a normal joystick
 */
class Joystick
{
    int _VRx;
    int _VRy;
    int _Sw;
    int VRx_val;
    int VRy_val;
    int _button_mode;
    int _joystick_mode;
    int previous_button_pressed_time = 0;
    int previous_joystick_direction_output_time = 0;

    Button button;
    void read();

public:
    Joystick(int VRx, int VRy, int Sw, int button_mode, int joystick_mode) : _VRx(VRx),
                                                                             _VRy(VRy),
                                                                             _Sw(Sw),
                                                                             VRx_val(0),
                                                                             VRy_val(0),
                                                                             previous_button_pressed_time(0),
                                                                             previous_joystick_direction_output_time(0),
                                                                             _button_mode(button_mode),
                                                                             _joystick_mode(joystick_mode),
                                                                             button(Sw){};

    joystick_direction update();
    joystick_direction direction;
    int Sw_val;
};

#endif