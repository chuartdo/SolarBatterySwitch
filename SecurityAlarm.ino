#define LIGHT_ON_MS 30000

/* Turn on warning LED light, alarm switch if circuit break switch is triggered */

bool useAlarm = false;
bool circuitOpen = false;
bool securityReset = false;
bool manualLightOn = false;

unsigned long triggeredMillis;
unsigned long lightOnTime;

void LEDLightSetup() {
  
  pinMode(LED_STRIP_PIN, OUTPUT);
  pinMode(ALARM_PIN, OUTPUT);
  pinMode(AC_LIGHT_PIN, OUTPUT);
  pinMode(OPEN_BREAK_PIN, INPUT);

  // Relay Setting  HIGH = open circuit  LOW = close circuit
  digitalWrite(LED_STRIP_PIN,HIGH);
  digitalWrite(AC_LIGHT_PIN,HIGH);
  digitalWrite(ALARM_PIN,HIGH);

 // attachInterrupt(OPEN_BREAK_PIN, CircuitBreak, FALLING);
}


void CircuitBreak() {
   Serial.print(F(" =BREAK= "));
   circuitOpen = true;
   triggeredMillis = millis();
}

/*
void SimpleLightLoop() {
 if (digitalRead(OPEN_BREAK_PIN) != HIGH) {
      digitalWrite(LED_STRIP_PIN, LOW);
      analogWrite(G_PIN,150);
      Serial.print(" =BREAK= ");

 }
 else {
       digitalWrite(LED_STRIP_PIN, HIGH);
       analogWrite(G_PIN,40);
        Serial.print(" =  ");
 }
}
*/
 void LEDLightLoop() {
   if (!circuitOpen && digitalRead(OPEN_BREAK_PIN) != HIGH) 
      CircuitBreak();   
  
      
  if ( circuitOpen ) {
       // turn on relay
         analogWrite(R_PIN,50);

      if (isNight) {
        digitalWrite(LED_STRIP_PIN, LOW);
        analogWrite(G_PIN,50);
        lightOnTime = millis();
        
        if (!securityReset &&  millis() - triggeredMillis > 5000) {
           securityReset = true;
           digitalWrite(ALARM_PIN, LOW);  
           analogWrite(R_PIN,80);  
        }
      }

      // Turn off light if breaker is closed
      if (digitalRead(OPEN_BREAK_PIN) == HIGH) {
          circuitOpen = false;
          analogWrite(R_PIN,0);  
      }
      
      // Turn off LED light strip after fixed interval
      if ( digitalRead(LED_STRIP_PIN) == LOW) {
            if ( millis() - lightOnTime > LIGHT_ON_MS) {    // Set ON time
              digitalWrite(LED_STRIP_PIN, HIGH);
              analogWrite(G_PIN,0);
              lightOnTime = gCurrentTime;
              ResetSecurity();
            }
      }

  } else {

    // Turn light off after fixed time
    if ( digitalRead(LED_STRIP_PIN) == LOW) {
        if ( millis() - lightOnTime > LIGHT_ON_MS) {    // Set ON time
          digitalWrite(LED_STRIP_PIN, HIGH);
          analogWrite(G_PIN,0);
          lightOnTime = gCurrentTime;
        }
    }
  }
    
}


void ResetSecurity() {
    Serial.print(F(" RE-SET "));
    circuitOpen = false;
    securityReset = false;
    digitalWrite(ALARM_PIN, 0);
    analogWrite(R_PIN, 0);
}

