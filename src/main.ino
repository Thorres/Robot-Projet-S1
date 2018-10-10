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
 #define opti45LEFT -250
 #define opti45RIGHT 50
 #define opti90LEFT -150
 #define opti90RIGHT 25
 #define opti180LEFT 225
 #define opti180RIGHT 100
 #define opti45LEFT2 175
 #define opti45RIGHT2 125
 #define opti90LEFT2 425
 #define opti90RIGHT2 225
 #define opti180LEFT2 300
 #define opti180RIGHT2 500
 //OPTIMISATION DES virage2moteursS
 /* virage2moteurs:
  - 45 LEFT : -275
  - 45 RIGHT : 50
  - 90 LEFT : 0
  - 90 RIGHT : 25
  - 180 LEFT : 225
  - 180 RIGHT : 100
  --------------------------------------------------------------------
  virage2moteurs2Moteurs
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
  if(ROBUS_IsBumper(0/*LEFT*/)==1){
    for( int i = 0; i != 4; i++){
      virage2moteurs (45, RIGHT, 100);
      delay(1000);
    }
  }
  if(ROBUS_IsBumper(3)==1){
    //Aller
    //distance A
     AvancerCorrigerLONG(2.13);
     //virage2moteurs B
     virage2moteurs(90, LEFT, opti90LEFT2);
     //B a C
     AvancerCorriger(0.430);
     //virage2moteurs C
     virage2moteurs(90, RIGHT, opti90RIGHT2);
     //C a D
     AvancerCorriger(0.395);
     //virage2moteurs D
     virage2moteurs(90, RIGHT, opti90RIGHT2);
     //D a E
     AvancerCorriger(0.385);
     //virage2moteurs E
     virage2moteurs(90, LEFT, opti90LEFT2);
     //E a F
     AvancerCorriger(0.305);
     //virage2moteurs F
     virage2moteurs(45, RIGHT, opti45RIGHT2);
     //F a G
     AvancerCorriger(0.525);
     //virage2moteurs G
     virage2moteurs(90, LEFT, opti90LEFT2);
     //G a Ha
     AvancerCorriger(0.830);
     //virage2moteurs Ha
     virage2moteurs(45, RIGHT, opti45RIGHT2);
     //Ha a Hb
     AvancerCorriger(0.345);
     //virage2moteurs Hb
     virage2moteurs(12, RIGHT, 0);
     //Hb a I
     AvancerCorriger(0.735);
     //virage2moteurs I
     virage2moteurs(180, RIGHT, opti180RIGHT2);
     //Retour
     //I a Hb
     AvancerCorriger(0.705);
     //virage2moteurs Hb
     virage2moteurs(10, LEFT, 0);
     //Hb a Ha
     AvancerCorriger(0.575);
     //virage2moteurs Ha
     virage2moteurs(45, LEFT, opti45LEFT2);
     //H a Ga
     AvancerCorriger(0.615);
     //virage2moteurs G
     virage2moteurs(90, RIGHT, opti90RIGHT2);
     //G a F
     AvancerCorriger(0.465);
     //virage2moteurs F
     virage2moteurs(45, LEFT, opti45LEFT2);
     //F a E
     AvancerCorriger(0.315);
     //virage2moteurs E
     virage2moteurs(90, RIGHT, opti90RIGHT2);
     //E a D
     AvancerCorriger(0.455);
     //virage2moteurs D
     virage2moteurs(90, LEFT, opti45LEFT2);
     //D a C
     AvancerCorriger(0.395);
     //virage2moteurs C
     virage2moteurs(90, LEFT, opti90LEFT2);
     //C a B
     AvancerCorriger(0.475);
     //virage2moteurs B
     virage2moteurs(90, RIGHT, opti90RIGHT2);
     //B a A
     AvancerCorrigerLONG(2.33);


  }
  if(ROBUS_IsBumper(1)==1){
    Serial.println(test);
    test -= 50;
    delay(100);
    Serial.println(test);
  }
  if(ROBUS_IsBumper(2)==1){
    delay(100);
    virage2moteurs(180, LEFT, 300);
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
    if (VG < 0.9)
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
    delay(50);
    Serial.print("Vitesse moteur : ");
    Serial.println(VG);
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
  while (comptClicsATTG < MetresToClics(distance) - 1500)
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
  int clicsRalen = 0;
  int l = 0;
  comptClicsATTG = 0;
  comptClicsREELD = 0;
  while(comptClicsATTG <= 1500)
  {
    float leftClic = ENCODER_Read(LEFT);
    float rightClic = ENCODER_Read(RIGHT);
    comptClicsATTG += leftClic;
    comptClicsREELD += rightClic;

    VD = AjustTrajec(VD, comptClicsATTG, comptClicsREELD);

    RenitClics();

    if (VG > 0.1)
    {
      VG -= 0.05;
      VD -= 0.05;
      MOTOR_SetSpeed(LEFT, VG);
      MOTOR_SetSpeed(RIGHT, VD);
    }
    delay(100);
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

void virage2moteurs(int angle, int direction, int optimisation)
{
  RenitClics();
  
  int rayon = 9.5, clicsAFaire = 0;
  double distance = 0;
  
  distance/*m*/ = angle;
  distance = distance / 360;
  distance = distance * 2 * PI;
  distance = distance * rayon / 100;
  clicsAFaire = MetresToClics(distance);
  Serial.println(clicsAFaire);
  
  if(direction == RIGHT)
  {
    float VG = 0.2;
    float VD = -0.2;
    long int comptClicsATTG = 0, comptClicsREELD = 0;
    while(comptClicsATTG < ((clicsAFaire-optimisation)))
    {
      Serial.println("salut");
      MOTOR_SetSpeed(RIGHT, VD);
      MOTOR_SetSpeed(LEFT, VG);

      delay(10);

      float leftClic = ENCODER_Read(LEFT);
      float rightClic = ENCODER_Read(RIGHT);
      comptClicsATTG += leftClic;
      comptClicsREELD += rightClic;
      Serial.println(comptClicsREELD);
      Serial.println(comptClicsATTG);

      VD = -(AjustTrajecViragesD(-VD, comptClicsATTG, comptClicsREELD));
    }
    ReinitMoteurs();
  }
  else
  {
    float VG = -0.2;
    float VD = 0.2;
    long int comptClicsATTG = 0, comptClicsREELD = 0;
    while(comptClicsATTG > -(clicsAFaire - (optimisation/2)))
    {
      MOTOR_SetSpeed(LEFT, VG);
      MOTOR_SetSpeed(RIGHT, VD);

      delay(10);

      float leftClic = ENCODER_Read(LEFT);
      float rightClic = ENCODER_Read(RIGHT);
      comptClicsATTG += leftClic;
      comptClicsREELD += rightClic;
      Serial.println(comptClicsREELD);
      Serial.println(comptClicsATTG);

      VD = AjustTrajecViragesG(VD, -comptClicsATTG, -comptClicsREELD);
    }
    ReinitMoteurs();
  }
}

float AjustTrajecViragesD(float vintD, long int comptATT, long int comptREEL) 
{
 
  float leftClic = ENCODER_Read(LEFT);
  float rightClic = ENCODER_Read(RIGHT);
  float modifMoteur = 0, diffPond = 0;
  float diffInt = 0;

  //if(vintD = 0.01)

  diffInt += ((comptATT - (-comptREEL)) * kI);
  /*Serial.print("Clics moteurs droit : ");
  Serial.println(comptATT);*/
  diffPond = (leftClic - (-rightClic)) * kP;
  /*Serial.print("Diffpond : ");
  Serial.println(diffPond);*/
  modifMoteur =(diffPond + diffInt + vintD);
  /*Serial.println(modifMoteur);
  Serial.println("---------------------------------");*/
  RenitClics();

  return modifMoteur;
}

float AjustTrajecViragesG(float vintD, long int comptATT, long int comptREEL) 
{
 
  float leftClic = ENCODER_Read(LEFT);
  float rightClic = ENCODER_Read(RIGHT);
  float modifMoteur = 0, diffPond = 0;
  float diffInt = 0;

  //if(vintD = 0.01)

  diffInt += ((comptATT - (-comptREEL)) * kI);
  /*Serial.print("Clics moteurs droit : ");
  Serial.println(comptATT);*/
  diffPond = (-leftClic - rightClic) * kP;
  /*Serial.print("Diffpond : ");
  Serial.println(diffPond);*/
  modifMoteur =(diffPond + diffInt + vintD);
  /*Serial.println(modifMoteur);
  Serial.println("---------------------------------");*/
  RenitClics();

  return modifMoteur;
}

