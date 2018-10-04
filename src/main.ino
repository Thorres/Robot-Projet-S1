/*
Projet: Déplacement Robot B Défi parcours
Equipe: P-10
Auteurs: Simon Torres, 
Description: Permet de compléter le défi du parcours
Date: 2018-09-27
*/

//Librairies
#include <LibRobus.h> // Essentielle pour utiliser RobUS

//Variables Globales
 float kP = 0.00085, kI = 0.00004;
 int test;
 #define opti45LEFT 50
 #define opti45RIGHT 100
 #define opti90LEFT 100
 #define opti90RIGHT -75
 #define opti180LEFT 0
 #define opti180RIGHT 0
 #define opti45LEFT2 300
 #define opti45RIGHT2 200
 #define opti90LEFT2 100
 #define opti90RIGHT2 400
 #define opti180LEFT2 300
 #define opti180RIGHT2 500
 //OPTIMISATION DES VIRAGES
 /* virage:
  - 45 LEFT : -275
  - 45 RIGHT : 50
  - 90 LEFT : 0
  - 90 RIGHT : 25
  - 180 LEFT : 225
  - 180 RIGHT : 100
  --------------------------------------------------------------------
  virage2Moteurs
  - 45 LEFT : 300
  - 45 RIGHT : 200
  - 90 LEFT : 200
  - 90 RIGHT : 400
  - 180 LEFT : 300
  - 180 RIGHT : 500

 */
/* ******************************************************************  SETUP  ************************************************** */
void setup()
{
  BoardInit();
  Serial.println("Hello");
  delay(500);
  test = 0;
  Serial.println("GO!");
}
/* ******************************************************************   LOOP   ************************************************** */
void loop() 
{
  
  if(AX_IsBumper(3)==1){
    //Aller
    //distance A
     AvancerCorrigerLONG(2.105);
     //virage B
     virage(90, LEFT, opti90LEFT);
     //B a C
     AvancerCorriger(0.31);
     //virage C
     virage(90, RIGHT, opti90RIGHT);
     //C a D
     AvancerCorriger(0.23);
     //virage D
     virage(90, RIGHT, opti90RIGHT);
     //D a E
     AvancerCorriger(0.26);
     //virage E
     virage(90, LEFT, opti90LEFT);
     //E a F
     AvancerCorriger(0.13);
     //virage F
     virage(45, RIGHT, opti45RIGHT);
     //F a G
     AvancerCorriger(0.37);
     //virage G
     virage(90, LEFT, opti90LEFT);
     //G a Ha
     AvancerCorriger(0.50);
     //virage Ha
     virage(45, RIGHT, opti45RIGHT);
     //Ha a Hb
     AvancerCorriger(0.32);
     //virage Hb
     virage(21.5, RIGHT, 0);
     //Hb a I
     AvancerCorriger(0.64);
     //virage I
     virage2Moteurs(180, LEFT, opti180LEFT2);
     //Retour
     //I a Hb
     AvancerCorriger(0.64);
     //virage Hb
     virage(21.5, LEFT, 0);
     //Hb a Ha
     AvancerCorriger(0.32);
     //virage Ha
     virage(49.5, LEFT, 0);
     //H a Ga
     AvancerCorriger(0.54);
     //virage G
     virage(89, RIGHT, 0);
     //G a F
     AvancerCorriger(0.37);
     //virage F
     virage(45, LEFT, 0);
     //F a E
     AvancerCorriger(0.13);
     //virage E
     virage(90, RIGHT, 0);
     //E a D
     AvancerCorriger(0.29);
     //virage D
     virage(92, LEFT,0);
     //D a C
     AvancerCorriger(0.23);
     //virage C
     virage(90, LEFT, 0);
     //C a B
     AvancerCorriger(0.31);
     //virage B
     virage(90, RIGHT, 0);
     //B a A
     AvancerCorrigerLONG(2.33);


  }
  if(AX_IsBumper(0/*LEFT*/)==1){
    virage(180, LEFT, 0);
  }
  if(AX_IsBumper(1)==1){
    virage(180, RIGHT, 0);
  }
  if(AX_IsBumper(2)==1){
    delay(100);
    virage2Moteurs(180, LEFT, -1000);
  }
  // SOFT_TIMER_Update(); // A decommenter pour utiliser des compteurs logiciels
  delay(10);// Delais pour décharger le CPU
}

