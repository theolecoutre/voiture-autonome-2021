
#include "Motors.h"
#include "Encoders.h"

#define DT 5 //sampling period in milliseconds

void setup() {
  
 InitMotors();
 InitEncoders();
 
 // initialization of the serial communication.
 Serial.begin(115200);
 Serial.setTimeout(10);
  
 //Définition de Variables envoyé par la caméra
  int obstacle =  0; //Lorsque la caméra détecte un obstacle 
  int obstaclePosition = 0; //Savoir si l'obstacle se trouve à gauche ou à droite 
  int vitesseMax = 3; // Deux mode : Zone 50 (avec 3V) et Zone 30 (avec 2V)
  int arret = 0; // On va attendre la passage d'un piéton
  
}


void loop() {
  // Main loop 
  waitNextPeriod();

  UpdateInputSignals();
  
  //update internal signals and compute the control law

  UpdateActuator();

  LogVariables();

  setM1AVoltage(-3,vitesseMax);
  setM2AVoltage(vitesseMax);
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
