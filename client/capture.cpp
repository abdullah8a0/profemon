#include "capture.h"
#include "http_req.h"
#include <ArduinoJson.h>
#include "Arduino.h"

char hex(int x)
{
    return x >= 10 ? x - 10 + 'a' : x + '0';
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

// main body of code
void catch_request(char *uid, char *RESPONSE)
{
    int offset = sprintf(json_body, "%s", PREFIX);
    int n = WiFi.scanNetworks(); // run a new scan. could also modify to use original scan from setup so quicker (though older info)
    Serial.println("scan done");
    if (n == 0)
    {
        Serial.println("no networks found");
    }
    else
    {
        int max_aps = max(min(MAX_APS, n), 1);
        for (int i = 0; i < max_aps; ++i)
        {                                                                                                                             // for each valid access point
            uint8_t *mac = WiFi.BSSID(i);                                                                                             // get the MAC Address
            offset += wifi_object_builder(json_body + offset, JSON_BODY_SIZE - offset, WiFi.channel(i), WiFi.RSSI(i), WiFi.BSSID(i)); // generate the query
            if (i != max_aps - 1)
            {
                offset += sprintf(json_body + offset, ","); // add comma between entries except trailing.
            }
        }
        sprintf(json_body + offset, "%s", SUFFIX);
        Serial.println(json_body);
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
        StaticJsonDocument<200> doc;
        char *start = strchr(response, '{');
        char *end = strrchr(response, '}');

        response[*end + 1] = '\0';

        // Deserialize the JSON document
        DeserializationError error = deserializeJson(doc, start);

        // Test if parsing succeeds.
        if (error)
        {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return;
        }

        double lat = doc["location"]["lat"];
        double lng = doc["location"]["lng"];

        sprintf(request_body, "user=%s&lat=%lf&lon=%lf&cipher=%s", "andi", lat, lng, uid);
        len = strlen(request_body);

        request[0] = '\0'; // set 0th byte to null
        offset = 0;        // reset offset variable for sprintf-ing
        offset += sprintf(request + offset, "GET /sandbox/sc/team5/catch.py HTTP/1.1\r\n");
        offset += sprintf(request + offset, "Host: 608dev-2.net\r\n");
        offset += sprintf(request + offset, "Content-Type: application/x-www-form-urlencoded\r\n");
        offset += sprintf(request + offset, "Content-Length: %d\r\n\r\n", len);
        offset += sprintf(request + offset, "%s\r\n", request_body);
        do_http_request("608dev-2.net", request, RESPONSE, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false);
    }
}