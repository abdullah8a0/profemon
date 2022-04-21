# Week 2 Deliverables

## Abdullah


## Andi & Ivy


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

