/*
Projet: Déplacement Robot A Défi parcours
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
 #define opti45LEFT2 175
 #define opti45RIGHT2 100
 #define opti90LEFT2 417
 #define opti90RIGHT2 237
 #define opti180LEFT2 300
 #define opti180RIGHT2 500
/* ******************************************************************  SETUP  ************************************************** */
void setup()
{
  BoardInit();
  //scan de la couleur sous le robot et l'assigner a la variable maCouleur

  delay(500);
  Serial.println("GO!");
}
/* ******************************************************************   LOOP   ************************************************** */
void loop() 
{
  /*Bumpers : Gauche = 0, Droit = 1, Avant = 2, Arriere = 3*/
  while(ROBUS_IsBumper(2)==1){
    //Ramasser le ballon lorsque le bumber avant est appuye
    ActivationServo();
    SERVO_SetAngle(0, 90);
    SERVO_SetAngle(1, 90);
    
    //Le robot se déplace jusqu'a rencontrer une couleur adverse
    while(/*Couleur == blanc ou maCouleur*/ ){
      //Appel de la fonction suiveur de ligne
    }

    //Le robot depose  le ballon lorsqu'il passe sur une couleur adverse ou du noir
    SERVO_SetAngle(0, 0);
    SERVO_SetAngle(1, 0);
    DesactivationServo();
    AvancerCorriger(0.3);
    virage2moteurs(180, RIGHT, opti180RIGHT2);
    AvancerCorriger(0.3);
  }
  // SOFT_TIMER_Update(); // A decommenter pour utiliser des compteurs logiciels
  delay(10);// Delais pour décharger le CPU
}

/* ******************************************************************FONCTIONS PERSONNELLES************************************************** */
void AvancerCorriger(float distance)
{
  float VG = 0.01;
  float VD = 0.01;
  long int comptClicsATTG = 0, comptClicsREELD = 0;
  /*Acceleration jusqu'a vitesse max pour 90% de la distance a faire*/
  while (comptClicsATTG < (MetresToClics((0.9 * distance))))
  {
    MOTOR_SetSpeed(LEFT, VG);
    MOTOR_SetSpeed(RIGHT, VD);
    delay(50);

    //Compteur de clics 
    float leftClic = ENCODER_Read(LEFT);
    float rightClic = ENCODER_Read(RIGHT);
    comptClicsATTG += leftClic;
    comptClicsREELD += rightClic;

    //Ajustement de la trajectoire avec le PID
    VD = AjustTrajec(VD,comptClicsATTG, comptClicsREELD);

    //Acceleration progressive
    RenitClics ();
    if (VG < 0.9)
    {
      VG += 0.03;
      VD += 0.03;
    }
  }

  int clicsRalen = 0;
  int l = 0;
  comptClicsATTG = 0;
  comptClicsREELD = 0;

  //Ralentissement progressif jusqu'a ce qu'il ai fait les 10% restants de la distance
  while(comptClicsATTG <= MetresToClics((0.1 * distance)))
  {
    //Compteur de clics
    float leftClic = ENCODER_Read(LEFT);
    float rightClic = ENCODER_Read(RIGHT);
    comptClicsATTG += leftClic;
    comptClicsREELD += rightClic;

    //Ajustement de la trajectoire avec le PID
    VD = AjustTrajec(VD, comptClicsATTG, comptClicsREELD);

    RenitClics();

    //Ralentissement progressif
    if (VG > 0.25)
    {
      VG -= 0.05;
      VD -= 0.05;
      MOTOR_SetSpeed(LEFT, VG);
      MOTOR_SetSpeed(RIGHT, VD);
    }
    delay(75);
  }
  ReinitMoteurs();
  comptClicsATTG = 0;
  comptClicsREELD = 0;
}

//Reinitialisation des moteurs et des clics
void ReinitMoteurs()
{
  MOTOR_SetSpeed(LEFT, 0.0);
  MOTOR_SetSpeed(RIGHT, 0.0);
  ENCODER_ReadReset(LEFT);
  ENCODER_ReadReset(RIGHT);
}

//Réinitialisation des clics
void RenitClics () 
{
  ENCODER_ReadReset(LEFT);
  ENCODER_ReadReset(RIGHT);
}

//Ajustement du trajet avec le PID
float AjustTrajec(float vintD, long int comptATT, long int comptREEL) 
{
 
  float leftClic = ENCODER_Read(LEFT);
  float rightClic = ENCODER_Read(RIGHT);
  float modifMoteur = 0, diffPond = 0;
  float diffInt = 0;

  diffInt += ((comptATT - comptREEL) * kI);
  diffPond = (leftClic - rightClic) * kP;
  modifMoteur = diffPond + diffInt + vintD;
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

void virage2moteurs(double angle, int direction, int optimisation)
{
  RenitClics();
  
  int rayon = 9.5, clicsAFaire = 0;
  double distance = 0;
  
  distance/*m*/ = ((angle / 360) * 2 * PI * rayon)/100;
  clicsAFaire = MetresToClics(distance);
  
  if(direction == RIGHT)
  {
    float VG = 0.2;
    float VD = -0.2;
    long int comptClicsATTG = 0, comptClicsREELD = 0;

    while(comptClicsATTG < ((clicsAFaire - optimisation / 2)))
    {
      MOTOR_SetSpeed(RIGHT, VD);
      MOTOR_SetSpeed(LEFT, VG);

      delay(10);

      //Compteur de clics
      float leftClic = ENCODER_Read(LEFT);
      float rightClic = ENCODER_Read(RIGHT);
      comptClicsATTG += leftClic;
      comptClicsREELD += rightClic;

      //Ajustement de la trajectoire avec le PID
      VD = -(AjustTrajecViragesD(-VD, comptClicsATTG, comptClicsREELD));
    }
    ReinitMoteurs();
  }
  else
  {
    float VG = -0.2;
    float VD = 0.2;
    long int comptClicsATTG = 0, comptClicsREELD = 0;
    while(comptClicsATTG > -(clicsAFaire))
    {
      MOTOR_SetSpeed(LEFT, VG);
      MOTOR_SetSpeed(RIGHT, VD);

      delay(10);

      float leftClic = ENCODER_Read(LEFT);
      float rightClic = ENCODER_Read(RIGHT);
      comptClicsATTG += leftClic;
      comptClicsREELD += rightClic;

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

  diffInt += ((comptATT - (-comptREEL)) * kI);
  diffPond = (leftClic - (-rightClic)) * kP;
  modifMoteur =(diffPond + diffInt + vintD);
  RenitClics();

  return modifMoteur;
}

float AjustTrajecViragesG(float vintD, long int comptATT, long int comptREEL) 
{
 
  float leftClic = ENCODER_Read(LEFT);
  float rightClic = ENCODER_Read(RIGHT);
  float modifMoteur = 0, diffPond = 0;
  float diffInt = 0;

  diffInt += ((comptATT - (-comptREEL)) * kI);
  diffPond = (-leftClic - rightClic) * kP;
  modifMoteur =(diffPond + diffInt + vintD);
  
  RenitClics();

  return modifMoteur;
}

void ActivationServo(){
  SERVO_Enable(0);
  SERVO_Enable(1);
}

void DesactivationServo(){
  SERVO_Disable(0);
  SERVO_Disable(1);
}