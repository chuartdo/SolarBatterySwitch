/* 
   Battery low voltage triggered charge controller for lead acid battery
   Enable or disable each of the following modules depend on your need by 
   uncommenting setup and loop for each module.
   
   Created by Leon Hong Chu  @chuartdo
 */  
   
#include <aws_iot_config_SDK.h>
#include <aws_iot_error.h>
#include <aws_iot_mqtt.h>
#include <aws_iot_version.h>

#define LIGHT_SENSOR_PIN A0
#define VOLT_SENSOR_PIN A1

#define R_PIN A3
#define G_PIN A4
#define B_PIN A5

#define CHARGER_RELAY 6
#define LED_STRIP_PIN 4
#define ALARM_PIN 7
#define AC_LIGHT_PIN 5
#define OPEN_BREAK_PIN 2

/*
#include <Bridge.h>
#include <BridgeServer.h>
#include <BridgeClient.h> 
BridgeServer server;
*/

int aws_state = 0;

long MOTION_COUNT = 0;
bool SECURITY_LIGHT = false;

void setup() {
  
  Serial.begin(115200);
  //while(!Serial);  
  
  ChargerSetup();
  LEDLightSetup();
  BlindsMotorSetup();

  AWSetup();
 
  //Bridge.begin();
  //Console.begin();
  //server.listenOnLocalhost();
  //server.begin();
}

void SetLEDColor(int r, int g, int b) {
    analogWrite(R_PIN, r);
    analogWrite(G_PIN, g);
    analogWrite(B_PIN, b);
}

unsigned long gCurrentTime ;

void loop() {
  gCurrentTime = millis();
  //AcceptWebInput(); 
  ChargerLoop();
  LEDLightLoop();
  BlindsMotorLoop();
  AWSLoop();
  delay(100);
 
}

/*
void XML_response(BridgeClient cl)
{    
    cl.print("<?xml version = \"1.0\" ?>");
    cl.print("<inputs>");
    // read analog pin A2
    cl.print("<analog>");
    cl.print(BatteryVoltage());
    cl.print("</analog>");
    cl.print("</inputs>");
}

void AcceptWebInput() {
    int r,g,b;
     BridgeClient client = server.accept();
    if(client) {  
      String command = client.readStringUntil('/');    

      if (command == "rgb") {
        //received string is "r.g.b"
        //parseInt "consumes" the string
        analogWrite(R_PIN, r=client.parseInt());
        analogWrite(G_PIN, g=client.parseInt());
        analogWrite(B_PIN, b=client.parseInt());
        Serial.print("RGB:");
        Serial.print(" R");
        Serial.print(r);
        Serial.print(" G: ");
        Serial.print(g);
        Serial.print(" B: ");
        Serial.println(b);    

        // turn on relay
          if (  b>=250) {
            // Turn on relay
            digitalWrite(CHARGER_RELAY,LOW);
          }else  {
            digitalWrite(CHARGER_RELAY, HIGH);
          }

         if (r > 250 ) {
            // Turn on relay
            digitalWrite(ALARM_PIN,LOW);
          } else {
            digitalWrite(ALARM_PIN, HIGH);
          }

          if (g > 250 ) {
            // Turn on relay
            digitalWrite(LED_STRIP_PIN,LOW);
          } else {
            digitalWrite(LED_STRIP_PIN, HIGH);
          }
          
      }

    if (command == "open") {
        Serial.print("* Open B *");
        OpenBlind();
      
    } else if (command == "close") {
        Serial.print("* Close B *");
        ShutBlind();

    }
    client.stop();
    }    
} 
*/

