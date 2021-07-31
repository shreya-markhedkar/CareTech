#include <ESP8266WiFi.h>;

#include <WiFiClient.h>;

#include <ThingSpeak.h>;

#include <Wire.h>
#include "MAX30100_PulseOximeter.h"


 
#define REPORTING_PERIOD_MS 1000

 
// Connections : SCL PIN - D1 , SDA PIN - D2 , INT PIN - D0
PulseOximeter pox;
 
int st;
float BPM, SpO2;
uint32_t tsLastReport = 0;
 
const unsigned char bitmap [] PROGMEM=
{
0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x18, 0x00, 0x0f, 0xe0, 0x7f, 0x00, 0x3f, 0xf9, 0xff, 0xc0,
0x7f, 0xf9, 0xff, 0xc0, 0x7f, 0xff, 0xff, 0xe0, 0x7f, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xf0,
0xff, 0xf7, 0xff, 0xf0, 0xff, 0xe7, 0xff, 0xf0, 0xff, 0xe7, 0xff, 0xf0, 0x7f, 0xdb, 0xff, 0xe0,
0x7f, 0x9b, 0xff, 0xe0, 0x00, 0x3b, 0xc0, 0x00, 0x3f, 0xf9, 0x9f, 0xc0, 0x3f, 0xfd, 0xbf, 0xc0,
0x1f, 0xfd, 0xbf, 0x80, 0x0f, 0xfd, 0x7f, 0x00, 0x07, 0xfe, 0x7e, 0x00, 0x03, 0xfe, 0xfc, 0x00,
0x01, 0xff, 0xf8, 0x00, 0x00, 0xff, 0xf0, 0x00, 0x00, 0x7f, 0xe0, 0x00, 0x00, 0x3f, 0xc0, 0x00,
0x00, 0x0f, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
int button = 16; // push button is connected
int temp = 0;    // temporary variable for reading the button pin status
int dt=100;

const char* ssid = ""; //Your Network SSID

const char* password = ""; //Your Network Password
WiFiClient client;

unsigned long myChannelNumber = 1460131; //Your Channel Number (Without Brackets)

const char * myWriteAPIKey = "ML0XI3SPOYBMLBXZ"; //Your Write API Key ML0XI3SPOYBMLBXZ

void onBeatDetected()
{
    Serial.println("Beat Detected!");
    
} 

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
pinMode(A0,INPUT);
pinMode(button, INPUT); // declare push button as input
delay(10);
 Serial.print("Initializing Pulse Oximeter..");
 
    if (!pox.begin())
    {
         Serial.println("FAILED");
         
    }
    else
    {
         
         Serial.println("SUCCESS");
         pox.setOnBeatDetectedCallback(onBeatDetected);
    }

// Connect to WiFi network

WiFi.begin(ssid, password);



ThingSpeak.begin(client);

}

void steps(){
  st = random(0,10);
  ThingSpeak.writeField(myChannelNumber, 3, temp, myWriteAPIKey);
}

void emg(){
  temp = digitalRead(button);
  Serial.println(temp);
  ThingSpeak.writeField(myChannelNumber, 6, temp, myWriteAPIKey);

}

void beat(){
    pox.update();
    
 
    BPM = pox.getHeartRate();
    SpO2 = pox.getSpO2();
     if (millis() - tsLastReport > REPORTING_PERIOD_MS)
    {
        Serial.print("Heart rate:");
        Serial.print(BPM);
        Serial.print(" bpm / SpO2:");
        Serial.print(SpO2);
        Serial.println(" %");
 
       
        tsLastReport = millis();
    }
    ThingSpeak.writeField(myChannelNumber, 1, BPM, myWriteAPIKey);
    ThingSpeak.writeField(myChannelNumber, 2, SpO2, myWriteAPIKey);
}
void loop() {
  // put your main code here, to run repeatedly:
beat();
emg();
steps();
}
