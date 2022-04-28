# Week 3 Deliverables

## Abdullah

## Ivy
### Profeball shell printed
![Profeball!](https://s7.gifyu.com/images/profeball.gif)

[Youtube Profeball! Link](https://youtube.com/shorts/HkVvfpYBmuw?feature=share) (in case above gif doesn't work)

### Progress made on electronics mountings (cad drawings, dxf files showing plans).
I dimensioned the holes and outermost plates for either side of the Profeball. Because of that, it's now very easy to make adjustments to where we want those electronics placed if we want to move them/how big the plates are/make new plates. 

I made room for the RC522 wiring to go to the bottom half of the shell. Plus room for the clip of the Pokeball on both plates. The top plate is actually a bit smaller (120mm) than the bottom plate (134mm) because it sits a little inside the top of the shell to avoid colliding with the ST7735. There's a hole in it because the joystick also collides with the plate a little. 

![Top Outside Plate](https://i.imgur.com/foPB1xh.png)

The bottom plate has room for the screen and joystick to be attached. I'm not sure yet if the joystick will be mounted from the top or bottom side, I need to see what thickness of arylic we have. If it is too thick, it will collide with the pins.

![Bottom Outside Plate](https://i.imgur.com/hFOAu6u.png)

I tested the dimensions by printing on a 1:1 scale on paper and cutting it out. For the laser cut version, I will have to sand down the outside edges so they're curved if the material is thick. You can see the size difference I mentioned earlier here.

![Cutouts for Paper](https://i.imgur.com/rK5Wxhp.png)

I'm thinking of wanting to frost or paint the arylic so you can't see the electronics inside, but I need to lasercut some non-paper prototypes first.

## Andi & Heidi
[demo video](https://youtu.be/xul82FiG2Hg)

Client side:
- we smoothed out the transition between different battle stages and made sure that all the ui elements display correctly, e.g., images stay on the screen, HP bars update properly, etc.
- We improved the joystick class so that now if the joystick is held down in the same direction for a period of time, the device would consider that as multiple presses in that direction spaced out by a certain amount of time.

Server side:
- Updated the damage calculation formula from using random numbers to using the actual pokemon game formula taking consideration of the power and accuracy of the moves, and the attack and defense of the profemons.
- Fixed some synchronization issues with the server and client.
- Refactored the code to make it more readable and modular.