/* ******************************************************************FONCTIONS PERSONNELLES************************************************** */
void AvancerCorrigerLONG(float distance)
{
  float VG = 0.01;
  float VD = 0.01;
  long int comptClicsATTG = 0, comptClicsREELD = 0;
  while (comptClicsATTG < (MetresToClics(distance) - 6400))
  {
    MOTOR_SetSpeed(LEFT, VG);
    MOTOR_SetSpeed(RIGHT, VD);
    /*Serial.print(" vg = ");
    Serial.print(VG);
    Serial.print(" vd = ");
    Serial.println(VD);*/
    delay(50);

    //Calcul des clics attendus (LEFT) et des clics reels (Droit)
    float leftClic = ENCODER_Read(LEFT);
    float rightClic = ENCODER_Read(RIGHT);
    comptClicsATTG += leftClic;
    comptClicsREELD += rightClic;
    /*Serial.print("Compteur clics attendus : ");
    Serial.println(comptClicsATTG);
    Serial.print("Compteur clics réels : ");
    Serial.println(comptClicsREELD);*/

    VD = AjustTrajec(VD,comptClicsATTG, comptClicsREELD);

    //Serial.println(comptClicsATTG);

    RenitClics ();
    if (VG < 0.8)
    {
      VG += 0.03;
      VD += 0.03;
    }
  }
  Serial.println(comptClicsATTG);
  int clicsRalen = 0;
  int l = 0;
  comptClicsATTG = 0;
  comptClicsREELD = 0;
  while(comptClicsATTG <= 6400)
  {
    float leftClic = ENCODER_Read(LEFT);
    float rightClic = ENCODER_Read(RIGHT);
    comptClicsATTG += leftClic;
    comptClicsREELD += rightClic;

    VD = AjustTrajec(VD, comptClicsATTG, comptClicsREELD);

    RenitClics();

    if (VG > 0.25)
    {
      VG -= 0.05;
      VD -= 0.05;
      MOTOR_SetSpeed(LEFT, VG);
      MOTOR_SetSpeed(RIGHT, VD);
    }
    delay(100);
    Serial.print("Vitesse moteur : ");
    Serial.println(VG);
  }
  ReinitMoteurs();
  comptClicsATTG = 0;
  comptClicsREELD = 0;
  delay(10);
}

void AvancerCorriger(float distance)
{
  float VG = 0.01;
  float VD = 0.01;
  long int comptClicsATTG = 0, comptClicsREELD = 0;
  while (comptClicsATTG < MetresToClics(distance))
  {
    MOTOR_SetSpeed(LEFT, VG);
    MOTOR_SetSpeed(RIGHT, VD);
    /*Serial.print(" vg = ");
    Serial.print(VG);
    Serial.print(" vd = ");
    Serial.println(VD);;*/
    delay(50);

    //Calcul des clics attendus (LEFT) et des clics reels (Droit)
    float leftClic = ENCODER_Read(LEFT);
    float rightClic = ENCODER_Read(RIGHT);
    comptClicsATTG += leftClic;
    comptClicsREELD += rightClic;
    /*Serial.print("Compteur clics attendus : ");
    Serial.println(comptClicsATTG);
    Serial.print("Compteur clics réels : ");
    Serial.println(comptClicsREELD);*/

    VD = AjustTrajec(VD,comptClicsATTG, comptClicsREELD);

    //Serial.println(comptClicsATTG);

    RenitClics ();
    if (VG < 0.4)
    {
      VG += 0.03;
      VD += 0.03;
    }
  }
  ReinitMoteurs();
  comptClicsATTG = 0;
  comptClicsREELD = 0;
  delay(100);
}

