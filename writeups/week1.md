# Week 1 Deliverables

## Abdullah

- Show that two ESP32S2 can verify each other's existence via initiating a game.

!(Video)[https://youtu.be/-b_8f32p_zI]

### Overview:

Each ESP is identified with a unique 4-character id.

For a game to be initiated between two close proximity players (P1 and P2), 3 things need to happen:

P2 needs to find P1’s id.
P1 needs to find P2’s id.
The server needs to know the ids of both of these players and log them.

The diagram shows the overall protocol that allows the above to happen. One ESP acts as a broadcaster, screaming into the void using its SSID. The SSID has the form “Profemon{id}” as a standardized method of communication.

![Overview of Handshake](https://drive.google.com/file/d/1IhUmKIPPkrWbb12-TSMUsvtpqTJ2sygx/view?usp=sharing)

When a player is listening and finds an Id floating in the void, it bundles up the two ids and sends them to the server. The server treats this as a game initiation request and accepts the request unless someone beat P2 into pairing with P1.

## Andi

- Show ID being scanned, request being handled by server, and professor and location being recorded in the database along with the player ID

## Heidi

- Demonstrate the selection process on the ESP, and show that the images are pulled from the server. The user would use either a button or a joystick to navigate the selection screen which contains one or two images of a professor on the screen at a time and be able to see through some UI element (an arrow or a box) which profemon is currently selected. Ideally, also integrate with requests to the server database to query which profemons a player has.

## Ivy

- Unlisted but necessary: Get RC522 scanning and printing out UIDs
  - Used MFRC522 library to read UID. Code from

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
