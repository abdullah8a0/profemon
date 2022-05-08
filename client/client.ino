#include <SPI.h>
#include <TFT_eSPI.h>
#include <string.h>
#include <JPEGDecoder.h>
#include "renderjpeg.h"
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include "base64.hpp"
#include <MFRC522.h>

#include "http_req.h"
#include "util.h"
// #include <map>

// tft
TFT_eSPI tft = TFT_eSPI();
const uint8_t SCREEN_HEIGHT = 160;
const uint8_t SCREEN_WIDTH = 128;
const uint8_t BUTTON1 = 45;
const uint8_t BUTTON2 = 39;
// joystick
const uint8_t VRx = 5;
const uint8_t VRy = 6;
const uint8_t Sw = 12;
Joystick joystick(VRx, VRy, Sw, 0, 1);
Button button1(BUTTON1);
Button button2(BUTTON2);
// rfid reader
#define SS_PIN 3
#define RST_PIN 4
MFRC522 rfid(SS_PIN, RST_PIN);

const uint8_t START = 0;
const uint8_t CAPTURE = 1;
const uint8_t GAME = 2;
uint8_t state = START;
uint8_t old_state = GAME;
const uint8_t GAME_START = 0;
const uint8_t GAME_PAIR = 1;
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

// Abdullah's pairing stuff
const uint8_t PAIR_START = 0;
const uint8_t PAIR_BROADCAST = 1;
const uint8_t PAIR_LISTEN = 2;
uint8_t pair_state = PAIR_START;

char network[] = "MIT";
char password[] = "";

// Some constants and some resources:
const int RESPONSE_TIMEOUT = 6000;     // ms to wait for response from host
const uint16_t OUT_BUFFER_SIZE = 5000; // size of buffer to hold HTTP response
char response[OUT_BUFFER_SIZE];        // char array buffer to hold HTTP request
char request_buffer[100];
const uint16_t IN_BUFFER_SIZE = 5000;
const uint16_t JSON_BODY_SIZE = 5000;
char request[IN_BUFFER_SIZE];
char request_body[JSON_BODY_SIZE];
char response_body[OUT_BUFFER_SIZE];
char json_body[JSON_BODY_SIZE];

// game related variables
StaticJsonDocument<500> doc;
uint16_t game_id = 2;
uint8_t user = 40;
uint32_t timer = millis();
uint32_t capture_timer = millis();

// start
uint8_t next_state = CAPTURE;
int color1 = TFT_GREEN;
int color2 = TFT_WHITE;

// capture
char c[1234];
int siz = 0;
double lat;
double lng;
const char PREFIX[] = "{\"wifiAccessPoints\": [";                 // beginning of json body
const char SUFFIX[] = "]}";                                       // suffix to POST request
const char API_KEY[] = "AIzaSyAQ9SzqkHhV-Gjv-71LohsypXUH447GWX8"; // don't change this and don't share this
const int MAX_APS = 10;

// selection
char *img_response;
char prof_names[10][25];
uint8_t **prof_images; // dynamically assign memory
uint16_t prof_images_size[10];
char temp_img[4000];
uint8_t profemon_count = 0;
int8_t curr_idx = 0;

// battle
char player_moves[4][20];
uint8_t curr_move = 0;
uint8_t *player_image;
uint8_t *opponent_image;
uint16_t player_image_size;
uint16_t opponent_image_size;
uint16_t player_max_hp;
uint16_t player_hp;
uint16_t player_new_hp;
uint16_t opponent_max_hp;
uint16_t opponent_hp;
uint16_t opponent_new_hp;
char display_text[2][100];
uint8_t battle_result = 0;
bool displayed_second_move = false;
const uint8_t CONT = 0;
const uint8_t WIN = 1;
const uint8_t LOSE = 2;

// std::map <joystick_direction, uint8_t*> dir_map;
// {
//     {NONE, {0, 1, 2, 3},
//     {JOYSTICK_UP, {0, 1, 0, 1}},
//     {JOYSTICK_DOWN, {2, 3, 2, 3}},
//     {JOYSTICK_LEFT, {0, 0, 2, 2}},
//     {JOYSTICK_RIGHT, {1, 1, 3, 3}}
// }

uint8_t dir_map_up[4] = {0, 1, 0, 1};
uint8_t dir_map_down[4] = {2, 3, 2, 3};
uint8_t dir_map_left[4] = {0, 0, 2, 2};
uint8_t dir_map_right[4] = {1, 1, 3, 3};

// ui constants
const uint8_t xm = 4; // width margin
const uint8_t ym = 4; // height margin
const uint8_t w = tft.width();
const uint8_t h = tft.height();
const uint8_t img_w = 32;
const uint8_t img_h = 42;
const uint8_t bar_len = w - 2 * xm - img_w;
const uint8_t bar_h = 8;
const uint8_t xo = 4;                                       // text offset x
const uint8_t yo = 8;                                       // text offset y
const uint8_t xb = 58;                                      // move box width
const uint8_t yb = 30;                                      // move box height
const uint8_t box_x[] = {xm, w - xm - xb, xm, w - xm - xb}; // move boxes
const uint8_t box_y[] = {h / 2 - yb - 2, h / 2 - yb - 2, h / 2 + 2, h / 2 + 2};

