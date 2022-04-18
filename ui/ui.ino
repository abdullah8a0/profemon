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


const uint8_t START = 0;
const uint8_t CAPTURE = 1;
const uint8_t GAME = 2;
uint8_t state = START;
uint8_t old_state = GAME;
const uint8_t GAME_START = 0;
const uint8_t GAME_PAIR  = 1;
const uint8_t GAME_SELECT = 2;
const uint8_t GAME_BATTLE = 3;
const uint8_t GAME_END = 4;
const uint8_t GAME_WAIT = 5;
uint8_t game_state = GAME_START;
uint8_t old_game_state = GAME_END;
const uint8_t BATTLE_MOVE = 0;
const uint8_t BATTLE_WAIT = 1;
const uint8_t BATTLE_UPDATE = 2;
uint8_t battle_state = BATTLE_MOVE;
uint8_t old_battle_state = BATTLE_UPDATE;


char network[] = "MIT";
char password[] = "";


//Some constants and some resources:
const int RESPONSE_TIMEOUT = 6000; //ms to wait for response from host
const uint16_t OUT_BUFFER_SIZE = 200; //size of buffer to hold HTTP response
// char old_response[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP request
char response[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP request
char request_buffer[100];

// game related variables
uint16_t game_id = 2;
uint32_t timer;

// selection
char* img_response;
char user_name[15];
char prof_names[10][25];
uint8_t** prof_images; // dynamically assign memory
uint16_t prof_images_size[10];
char temp_img[4000];
uint8_t profemon_count = 0;
int8_t curr_idx = 0;

// battle
char player_moves[4][20];
uint8_t curr_move = 0;
uint8_t* player_image;
uint8_t* opponent_image;
uint16_t player_image_size;
uint16_t opponent_image_size;
uint16_t player_max_hp;
uint16_t player_hp;
uint16_t player_new_hp;
uint16_t opponent_max_hp;
uint16_t opponent_hp;
uint16_t opponent_new_hp;
char display_text[2][100];

// ui constants
const uint8_t xm = 4; // width margin
const uint8_t ym = 4; // height margin
const uint8_t w = tft.width();
const uint8_t h = tft.height();
const uint8_t img_w = 32;
const uint8_t img_h = 40;
const uint8_t bar_len = w - 2*xm - img_w;
const uint8_t bar_h = 8;
const uint8_t xo = 4; // text offset x
const uint8_t yo = 8; // text offset y
const uint8_t xb = 58; // move box width
const uint8_t yb = 30; // move box height
const uint8_t box_x[] = {xm, w-xm-xb, xm, w-xm-xb}; // move boxes
const uint8_t box_y[] = {h/2-yb-2, h/2-yb-2, h/2+2, h/2+2};


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

  state = GAME;
  game_state = GAME_WAIT;
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
            get_profemons();
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(0, 0, 2);
            tft.println("Select Profemon.");
            old_game_state = game_state;
            curr_idx = 0;
            select_profemon(curr_idx);
          }

          if (b1 == 1) {
            curr_idx = (curr_idx + 1) % profemon_count;
            select_profemon(curr_idx);
          }
          else if (b2 == 1) {
            curr_idx = (curr_idx - 1 + profemon_count) % profemon_count;
            select_profemon(curr_idx);
          }
          else if (b1 == 2) {
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(0, 0, 2);
            tft.printf("Selected Profemon\n  %s.", prof_names[curr_idx]);
            send_selection(curr_idx);
            Serial.println("Sent Profemon selection to the server.");
            delay(2000);
            if (battle_init()) {
              game_state = GAME_BATTLE;
            } else {
              game_state = GAME_WAIT;
            }
          }
        break;

        case GAME_WAIT:
          if(old_game_state != game_state) {
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(0, 0, 2);
            tft.setTextColor(TFT_GREEN, TFT_BLACK);
            tft.println("Waiting for \nopponent...");
            old_game_state = game_state;
            timer = millis();
          }
          if (millis() - timer > 2000) {
            if (battle_init()) {
              delay(2000);
              game_state = GAME_BATTLE;
            } else {
              game_state = GAME_WAIT;
              timer = millis();
            }
          }
        break;

        case GAME_BATTLE:
        if(old_game_state != game_state) {
            old_game_state = game_state;
            // free memory used to store profemon images
            for (int i = 0; i < profemon_count; i++) {
              free(prof_images[i]);
            }
            display_battle();
            battle_state = BATTLE_MOVE;
          }
          
          switch (battle_state) {
            case BATTLE_MOVE:
              if (old_battle_state != battle_state) {
                tft.fillRect(0, ym+img_h, w, h-2*(img_h+ym), TFT_BLACK);
                select_move(curr_move);
                old_battle_state = battle_state;
              }
              if (b1 == 1) {
                curr_move = (curr_move + 1) % 4;
                select_move(curr_move);
              }
              else if (b2 == 1) {
                curr_move = (curr_move - 1 + 4) % 4;
                select_move(curr_move);
              }
              else if (b1 == 2) {
                send_move(curr_move);
                if (battle_step()) {
                  game_state = BATTLE_UPDATE;
                } else {
                  game_state = BATTLE_WAIT;
                }
              }
            break;

            case BATTLE_WAIT:
              if (old_battle_state != battle_state) {
                timer = millis();
                old_battle_state = battle_state;
              }
              if (millis() - timer > 2000) {
                if (battle_step()) {
                  game_state = BATTLE_UPDATE;
                } else {
                  game_state = BATTLE_WAIT;
                  timer = millis();
                }
              }
            break;
            
            case BATTLE_UPDATE:
              if (old_battle_state != battle_state) {
                tft.fillRect(0, ym+img_h, w, h-img_h-ym, TFT_BLACK);
                tft.setCursor(0, ym+img_h+10, 2);
                tft.setTextColor(TFT_WHITE, TFT_BLACK);
                tft.println(display_text[0]);
                old_battle_state = battle_state;
                display_hp();
                timer = millis();
              }

              if (millis() - timer > 3000) {
                tft.fillRect(0, ym+img_h, w, h-img_h-ym, TFT_BLACK);
                tft.setCursor(0, ym+img_h+10, 2);
                tft.println(display_text[1]);
                player_hp = player_new_hp;
                opponent_hp = opponent_new_hp;
                display_hp();
              }

              if (millis() - timer > 6000) {
                battle_state = BATTLE_MOVE;
              }
            break;
          }
        break;

        case GAME_END:
          if(old_game_state != game_state) {
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(0, 0, 2);
            tft.setTextColor(TFT_GREEN, TFT_BLACK);
            tft.println("You won!");
            old_game_state = game_state;

            // free memory used for battle images
            free(player_image);
            free(opponent_image);
          }

          if(b1 == 1) {
            state = START;
          }
        break;
      }
    break;
  }

}

