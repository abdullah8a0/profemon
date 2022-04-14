# Week 1 Deliverables

## Abdullah

- Show that two ESP32S2 can verify each other's existence via initiating a game.

[Video](https://youtu.be/-b_8f32p_zI)

### Overview:

Each ESP is identified with a unique 4-character id.

For a game to be initiated between two close proximity players (P1 and P2), 3 things need to happen:

P2 needs to find P1’s id.
P1 needs to find P2’s id.
The server needs to know the ids of both of these players and log them.

The diagram shows the overall protocol that allows the above to happen. One ESP acts as a broadcaster, screaming into the void using its SSID. The SSID has the form “Profemon{id}” as a standardized method of communication.

![Overview of Handshake](https://i.imgur.com/BRl07R8.jpeg)

When a player is listening and finds an Id floating in the void, it bundles up the two ids and sends them to the server. The server treats this as a game initiation request and accepts the request unless someone beat P2 into pairing with P1.

## Andi

- Show ID being scanned, request being handled by server, and professor and location being recorded in the database along with the player ID

## Heidi

### State machine for client side 
I implemented a state machine for the client side and added some rudimentary UI elements for each state to indicate which state the player is currently in. There are two main functionalities for the player: capturing a Profemon and battling. The player can transition from the `START` state either to the `CAPTURE` state or the `GAME` state. In the `GAME` state, the player progresses from `GAME_START`, `GAME_PAIR` (player pairs with the opponent), `GAME_SELECT` (player selects the Profemon for battle), `GAME_BATTLE` and `GAME_END`. 

### Selecting Profemon
The main UI functionality implemented this week is the Profemon selection process, where the player selects the Profemon they want to use in the battle. The player can use the two buttons to navigate the list of Profemons they have, and long press the button to select the Profemon. For each Profemon, there is an image of the professor displayed in the center. Since local storage of these large image files seems to be easier to manage, we decided for now that images of the professors would be stored locally instead of pulled from the server, which presents some additional challenges of format conversions. 

### Next steps (included for reference/central organization)
- Migrate the user input from using buttons to using the joystick. 
- Integrate various components of the project that are already implemented into the core UI, i.e., pairing with another player, capturing Profemons, and querying the server for a captured Profemon list.
- Replace the rudimentary text instructions with more appropriate UI elements at each state. 
- Implement a UI incorporating images of player Profemon and opponent Profemon for the battle process. 

## Ivy

- Unlisted but necessary: Get RC522 scanning and printing out UIDs
  - Used MFRC522 library to read UID. Code from esp32.io

```cpp
void loop() {
  if (rfid.PICC_IsNewCardPresent()) { // new tag is available
    if (rfid.PICC_ReadCardSerial()) { // NUID has been readed
      MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
      Serial.print("RFID/NFC Tag Type: ");
      Serial.println(rfid.PICC_GetTypeName(piccType));

      // print UID in Serial Monitor in the hex format
      Serial.print("UID:");
      siz = 0;
      for (int i = 0;i < rfid.uid.size; i++) {
        c[siz++] = hex(rfid.uid.uidByte[i] >> 4);
        c[siz++] = hex(rfid.uid.uidByte[i] & 15);
        c[siz++] = ' ';
      }
      c[siz-1] = '\0';

      Serial.println(c);

      rfid.PICC_HaltA(); // halt PICC
      rfid.PCD_StopCrypto1(); // stop encryption on PCD
    }
  }
}

char hex(int x)
{
	return x < 10 ? x + '0' : x - 10 + 'A';
}
```

- Show database with professor IDs collected so far with associated name. ![Using Postman to show names and IDs in Database](http://g.recordit.co/OAIGbqmCWe.gif)
- Have hardware parts ordered in spreadsheet.
  - Ordered power banks and RC522's. Will get joysticks and Dupont wires from lab.
  - RC522's were evaluated to be bad by an LA, so I put in an order for more. We currently only have 1 working RC522 which is currently being used by Andi for testing purposes.
- Show schematic for RC522 in system.
  - [Link to current schematic](https://drive.google.com/file/d/1guy1b2j0ekgfu1Y9LPn_IptbRYLgjF4g/view?usp=sharing)
