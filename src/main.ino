/*
Projet: Déplacement Robot B Défi parcours
Equipe: P-10
Auteurs: Simon Torres, 
Description: Permet de compléter le défi du parcours
Date: 2018-09-27
loooooooooooooooooooooooool
loll
*/

//Librairies
#include <LibRobus.h> // Essentielle pour utiliser RobUS

//Variables Globales
 float kP = 0.00085, kI = 0.00004;
 int gauche = 0, droite = 1;
 int test = 0;
 //OPTIMISATION DES VIRAGES
 /* virage:
  - 45 gauche :
  - 45 droite : 
  - 90 gauche :
  - 90 droite :
  - 180 gauche :
  - 180 droite :
  --------------------------------------------------------------------
  virage2Moteurs
  - 45 gauche :
  - 45 droite : 
  - 90 gauche :
  - 90 droite :
  - 180 gauche :
  - 180 droite :

 */
/* ******************************************************************  SETUP  ************************************************** */
void setup()
{
  BoardInit();
  Serial.println("Hello");
  delay(2000);
  Serial.println("GO!");
}
/* ******************************************************************   LOOP   ************************************************** */
void loop() 
{
  
  if(AX_IsBumper(3)==1){
    //Aller
    //distance A
     AvancerCorrigerLONG(2.33);
     //virage B
     virage(90, gauche);
     //B a C
     AvancerCorriger(0.31);
     //virage C
     virage(90, droite);
     //C a D
     AvancerCorriger(0.23);
     //virage D
     virage(92, droite);
     //D a E
     AvancerCorriger(0.29);
     //virage E
     virage(90, gauche);
     //E a F
     AvancerCorriger(0.13);
     //virage F
     virage(45, droite);
     //F a G
     AvancerCorriger(0.37);
     //virage G
     virage(89, gauche);
     //G a Ha
     AvancerCorriger(0.54);
     //virage Ha
     virage(49.5, droite);
     //Ha a Hb
     AvancerCorriger(0.32);
     //virage Hb
     virage(21.5, droite);
     //Hb a I
     AvancerCorriger(0.64);
     //virage I
     virage2Moteurs(194, gauche, 150);
     //Retour
     /*AvancerCorriger(0.76);
     virage(12.5, gauche);
     AvancerCorriger(0.32);
     virage(45, gauche);
     AvancerCorriger(0.73);
     virage(90, droite);
     AvancerCorriger(0.28);
     virage(45, gauche);
     AvancerCorriger(0.13);
     virage(90, droite);
     AvancerCorriger(0.26);
     virage(90, gauche);
     AvancerCorriger(0.26);
     virage(90, gauche);
     AvancerCorriger(0.26);
     virage(90, droite);
     AvancerCorrigerLONG(2.13);*/


  }
   if(AX_IsBumper(2)==1){
    virage2Moteurs(45, droite, test);
  }
    if(AX_IsBumper(0/*gauche*/)==1){
     test += 100;
  }
    if(AX_IsBumper(1)==1){
     test -= 100;
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
  while (comptClicsATTG < MetresToClics(distance) - 5000)
  {
    MOTOR_SetSpeed(LEFT, VG);
    MOTOR_SetSpeed(RIGHT, VD);
    /*Serial.print(" vg = ");
    Serial.print(VG);
    Serial.print(" vd = ");
    Serial.println(VD);;*/
    delay(50);

    //Calcul des clics attendus (Gauche) et des clics reels (Droit)
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
    if (VG < 0.7)
    {
      VG += 0.03;
      VD += 0.03;
    }
  }
  MOTOR_SetSpeed(LEFT, VG);
  MOTOR_SetSpeed(RIGHT, VG);
  int clicsRalen = 0;
  while(clicsRalen <= 5000)
  {
    Serial.println("While");
    int leftClic = ENCODER_Read(LEFT);
    clicsRalen += leftClic;

    MOTOR_SetSpeed(LEFT, VG);
    MOTOR_SetSpeed(RIGHT, VD);
    delay(25);

    if (VG > 0.1)
    {
      VG -= 0.05;
      VD -= 0.05;
    }

    Serial.print(VD);
    Serial.print(VG);
  }
  ReinitMoteurs();
  comptClicsATTG = 0;
  comptClicsREELD = 0;
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

    //Calcul des clics attendus (Gauche) et des clics reels (Droit)
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
    if (VG < 0.3)
    {
      VG += 0.03;
      VD += 0.03;
    }
  }
  ReinitMoteurs();
  comptClicsATTG = 0;
  comptClicsREELD = 0;
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

void virage(float angle, int direction)
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
  
  if(direction == droite)
  {
    Serial.println(distance);
    while(ENCODER_Read(LEFT) < clicsAFaire - 100)
    {
      MOTOR_SetSpeed(1, 0);
      MOTOR_SetSpeed(0, 0.2);
    }
  }
  else
  {
    while(ENCODER_Read(RIGHT) < clicsAFaire - 100)
    {
      MOTOR_SetSpeed(0, 0);
      MOTOR_SetSpeed(1, 0.2);
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
  
  if(direction == droite)
  {
    while(ENCODER_Read(LEFT) < (clicsAFaire - optimisation)/2)
    {
      MOTOR_SetSpeed(1, -0.2);
      MOTOR_SetSpeed(0, 0.2);
    }
  }
  else
  {
    while(ENCODER_Read(RIGHT) < (clicsAFaire - optimisation)/2)
    {
      MOTOR_SetSpeed(0, -0.2);
      MOTOR_SetSpeed(1, 0.2);
    }
  }
  ReinitMoteurs();
}

