#include "Motors.h"


void InitMotors_anc()
{
 pinMode(BI1,OUTPUT);
 pinMode(BI2,OUTPUT);
 pinMode(PWM,OUTPUT);

}
void InitMotors()
{
 //Moteur 1
 pinMode(M1_BI1, OUTPUT);
 pinMode(M1_BI1, OUTPUT);
 pinMode(M1_PWM; OUTPUT);
 
 //Moteur 2
 pinMode(M2_BI1, OUTPUT);
 pinMode(M2_BI1, OUTPUT);
 pinMode(M2_PWM; OUTPUT);
}

// fonction permettant de gerer l'alimentation moteur (sens et amplitude)
void setM1AVoltage(float valeur)
{
  valeur = valeur * VOLT_TO_PWM;
  if(valeur<0)
  {
    digitalWrite(M1_BI1,1);
    digitalWrite(M1_BI2,0);
  }
  else
   {
    digitalWrite(M1_BI1,0);
    digitalWrite(M1_BI2,1);
  }
  analogWrite(M1_PWM,constrain(abs(valeur),0,MAXPWM));
}

void setM2AVoltage(float valeur)
{
  valeur = valeur * VOLT_TO_PWM;
  if(valeur<0)
  {
    digitalWrite(M2_BI1,1);
    digitalWrite(M2_BI2,0);
  }
  else
   {
    digitalWrite(M2_BI1,0);
    digitalWrite(M2_BI2,1);
  }
  analogWrite(M2_PWM,constrain(abs(valeur),0,MAXPWM));
}
void setMotorAVoltage_anc(float valeur)
{
  valeur = valeur * VOLT_TO_PWM;
  if(valeur<0)
  {
    digitalWrite(BI1,1);
    digitalWrite(BI2,0);
  }
  else
   {
    digitalWrite(BI1,0);
    digitalWrite(BI2,1);
  }
  analogWrite(PWM,constrain(abs(valeur),0,MAXPWM));
}
