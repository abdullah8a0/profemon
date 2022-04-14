#include <SPI.h>
#include <TFT_eSPI.h>
#include<string.h>
#include <JPEGDecoder.h>
#include "renderjpeg.h"
#include "images.h"
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>

#include "Button.h"

TFT_eSPI tft = TFT_eSPI();
const int SCREEN_HEIGHT = 160;
const int SCREEN_WIDTH = 128;
const int BUTTON1 = 45;
const int BUTTON2 = 39;

Button button1(BUTTON1);
Button button2(BUTTON2);

uint8_t state = 0;
uint8_t old_state = 1;
const uint8_t START = 0;
const uint8_t CAPTURE = 1;
const uint8_t GAME = 2;
uint8_t game_state = 0;
uint8_t old_game_state = 1;
const uint8_t GAME_START = 0;
const uint8_t GAME_PAIR  = 1;
const uint8_t GAME_SELECT = 2;
const uint8_t GAME_BATTLE = 3;
const uint8_t GAME_END = 4;


char network[] = "MIT";
char password[] = "";


//Some constants and some resources:
const int RESPONSE_TIMEOUT = 6000; //ms to wait for response from host
const uint16_t OUT_BUFFER_SIZE = 1000; //size of buffer to hold HTTP response
char old_response[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP request
char response[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP request

char profemons[20][25];
uint8_t profemon_count = 0;
int8_t curr_idx = 0;


void setup() {
  Serial.begin(115200);
  tft.begin();
  tft.setRotation(2);
  tft.setTextSize(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);

  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
}

void loop() {
  int b1 = button1.update();
  int b2 = button2.update();

  switch(state) {
    case START:
      if(old_state != state) {
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(0, 0, 2);
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        tft.println("Press Button 1 \nto battle.");
        tft.println("Press Button 2 \nto capture.");
        old_state = state;
      }
      if (b1 == 1) {
        state = GAME;
      }
      else if (b2 == 1) {
        state = CAPTURE;
      }
    break;

    case CAPTURE:
      if(old_state != state) {
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(0, 0, 2);
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        tft.println("Scan ID to capture.");
        old_state = state;
      }
      if (b1 == 1) {
        state = START;
      }
      else if (b2 == 1) {
        state = START;
      }
    break;

    case GAME:
      if(old_state != state) {
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(0, 0, 2);
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        old_state = state;
        old_game_state = GAME_BATTLE;
        game_state = GAME_START;
      }
      switch (game_state) {
        case GAME_START:
          if(old_game_state != game_state) {
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(0, 0);
            tft.setTextColor(TFT_GREEN, TFT_BLACK);
            tft.println("Press Button 1 to start pairing with an opponent.");
            old_game_state = game_state;

            // obtain profemon list from server
            strcpy(profemons[0], "JoeSteinmeyer");
            strcpy(profemons[1], "AnaBell");
            strcpy(profemons[2], "AdamHartz");
            profemon_count = 3;
          }
          if(b1 == 1) {
            game_state = GAME_PAIR;
          }
        break;

        case GAME_PAIR:
          if(old_game_state != game_state) {
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(0, 0, 2);
            tft.setTextColor(TFT_GREEN, TFT_BLACK);
            tft.println("Pairing...");
            old_game_state = game_state;
          }
          if(b1 == 1) {
            game_state = GAME_SELECT;
          }
        break;

        case GAME_SELECT:
          if(old_game_state != game_state) {
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(0, 0, 2);
            tft.setTextColor(TFT_GREEN, TFT_BLACK);
            tft.println("Select Profemon.");
            old_game_state = game_state;
            curr_idx = 0;
            selectProfemon(profemons[curr_idx]);
          }

          if (b1 == 1) {
            curr_idx++;
            if (curr_idx >= profemon_count) {
              curr_idx = 0;
            }
            selectProfemon(profemons[curr_idx]);
          }
          else if (b2 == 1) {
            curr_idx--;
            if (curr_idx < 0) {
              curr_idx = profemon_count - 1;
            }
            selectProfemon(profemons[curr_idx]);
          }
          else if (b1 == 2) {
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(0, 0, 2);
            tft.printf("Selected Profemon\n  %s.", profemons[curr_idx]);
            delay(1000);
            game_state = GAME_BATTLE;
          }
        break;

        case GAME_BATTLE:
        if(old_game_state != game_state) {
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(0, 0, 2);
            tft.setTextColor(TFT_GREEN, TFT_BLACK);
            tft.println("Battle started!\nSelect your \nnext attack.");
            old_game_state = game_state;
          }

          if(b1 == 1) {
            game_state = GAME_END;
          }

        break;

        case GAME_END:
          if(old_game_state != game_state) {
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(0, 0, 2);
            tft.setTextColor(TFT_GREEN, TFT_BLACK);
            tft.println("You won!");
            old_game_state = game_state;
          }

          if(b1 == 1) {
            state = START;
          }
        break;
      }
    break;
  }

}

void selectProfemon(char* name) {
  if (strcmp(name, "JoeSteinmeyer") == 0) {
    drawArrayJpeg(JoeSteinmeyer, sizeof(JoeSteinmeyer), 16, 20);
  }
  else if (strcmp(name, "AnaBell") == 0) {
    drawArrayJpeg(AnaBell, sizeof(AnaBell), 16, 20);
  }
  else if (strcmp(name, "AdamHartz") == 0) {
    drawArrayJpeg(AdamHartz, sizeof(AdamHartz), 16, 20);
  }
  tft.setCursor(16, 142, 2);
  tft.printf("%s        ", name);
}


void drawArrayJpeg(const uint8_t arrayname[], uint32_t array_size, int xpos, int ypos) {

  int x = xpos;
  int y = ypos;

  JpegDec.decodeArray(arrayname, array_size);
  jpegInfo();
  renderJPEG(x, y, &tft);

}