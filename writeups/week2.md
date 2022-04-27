# Week 2 Deliverables

## Everyone
### Show an integrated product in a video which can capture profemons and step through a game.
!(Week 2 Everyone)[https://youtu.be/0OERUU5kmrU]

## Abdullah & Andi & Ivy
  ### Use the dual AP/Station mode in the ESP32 to speed up the handshake by concurrency, integrate with serverside code
  * The handshake and pairing process is entirely finished on the client along with the server side code to support this. We spent a lot of time integrating this with the UI and server code. The game ID is generated using the UNIX and user IDs.
  ### Put images on server and enable image retrieval. 
  *  We uploaded all of our images to the server, and then resize them as needed based on whether we are displaying for selection with `jpg2bin()` or for the battle displa with `jpg2bin_small()`. After we resize it, we convert the binary representation of the jpeg into base64 send it to the esp32.
```python
def jpg2bin(jpg_file, width=96, height=120):
    img = Image.open(jpg_file)
    img = img.resize((width, height))
    output = io.BytesIO()
    img.save(output, format='JPEG')
    return output.getvalue()

def jpg2bin_small(jpg_file, width=32, height=40):
    img = Image.open(jpg_file)
    img = img.resize((width, height))
    output = io.BytesIO()
    img.save(output, format='JPEG')
    return output.getvalue()

def get_profemon_image(name, size = 'big'):
    file_name = '{}.jpg'.format(name)
    file_path = os.path.join(dir_path,file_name)
    binary = jpg2bin(file_path)
    if size == 'small':
        binary = jpg2bin_small(file_path)
    encodedbin = base64.standard_b64encode(binary)
    encodedbin = encodedbin.decode('ascii')
    return len(binary), encodedbin
```
  ### Determining game mechanics amd create rudimentary battle stages on server for stepping through the game and returning the results to the devices. 
  * Shown below is a FSM of battle steps since the code is too long to stuff into the writeup. 
  1. A game is initated by a user sending a POST request with their user_id, a mutually agreed upon game_id, and their profemon name. Then their opponent must do the same. Once that happens, a game has begun on the server side.
  2. Next, for the client to display the start state, both clients will send a GET request to the server to obtain the smaller Profemon images and their max HP. Now the battle is initatied on the client side.
  3. Now both sides can select their moves and their ESPs will send POST requests to the server. After, the client will repeatedly query the server with GET requests until the server sends back the json file for the updated display. Meanwhile, the server will wait for both moves to be recieved before calculating damage and prepare two seperate json files to send to both players. There are two seperate files because we want both players to be able to view the battle from their own perspective. 
  4. Then the third step will repeat until one of the profemon faints (hp <= 0). When this happens, the server will indicate this to the esps and clear the game data.
  
![Battle_FSM](https://i.imgur.com/Ca6OkfV.png)
  
  ### Designing Profemon moves/stats/etc.
  * Profesors/instructors were associated with a pokemon, then assigned stats according to those pokemons' stats distributions if they were at Level 50 in game. Hence, every profemon is now unique in that sense. We wanted the Profemons to all be approximately the same strength, so we did adjust the stats a little as needed. We also made moves for each Profemon, borrowing from existing names and making up own based on puns and uploaded those to a databse in the server. We then uploaded these tables to a database in our server to use for calculating damage.
  
| cipher      | name              | hp  | atk | def | spatk | spdef | spd | move1        | move2        | move3         | move4     |
|-------------|-------------------|-----|-----|-----|-------|-------|-----|--------------|--------------|---------------|-----------|
| 0E 14 4B A6 | JoeSteinmeyer     | 108 | 68  | 53  | 63    | 63    | 103 | Double Team  | Volt Switch  | Signal Beam   | Confuse   |
| 0E 3E 4B A6 | AdamHartz         | 128 | 83  | 68  | 53    | 68    | 98  | Fury Swipes  | Astonish     | Recurse       | Bind      |
| 1E 7F 71 A6 | PattiChristie     | 93  | 23  | 243 | 23    | 243   | 18  | Hyper Voice  | Sludge Bomb  | Absolute Zero | Confuse   |
| 43 81 40 86 | SaraEllison       | 133 | 68  | 68  | 58    | 58    | 73  | Future Sight | Confuse      | Optimize      | Clear Fog |
| 8D BB 6D D5 | MohamedAbdelhafez | 113 | 68  | 93  | 48    | 73    | 43  | Gravity      | Flash Cannon | Magnet Beam   | Confuse   |
| 93 41 30 A7 | TestSubject       | 1   | 1   | 1   | 1     | 1     | 1   | Confuse      | Astonish     | Optimize      | Clear Fog |


| move_name     | phy_or_spec | type     | pow | acc  |
|---------------|-------------|----------|-----|------|
| Fury Swipes   | phy         | normal   | 60  | 0.95 |
| Astonish      | spec        | psychic  | 50  | 0.95 |
| Recurse       | spec        | psychic  | 50  | 0.95 |
| Bind          | spec        | normal   | 50  | 0.95 |
| Double Team   | phy         | normal   | 50  | 0.95 |
| Volt Switch   | spec        | electric | 70  | 0.95 |
| Signal Beam   | spec        | steel    | 75  | 0.95 |
| Gravity       | spec        | psychic  | 90  | 0.95 |
| Flash Cannon  | spec        | steel    | 80  | 0.95 |
| Magnet Beam   | spec        | steel    | 60  | 0.95 |
| Hyper Voice   | spec        | normal   | 90  | 0.95 |
| Sludge Bomb   | phy         | poison   | 90  | 0.95 |
| Absolute Zero | spec        | ice      | 300 | 0.30 |
| Confuse       | spec        | psychic  | 65  | 0.95 |
| Future Sight  | spec        | psychic  | 70  | 0.95 |
| Optimize      | physical    | normal   | 60  | 0.95 |
| Clear Fog     | spec        | poison   | 50  | 0.95 |

  ### Show battle being stepped through. 
    * See video
  ### Try to get HP bar display working without graphics issues.
    * See video

## Heidi


The main tasks completed this week are the following:
- retrieving images from the server for the Profemon selection.
- designed the UI for the battle stage, which includes sending the Profemon selection, retrieving the images of the player's and the opponent's profemon selection, and the moves the player profemon has, sending the selected moves the server, and receiving the updated hp values and a display text from the server, and then the cycle repeats. 
- migrated the system from using button inputs to joystick inputs.
- integrated the capture code. 

### Retrieving images from the server
In the `get_profemons()` function, we retrieve all the profemons a player has from the server. Before we receive any data, `len=true` is included in the request, so that the server returns the length of the entire message that will be transmitted to the client for the request without the `len` argument. The length is stored in `IMG_BUFFER_SIZE`, and then memory for storing the response is dynamically assigned via `calloc`:
```cpp
img_response = (char*)calloc(IMG_BUFFER_SIZE, sizeof(char));
```
Then, we deserialize the json received from the server, which includes the length of each unencoded image, as well as the encoded images in base64 format. Thus, for each Profemon, we again dynamically allocate memory for storing the unencoded jpeg image and store the arrays of pointers in `prof_images`, store the encoded base64 string in `temp_img`, and then decode the base64 encoded image into `prof_images`.
```cpp
for (int i = 0; i < profemon_count; i++) {
    strcpy(temp_img, doc["team"][i]["image"]);
    prof_images_size[i] = doc["team"][i]["len"];
    prof_images[i] = (uint8_t*)calloc(prof_images_size[i], sizeof(uint8_t));
    decode_base64((unsigned char*) temp_img, (unsigned char*) prof_images[i]);
  }
```
Then, after the images and the names of the Profemons are stored on the devide side, we free the memory allocated for the json received from the server via `free(img_response)` .

### Battle stage device side design
First, the client sends the Profemon selection to the server, and then periodially queries the server for the player's and opponent's profemon selection, including the images, hp values, and move names. The server returns "wait" until it has received Profemon selection from both players. Then, the client again decodes the json and dynamically allocates memory for the two images and decode the base64 encoded images. 

After the initialization, the player sends the selected moves to the server via the `send_move()` function. The server receives the selected moves. Once the server has moves from both players, it calculates and updates the hp values of the profemons. Similarly, while waiting for the opponent to move, the device periodically sends GET requests to the server for the updated information. The server would respond with "wait" until it receives both players' moves, in which case the server sends the updated hp values and a display text to the clients. The client receives the updated hp values and the display text and updates the UI sequentially given the two moves received.

