#include "Encoders.h"

volatile int position1 = 0;
volatile long duration1 = 0;
volatile long timeOfLastPulse = 0;
volatile char dir = 0;
int getPosition1()
{
  noInterrupts();
  int temp = position1;
  interrupts();
  return temp;
}

// get the actual speed, computed from the time between two pulses.
float getSpeed(){
  noInterrupts();
  long tempDur = duration1;
  char tempDir = dir; 
  interrupts();
  if(tempDur == 0)
    return 0;
  else 
    if(dir>0)
      return 1000000.0/( (float)tempDur );
    else 
      return -1000000.0/( (float)tempDur );
    
}

void InitEncoders()
{ 
  pinMode(PORT1_NE2,INPUT_PULLUP);
  pinMode(PORT1_NE1,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PORT1_NE2),ISR_encoder1,RISING);
}


// fonction appelée lors des interruptions 'front montant'
void ISR_encoder1(){
  long newTime = micros();
  if(digitalRead(PORT1_NE1)) // detection du sens de rotation
    {
      position1++;
      dir = 1;
    }
    else
    {
      position1--;
      dir = -1;
    }
  duration1 = newTime - timeOfLastPulse;
  timeOfLastPulse = newTime;
}
