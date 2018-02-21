/*
 * Copyright 2010-2016 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#include <aws_iot_mqtt.h>
#include <aws_iot_version.h>
#include "aws_iot_config.h"

aws_iot_mqtt_client myClient;
char JSON_buf[100];
bool success_connect = false;

bool print_log(const char* src, int code) {
  bool ret = true;
  if(code == 0) {
    #ifdef AWS_IOT_DEBUG
      Serial.print(F("[LOG] command: "));
      Serial.print(src);
      Serial.println(F(" completed."));
    #endif
    aws_state +=1;
    ret = true;
  }
  else {
    #ifdef AWS_IOT_DEBUG
      Serial.print(F("[ERR] command: "));
      Serial.print(src);
      Serial.print(F(" code: "));
      Serial.println(code);
    #endif
    ret = false;
  }
  Serial.flush();
  return ret;
}

// Extract value string from specified key in single level Json array
String parse_value(char* src, char* key) {
    String keyVal;
    String result ="";
    keyVal += src;
    int keyPos = keyVal.indexOf(key);
    if (keyPos > 0) {
      int dividerPos = keyVal.indexOf(",",keyPos);
      if (dividerPos < keyPos)
        dividerPos = keyVal.lastIndexOf("}");
      int colonPos = keyVal.indexOf(":",keyPos);
      result = keyVal.substring(colonPos+1, dividerPos);
      Serial.println("Parsed " + result);
    }
    return result;
}

void activate_relay(char* src, int pinNum, char* key) { 
      String result = parse_value(src,key);
      result.toLowerCase();         
      if (result.indexOf("on") > 0)
         digitalWrite(pinNum,HIGH);
      if (result.indexOf("off")> 0)
         digitalWrite(pinNum,LOW);      
}

void process_key_value(char* src) {
  
   activate_relay(src, LED_STRIP_PIN,"led");
   activate_relay(src, AC_LIGHT_PIN,"light");
   activate_relay(src, ALARM_PIN,"alarm");

   //Set position of the blinds
   int bpos;
   String result = parse_value(src, "BlindsPos");
   if (  result.length() > 0 ) {
     bpos = result.toInt();
     SetBlindsRotation(bpos);
   }
}

void msg_callback_delta(char* src, unsigned int len, Message_status_t flag) {
  char buff[100];

  Serial.print("IOT Callback received");
  if(flag == STATUS_NORMAL) {
    
    // Get the whole delta section and update Arduino relays and states
    print_log("getDeltaKeyValue", myClient.getDeltaValueByKey(src, "", JSON_buf, 100));
    Serial.println(JSON_buf);
    process_key_value(JSON_buf);

    // Append sensor status info at end of delta JSON and 
    JSON_buf[strlen(JSON_buf) -1] = ',';
    sprintf (  buff, "%s\"ChargeCount\": %d,\"Voltage\": %s}",
        JSON_buf, chargeCount, String(BatteryVoltage()).c_str());
   // sprintf (  buff, "%s\"BlindsPos\": %d,\"Voltage\": %s}",
   //     JSON_buf, GetBlindsRotation(), String(BatteryVoltage()).c_str());
    Serial.println(buff);
    
    // Update reported Things Sadowed section with new status and state values
    String payload = "{\"state\":{\"reported\":";
    payload += buff; //JSON_buf; buff;
    payload += "}}";
    payload.toCharArray(JSON_buf, 100);
    //Serial.println(JSON_buf);
    print_log("update shadow", myClient.shadow_update(AWS_IOT_MY_THING_NAME, JSON_buf, strlen(JSON_buf), NULL, 5));
  }
}


void SetupIOT() { 
     if(print_log("setup", myClient.setup(AWS_IOT_CLIENT_ID))) {
         if(print_log("config", myClient.config(AWS_IOT_MQTT_HOST, AWS_IOT_MQTT_PORT, AWS_IOT_ROOT_CA_PATH, AWS_IOT_PRIVATE_KEY_PATH, AWS_IOT_CERTIFICATE_PATH))) {
            if(print_log("connect:", myClient.connect())) {
                success_connect = true;
  
                // Subscribe to "topic1"
                //if((rc = myClient.subscribe("status", 1, msg_callback)) != 0) {
                //  Serial.println("Subscribe failed!");
                //  Serial.println(rc);
                
                print_log("shadow init", myClient.shadow_init(AWS_IOT_MY_THING_NAME));
                print_log("register thing shadow delta function",
                     myClient.shadow_register_delta_func(AWS_IOT_MY_THING_NAME, msg_callback_delta));
            }
         }
     }
}

unsigned long lastLoopMillis;

void AWSetup() {
  char curr_version[80];
  snprintf_P(curr_version, 80, PSTR("AWS IoT SDK Version(dev) %d.%d.%d-%s\n"), VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_TAG);
  Serial.println(curr_version);
  SetupIOT();
  delay(1000);
  lastLoopMillis = millis();
}

void AWSLoop() {
  if ( millis() - lastLoopMillis < 1000) {
      return;   
  }
  lastLoopMillis = millis();
  
  if(success_connect) {
    if(myClient.yield()) {
      Serial.println(F("bad Yield failed."));  
      myClient.disconnect();
      success_connect = false;
    }
  }else {
    Serial.println(F("Re-connecting")); 
    myClient.disconnect();
    SetupIOT();
  }
}
 
