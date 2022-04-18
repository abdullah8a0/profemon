#ifndef capture_h
#define capture_h

// TODO: memory bugs in catch_request

const char PREFIX[] = "{\"wifiAccessPoints\": [";                 // beginning of json body
const char SUFFIX[] = "]}";                                       // suffix to POST request
const char API_KEY[] = "AIzaSyAQ9SzqkHhV-Gjv-71LohsypXUH447GWX8"; // don't change this and don't share this

const uint8_t BUTTON = 45;
const int MAX_APS = 15;

const uint16_t RESPONSE_TIMEOUT = 6000;
const uint16_t IN_BUFFER_SIZE = 5000;  // size of buffer to hold HTTP request
const uint16_t OUT_BUFFER_SIZE = 1000; // size of buffer to hold HTTP response
const uint16_t JSON_BODY_SIZE = 3000;
char request[IN_BUFFER_SIZE];
char response[OUT_BUFFER_SIZE]; // char array buffer to hold HTTP request
char json_body[JSON_BODY_SIZE];
char request_body[JSON_BODY_SIZE];

char hex(int x)
{
    return x >= 10 ? x - 10 + 'a' : x + '0';
}

void catch_request(char *uid, char *RESPONSE);

#endif