// buzzer audio
double A_1 = 55; //A_1 55 Hz  for note generation
const uint8_t NOTE_COUNT = 97; //number of notes set at six octaves from

struct Riff {
  double notes[256];
  int length;
  float note_period;
};

float new_note = 0;
float old_note = 0;
double MULT = 1.059463094359;

//pins for LCD and AUDIO CONTROL
uint8_t LCD_CONTROL = 21;
uint8_t AUDIO_TRANSDUCER = 14;

//PWM Channels. The LCD will still be controlled by channel 0, we'll use channel 1 for audio generation
uint8_t LCD_PWM = 0;
uint8_t AUDIO_PWM = 1;

//arrays you need to prepopulate for use in the run_instrument() function
double note_freqs[NOTE_COUNT];
float accel_thresholds[NOTE_COUNT + 1];

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


#define HOSTING_TIMEOUT_MS 5000

void setup()
{
  Serial.begin(115200); // for debugging if needed.
  SPI.begin();          // init SPI bus
  rfid.PCD_Init();      // init MFRC522

  WiFi.mode(WIFI_MODE_APSTA);

  WiFi.begin(network, password); // attempt to connect to wifi
  uint8_t count = 0;             // count used for Wifi check times
  Serial.print("Attempting to connect to ");
  Serial.println(network);
  while (WiFi.status() != WL_CONNECTED && count < 12)
  {
    delay(500);
    Serial.print(".");
    count++;
  }
  delay(2000);
  if (WiFi.isConnected())
  { // if we connected then print our IP, Mac, and SSID we're on
    Serial.println("CONNECTED!");
    Serial.printf("%d:%d:%d:%d (%s) (%s)\n", WiFi.localIP()[3], WiFi.localIP()[2],
                  WiFi.localIP()[1], WiFi.localIP()[0],
                  WiFi.macAddress().c_str(), WiFi.SSID().c_str());
    delay(500);
  }
  else
  { // if we failed to connect just Try again.
    Serial.println("Failed to Connect :/  Going to restart");
    Serial.println(WiFi.status());
    ESP.restart(); // restart the ESP (proper way)
  }

  tft.begin();
  tft.setRotation(2);
  tft.setTextSize(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);

  double note_freq = A_1;
  //fill in note_freq with appropriate frequencies from 55 Hz to 55*(MULT)^{NOTE_COUNT-1} Hz
  for(int i = 0;i < NOTE_COUNT;i++)
    note_freqs[i] = note_freq * pow(MULT,i);
  new_note = note_freqs[39];

  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);

  pinMode(AUDIO_TRANSDUCER, OUTPUT);
  ledcSetup(AUDIO_PWM, 200, 12);//12 bits of PWM precision
  ledcWrite(AUDIO_PWM, 0); //0 is a 0% duty cycle for the NFET
  ledcAttachPin(AUDIO_TRANSDUCER, AUDIO_PWM);

  analogReadResolution(10);
  pinMode(VRx, INPUT);
  pinMode(VRy, INPUT);
  pinMode(Sw, INPUT_PULLUP);
}

char *myid = make_id(user);
char *other_id = (char *)malloc(sizeof(char) * 5);
char *temp = (char *)malloc(sizeof(char) * 20);

