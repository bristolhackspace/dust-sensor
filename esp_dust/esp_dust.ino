/*

This is a blend of the sparkfun data demo for the ESP8266 and the grove dust sensor demo.

If we're using an esp12 then we could do the deep sleep in between samples.

 */
#include <ESP8266WiFi.h>
#include "secrets.h"
#define DUST 2
#define TX 1 //led attached
#define MAX_CONC 20000 //this is the highest we'll ever get. Often value is much higher - due to sensor plug bouncing (module won't boot if sensor in place), or sensor on its side seems to make a difference too.

unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 1000 * 60 * 1;//sample 5 mins
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;


void setup() {
  Serial.begin(9600);
  pinMode(DUST, INPUT);
  starttime = millis();//get the current time;
  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.flush();
 Serial.end();
pinMode(TX,OUTPUT);
for(int i = 0; i < 20; i ++)
{
 //do a led pattern to show when the wifi is setup
  digitalWrite(TX,false);
  delay(100);
  digitalWrite(TX,true);
  delay(100);
}

  Serial.begin(9600);
  Serial.println("");
  Serial.println("start loop");

}

int value = 0;

void loop() {
 
  //try to make things more reliable
  if (WiFi.status() != WL_CONNECTED) 
  	ESP.restart();

  duration = pulseIn(DUST, HIGH);
  lowpulseoccupancy = lowpulseoccupancy+duration;

  //if we've reached the end of the sample time
  if ((millis()-starttime) > sampletime_ms)
  {
    ratio = lowpulseoccupancy/(sampletime_ms*10.0);  // Integer percentage 0=>100
    concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62; // using spec sheet curve
    Serial.print("concentration = ");
    Serial.print(concentration);
    Serial.println(" pcs/0.01cf");
    Serial.println("\n");
    lowpulseoccupancy = 0;
    starttime = millis();
   
    if(concentration > MAX_CONC)
    {
	Serial.println("bad value, not posting");
	return; 
    }
    
    //send the data to sparkfun
    Serial.print("connecting to ");
    Serial.println(host);
    
    // Use WiFiClient class to create TCP connections
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(host, httpPort)) {
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
    
    Serial.print("Requesting URL: ");
    Serial.println(url);
    
    // This will send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" + 
                 "Connection: close\r\n\r\n");
    delay(10);
    
    // Read all the lines of the reply from server and print them to Serial
    while(client.available()){
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }
  
    Serial.println();
    Serial.println("closing connection");
  }
}
