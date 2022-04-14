#include <SPI.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

#include <JPEGDecoder.h>
#include "renderjpeg.h"
#include "images.h"

#include "Button.h"

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
  Serial.println(b1);
  Serial.println(b2);
  switch(state) {
    case START:
      if(old_state != state) {
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(0, 0, 2);
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        tft.println("  Press Button 1 \n  to capture.");
        tft.println("  Press Button 2 \n  to battle.");
        old_state = state;
      }
      if (b1 == 1) {
        state = CAPTURE;
      }
      else if (b2 == 1) {
        state = GAME;
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
            drawArrayJpeg(JoeSteinmeyer, sizeof(JoeSteinmeyer), 16, 20);
            tft.setCursor(10, 142, 2);
            tft.println("  JoeSteinmeyer  ");
            old_game_state = game_state;
          }
          if (b1 == 1) {
            drawArrayJpeg(AnaBell, sizeof(AnaBell), 16, 20);
            tft.setCursor(10, 142, 2);
            tft.println("     AnaBell     ");
          }
          else if (b2 == 1) {
            drawArrayJpeg(Steinmeyer, sizeof(Steinmeyer), 16, 20);
            tft.setCursor(10, 142, 2);
            tft.println("    AdamHartz    ");
          }
          else if (b1 == 2) {
            tft.setCursor(0, 0, 2);
            tft.println("Selected Profemon.");
            delay(1000);
            game_state = GAME_BATTLE;
          }
        break;

        case GAME_BATTLE:
        if(old_game_state != game_state) {
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(0, 0, 2);
            tft.setTextColor(TFT_GREEN, TFT_BLACK);
            tft.println("Select your next attack.");
            old_game_state = game_state;
          }

          if(b1 == 1) {
            game_state = GAME_END;
          }

        break;

        case GAME_END:
          if(old_game_state != game_state) {
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(0, 0, 3);
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

void drawArrayJpeg(const uint8_t arrayname[], uint32_t array_size, int xpos, int ypos) {

  int x = xpos;
  int y = ypos;

  JpegDec.decodeArray(arrayname, array_size);
  jpegInfo();
  renderJPEG(x, y);

}