void loop()
{
  joystick_direction joydir = joystick.update();
  uint8_t joyb = joystick.Sw_val;
  if (joydir != NONE) {
    play_riff(direction_riff, direction_riff_length, direction_duration);
  }
  if (joyb == 1) {
    play_riff(select_riff, select_riff_length, select_duration);
  }
  switch (state)
  {
  case START:
    if (old_state != state)
    {
      tft.fillScreen(TFT_BLACK);
      old_state = state;
    }

    tft.setTextColor(color1, TFT_BLACK);
    tft.drawRoundRect(20, 40, w - 20 * 2, 30, 4, color1);
    tft.setCursor(35, 48, 2);
    tft.println("CAPTURE");
    tft.setTextColor(color2, TFT_BLACK);
    tft.drawRoundRect(20, 85, w - 20 * 2, 30, 4, color2);
    tft.setCursor(40, 93, 2);
    tft.println("BATTLE");

    if (joydir == JOYSTICK_UP)
    {
      next_state = CAPTURE;
      color1 = TFT_GREEN;
      color2 = TFT_WHITE;
    }
    else if (joydir == JOYSTICK_DOWN)
    {
      next_state = GAME;
      color1 = TFT_WHITE;
      color2 = TFT_GREEN;
    }

    if (joyb == 1)
    {
      state = next_state;
    }

    break;

  case CAPTURE:
    if (old_state != state)
    {
      tft.fillScreen(TFT_BLACK);
      tft.setCursor(0, 0, 2);
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.println("Scan ID to capture.");
      tft.println("\nPress to return.");
      old_state = state;
    }
    if (rfid.PICC_IsNewCardPresent())
    { // new tag is available
      if (rfid.PICC_ReadCardSerial())
      { // NUID has been readed
        // print UID in Serial Monitor in the hex format
        Serial.print("UID:");
        siz = 0;
        for (int i = 0; i < rfid.uid.size; i++)
        {
          c[siz++] = HEXX(rfid.uid.uidByte[i] >> 4);
          c[siz++] = HEXX(rfid.uid.uidByte[i] & 15);
          c[siz++] = ' ';
        }
        c[siz - 1] = '\0';
        Serial.println(c);
        rfid.PICC_HaltA();      // halt PICC
        rfid.PCD_StopCrypto1(); // stop encryption on PCD
        catch_request(c, response_body);
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(0, 0, 2);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.println(response_body);
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        tft.printf("\nLat: %.4f\nLon: %.4f\n", lat, lng);
        play_riff(capture_riff, capture_riff_length, capture_duration);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.println("\nPress to return.");
      }
    }
    if (joyb == 1)
    {
      state = START;
    }
    break;

  case GAME:
    if (old_state != state)
    {
      tft.fillScreen(TFT_BLACK);
      tft.setCursor(0, 0, 2);
      tft.setTextColor(TFT_GREEN, TFT_BLACK);
      old_state = state;
      if (not user_has_profemons()){
        state = START;
      } else {
        old_game_state = GAME_BATTLE;
        game_state = GAME_START;
      }
    }
    switch (game_state)
    {
    case GAME_START:
      if (old_game_state != game_state)
      {
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(0, 0);
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        tft.println("Press to start pairing with an opponent.");
        old_game_state = game_state;
      }

      if (joyb == 1)
      {
        game_state = GAME_PAIR;
      }
      break;

    case GAME_PAIR:
      if (old_game_state != game_state)
      {
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(0, 0, 2);
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        tft.println("Go left to listen, right to broadcast.");
        old_game_state = game_state;
      }
      // if (joyb == 1)
      // {
      //   game_state = GAME_SELECT;
      // }
      // do a pairing fsm here

      switch (pair_state)
      {
      case PAIR_START:

        if (joydir == JOYSTICK_LEFT)
        {
          pair_state = PAIR_LISTEN;
        }
        else if (joydir == JOYSTICK_RIGHT)
        {
          pair_state = PAIR_BROADCAST;
        }
        else
        {
          // Serial.println("PAIR_START");
        }
        break;
      case PAIR_BROADCAST:
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(0, 0, 2);
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        tft.println("Broadcasting...");
        // do broadcast here
        if (broadcast(myid))
        {
          WiFi.softAPdisconnect();
          Serial.println("Broadcast Successful");
          if (!sync_ids(myid, temp))
          {
            Serial.println("Sync Failed");
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(0, 0, 2);
            tft.setTextColor(TFT_GREEN, TFT_BLACK);
            tft.println("Broadcast Successful. But Sync failed. Try again.");
            pair_state = PAIR_START;
          }
          else
          {
            game_id = atoi(temp);
            Serial.println(game_id);
            pair_state = PAIR_START;
            game_state = GAME_SELECT;
          }
        }
        else
        {
          pair_state = PAIR_START;
          tft.fillScreen(TFT_BLACK);
          tft.setCursor(0, 0, 2);
          tft.setTextColor(TFT_GREEN, TFT_BLACK);
          tft.println("Broadcast failed. Try again.");
        }
        break;
      case PAIR_LISTEN:

        tft.fillScreen(TFT_BLACK);
        tft.setCursor(0, 0, 2);
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        tft.println("Listening...");
        // do listen here
        if (listen(other_id))
        {
          post_ids(myid, other_id);
          sync_ids(myid, temp);
          game_id = atoi(temp);
          Serial.println(game_id);
          pair_state = PAIR_START;
          game_state = GAME_SELECT;
        }
        else
        {
          pair_state = PAIR_START;
          tft.fillScreen(TFT_BLACK);
          tft.setCursor(0, 0, 2);
          tft.setTextColor(TFT_GREEN, TFT_BLACK);
          tft.println("Listening failed. Try again.");
        }
        break;

      default:
        break;
      }

      break;

    case GAME_SELECT:
      if (old_game_state != game_state)
      {
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

      if (joydir == JOYSTICK_RIGHT)
      {
        curr_idx = (curr_idx + 1) % profemon_count;
        select_profemon(curr_idx);
      }
      else if (joydir == JOYSTICK_LEFT)
      {
        curr_idx = (curr_idx - 1 + profemon_count) % profemon_count;
        select_profemon(curr_idx);
      }
      else if (joyb == 1)
      {
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(0, 0, 2);
        tft.printf("Selected Profemon\n  %s.", prof_names[curr_idx]);
        send_selection(curr_idx);
        Serial.println("Sent Profemon selection to the server.");
        delay(2000);
        if (battle_init())
        {
          game_state = GAME_BATTLE;
        }
        else
        {
          game_state = GAME_WAIT;
        }
      }
      break;

    case GAME_WAIT:
      if (old_game_state != game_state)
      {
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(0, 0, 2);
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        tft.println("Waiting for \nopponent...");
        old_game_state = game_state;
        timer = millis();
      }
      if (millis() - timer > 2000)
      {
        if (battle_init())
        {
          delay(2000);
          game_state = GAME_BATTLE;
        }
        else
        {
          game_state = GAME_WAIT;
          timer = millis();
        }
      }
      break;

    case GAME_BATTLE:
      if (old_game_state != game_state)
      {
        battle_result = CONT;
        old_game_state = game_state;
        // free memory used to store profemon images
        for (int i = 0; i < profemon_count; i++)
        {
          free(prof_images[i]);
        }
        display_battle();
        battle_state = BATTLE_MOVE;
      }
      switch (battle_state)
      {
      case BATTLE_MOVE:
        if (old_battle_state != battle_state)
        {
          curr_move = 0;
          tft.fillRect(0, ym + img_h, w, h - 2 * (img_h + ym), TFT_BLACK);
          select_move(curr_move);
          old_battle_state = battle_state;
        }
        if (joydir == JOYSTICK_UP) {
          curr_move = dir_map_up[curr_move];
        } else if (joydir == JOYSTICK_DOWN) {
          curr_move = dir_map_down[curr_move];
        } else if (joydir == JOYSTICK_RIGHT) {
          curr_move = dir_map_right[curr_move];
        } else if (joydir == JOYSTICK_LEFT) {
          curr_move = dir_map_left[curr_move];
        }
        if (joydir != NONE) {
          select_move(curr_move);
        }

        if (joyb == 1)
        {
          send_move(curr_move);
          if (battle_step())
          {
            battle_state = BATTLE_UPDATE;
          }
          else
          {
            battle_state = BATTLE_WAIT;
          }
        }
        break;

      case BATTLE_WAIT:
        if (old_battle_state != battle_state)
        {
          timer = millis();
          old_battle_state = battle_state;
        }
        if (millis() - timer > 2000)
        {
          if (battle_step())
          {
            battle_state = BATTLE_UPDATE;
          }
          else
          {
            battle_state = BATTLE_WAIT;
            timer = millis();
          }
        }
        break;

      case BATTLE_UPDATE:
        if (old_battle_state != battle_state)
        {
          tft.fillRect(0, ym + img_h, w, h - (img_h + ym) * 2, TFT_BLACK);
          tft.setCursor(0, ym + img_h + 10, 2);
          tft.setTextColor(TFT_WHITE, TFT_BLACK);
          tft.println(display_text[0]);
          old_battle_state = battle_state;
          display_hp();
          displayed_second_move = false;
          timer = millis();
          if (strstr(display_text[0], "faint") != NULL)
          {
            play_riff(faint_riff, faint_riff_length, faint_duration);
          } else if (strstr(display_text[0], "miss") != NULL) {
            play_riff(miss_riff, miss_riff_length, miss_duration);
          } else {
            play_riff(attack_riff, attack_riff_length, attack_duration);
          }
        }

        if (millis() - timer > 3000 && displayed_second_move == false)
        {
          displayed_second_move = true;
          tft.fillRect(0, ym + img_h, w, h - (img_h + ym) * 2, TFT_BLACK);
          tft.setCursor(0, ym + img_h + 10, 2);
          tft.println(display_text[1]);
          player_hp = player_new_hp;
          opponent_hp = opponent_new_hp;
          display_hp();
          if (player_hp == 0) {
            battle_result = LOSE;
          } else if (opponent_hp == 0) {
            battle_result = WIN;
          }
          if (strstr(display_text[1], "faint") != NULL)
          {
            play_riff(faint_riff, faint_riff_length, faint_duration);
          } else if (strstr(display_text[1], "miss") != NULL) {
            play_riff(miss_riff, miss_riff_length, miss_duration);
          } else {
            play_riff(attack_riff, attack_riff_length, attack_duration);
          }
        }

        if (millis() - timer > 6000)
        {
          if (battle_result != CONT)
          {
            game_state = GAME_END;
          }
          battle_state = BATTLE_MOVE;
        }
        break;
      }
      break;

    case GAME_END:
      if (old_game_state != game_state)
      {
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(30, 48, 2);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        if (battle_result == WIN)
        {
          tft.println("YOU WON!");
          play_riff(win_riff, win_riff_length, win_duration);
        }
        else if (battle_result == LOSE)
        {
          tft.println("YOU LOST!");
          play_riff(lose_riff, lose_riff_length, lose_duration);
        }
        tft.println("\n\nPress to return to the main menu");
        old_game_state = game_state;

        // free memory used for battle images
        free(player_image);
        free(opponent_image);
      }

      if (joyb == 1)
      {
        state = START;
      }
      break;
    }
    break;
  }
}

///////// audio //////////////
Riff make_riff(int8_t *d, int length, double duration)
{
  Riff newRiff;
  for(int i = 0;i < length;i++)
  {
    if(d[i] >= 100 || d[i] < -100)
    {
      int actual = d[i] > 100 ? d[i] - 100 : d[i] + 100;
      newRiff.notes[i] = -new_note * pow(MULT,actual);
    }
    else if(d[i] != 99)
      newRiff.notes[i] = new_note * pow(MULT,d[i]);
    else
      newRiff.notes[i] = 0;
  }
  newRiff.length = length;
  newRiff.note_period = duration;
  return newRiff;
}

void play_riff(int8_t* notes_song_to_play, int length_of_song, double duration)
{
  double last_note = 0;
  Riff song_to_play = make_riff(notes_song_to_play, length_of_song, duration);
  for(int i = 0;i < song_to_play.length;i++)
  {
    double this_note = song_to_play.notes[i];
    Serial.println(this_note);
    if(this_note - last_note < -0.01 || this_note - last_note > 0.01)
      ledcWriteTone(AUDIO_PWM, this_note);
    delay(song_to_play.note_period);
    last_note = this_note;
  }
}

///////// GAME_PAIR //////////
bool connect_wifi()
{
  WiFi.begin("MIT GUEST", ""); // attempt to connect to wifi
  uint8_t count = 0;           // count used for Wifi check times
  while (WiFi.status() != WL_CONNECTED && count < 12)
  {
    delay(500);
    Serial.print(".|");
    count++;
  }
  delay(2000);
  if (WiFi.isConnected())
  { // if we connected then print our IP, Mac, and SSID we're on
    Serial.println("CONNECTED!");
    // Serial.printf("%d:%d:%d:%d (%s) (%s)\n", WiFi.localIP()[3], WiFi.localIP()[2],
    //               WiFi.localIP()[1], WiFi.localIP()[0],
    //               WiFi.macAddress().c_str(), WiFi.SSID().c_str());
    return true;
  }
  else
  { // if we failed to connect just Try again.
    Serial.println("Failed to Connect :/  Going to restart");
    // Serial.println(WiFi.status());
    return false;
  }
}

bool broadcast(char *my_id)
{ // TODO: boadcast while syncing. stop boradcasting if sync is done.

  char *ssid = (char *)malloc(sizeof(char) * 13);
  strcpy(ssid, "Profemon");
  strcat(ssid, myid);
  char wifi_pass[] = "Profemon";
  WiFi.softAP(ssid, wifi_pass);
  free(ssid);
  int start_time = millis();
  while (millis() - start_time < HOSTING_TIMEOUT_MS)
  {
  }

  return true;
}

// returns a 4 character string of id
char *make_id(int id)
{
  char *id_str = (char *)malloc(sizeof(char) * 5);

  id_str[0] = (id / 1000) + '0';
  id_str[1] = ((id % 1000) / 100) + '0';
  id_str[2] = ((id % 100) / 10) + '0';
  id_str[3] = (id % 10) + '0';
  id_str[4] = '\0';
  return id_str;
}

bool listen(char *other_id)
{
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  char wifi_pass[] = "Profemon";
  if (n <= 0)
  {
    Serial.print(n);
    Serial.println(" networks found");
  }
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
    {
      Serial.printf("%s\n", WiFi.SSID(i).c_str());
      if (strncmp(WiFi.SSID(i).c_str(), "Profemon", 8) == 0)
      {
        char *that_ssid = (char *)malloc(sizeof(char) * 13);
        strcpy(that_ssid, WiFi.SSID(i).c_str());

        Serial.printf("Connecting to Profemon: %s\n", that_ssid);
        for (int i = 8; i < 13; i++)
        {
          other_id[i - 8] = that_ssid[i];
        }
        free(that_ssid);
        return true;
      }
    }
  }
  return false;
}

