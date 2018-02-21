/* 
 Control charger based on voltage

 Voltage sensor
 * Ajustment of voltage scale factor required for accurate voltage reading of your device. 
 * 
 * This file is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. 
 * 
 * Created by Leon Hong Chu  @chuartdo
 */
#define ACTIVATE_CHARGE_VOLTAGE 12.2
#define CHARGE_CUTOFF_VOLTAGE 14.7


unsigned long relayOnTime=0;

// Light detector
int LightSensorVal = 0;

bool isNight = false;
String startString;
long previousTime = 0;
int chargeCount = 0;
float voltage;


void ChargerSetup() {
  pinMode(R_PIN, OUTPUT);
  pinMode(G_PIN, OUTPUT);
  pinMode(B_PIN, OUTPUT);
  pinMode(CHARGER_RELAY, OUTPUT);
  pinMode(LIGHT_SENSOR_PIN, INPUT);
  pinMode(VOLT_SENSOR_PIN, INPUT);
}


short spikeCheck = 0;
void ActivateCharger() {
  spikeCheck+=1;
  if (spikeCheck > 5) {
    digitalWrite(CHARGER_RELAY, LOW);
    relayOnTime = gCurrentTime;
    analogWrite(B_PIN,155);
    spikeCheck = 0;
    chargeCount ++;
  }
}

void ChargerLoop() {

  if ( gCurrentTime - previousTime > 1000) {
    previousTime = gCurrentTime;
    
    // Read Sensors 
    voltage = BatteryVoltage();
    LightSensorVal = analogRead(LIGHT_SENSOR_PIN);

    isNight = LightSensorVal > 500;

    // Turn on charger to prevent low voltage at night. 
    //Charge only if voltage sensor is connected to battery terminal   
    if (isNight && voltage < ACTIVATE_CHARGE_VOLTAGE && voltage > 3) {
        ActivateCharger();
    } 

    // Turn off charger to prevent over charge
    if (voltage >  CHARGE_CUTOFF_VOLTAGE || gCurrentTime - relayOnTime > 7*60000  ) {
      digitalWrite(CHARGER_RELAY,HIGH);
      analogWrite(B_PIN,0);
    }

    // Security relay
    if (SECURITY_LIGHT) {
      // analogWrite(Relay, 255);
      // relayOnTime = gCurrentTime;
      analogWrite(G_PIN,155);
      // turn on relay
      Serial.print(F("Turn on light for 2 min"));
      SECURITY_LIGHT = false;
    }
    else {
       analogWrite(B_PIN,voltage>5?(voltage - 11.0)*2:0);
    }
    
    Serial.print(aws_state);
    Serial.print(F(" C"));
    Serial.print(chargeCount);
    Serial.print(F(" L: "));
    Serial.print(LightSensorVal);
    Serial.print(F(" V: "));
    Serial.println(voltage);    
    Serial.print(F("  :"));
    Serial.flush();

  }
}

float vin = 0.0;

float BatteryVoltage() {
    float volt = 0;
    float Calibration = 25.0;
    volt = analogRead(VOLT_SENSOR_PIN);
    vin =  volt * Calibration  / 1024.0; 
    return vin;
}



