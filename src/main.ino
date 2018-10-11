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
 #define opti45LEFT2 175
 #define opti45RIGHT2 100
 #define opti90LEFT2 400
 #define opti90RIGHT2 225
 #define opti180LEFT2 300
 #define opti180RIGHT2 500
/* ******************************************  SETUP  *********************************** */
void setup()
{
  BoardInit();
  Serial.println("Hello");
  delay(500);
  Serial.println("GO!");
}
/* ****************************************   LOOP   **************************************** */
void loop() 
{
  if(ROBUS_IsBumper(3)==1){
    //Aller
    //distance A
     AvancerCorriger(2.13);
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
     // -----------------   Retour   -----------------//
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
     AvancerCorriger(2.33);
  }
  // SOFT_TIMER_Update(); // A decommenter pour utiliser des compteurs logiciels
  delay(10);// Delais pour décharger le CPU
}

/* ***************************FONCTIONS PERSONNELLES****************************************** */

/*Fonction de deplacement en ligne droite avec acceleration, ralentissement et PID*/
void AvancerCorriger(float distance)
{
  float VG = 0.01;
  float VD = 0.01;
  long int comptClicsATTG = 0, comptClicsREELD = 0;

  while (comptClicsATTG < (90 / 100 * MetresToClics(distance)))
  {
    MOTOR_SetSpeed(LEFT, VG);
    MOTOR_SetSpeed(RIGHT, VD);
   
    delay(50);

    //Calcul des clics attendus (LEFT) et des clics reels (RIGHT)
    comptClicsATTG += ENCODER_Read(LEFT);
    comptClicsREELD += ENCODER_Read(RIGHT);

    //Ajustement de la vitesse VD a l'aide du PID
    VD = AjustTrajec(VD,comptClicsATTG, comptClicsREELD);

    //Acceleration progressive
    reinitClics ();
    if (VG < 0.9)
    {
      VG += 0.03;
      VD += 0.03;
    }
  }

  // --------- Section du ralentissement -------------- //
  /*Quande le robot a effectue 90% de son trajet il sort du premier while
  et entre dans le suivant pour effectuer les 10% de la distance entre en ralentissant*/
  comptClicsATTG = 0, comptClicsREELD = 0;
  
  while(comptClicsATTG <= (10 / 100 * MetresToClics(distance)))
  {
    //Calcul des clics attendus (LEFT) et des clics reels (RIGHT)
    comptClicsATTG += ENCODER_Read(LEFT);
    comptClicsREELD += ENCODER_Read(RIGHT);

    //Ajustement de la vitesse VD a l'aide du PID
    VD = AjustTrajec(VD, comptClicsATTG, comptClicsREELD);

    reinitClics();

    //Ralentissement progressif
    if (VG > 0.40)
    {
      VG -= 0.05;
      VD -= 0.05;
      MOTOR_SetSpeed(LEFT, VG);
      MOTOR_SetSpeed(RIGHT, VD);
    }
    delay(75);
  }
  ReinitMoteurs();
  comptClicsATTG = 0, comptClicsREELD = 0;
}

