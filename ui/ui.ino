#include <SPI.h>
#include <TFT_eSPI.h>
#include<string.h>
#include <JPEGDecoder.h>
#include "renderjpeg.h"
#include "images.h"
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include "base64.hpp"

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
const uint16_t OUT_BUFFER_SIZE = 200; //size of buffer to hold HTTP response
// char old_response[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP request
char response[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP request
char request_buffer[100];

// user related variables
char* img_response;
char user_name[15];
char prof_names[10][25];
// uint8_t prof_images[5][4000]; // preassign memory for images
uint8_t** prof_images; // dynamically assign memory
uint16_t prof_images_size[10];
char temp_img[4000];

uint8_t profemon_count = 0;
int8_t curr_idx = 0;


void setup() {
  Serial.begin(115200); //for debugging if needed.
  WiFi.begin(network, password); //attempt to connect to wifi
  uint8_t count = 0; //count used for Wifi check times
  Serial.print("Attempting to connect to ");
  Serial.println(network);
  while (WiFi.status() != WL_CONNECTED && count < 12) {
    delay(500);
    Serial.print(".");
    count++;
  }
  delay(2000);
  if (WiFi.isConnected()) { //if we connected then print our IP, Mac, and SSID we're on
    Serial.println("CONNECTED!");
    Serial.printf("%d:%d:%d:%d (%s) (%s)\n", WiFi.localIP()[3], WiFi.localIP()[2],
                  WiFi.localIP()[1], WiFi.localIP()[0],
                  WiFi.macAddress().c_str() , WiFi.SSID().c_str());    delay(500);
  } else { //if we failed to connect just Try again.
    Serial.println("Failed to Connect :/  Going to restart");
    Serial.println(WiFi.status());
    ESP.restart(); // restart the ESP (proper way)
  }
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
        // get username
        strcpy(user_name, "andi");
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
            // obtain profemon list from server
            tft.println("Retrieving your \nProfemons...");
            getProfemons();
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(0, 0, 2);
            tft.println("Select Profemon.");
            old_game_state = game_state;
            curr_idx = 0;
            selectProfemon(curr_idx);
          }

          if (b1 == 1) {
            curr_idx = (curr_idx + 1) % profemon_count;
            selectProfemon(curr_idx);
          }
          else if (b2 == 1) {
            curr_idx = (curr_idx - 1 + profemon_count) % profemon_count;
            selectProfemon(curr_idx);
          }
          else if (b1 == 2) {
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(0, 0, 2);
            tft.printf("Selected Profemon\n  %s.", prof_names[curr_idx]);
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

            for (int i = 0; i < profemon_count; i++) {
              free(prof_images[i]);
            }
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

void getProfemons() {
  // get length of the response
  memset(request_buffer, 0, sizeof(request_buffer));
  sprintf(request_buffer, "GET /sandbox/sc/team5/get_profemons.py?user=%s&len=1 HTTP/1.1\r\n", user_name);
  strcat(request_buffer, "Host: 608dev-2.net\r\n\r\n");
  do_http_request("608dev-2.net", request_buffer, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false);
  uint16_t IMG_BUFFER_SIZE = atoi(response) + 10;
  img_response = (char*)calloc(IMG_BUFFER_SIZE, sizeof(char));
  // get the images
  memset(request_buffer, 0, sizeof(request_buffer));
  sprintf(request_buffer, "GET /sandbox/sc/team5/get_profemons.py?user=%s HTTP/1.1\r\n", user_name);
  strcat(request_buffer, "Host: 608dev-2.net\r\n\r\n");
  do_http_request("608dev-2.net", request_buffer, img_response, IMG_BUFFER_SIZE, RESPONSE_TIMEOUT, false);
  StaticJsonDocument<500> doc;
  deserializeJson(doc, img_response);
  profemon_count = doc["count"];
  Serial.println(profemon_count);

  for (int i=0; i < profemon_count; i++) {
    strcpy(prof_names[i], doc["team"][i]["name"]);
  }

  prof_images = (uint8_t**)calloc(profemon_count, sizeof(uint8_t*));
  for (int i = 0; i < profemon_count; i++) {
    strcpy(temp_img, doc["team"][i]["image"]);
    prof_images_size[i] = doc["team"][i]["len"];
    prof_images[i] = (uint8_t*)calloc(prof_images_size[i], sizeof(uint8_t));
    decode_base64((unsigned char*) temp_img, (unsigned char*) prof_images[i]);
  }

  free(img_response);
}


void selectProfemon(uint8_t i) {
  drawArrayJpeg(prof_images[i], prof_images_size[i], 16, 20);
  tft.setCursor(16, 142, 2);
  tft.printf("%s        ", prof_names[i]);
}


void drawArrayJpeg(const uint8_t arrayname[], uint32_t array_size, int xpos, int ypos) {

  int x = xpos;
  int y = ypos;

  JpegDec.decodeArray(arrayname, array_size);
  jpegInfo();
  renderJPEG(x, y, &tft);

}