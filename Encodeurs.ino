  //entrées utilisées pour les encodeurs incrémentaux
#define PORT1_NE1 31
#define PORT1_NE2 18

#define PORT2_NE1 38
#define PORT2_NE2 19

//volatile int position1 = 0;
volatile long duration1 = 0;
volatile long timeOfLastPulse1 = 0;
volatile char dir1 = 0;
//volatile int position2 = 0;
volatile long duration2 = 0;
volatile long timeOfLastPulse2 = 0;
volatile char dir2 = 0;

//FONCTIONS>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

void InitEncoders() { 
  pinMode(PORT1_NE2,INPUT_PULLUP);
  pinMode(PORT1_NE1,INPUT_PULLUP);
  pinMode(PORT2_NE2,INPUT_PULLUP);
  pinMode(PORT2_NE1,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PORT1_NE2),ISR_encoder1,RISING);
  attachInterrupt(digitalPinToInterrupt(PORT2_NE2),ISR_encoder2,RISING);
}


int getPosition1() {
  noInterrupts();
  int temp1 = position1;
  interrupts();
  return temp1;
}


int getPosition2() {
  noInterrupts();
  int temp2 = position2;
  interrupts();
  return temp2;
}


float getSpeed1() {
  noInterrupts();
  long tempDur1 = duration1;
  char tempDir1 = dir1; 
  interrupts();
  if(tempDur1 == 0)
    return 0;
  else 
    if(dir1>0)
      return 1000000.0/( (float)tempDur1 );
    else 
      return -1000000.0/( (float)tempDur1 );    
}


float getSpeed2() {
  noInterrupts();
  long tempDur2 = duration2;
  char tempDir2 = dir2; 
  interrupts();
  if(tempDur2 == 0)
    return 0;
  else 
    if(dir2>0)
      return 1000000.0/( (float)tempDur2 );
    else 
      return -1000000.0/( (float)tempDur2 ); 
}

void ISR_encoder1() {
  long newTime1 = micros();
  if(digitalRead(PORT1_NE1)) {       // detection du sens de rotation
      position1++;
      dir1 = 1;
    } else {
      position1--;
      dir1 = -1;
    }
  duration1 = newTime1 - timeOfLastPulse1;
  timeOfLastPulse1 = newTime1;
}

void ISR_encoder2() {
  long newTime2 = micros();
  if(digitalRead(PORT2_NE1)) {       // detection du sens de rotation
      position2++;
      dir2 = 1;
    } else {
      position2--;
      dir2 = -1;
    }
  duration2 = newTime2 - timeOfLastPulse2;
  timeOfLastPulse2 = newTime2;
}
