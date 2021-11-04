// PIN utilisées pour l'alimentation moteur 1 (on garde pour le moment les variables initiales)
#define PWM 12
#define BI1 35
#define BI2 34


//PIN utilisées pour l'alimentation moteur 1 
#define M1_PWM 12 
#define M1_BI1 35
#define M1_BI2 34

//PIN utilisées pour l'alimentation moteur 2
#define M2_PWM 8
#define M2_BI1 36
#define M2_BI2 37

#define MAXPWM 190 // maximum duty cycle for the PWM is 255/MAXPWM
#define VOLT_TO_PWM 255.0/12.0

void InitMotors();
void InitMotors_anc();
void setMotorAVoltage_anc(float value);
void setM1AVoltage(float value);
void setM2AVoltage(float value);