void send_selection(uint8_t i) {
  memset(request_buffer, 0, sizeof(request_buffer));
  sprintf(request_buffer, "POST /sandbox/sc/team5/battle.py?user=%s&game_id=%d&profemon=%s HTTP/1.1\r\n", user_name, game_id, prof_names[i]);
  strcat(request_buffer, "Host: 608dev-2.net\r\n\r\n");
  do_http_request("608dev-2.net", request_buffer, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false);
  Serial.println(response);
}

bool battle_init() {
  memset(request_buffer, 0, sizeof(request_buffer));
  sprintf(request_buffer, "GET /sandbox/sc/team5/battle.py?user=%s&game_id=%d&init=true&len=true HTTP/1.1\r\n", user_name, game_id);
  strcat(request_buffer, "Host: 608dev-2.net\r\n\r\n");
  do_http_request("608dev-2.net", request_buffer, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false);
  if (strcmp(response, "wait\n") == 0) {
    return false;
  }
  uint16_t IMG_BUFFER_SIZE = atoi(response) + 10;
  img_response = (char*)calloc(IMG_BUFFER_SIZE, sizeof(char));
  memset(request_buffer, 0, sizeof(request_buffer));
  sprintf(request_buffer, "GET /sandbox/sc/team5/battle.py?user=%s&game_id=%d&init=true HTTP/1.1\r\n", user_name, game_id);
  strcat(request_buffer, "Host: 608dev-2.net\r\n\r\n");
  do_http_request("608dev-2.net", request_buffer, img_response, IMG_BUFFER_SIZE, RESPONSE_TIMEOUT, false);
  StaticJsonDocument<500> doc;
  deserializeJson(doc, img_response);
  delay(1000);
  // initialize hp
  player_max_hp = doc["player"]["max_hp"];
  opponent_max_hp = doc["opponent"]["max_hp"];
  if (player_max_hp == 0) {
    player_max_hp = 100;
  }
  if (opponent_max_hp == 0) {
    opponent_max_hp = 100;
  }
  player_hp = player_max_hp;
  opponent_hp = opponent_max_hp;
  // get player moves
  delay(2000);
  for (int i = 0; i < 4; i++) {
    strcpy(player_moves[i], doc["player"]["moves"][i]);
  }
  // get images
  player_image_size = doc["player"]["len"];
  opponent_image_size = doc["opponent"]["len"];
  strcpy(temp_img, doc["player"]["image"]);
  player_image = (uint8_t*)calloc(player_image_size, sizeof(uint8_t));
  decode_base64((unsigned char*) temp_img, (unsigned char*) player_image);
  strcpy(temp_img, doc["opponent"]["image"]);
  opponent_image = (uint8_t*)calloc(opponent_image_size, sizeof(uint8_t));
  decode_base64((unsigned char*) temp_img, (unsigned char*) opponent_image);
  free(img_response);
  return true;
}

