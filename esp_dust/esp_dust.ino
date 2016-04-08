/*
This is a blend of the sparkfun data demo for the ESP8266 and the grove dust sensor demo.

If we're using an esp12 then we could do the deep sleep in between samples.

using LCD library from https://github.com/carlosefr/pcd8544
needed to change avr/pgmspace.h -> pgmspace.h to work with esp8266
PCD8544 - Interface with Philips PCD8544 (or compatible) LCDs.
Copyright (c) 2010 Carlos Rodrigues <cefrodrigues@gmail.com>
*/

#include <pgmspace.h>
#include <PCD8544.h>
#include <ESP8266WiFi.h>
#include "secrets.h"
#define DUST 16
#define MAX_CONC 500000 //this is the highest we'll ever get. Value can be much higher if sensor on its side.

#define SAMPLETIME_MS 30000 //sample every 30 secs
#define LCD_UPDATE_TIME 5000

// eeprom addresses (storing 2byte ints so each address is +2)
#define EEP_WIFI_CONN 0
#define EEP_REBOOTS 2 

// state machine
enum states
{
    NOT_CONNECTED,
    SAMPLING,
    CHECK_WIFI,
    POSTING,
};

static PCD8544 lcd(5,4,12,13,14);

states state = NOT_CONNECTED;
int last_state = -1;

void setup()
{
    lcd.begin(84, 48);
    lcd.setContrast(70);

    Serial.begin(9600);
    Serial.println();
    Serial.println();

    // update number of times rebooted
    setupEEPROM();
    EEPROMWriteInt(EEP_REBOOTS, EEPROMReadInt(EEP_REBOOTS) + 1);

    // pin modes
    pinMode(DUST, INPUT);
}

void loop()
{
    static unsigned long start_time;
    static unsigned long lcd_time;
    static long lowpulseoccupancy;

    if(last_state != state)
    {
        last_state = state;
        update_lcd(lowpulseoccupancy);
    }
    if((millis() - lcd_time) > LCD_UPDATE_TIME)
    {
        lcd_time = millis();
        update_lcd(lowpulseoccupancy);
    }

    switch(state)
    {
        case NOT_CONNECTED:
        {
            Serial.println("not connected");
            start_wifi();
            EEPROMWriteInt(EEP_WIFI_CONN, EEPROMReadInt(EEP_WIFI_CONN) + 1);
            state = SAMPLING;
            break;
        }
        case SAMPLING:
        {
//            Serial.println("sampling");
            if((millis() - start_time) > SAMPLETIME_MS)
                state = CHECK_WIFI;
            else
                yield();
                //wait for low pulse, timeout after 800ms seems reilable
                //WDT is 1000ms
                //http://bbs.espressif.com/viewtopic.php?f=7&t=203
                lowpulseoccupancy += pulseIn(DUST, LOW, 800*1000);
                yield();
            break;
        }
        case CHECK_WIFI:
        {
            Serial.println("check wifi");
            if(WiFi.status() != WL_CONNECTED) 
                state = NOT_CONNECTED;
            else
                state = POSTING;
            break;
        }
        case POSTING:
        {
            Serial.println("posting");
            post(lowpulseoccupancy);
            start_time = millis();
            lowpulseoccupancy = 0;
            state = SAMPLING;
            break;
        }
    }

    delay(5);

}

void post(long lowpulseoccupancy)
{
    Serial.print("lpo=");
    Serial.println(lowpulseoccupancy/1000000);
    float ratio = lowpulseoccupancy/(SAMPLETIME_MS*10.0);  // Integer percentage 0=>100
    Serial.print("ratio=");
    Serial.println(ratio);
    float concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62; // using spec sheet curve
    Serial.print("concentration = ");
    Serial.print(concentration);
    Serial.println(" pcs/0.01cf");
    Serial.println("\n");
    /*
    if(concentration > MAX_CONC)
    {
        Serial.println("bad value, not posting");
        return; 
    }
    */
    //send the data to sparkfun
    Serial.print("connecting to ");
    Serial.println(host);
    
    // Use WiFiClient class to create TCP connections
    WiFiClient client;
    const int httpPort = 80;
    if(!client.connect(host, httpPort)) 
    {
        Serial.println("connection failed");
        return;
    }
    
    // We now create a URI for the request
    String url = "/input/";
    url += streamId;
    url += "?private_key=";
    url += privateKey;
    url += "&dust=";
    url += concentration;
    url += "&num_boots=";
    url += EEPROMReadInt(EEP_REBOOTS);
    url += "&num_connects=";
    url += EEPROMReadInt(EEP_WIFI_CONN);
    
    Serial.print("Requesting URL: ");
    Serial.println(url);
    
    // This will send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" + 
                 "Connection: close\r\n\r\n");
    delay(10);
    
    // Read all the lines of the reply from server and print them to Serial
    while(client.available())
    {
        String line = client.readStringUntil('\r');
        Serial.print(line);
    }
  
    Serial.println();
    Serial.println("closing connection");
}

void start_wifi()
{
    WiFi.mode(WIFI_STA);
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(500);
        Serial.print(".");
        update_lcd(0);
    }

    Serial.println("");
    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    WiFi.mode(WIFI_STA);
    Serial.println(WiFi.localIP());
}
