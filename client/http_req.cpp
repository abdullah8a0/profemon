#include "http_req.h"
#include <stdint.h>
#include "Arduino.h"
#include <ArduinoJson.h>

WiFiClientSecure client; // global WiFiClient Secure object
WiFiClient client2;

typedef unsigned char uint8_t;
char *CA_CERT = "-----BEGIN CERTIFICATE-----\n"
                "MIIDdTCCAl2gAwIBAgILBAAAAAABFUtaw5QwDQYJKoZIhvcNAQEFBQAwVzELMAkG\n"
                "A1UEBhMCQkUxGTAXBgNVBAoTEEdsb2JhbFNpZ24gbnYtc2ExEDAOBgNVBAsTB1Jv\n"
                "b3QgQ0ExGzAZBgNVBAMTEkdsb2JhbFNpZ24gUm9vdCBDQTAeFw05ODA5MDExMjAw\n"
                "MDBaFw0yODAxMjgxMjAwMDBaMFcxCzAJBgNVBAYTAkJFMRkwFwYDVQQKExBHbG9i\n"
                "YWxTaWduIG52LXNhMRAwDgYDVQQLEwdSb290IENBMRswGQYDVQQDExJHbG9iYWxT\n"
                "aWduIFJvb3QgQ0EwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDaDuaZ\n"
                "jc6j40+Kfvvxi4Mla+pIH/EqsLmVEQS98GPR4mdmzxzdzxtIK+6NiY6arymAZavp\n"
                "xy0Sy6scTHAHoT0KMM0VjU/43dSMUBUc71DuxC73/OlS8pF94G3VNTCOXkNz8kHp\n"
                "1Wrjsok6Vjk4bwY8iGlbKk3Fp1S4bInMm/k8yuX9ifUSPJJ4ltbcdG6TRGHRjcdG\n"
                "snUOhugZitVtbNV4FpWi6cgKOOvyJBNPc1STE4U6G7weNLWLBYy5d4ux2x8gkasJ\n"
                "U26Qzns3dLlwR5EiUWMWea6xrkEmCMgZK9FGqkjWZCrXgzT/LCrBbBlDSgeF59N8\n"
                "9iFo7+ryUp9/k5DPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNVHRMBAf8E\n"
                "BTADAQH/MB0GA1UdDgQWBBRge2YaRQ2XyolQL30EzTSo//z9SzANBgkqhkiG9w0B\n"
                "AQUFAAOCAQEA1nPnfE920I2/7LqivjTFKDK1fPxsnCwrvQmeU79rXqoRSLblCKOz\n"
                "yj1hTdNGCbM+w6DjY1Ub8rrvrTnhQ7k4o+YviiY776BQVvnGCv04zcQLcFGUl5gE\n"
                "38NflNUVyRRBnMRddWQVDf9VMOyGj/8N7yy5Y0b2qvzfvGn9LhJIZJrglfCm7ymP\n"
                "AbEVtQwdpf5pLGkkeB6zpxxxYu7KyJesF12KwvhHhm4qxFYxldBniYUr+WymXUad\n"
                "DKqC5JlR3XC321Y9YeRq4VzW9v493kHMB65jUr9TU/Qr6cf9tveCX4XSQRjbgbME\n"
                "HMUfpIBvFSDJ3gyICh3WZlXi/EjJKSZp4A==\n"
                "-----END CERTIFICATE-----\n";


uint8_t char_append(char *buff, char c, uint16_t buff_size)
{
    int len = strlen(buff);
    if (len > buff_size)
        return false;
    buff[len] = c;
    buff[len + 1] = '\0';
    return true;
}

void do_http_request(char *host, char *request, char *response, uint16_t response_size, uint16_t response_timeout, uint8_t serial)
{
    if (client2.connect(host, 80))
    { // try to connect to host on port 80
        if (serial)
            Serial.print(request); // Can do one-line if statements in C without curly braces
        client2.print(request);
        memset(response, 0, response_size); // Null out (0 is the value of the null terminator '\0') entire buffer
        uint32_t count = millis();
        while (client2.connected())
        { // while we remain connected read out data coming back
            client2.readBytesUntil('\n', response, response_size);
            if (serial)
                Serial.println(response);
            if (strcmp(response, "\r") == 0)
            { // found a blank line!
                break;
            }
            memset(response, 0, response_size);
            if (millis() - count > response_timeout)
                break;
        }
        memset(response, 0, response_size);
        count = millis();
        while (client2.available())
        { // read out remaining text (body of response)
            char_append(response, client2.read(), response_size);
        }
        if (serial)
            Serial.println(response);
        client2.stop();
        if (serial)
            Serial.println("-----------");
    }
    else
    {
        if (serial)
            Serial.println("connection failed :/");
        if (serial)
            Serial.println("wait 0.5 sec...");
        client2.stop();
    }
}

void do_https_request(char *host, char *request, char *response, uint16_t response_size, uint16_t response_timeout, uint8_t serial)
{
    client.setHandshakeTimeout(30);
    client.setCACert(CA_CERT); // set cert for https
    if (client.connect(host, 443, 4000))
    { // try to connect to host on port 443
        if (serial)
            Serial.print(request); // Can do one-line if statements in C without curly braces
        client.print(request);
        response[0] = '\0';
        // memset(response, 0, response_size); //Null out (0 is the value of the null terminator '\0') entire buffer
        uint32_t count = millis();
        while (client.connected())
        { // while we remain connected read out data coming back
            client.readBytesUntil('\n', response, response_size);
            if (serial)
                Serial.println(response);
            if (strcmp(response, "\r") == 0)
            { // found a blank line!
                break;
            }
            memset(response, 0, response_size);
            if (millis() - count > response_timeout)
                break;
        }
        memset(response, 0, response_size);
        count = millis();
        while (client.available())
        { // read out remaining text (body of response)
            char_append(response, client.read(), response_size);
        }
        if (serial)
            Serial.println(response);
        client.stop();
        if (serial)
            Serial.println("-----------");
    }
    else
    {
        if (serial)
            Serial.println("connection failed :/");
        if (serial)
            Serial.println("wait 0.5 sec...");
        client.stop();
    }
}