void display_hp() {
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  // opponent hp
  tft.fillRect(xm, ym+8, bar_len, bar_h, TFT_RED);
  tft.fillRect(xm, ym+8, bar_len * (opponent_max_hp - opponent_hp) / opponent_max_hp, bar_h, TFT_BLACK);
  tft.drawRect(xm, ym+8, bar_len, bar_h, TFT_WHITE);
  // player hp
  tft.fillRect(xm+img_w, h-ym-8, bar_len, bar_h, TFT_BLACK);
  tft.fillRect(xm+img_w, h-ym-8, bar_len * player_hp / player_max_hp, bar_h, TFT_RED);
  tft.drawRect(xm+img_w, h-ym-8, bar_len, bar_h, TFT_WHITE);
  tft.setCursor(w-xm-50, h-ym-20, 1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.printf("%3d/%3d ", player_hp, player_max_hp);
}


void select_move(uint8_t idx) {
  for (int i=0; i < 4; i++) {
    if (i != idx) {
      tft.drawRoundRect(box_x[i], box_y[i], xb, yb, 2, TFT_WHITE);
      tft.setCursor(box_x[i]+xo, box_y[i]+yo, 1);
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.println(player_moves[i]);
    } else {
      tft.drawRoundRect(box_x[i], box_y[i], xb, yb, 2, TFT_GREEN);
      tft.setCursor(box_x[i]+xo, box_y[i]+yo, 1);
      tft.setTextColor(TFT_GREEN, TFT_BLACK);
      tft.println(player_moves[i]);
    }
  }
}

void display_battle() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  // draw images
  drawArrayJpeg(opponent_image, opponent_image_size, w-xm-img_w, ym);
  drawArrayJpeg(player_image, player_image_size, xm, h-ym-img_h);

  display_hp();
  select_move(curr_move);
}

void send_move(uint8_t idx) {
  return;
}

bool battle_step() {
  return false;
}

void get_profemons() {
  // get length of the response
  memset(request_buffer, 0, sizeof(request_buffer));
  sprintf(request_buffer, "GET /sandbox/sc/team5/get_profemons.py?user=%s&len=true HTTP/1.1\r\n", user_name);
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


void select_profemon(uint8_t i) {
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