/*Fonction de virage dans tous les angles et toutes les directions avec 2 moteurs 
tournants en sens inverses et en incluant le calcul du PID*/
void virage2moteurs(int angle, int direction, int optimisation)
{
  reinitClics();
  
  //VOIR CORRECTION DU RAYON EN FLOAT ET TESTER
  int clicsAFaire = 0;
  double distance = 0, rayon  = 9.5;
  
  distance = angle / 360 * 2 * PI * rayon / 100;
  clicsAFaire = MetresToClics(distance);
  
  if(direction == RIGHT)
  {
    float VG = 0.2;
    float VD = -0.2;
    long int comptClicsATTG = 0, comptClicsREELD = 0;
    while(comptClicsATTG < ((clicsAFaire-optimisation)))
    {
      MOTOR_SetSpeed(RIGHT, VD);
      MOTOR_SetSpeed(LEFT, VG);

      delay(20);

      //Calcul des clics attendus (LEFT) et des clics reels (RIGHT)
      comptClicsATTG += ENCODER_Read(LEFT);
      comptClicsREELD += ENCODER_Read(RIGHT);

      //Ajustement de la vitesse VD a l'aide du PID
      VD = -(AjustTrajecVirages(VD, comptClicsATTG, comptClicsREELD));

      reinitClics();
    }
    ReinitMoteurs();
  }

  else if (direction == LEFT)
  {
    float VG = -0.2;
    float VD = 0.2;
    long int comptClicsATTG = 0, comptClicsREELD = 0;
    while(comptClicsATTG > -(clicsAFaire - (optimisation/2)))
    {
      MOTOR_SetSpeed(LEFT, VG);
      MOTOR_SetSpeed(RIGHT, VD);

      delay(20);

      //Calcul des clics attendus (LEFT) et des clics reels (RIGHT)
      comptClicsATTG += ENCODER_Read(LEFT);
      comptClicsREELD += ENCODER_Read(RIGHT);

      //Ajustement de la vitesse VD a l'aide du PID
      VD = AjustTrajecVirages(VD, -comptClicsATTG, -comptClicsREELD);

      reinitClics();
    }
    ReinitMoteurs();
  }
}

/*Fonction de reinitialisation des moteurs et des compteurs de clics*/
void ReinitMoteurs()
{
  MOTOR_SetSpeed(LEFT, 0.0);
  MOTOR_SetSpeed(RIGHT, 0.0);
  ENCODER_ReadReset(LEFT);
  ENCODER_ReadReset(RIGHT);
}

/*Fonction de reinitialisation des compteurs d'encodeurs*/
void reinitClics () 
{
  ENCODER_ReadReset(LEFT);
  ENCODER_ReadReset(RIGHT);
}

/*Fonction de correction de la trajectoire a l'aide du PID*/
float AjustTrajec(float vintD, long int comptATT, long int comptREEL) 
{
  float leftClic = ENCODER_Read(LEFT);
  float rightClic = ENCODER_Read(RIGHT);
  float modifMoteur = 0, diffPond = 0;
  float diffInt = 0;

  //Calcul du PID 
  diffInt += ((comptATT - comptREEL) * kI);
  diffPond = (leftClic - rightClic) * kP;
  modifMoteur = diffPond + diffInt + vintD;

  reinitClics();

  //Retour de la valeur modifier du moteur esclave (VD)
  return modifMoteur;
}

/*Fonction de conversion de la distance donnee(m) en clics de roue*/
long int MetresToClics(float distance)
{
  float circonference, diametre = 0.077;
  long int distClics;

  //Transfert de la distance (m) en clics, 3200 clics par tour de roue:
  circonference = PI * diametre;
  distClics = distance / circonference * 3200;

  return distClics;
}

/* Fonction d'ajustement des clics dans un virage */
float AjustTrajecVirages(float vintD, long int comptATT, long int comptREEL) 
{
  float leftClic = ENCODER_Read(LEFT);
  float rightClic = ENCODER_Read(RIGHT);
  float modifMoteur = 0, diffPond = 0;
  float diffInt = 0;

  if (vintD > 0)
  {
    //Calcul du PID pour un virage a gauche
    diffInt += ((comptATT - (-comptREEL)) * kI);
    diffPond = (-leftClic - rightClic) * kP;
    modifMoteur =(diffPond + diffInt + vintD);
  }

  else
  {
    vintD = -vintD;

    //Calcul du PID pour un virage a droite
    diffInt += ((comptATT - (-comptREEL)) * kI);
    diffPond = (leftClic - (-rightClic)) * kP;    
    modifMoteur =(diffPond + diffInt + vintD);
  }
  
  reinitClics();
  return modifMoteur;
}

