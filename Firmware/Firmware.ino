
#include "Motors.h"
#include "Encoders.h"

#define DT 5 //sampling period in milliseconds

void setup() {
  
 InitMotors();
 InitEncoders();
 
 // initialization of the serial communication.
 Serial.begin(115200);
 Serial.setTimeout(10);

 
}


void loop() {
  // Main loop 
  waitNextPeriod();

  UpdateInputSignals();
  
  //update internal signals and compute the control law

  UpdateActuator();

  LogVariables();

  setM1AVoltage(3);
  setM2AVoltage(3);
}

void UpdateInputSignals(){
  //use getSpeed() declared in Encoder.h
}
  
void UpdateActuator(){
  //use setMotorAVoltage(...) declared in Motors.h

}

void LogVariables(){
  //Use Serial.print and Serial.println
  
}

void waitNextPeriod() 
{
  static long LastMillis=0; 
  long timeToWait = DT - ( millis() - LastMillis) ;
  if(timeToWait>0)
    delay(timeToWait);
  LastMillis=millis();
}
