# Week 3 Deliverables

## Abdullah
### Assuming dual station mode works, rewrite parts of code that depend on single station mode so that pairing is less buggy.

## Ivy
### Present Profeball example with electronics plates mounted inside and wiring to modules. Try to get the second one done barring issues with the 3D printer filament getting tangled in the middle of the night >:( 
![Assembled Profeball Tour](https://www.youtube.com/watch?v=zVC25HnnGds)

Unfortunately the second one hasn't been assembled, although all the parts are printed and plates are laser cut. I did indeed have issues with the filament getting tangled in the middle of the night and coming in in the morning to a spagetti mess of filament.

I also only had one ESP since my teammates needed theirs to test still, and it took quite a few hours to put together the first profeball between the snipping and soldering and dremeling (on the shell for the battery pack to fit). I assume the second one would go faster now that I have had trial and error. 

![Parts for Both Profeballs](https://imgur.com/a/B2M47RF)

Figuring out a way to mount the electronics and feed them through and keep them from getting tangled was much more of a challenge than I initially expected. Especially because the wires loved to come back out of the breadboard holes after I moved the part it was connected to. I ended up putting a dab of hot glue to secure them in place. I would like to try this with a solderable breadboard, for the sake of the wires staying where I want them to be and the breadboard requiring less volume. Unfortuantely, for the same reason I didn't use superglue on this, I would struggle to return an esp in pristine condition.
    
### Show more Profemon created in the server.
At Joe's suggestion, we went ahead and expanded the Profemon databse with some "fake" Profemon (Profemon not associated with real IDs). So now the table in profemon.db looks like. All the Profemon below TestSubject are new. Their moves were also updated in the other databse and they have images in the server.

| cipher      | name              | hp  | atk | def | spatk | spdef | spd | move1        | move2        | move3         | move4       |
|-------------|-------------------|-----|-----|-----|-------|-------|-----|--------------|--------------|---------------|-------------|
| 0E 14 4B A6 | JoeSteinmeyer     | 108 | 68  | 53  | 63    | 63    | 103 | Double Team  | Volt Switch  | Signal Beam   | Confuse     |
| 0E 3E 4B A6 | AdamHartz         | 128 | 83  | 68  | 53    | 68    | 98  | Fury Swipes  | Astonish     | Recurse       | Bind        |
| 1E 7F 71 A6 | PattiChristie     | 93  | 23  | 243 | 23    | 243   | 18  | Hyper Voice  | Sludge Bomb  | Absolute Zero | Confuse     |
| 43 81 40 86 | SaraEllison       | 133 | 68  | 68  | 58    | 58    | 73  | Future Sight | Confuse      | Optimize      | Clear Fog   |
| 8D BB 6D D5 | MohamedAbdelhafez | 113 | 68  | 93  | 48    | 73    | 43  | Gravity      | Flash Cannon | Magnet Beam   | Confuse     |
| 93 41 30 A7 | TestSubject       | 1   | 1   | 1   | 1     | 1     | 1   | Confuse      | Astonish     | Optimize      | Clear Fog   |
| 75 12 B5 82 | EricLander        | 121 | 51  | 56  | 71    | 91    | 76  | Helix Punch  | Sequence     | Tri-Attack    | Clear Fog   |
| I2 03 L4 87 | ErikDemaine       | 123 | 48  | 58  | 133   | 68    | 38  | Fold         | Unfold       | Atonish       | Optimize    |
| 93 30 48 H2 | JeffGrossman      | 118 | 62  | 62  | 78    | 78    | 58  | Solar Beam   | Brine        | Muddy Water   | Sunny Day   |
| 0E H4 93 A1 | SimonaSocrate     | 143 | 93  | 63  | 48    | 48    | 48  | Torsion      | Stress       | Strain        | Axial Load  |
| K8 23 LF 12 | GilbertStrang     | 93  | 38  | 58  | 83    | 103   | 73  | Mirror Force | Invert       | Reduce        | Diagonalize |
## Andi & Heidi
### Show video battle being stepped through with sound effect.

![Battle with sound](https://www.youtube.com/watch?v=iJIVGkFIWJU)

After trying the MP3, we realized it didn't give us the retro sounds we wanted and was very bulky. So we went with using the buzzer. We made riffs that play after the player moves the joysticks and throughout the battle sequence based off the sounds in Pokemon.

```cpp
// riffs
int8_t capture_riff[] = {9, 9, 9, 99, 5, 5, 5, 99, 0, 0, 0, 0, 0, 0, 0, 99, 10, 99, 10, 99, 10, 99, 7, 7, 7, 99, 10, 99, 9, 9, 9, 9, 9, 9, 9, 99};
int capture_riff_length = sizeof capture_riff / sizeof capture_riff[0];
double capture_duration = 94;

int8_t direction_riff[] = {-4, 99};
int direction_riff_length = 2;
double direction_duration = 50;

int8_t select_riff[] = {23, 28, 99};
int select_riff_length = 3;
double select_duration = 50;

int8_t miss_riff[] = {19, 18, 17, 16, 16, 16, 99};
int miss_riff_length = sizeof miss_riff / sizeof miss_riff[0];
double miss_duration = 120;

int8_t attack_riff[] = {14, 18, 99};
int attack_riff_length = sizeof attack_riff / sizeof attack_riff[0];
double attack_duration = 120;

int8_t faint_riff[] = {14, 14, 10, 99, 14, 14, 10, 10, 10, 10, 10, 99};
int faint_riff_length = sizeof faint_riff / sizeof faint_riff[0];
double faint_duration = 100;

int8_t win_riff[] = {16, 18, 20, 21, 23, 23, 27, 27, 28, 28, 27, 27, 28, 28, 28, 28, 28, 28, 28, 28, 28, 99};
int win_riff_length = sizeof win_riff / sizeof win_riff[0];
double win_duration = 80;

int8_t lose_riff[] = {5, 5, 5, 5, 6, 6, 5, 5, 5, 5, 0, 0, 3, 3, 3, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 99};
int lose_riff_length = sizeof lose_riff / sizeof lose_riff[0];
double lose_duration = 80;
```