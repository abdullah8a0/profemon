#include "pairing.h"
#include "http_req.h"
#include <WiFi.h>

bool connect_wifi()
{
    WiFi.begin("MIT Guest", ""); // attempt to connect to wifi
    uint8_t count = 0;           // count used for Wifi check times
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
        // Serial.printf("%d:%d:%d:%d (%s) (%s)\n", WiFi.localIP()[3], WiFi.localIP()[2],
        //               WiFi.localIP()[1], WiFi.localIP()[0],
        //               WiFi.macAddress().c_str(), WiFi.SSID().c_str());
        return true;
    }
    else
    { // if we failed to connect just Try again.
        // Serial.println("Failed to Connect :/  Going to restart");
        // Serial.println(WiFi.status());
        return false;
    }
}

bool broadcast(char *my_id)
{ // TODO: boadcast while syncing. stop boradcasting if sync is done.

    char *ssid = (char *)malloc(sizeof(char) * 13);
    strcpy(ssid, "Profemon");
    strcat(ssid, my_id);

    char wifi_pass[] = "Profemon";

    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, wifi_pass);
    int start_time = millis();
    while (millis() - start_time < HOSTING_TIMEOUT_MS)
    {
        //     WiFiClient client = server.available();
        //     int already_connected = 0;
        //     if (!already_connected)
        //     {
        //         // clear out the input buffer:
        //         client.flush();
        //         Serial.println("We have a new client");
        //         client.println("Hello, client!");
        //         already_connected = 1;
        //     }
        //     char client_id[5];
        //     for (int i = 0; i < 5; i++) // get 4 char code from client
        //     {
        //         if (client.available())
        //         {
        //             char c = client.read();
        //             client_id[i] = c;
        //             Serial.print(c);
        //         }
        //         else
        //         {
        //             Serial.println("Client didn't send complete code");
        //             break;
        //         }
        //     }

        //     if (client_id[4] != '\0')
        //     {
        //         Serial.println("Client sent incomplete code");
        //         client.println("Client sent incomplete code");
        //     }
        //     else
        //     {
        //         Serial.println("Client sent complete code");
        //         client.println("Client sent complete code");
        //     }

        //     // paste the code into the other_id
        //     for (int i = 0; i < 4; i++)
        //     {
        //         other_id[i] = client_id[i];
        //     }
        //     return true
    }
    free(ssid);
    if (connect_wifi())
    {

        return true;
    }
    else
    {

        return false;
    }
}

bool listen(char *other_id)
{
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    char wifi_pass[] = "Profemon";
    if (n <= 0)
    {
        Serial.println("no networks found");
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
    sprintf(request, "POST http://608dev-2.net/sandbox/sc/team5/temp.py HTTP/1.1\r\n");
    strcat(request, "Host: 608dev-2.net\r\n");
    strcat(request, "Content-Type: application/x-www-form-urlencoded\r\n");
    sprintf(request + strlen(request), "Content-Length: %d\r\n", body_len); // append string formatted to end of request buffer
    strcat(request, "\r\n");                                                // new line from header to body
    strcat(request, body);                                                  // body
    strcat(request, "\r\n");                                                // new line
    Serial.println(request);
    do_http_request("608dev-2.net", request, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
    Serial.println(response); // viewable in Serial Terminal
}

void sync_ids(char *my_id, char *game_id)
{
    char body[100];                // for body
    sprintf(body, "me=%s", my_id); // generate body, posting to User, 1 step
    int body_len = strlen(body);   // calculate body length (for header reporting)
    sprintf(request, "POST http://608dev-2.net/sandbox/sc/team5/sync.py HTTP/1.1\r\n");
    strcat(request, "Host: 608dev-2.net\r\n");
    strcat(request, "Content-Type: application/x-www-form-urlencoded\r\n");
    sprintf(request + strlen(request), "Content-Length: %d\r\n", body_len); // append string formatted to end of request buffer
    strcat(request, "\r\n");                                                // new line from header to body
    strcat(request, body);                                                  // body
    strcat(request, "\r\n");                                                // new line
    Serial.println(request);
    do_http_request("608dev-2.net", request, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
    Serial.println(response); // viewable in Serial Terminal
    // copy respose to game_id
    // game id can be arbitrary length
    for (int i = 0; i < strlen(response); i++)
    {
        game_id[i] = response[i];
    }
}
