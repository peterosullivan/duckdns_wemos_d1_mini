/*
    HTTP over TLS (HTTPS) example sketch
    This example demonstrates how to use
    WiFiClientSecure class to access HTTPS API.
    We fetch and display the status of
    esp8266/Arduino project continuous integration
    build.
    Limitations:
        only RSA certificates
        no support of Perfect Forward Secrecy (PFS)
        TLSv1.2 is supported since version 2.4.0-rc1
    Created by Ivan Grokhotkov, 2015.
    This example is in public domain.
*/

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include "secret.h"

const unsigned long SECOND = 1000;
const unsigned long HOUR = 3600 * SECOND;
const char* host = "www.duckdns.org";
const int httpPort = 80;

void setup()
{
    Serial.begin(115200);
    delay(10);
    Serial.println();
    Serial.print("connecting to ");
    Serial.println(ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    if (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
        Serial.printf("WiFi Failed!\n");
        return;
    }

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}

void loop()
{
    // Use WiFiClientSecure class to create TLS connection
    WiFiClient client;
    Serial.print("connecting to ");
    Serial.println(host);

    if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
        delay(2000);
        return;
    }

    //String url = "/update?domains=" << duckdns_subdomain << "&token=" << duckdns_token;

    char url[100];
    sprintf(url, "/update?domains=%s&token=%s", duckdns_subdomain, duckdns_token);

    Serial.print("requesting URL: ");
    Serial.println(url);
        
    client.print(String("GET ") + url + " HTTP/1.1\r\n"
                + "Host: " + host + "\r\n"
                + "User-Agent: BuildFailureDetectorESP8266\r\n"
                + "Connection: close\r\n\r\n"
                + "Accept: */*"
                );

    Serial.println("request sent");

    unsigned long timeout = millis();
    while (client.connected())
    {
        if (millis() - timeout > 5000) break;
        String line = client.readStringUntil('\n');
        Serial.println(line);
    }

    Serial.println("closing connection");
    delay(1 * HOUR);
}