void ReinitMoteurs()
{
  MOTOR_SetSpeed(LEFT, 0.0);
  MOTOR_SetSpeed(RIGHT, 0.0);
  ENCODER_ReadReset(LEFT);
  ENCODER_ReadReset(RIGHT);
}

void RenitClics () 
{
  ENCODER_ReadReset(LEFT);
  ENCODER_ReadReset(RIGHT);
}

float AjustTrajec(float vintD, long int comptATT, long int comptREEL) 
{
 
  float leftClic = ENCODER_Read(LEFT);
  float rightClic = ENCODER_Read(RIGHT);
  float modifMoteur = 0, diffPond = 0;
  float diffInt = 0;

  //if(vintD = 0.01)

  diffInt += ((comptATT - comptREEL) * kI);
  /*Serial.print("Diffint : ");
  Serial.println(diffInt);*/
  diffPond = (leftClic - rightClic) * kP;
  /*Serial.print("Diffpond : ");
  Serial.println(diffPond);*/
  modifMoteur = diffPond + diffInt + vintD;
  /*Serial.println(modifMoteur);
  Serial.println("---------------------------------");*/
  RenitClics();

  return modifMoteur;
}

long int MetresToClics(float distance)
{
  float circonference, diametre = 0.077;
  long int distClics;

  circonference = PI * diametre;
  distClics = distance / circonference * 3200;

  return distClics;
}

void virage(float angle, int direction, int optimisation)
{
  /*transformation de l'angle en distance d'arc de cercle
  distance = proportion de l'angle sur 360 * circonférence
  rayon de courbure du cercle := distance entre les ReinitMoteurs*/
  RenitClics();
  
  int rayon = 19/*(cm)*/, clicsAFaire = 0;
  double distance = 0;
  
  distance/*m*/ = ((angle / 360) * 2 * PI * rayon)/100;
  Serial.println(distance);
  clicsAFaire = MetresToClics(distance);
  
  if(direction == RIGHT)
  {
    Serial.println(distance);
    while(ENCODER_Read(LEFT) < clicsAFaire - optimisation)
    {
      MOTOR_SetSpeed(RIGHT, 0);
      MOTOR_SetSpeed(LEFT, 0.3);
    }
  }
  else
  {
    while(ENCODER_Read(RIGHT) < clicsAFaire - optimisation)
    {
      MOTOR_SetSpeed(LEFT, 0);
      MOTOR_SetSpeed(RIGHT, 0.3);
    }
  }
  ReinitMoteurs();
}

void virage2Moteurs(int angle, int direction, int optimisation)
{
  RenitClics();
  
  int rayon = 19, clicsAFaire = 0;
  double distance = 0;
  
  distance/*m*/ = angle;
  Serial.println(distance);
  distance = distance / 360;
  Serial.println(distance);
  distance = distance * 2 * PI;
  Serial.println(distance);
  distance = distance * rayon / 100;
  Serial.println(distance);
  Serial.println(rayon);
  Serial.println(angle);
  Serial.println("------------");
  clicsAFaire = MetresToClics(distance);
  
  if(direction == RIGHT)
  {
    while(ENCODER_Read(LEFT) < (clicsAFaire - (optimisation/2))/2)
    {
      MOTOR_SetSpeed(RIGHT, -0.2);
      MOTOR_SetSpeed(LEFT, 0.2);
    }
  }
  else
  {
    while(ENCODER_Read(RIGHT) < (clicsAFaire - (optimisation/2))/2)
    {
      MOTOR_SetSpeed(LEFT, -0.2);
      MOTOR_SetSpeed(RIGHT, 0.2);
    }
  }
  ReinitMoteurs();
}

