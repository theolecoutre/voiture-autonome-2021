  //entrées utilisées pour l'alimentation moteur 1 
#define M1_PWM 12
#define M1_BI1 35
#define M1_BI2 34
  //entrées utilisées pour l'alimentation moteur 2
#define M2_PWM 8
#define M2_BI1 36
#define M2_BI2 37

#define MAXPWM 190
#define VOLT_TO_PWM 255.0/12.0

//FONCTIONS>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

void InitMotors() {
 //Moteur 1
 pinMode(M1_BI1, OUTPUT);
 pinMode(M1_BI2, OUTPUT);
 pinMode(M1_PWM, OUTPUT);
 //Moteur 2
 pinMode(M2_BI1, OUTPUT);
 pinMode(M2_BI2, OUTPUT);
 pinMode(M2_PWM, OUTPUT);
}


void VoltMotors(float voltM1,float voltM2) {
  voltM1 = voltM1 * VOLT_TO_PWM;
  voltM2 = voltM2 * VOLT_TO_PWM;
  if(voltM1<0) {
    digitalWrite(M1_BI1,1);
    digitalWrite(M1_BI2,0);
  } else {
    digitalWrite(M1_BI1,0);
    digitalWrite(M1_BI2,1);
  }
  if(voltM2>0) {
    digitalWrite(M2_BI1,1);
    digitalWrite(M2_BI2,0);
  } else {
    digitalWrite(M2_BI1,0);
    digitalWrite(M2_BI2,1);
  }
  analogWrite(M1_PWM,constrain(abs(voltM1),0,MAXPWM));
  analogWrite(M2_PWM,constrain(abs(voltM2),0,MAXPWM));
}