void post_ids(char *my_id, char *other_id)
{
  char body[100];                                  // for body
  sprintf(body, "me=%s&them=%s", my_id, other_id); // generate body, posting to User, 1 step
  int body_len = strlen(body);                     // calculate body length (for header reporting)
  sprintf(request_buffer, "POST http://608dev-2.net/sandbox/sc/team5/begin.py HTTP/1.1\r\n");
  strcat(request_buffer, "Host: 608dev-2.net\r\n");
  strcat(request_buffer, "Content-Type: application/x-www-form-urlencoded\r\n");
  sprintf(request_buffer + strlen(request_buffer), "Content-Length: %d\r\n", body_len); // append string formatted to end of request_buffer buffer
  strcat(request_buffer, "\r\n");                                                       // new line from header to body
  strcat(request_buffer, body);                                                         // body
  strcat(request_buffer, "\r\n");                                                       // new line
  Serial.println(request_buffer);
  do_http_request("608dev-2.net", request_buffer, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
  Serial.println(response); // viewable in Serial Terminal
}

bool sync_ids(char *my_id, char *game_id)
{
  if (WiFi.status() != WL_CONNECTED)
  {
    if (!connect_wifi())
    {
      return false;
    }
  }
  char body[100];                // for body
  sprintf(body, "me=%s", my_id); // generate body, posting to User, 1 step
  int body_len = strlen(body);   // calculate body length (for header reporting)
  sprintf(request_buffer, "POST http://608dev-2.net/sandbox/sc/team5/sync.py HTTP/1.1\r\n");
  strcat(request_buffer, "Host: 608dev-2.net\r\n");
  strcat(request_buffer, "Content-Type: application/x-www-form-urlencoded\r\n");
  sprintf(request_buffer + strlen(request_buffer), "Content-Length: %d\r\n", body_len); // append string formatted to end of request_buffer buffer
  strcat(request_buffer, "\r\n");                                                       // new line from header to body
  strcat(request_buffer, body);                                                         // body
  strcat(request_buffer, "\r\n");                                                       // new line
  Serial.println(request_buffer);
  do_http_request("608dev-2.net", request_buffer, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
  Serial.println(response); // viewable in Serial Terminal
  // copy respose to game_id
  // game id can be arbitrary length
  for (int i = 0; i < strlen(response); i++)
  {
    game_id[i] = response[i];
  }
  return true;
}

///////// GAME_SELECT //////////

bool user_has_profemons()
{
  memset(request_buffer, 0, sizeof(request_buffer));
  sprintf(request_buffer, "GET /sandbox/sc/team5/get_profemons.py?user=%d&len=true HTTP/1.1\r\n", user);
  strcat(request_buffer, "Host: 608dev-2.net\r\n\r\n");
  do_http_request("608dev-2.net", request_buffer, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false);
  if (atoi(response) < 50)
  {
    tft.println("You don't have any profemons yet...");
    delay(3000);
    return false;
  } else {
    return true;
  }
}

void get_profemons()
{
  // get length of the response
  memset(request_buffer, 0, sizeof(request_buffer));
  sprintf(request_buffer, "GET /sandbox/sc/team5/get_profemons.py?user=%d&len=true HTTP/1.1\r\n", user);
  strcat(request_buffer, "Host: 608dev-2.net\r\n\r\n");
  do_http_request("608dev-2.net", request_buffer, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false);
  uint16_t IMG_BUFFER_SIZE = atoi(response) + 10;
  img_response = (char *)calloc(IMG_BUFFER_SIZE, sizeof(char));
  // get the images
  memset(request_buffer, 0, sizeof(request_buffer));
  sprintf(request_buffer, "GET /sandbox/sc/team5/get_profemons.py?user=%d HTTP/1.1\r\n", user);
  strcat(request_buffer, "Host: 608dev-2.net\r\n\r\n");
  do_http_request("608dev-2.net", request_buffer, img_response, IMG_BUFFER_SIZE, RESPONSE_TIMEOUT, false);
  deserializeJson(doc, img_response);
  profemon_count = doc["count"];

  for (int i = 0; i < profemon_count; i++)
  {
    strcpy(prof_names[i], doc["team"][i]["name"]);
  }

  prof_images = (uint8_t **)calloc(profemon_count, sizeof(uint8_t *));
  for (int i = 0; i < profemon_count; i++)
  {
    strcpy(temp_img, doc["team"][i]["image"]);
    prof_images_size[i] = doc["team"][i]["len"];
    prof_images[i] = (uint8_t *)calloc(prof_images_size[i], sizeof(uint8_t));
    decode_base64((unsigned char *)temp_img, (unsigned char *)prof_images[i]);
  }

  free(img_response);
}

void select_profemon(uint8_t i)
{
  drawArrayJpeg(prof_images[i], prof_images_size[i], 16, 20);
  tft.setCursor(16, 142, 2);
  tft.printf("%s        ", prof_names[i]);
}

void send_selection(uint8_t i)
{
  memset(request_buffer, 0, sizeof(request_buffer));
  sprintf(request_buffer, "POST /sandbox/sc/team5/battle.py?user=%d&game_id=%d&profemon=%s HTTP/1.1\r\n", user, game_id, prof_names[i]);
  strcat(request_buffer, "Host: 608dev-2.net\r\n\r\n");
  do_http_request("608dev-2.net", request_buffer, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false);
  Serial.println(response);
}

///////// GAME_BATTLE //////////

bool battle_init()
{
  memset(request_buffer, 0, sizeof(request_buffer));
  sprintf(request_buffer, "GET /sandbox/sc/team5/battle.py?user=%d&game_id=%d&init=true&len=true HTTP/1.1\r\n", user, game_id);
  strcat(request_buffer, "Host: 608dev-2.net\r\n\r\n");
  do_http_request("608dev-2.net", request_buffer, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false);
  if (strstr(response, "wait") != NULL)
  {
    return false;
  }
  uint16_t IMG_BUFFER_SIZE = atoi(response) + 10;
  img_response = (char *)calloc(IMG_BUFFER_SIZE, sizeof(char));
  memset(request_buffer, 0, sizeof(request_buffer));
  sprintf(request_buffer, "GET /sandbox/sc/team5/battle.py?user=%d&game_id=%d&init=true HTTP/1.1\r\n", user, game_id);
  strcat(request_buffer, "Host: 608dev-2.net\r\n\r\n");
  do_http_request("608dev-2.net", request_buffer, img_response, IMG_BUFFER_SIZE, RESPONSE_TIMEOUT, false);
  deserializeJson(doc, img_response);
  // initialize hp
  player_max_hp = doc["player"]["max_hp"];
  opponent_max_hp = doc["opponent"]["max_hp"];
  if (player_max_hp == 0)
  {
    player_max_hp = 100;
  }
  if (opponent_max_hp == 0)
  {
    opponent_max_hp = 100;
  }
  player_hp = player_max_hp;
  opponent_hp = opponent_max_hp;
  // get player moves
  for (int i = 0; i < 4; i++)
  {
    strcpy(player_moves[i], doc["player"]["moves"][i]);
  }
  // get images
  player_image_size = doc["player"]["len"];
  opponent_image_size = doc["opponent"]["len"];
  strcpy(temp_img, doc["player"]["image"]);
  player_image = (uint8_t *)calloc(player_image_size, sizeof(uint8_t));
  decode_base64((unsigned char *)temp_img, (unsigned char *)player_image);
  strcpy(temp_img, doc["opponent"]["image"]);
  opponent_image = (uint8_t *)calloc(opponent_image_size, sizeof(uint8_t));
  decode_base64((unsigned char *)temp_img, (unsigned char *)opponent_image);
  free(img_response);
  return true;
}

void display_hp()
{
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  // opponent hp
  tft.fillRect(xm, ym + 8, bar_len, bar_h, TFT_RED);
  tft.fillRect(xm, ym + 8, bar_len * (opponent_max_hp - opponent_hp) / opponent_max_hp, bar_h, TFT_BLACK);
  tft.drawRect(xm, ym + 8, bar_len, bar_h, TFT_WHITE);
  // player hp
  tft.fillRect(xm + img_w, h - ym - 8, bar_len, bar_h, TFT_BLACK);
  tft.fillRect(xm + img_w, h - ym - 8, bar_len * player_hp / player_max_hp, bar_h, TFT_RED);
  tft.drawRect(xm + img_w, h - ym - 8, bar_len, bar_h, TFT_WHITE);
  tft.setCursor(w - xm - 50, h - ym - 20, 1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.printf("%3d/%3d ", player_hp, player_max_hp);
}

void select_move(uint8_t idx)
{
  for (int i = 0; i < 4; i++)
  {
    if (i != idx)
    {
      tft.drawRoundRect(box_x[i], box_y[i], xb, yb, 2, TFT_WHITE);
      tft.setCursor(box_x[i] + xo, box_y[i] + yo, 1);
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.println(player_moves[i]);
    }
    else
    {
      tft.drawRoundRect(box_x[i], box_y[i], xb, yb, 2, TFT_GREEN);
      tft.setCursor(box_x[i] + xo, box_y[i] + yo, 1);
      tft.setTextColor(TFT_GREEN, TFT_BLACK);
      tft.println(player_moves[i]);
    }
  }
}

void display_battle()
{
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  // draw images
  drawArrayJpeg(opponent_image, opponent_image_size, w - xm - img_w, ym);
  drawArrayJpeg(player_image, player_image_size, xm, h - ym - img_h);
  display_hp();
  select_move(curr_move);
}

void send_move(uint8_t i)
{
  memset(request_buffer, 0, sizeof(request_buffer));
  sprintf(request_buffer, "POST /sandbox/sc/team5/battle.py?user=%d&game_id=%d&move=%s HTTP/1.1\r\n", user, game_id, player_moves[i]);
  strcat(request_buffer, "Host: 608dev-2.net\r\n\r\n");
  do_http_request("608dev-2.net", request_buffer, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false);
  Serial.println(response);
}

bool battle_step()
{
  memset(request_buffer, 0, sizeof(request_buffer));
  sprintf(request_buffer, "GET /sandbox/sc/team5/battle.py?user=%d&game_id=%d HTTP/1.1\r\n", user, game_id);
  strcat(request_buffer, "Host: 608dev-2.net\r\n\r\n");
  do_http_request("608dev-2.net", request_buffer, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false);
  Serial.println(response);
  if (strstr(response, "wait") != NULL)
  {
    return false;
  }
  deserializeJson(doc, response);
  player_hp = doc["move1"]["player_hp"];
  opponent_hp = doc["move1"]["opponent_hp"];
  player_new_hp = doc["move2"]["player_hp"];
  opponent_new_hp = doc["move2"]["opponent_hp"];
  strcpy(display_text[0], doc["move1"]["display_text"]);
  strcpy(display_text[1], doc["move2"]["display_text"]);
  return true;
}

void drawArrayJpeg(const uint8_t arrayname[], uint32_t array_size, int xpos, int ypos)
{

  int x = xpos;
  int y = ypos;

  JpegDec.decodeArray(arrayname, array_size);
  jpegInfo();
  renderJPEG(x, y, &tft);
}

///////// CAPTURE //////////

char hex(int x)
{
  return x >= 10 ? x - 10 + 'a' : x + '0';
}

char HEXX(int x)
{
  return x >= 10 ? x - 10 + 'A' : x + '0';
}

int wifi_object_builder(char *object_string, uint32_t os_len, uint8_t channel, int signal_strength, uint8_t *mac_address)
{
  int offset = 0;
  offset += sprintf(object_string + offset, "{\"macAddress\": \"");
  for (int i = 0; i < 6; i++)
  {
    int x = mac_address[i] >> 4;
    int y = mac_address[i] & 15;
    offset += sprintf(object_string + offset, "%c%c", hex(x), hex(y));
    if (i < 5)
      offset += sprintf(object_string + offset, ":");
  }
  offset += sprintf(object_string + offset, "\",\"signalStrength\": %d,\"age\": 0,\"channel\": %d}", signal_strength, channel);
  if (offset > os_len)
  {
    object_string[0] = '\0';
    return 0;
  }
  return offset;
}

char *SERVER = "googleapis.com"; // Server URL

void catch_request(char *uid, char *RESPONSE)
{
  memset(json_body, 0, sizeof json_body);
  int offset = sprintf(json_body, "%s", PREFIX);
  int n = WiFi.scanNetworks();
  if (n == 0)
  {
    Serial.println("no networks found");
    return;
  }
  else
  {
    int max_aps = max(min(MAX_APS, n), 1);
    for (int i = 0; i < max_aps; ++i)
    {                                                                                                                           // for each valid access point
      uint8_t *mac = WiFi.BSSID(i);                                                                                             // get the MAC Address
      offset += wifi_object_builder(json_body + offset, JSON_BODY_SIZE - offset, WiFi.channel(i), WiFi.RSSI(i), WiFi.BSSID(i)); // generate the query
      if (i != max_aps - 1)
      {
        offset += sprintf(json_body + offset, ","); // add comma between entries except trailing.
      }
    }
    sprintf(json_body + offset, "%s", SUFFIX);
    int len = strlen(json_body);
    // Make a HTTP request:
    request[0] = '\0'; // set 0th byte to null
    offset = 0;        // reset offset variable for sprintf-ing
    offset += sprintf(request + offset, "POST https://www.googleapis.com/geolocation/v1/geolocate?key=%s  HTTP/1.1\r\n", API_KEY);
    offset += sprintf(request + offset, "Host: googleapis.com\r\n");
    offset += sprintf(request + offset, "Content-Type: application/json\r\n");
    offset += sprintf(request + offset, "cache-control: no-cache\r\n");
    offset += sprintf(request + offset, "Content-Length: %d\r\n\r\n", len);
    offset += sprintf(request + offset, "%s\r\n", json_body);
    do_https_request(SERVER, request, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false);
    StaticJsonDocument<500> doc;
    char *start = strchr(response, '{');
    char *end = strrchr(response, '}');
    response[*end + 1] = '\0';
    deserializeJson(doc, start);
    Serial.println("deserializeJson");
    lat = doc["location"]["lat"];
    lng = doc["location"]["lng"];
  }

  sprintf(request_body, "user=%d&lat=%lf&lon=%lf&cipher=%s", user, lat, lng, uid);
  int len = strlen(request_body);
  request[0] = '\0'; // set 0th byte to null
  offset = 0;        // reset offset variable for sprintf-ing
  offset += sprintf(request + offset, "POST /sandbox/sc/team5/catch.py HTTP/1.1\r\n");
  offset += sprintf(request + offset, "Host: 608dev-2.net\r\n");
  offset += sprintf(request + offset, "Content-Type: application/x-www-form-urlencoded\r\n");
  offset += sprintf(request + offset, "Content-Length: %d\r\n\r\n", len);
  offset += sprintf(request + offset, "%s\r\n", request_body);
  do_http_request("608dev-2.net", request, RESPONSE, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false);
}