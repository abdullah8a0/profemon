#ifndef capture_h
#define capture_h

// TODO: memory bugs in catch_request

const char PREFIX[] = "{\"wifiAccessPoints\": [";                 // beginning of json body
const char SUFFIX[] = "]}";                                       // suffix to POST request
const char API_KEY[] = "AIzaSyAQ9SzqkHhV-Gjv-71LohsypXUH447GWX8"; // don't change this and don't share this

const int MAX_APS = 15;

char hex(int x);

void catch_request(char *uid, char *RESPONSE);

#endif