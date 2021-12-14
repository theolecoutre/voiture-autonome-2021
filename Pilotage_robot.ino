 //EN-TÊTE>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

#define DT 5 //période d'échantillonnage

#define DEFSPEED 20
#define SPD_TO_VOLT 5.0/40.0
#define COEFF_ROT 0.48 //On définit un coefficient traduisant le glissement des chenilles sur le sol (pertes) pendant que le robot pivote (estimé expérimentalement comme le rapport entre la rotation théorique et la rotation réelle observée)

//#define DW 1.18   //vitesse de rotation fixée du robot : w = (2a/l)DW = 2pi/3 rad/s => DW = 1.18 rad/s
//#define UA_TO_MPS 0.27/50.0  //quotient permettant de calculer la vitesse en m/s du robot à partir de la vitesse en unité arbitraire : on a pour DEFSPEED = 50, Vitesse = 0.27 m/s (à partir de mesures)
#define TICK_TO_DEG 0.96 //il y a 375 ticks pour un tour de roue (360 degrés)

  //paramètres géométriques du robot
float l = 115.0;
float a = 31.5;

typedef struct {
//  int mod;
  float vit;
  float ang;
} dat;

  //déclaration des variables globales utiles à la navigation
// volatile int mode;  //modes de navigation (Mode = 0 (marche normale) ; 1 (intersection) ; 2 (obstacle franchissable) ; 3 (s'arrêter) ; 4 (faire demi-tour))
volatile float vitesse;  //vitesse (arbitraire) autorisée sur la portion de route empruntée
volatile float angle;  //angle en degré

volatile int position1 = 0;
volatile int position2 = 0;

  //déclaration des fonctions
void InitMotors();
void VoltMotors(float voltM1,float voltM2);
void InitEncoders();
int getPosition1();
int getPosition2();
float getSpeed1();
float getSpeed2();
void ISR_encoder1();
void ISR_encoder2();
void WaitNextPeriod();
dat GetData(); //Fonction de récupération des données fournies par le RasberryPi
void Avancer(float vitesse); //vitesse en unités arbitraires
void Pivoter(float angle); //angle en degrés

//SETUP>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(10);
  Serial2.begin(9600);
  InitMotors();
  InitEncoders();
  vitesse=0.0;
  angle=0.0;
}


//LOOP>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

void loop() {
  WaitNextPeriod();
  dat data=GetData();
  // mode=data.mod;
  vitesse=data.vit;
  angle=data.ang;
  Serial.println(vitesse);
  //vitesse=20;
  //angle=45;
  Avancer(vitesse);
  //Pivoter(angle);
}


//FONCTIONS>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

void WaitNextPeriod() {
  static long LastMillis=0; 
  long timeToWait = DT - (millis() - LastMillis) ;
  if(timeToWait>0)
    delay(timeToWait);
  LastMillis=millis();
}


void Avancer(float vitesse) {
  if (abs(vitesse)+vitesse == 0) {
    VoltMotors(-vitesse*SPD_TO_VOLT,-vitesse*SPD_TO_VOLT);
  } else {
    VoltMotors(vitesse*SPD_TO_VOLT,vitesse*SPD_TO_VOLT);
  }
  position1 = 0;
  position2 = 0;
}


void Pivoter(float angle) {
  float omeg=0;
  if (abs(angle)+angle == 0) {
    while (omeg-angle>0) {
      VoltMotors(-DEFSPEED*SPD_TO_VOLT,DEFSPEED*SPD_TO_VOLT);
      //Serial.println(omeg); //on affiche l'angle de rotation
      delay(50);
      omeg=COEFF_ROT*(position1+position2)*TICK_TO_DEG*a/l;
    }
  } else {
    while (angle-omeg>0) {
      VoltMotors(DEFSPEED*SPD_TO_VOLT,-DEFSPEED*SPD_TO_VOLT);
      //Serial.println(omeg); //on affiche l'angle de rotation
      delay(50);
      omeg=COEFF_ROT*(position1+position2)*TICK_TO_DEG*a/l;
    }
  }
  position1 = 0;
  position2 = 0;
}
