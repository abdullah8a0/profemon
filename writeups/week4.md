# Week 3 Deliverables

## Abdullah
### Assuming dual station mode works, rewrite parts of code that depend on single station mode so that pairing is less buggy.

## Ivy
### Present Profeball example with electronics plates mounted inside and wiring to modules. Try to get the second one done barring issues with the 3D printer filament getting tangled in the middle of the night >:( 
![Assembled Profeball Tour](https://www.youtube.com/watch?v=zVC25HnnGds)

Unfortunately the second one hasn't been assembled, although all the parts are printed and plates are laser cut. I only had one ESP since my teammates needed theirs to test still, and it took a few hours to put together the first profeball between the snipping and soldering. I assume the second one would go faster now that I have had trial and error. 

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