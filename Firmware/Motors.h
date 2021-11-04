// PIN utilisées pour l'alimentation moteur
#define PWM 12
#define BI1 35
#define BI2 34

#define MAXPWM 190 // maximum duty cycle for the PWM is 255/MAXPWM
#define VOLT_TO_PWM 255.0/12.0

void InitMotors();
void setMotorAVoltage(float value);
