#include <Stepper.h>
/* 
 Controls Windows' Blinds motor
 Manual button to control blinds
 
 * Modify stepper motor settings based on your device.
 * 
 * This file is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. 
 * 
 * Created by Leon Hong Chu  @chuartdo
 */
#define UP_BUTTON_PIN 8
#define DOWN_BUTTON_PIN 13

#define POSITION_SLIDER_PIN A2

#define STEPS_PER_REVOLUTION 64
#define MOTOR_INC_STEPS 300

// Set the number of motor steps to fully turn blinds from closed facing down to facing up.
#define MAX_STEP 20000  


const int OpenPosition = MAX_STEP / 2;   // Mark the max motor position can be set manully
int BlindsPosition = OpenPosition;  // current blinds postion defult to open on start up
int ClosedUpPosition = MAX_STEP;
int ClosedDownPosition = 0;

int DesiredPosition = BlindsPosition;
int SliderPosition = 50;

bool AutoBlindsControl = false;  // Turn on / off blinds based on light sensor

// initialize the stepper library on pins 8 through 11:
// Pin connection depends on type of motor used
Stepper myStepper(STEPS_PER_REVOLUTION, 9,11,10,12);

void StopMotor() {
  digitalWrite(9,LOW);
  digitalWrite(10,LOW);
  digitalWrite(11,LOW);
  digitalWrite(12,LOW);
}

void BlindsMotorSetup() {   
   pinMode(UP_BUTTON_PIN, INPUT);
   pinMode(DOWN_BUTTON_PIN, INPUT);
   pinMode(POSITION_SLIDER_PIN, INPUT);

   myStepper.setSpeed(0);
}

void SetDesiredPosition(int pos) {
          SetLEDColor(0,255,0);
          delay(500);
          SetLEDColor(0,0,0);
          DesiredPosition = pos;
          Serial.print(F("Reset Open Position"));
          Serial.println(pos);
}

void BlindsMotorLoop() {
  
   
   int direc = 0;

   if (AutoBlindsControl) {
      if (isNight) 
        ShutBlind();
   }

  // Manul slider switch to turn blinds
 
   int input_position = map(analogRead(POSITION_SLIDER_PIN),0,1024,0,100);
   if (input_position !=  SliderPosition) {
       SliderPosition = input_position;
        Serial.print(F(" Slider Position "));
        Serial.println(SliderPosition);
        SetBlindsRotation(SliderPosition);
   }

   // When both manual buttons are pressed at the same time, 
   // reset current position to open position (blinds' blade are level)
   if (digitalRead(UP_BUTTON_PIN) == HIGH) {
       if (digitalRead(DOWN_BUTTON_PIN) == HIGH) {
          //ClosedUpPosition = BlindsPosition;
          BlindsPosition = OpenPosition;
          return;
       } 
       TurnStepMotor(MOTOR_INC_STEPS);
       return;
   
   } else if (digitalRead(DOWN_BUTTON_PIN) == HIGH) {
       if (digitalRead(UP_BUTTON_PIN) == HIGH) {
         // ClosedDownPosition = BlindsPosition;
          BlindsPosition = OpenPosition;
          return;
       }     
        TurnStepMotor(-MOTOR_INC_STEPS);
       return;
   } else {
     if ( DesiredPosition - BlindsPosition > MOTOR_INC_STEPS)
        direc = 1;
     else if (BlindsPosition - DesiredPosition > MOTOR_INC_STEPS)
        direc = -1;
     else 
        direc = 0;
   }
   TurnWindowBlinds(direc * MOTOR_INC_STEPS);
 }


void TurnStepMotor(int amount) {
  myStepper.setSpeed(MOTOR_INC_STEPS);
  myStepper.step(amount);
}

// Keep internal position of the blinds. to prevent over turning based on set boundary
// Tweek the maximum and minimum values for each stepper motor.
void TurnWindowBlinds (int amount) {
  if (amount == 0) {
     // turn off motor
     StopMotor();
     return;
  }
  
  Serial.print(F("Steps:"));
  Serial.print(amount);
  Serial.print(F(" pos: "));
  Serial.print(BlindsPosition);
  Serial.print(F(" desired: "));
  Serial.println(DesiredPosition);
  myStepper.setSpeed(MOTOR_INC_STEPS);
 
  BlindsPosition += amount ;
  // if amount is over, find the difference
  
  if (abs(BlindsPosition) < MAX_STEP) {
    myStepper.step(amount);
  } else {
      int direc = (BlindsPosition > 0)?1:-1;
      int diff = MAX_STEP * direc  - BlindsPosition;
      // reduce amount to fit within limit
      myStepper.step(diff);

      //Set to limit
      BlindsPosition = (MAX_STEP-1) * direc;
  }
  if (BlindsPosition < 0)
    BlindsPosition = 0;
  else if (BlindsPosition > MAX_STEP)
    BlindsPosition = MAX_STEP;
  
  delay(30);
}

void ShutBlind() {
  DesiredPosition = 0;
 }

void OpenBlind() {
  DesiredPosition = OpenPosition;
 }

void SetBlindsRotation(int rotation) {  
   if (rotation >=0 && rotation <=100)
     DesiredPosition = map(rotation, 0,100, 0, MAX_STEP);
}

// range between 0 and 100. 0 is blinds fully turned downward  100 is blinds turned upward
int GetBlindsRotation() {
   return map(BlindsPosition, 0, MAX_STEP, 0,100);
}


