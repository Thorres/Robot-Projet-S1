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
 #define ECART 0.2
 int etatMoteurs = 0;
 long int comptClicsATTG = 0, comptClicsREELD = 0;
 char tableauSuiveur [5] ;
 float VG = 0.3;
 float VD = 0.3;
/* ******************************************************************  SETUP  ************************************************** */
void setup()
{
  BoardInit();
  //scan de la couleur sous le robot et l'assigner a la variable maCouleur
  BrasSol();
  pinMode(47, INPUT);
  delay(500);
  Serial.println("GO!");
  while(ROBUS_IsBumper(3) == 0){
    delay(100);
  }
  delay(5000);
}
/* ******************************************************************   LOOP   ************************************************** */
void loop() 
{
  //ActionSuiveur();
  BrasSol();
  ActionSuiveur();
  if(DetectionSifflet() == 1){
    int Siffler = (DetectionSifflet());
    ReinitMoteurs();
    BrasHaut();
    delay(10000);
  }
  // SOFT_TIMER_Update(); // A decommenter pour utiliser des compteurs logiciels
  delay(10);// Delais pour décharger le CPU
}

/* ******************************************************************FONCTIONS PERSONNELLES************************************************** */
void BrasHaut(){
  ActivationServo();
  delay(100);
  //SERVO 0 = gauche & SServo 1 = droite
  SERVO_SetAngle(0, 90);
  SERVO_SetAngle(1, 70);
  delay(500);
  DesactivationServo();
}

void BrasSol(){
  ActivationServo();
  delay(100);
  //SERVO 0 = gauche & SServo 1 = droite
  SERVO_SetAngle(0, 125);
  SERVO_SetAngle(1, 30);
  delay(500);
  DesactivationServo();
}

int DetectionSifflet(){
  int pinMicro = 47;
  int Siffler = 0;

  Siffler = digitalRead(pinMicro);
  return Siffler;
}

int randomInt(int a, int b){
  return rand() % (b - a) + a;
}

void distanceIR (int id) {
  id = 0;
  Serial.print("IR: ");
  Serial.println(ROBUS_ReadIR(0));
  float ir = ROBUS_ReadIR(id);
  
  if (ir > 175 && ir < 225) {
    virage2moteurs(90, RIGHT, 0);
  }
}

void distanceSO (int id) {
  id = 0;
  //SONAR
  Serial.print("Sonar: ");
  Serial.println(SONAR_GetRange(0));
  float so = SONAR_GetRange(id);

  if (so > 27.5 && so < 32.5) {
    virage2moteurs(90, RIGHT, 0);
  }
}

void ActionBras(){
  /*Bumpers : Gauche = 0, Droit = 1, Avant = 2, Arriere = 3*/
  if(ROBUS_IsBumper(2)==1){
    //Ramasser le ballon lorsque le bumper avant est appuye
    ActivationServo();
    delay(100);
    SERVO_SetAngle(0, 125);
    SERVO_SetAngle(1, 125);
    delay(400);
    DesactivationServo();

    //D/placement random avec les bras dans les airs
    AvancerCorriger(rand()/RAND_MAX * 0.5);
    virage2moteurs(randomInt(0, 180), RIGHT, 0);
    AvancerCorriger(rand()/RAND_MAX * 0.3);
    virage2moteurs(randomInt(0, 180), LEFT, 0);

    //Le robot drop le ballon
    ActivationServo();
    delay(100);
    SERVO_SetAngle(0, 180);
    SERVO_SetAngle(1, 180);
    delay(400);
    DesactivationServo();
  }
}

void ActionSuiveur(){
  //Noir = 1 et blanc = 0
  UpdateSuiveur();
  if(ROBUS_IsBumper(2) == 1){
    virage2moteurs(90, RIGHT, opti90RIGHT2);
  }

  else{
    //Correction a Gauche
    if (tableauSuiveur == "11000" || tableauSuiveur ==  "11100" || tableauSuiveur == "01100" || tableauSuiveur == "10000"){
      //1 1 1 0 0 : 0 1 1 0 0  : 1 1 0 0 0 : 1 0 0 0 0
      MOTOR_SetSpeed(RIGHT, VD + 0.1);
      MOTOR_SetSpeed(LEFT, VG);
    }

    //Correction a droite
    if(tableauSuiveur == "00011" || tableauSuiveur ==  "00111" || tableauSuiveur == "00110" || tableauSuiveur == "00001"){
      //0 0 1 1 1 : 0 0 1 1 0 : 0 0 0 1 1 : 0 0 0 0 1
      MOTOR_SetSpeed(RIGHT, VD);
      MOTOR_SetSpeed(LEFT, VG + 0.1);
    }

    //Les cas ou le robot doit avancer en ligne droite
    else {
    AvancerCorrigerIndefini();
    }
  }
}

void UpdateSuiveur(){
  //Capteur en position 1
  if((vOutSuiveurExtr() >= 1.3 - ECART && vOutSuiveurExtr() <= 1.3 + ECART) || 
     (vOutSuiveurExtr() >= 3.9 - ECART && vOutSuiveurExtr() <= 3.9 + ECART))
    tableauSuiveur[0] = 1;
  else 
    tableauSuiveur[0] = 0;
  
  //Capteur en position 5
  if((vOutSuiveurExtr()  >= 2.6 - ECART && vOutSuiveurExtr() <= 2.6 + ECART) || 
    (vOutSuiveurExtr() >= 3.9 - ECART && vOutSuiveurExtr() <= 3.9 + ECART))
    tableauSuiveur[4] = 1;
  else 
    tableauSuiveur[4] = 0;
  
  //Capteur en position 2
  if((vOutSuiveurMilieu() >= 1.83 - ECART && vOutSuiveurMilieu() <= 1.83 + ECART) || 
    (vOutSuiveurMilieu() >= 0.637 - ECART && vOutSuiveurMilieu() <= 0.637 + ECART) ||
    (vOutSuiveurMilieu() >= 0 - ECART && vOutSuiveurMilieu() <= 0 + ECART))
    tableauSuiveur[1] = 1;
  else 
    tableauSuiveur[1] = 0;

  //Capteur en position 3
  if((vOutSuiveurMilieu() >= 0 - ECART && vOutSuiveurMilieu() <= 0 + ECART) ||
    (vOutSuiveurMilieu() >= 0.637 - ECART && vOutSuiveurMilieu() <= 0.637 + ECART) ||
    (vOutSuiveurMilieu() >= 2.42 - ECART && vOutSuiveurMilieu() <= 2.42 + ECART))
    tableauSuiveur[2] = 1;
  else 
    tableauSuiveur[2] = 0;

  //Capteur en position 4
  if((vOutSuiveurMilieu() >= 0 - ECART && vOutSuiveurMilieu() <= 0 + ECART) ||
    (vOutSuiveurMilieu() >= 2.42 - ECART && vOutSuiveurMilieu() <= 2.42 + ECART) ||
    (vOutSuiveurMilieu() >= 3.62 - ECART && vOutSuiveurMilieu() <= 3.62 + ECART))
    tableauSuiveur[3] = 1;
  else 
    tableauSuiveur[3] = 0;

  Serial.println(tableauSuiveur[0]);
  
}

float vOutSuiveurExtr(){
  float sensorA1 = A1, vOutExtr = 0 ;

  vOutExtr = analogRead(sensorA1) / 200.00;
  //Tension = analogread / 200
  Serial.print("La tension est de : ");
  Serial.println(vOutExtr); 
  delay(50);
  return vOutExtr;
}

float vOutSuiveurMilieu(){
  float sensorA0 = A0, vOutMilieu = 0 ;

  vOutMilieu = analogRead(sensorA0) / 200.00;
  //Tension = analogread / 200
  Serial.print("La tension est de : ");
  Serial.println(vOutMilieu); 
  delay(50);
  return vOutMilieu;
}

void AvancerCorriger(float distance){
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

void AvancerCorrigerIndefini(){
  
  
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

    RenitClics ();
}

void ReculerCorriger(float distance){
  Serial.println("Debut");
  float VG = -0.01;
  float VD = -0.01;
  long int comptClicsATTG = 0, comptClicsREELD = 0;
  int clics = - MetresToClics((0.9 * distance));
  Serial.println(clics);
  Serial.println(comptClicsATTG);
  /*Acceleration jusqu'a vitesse max pour 90% de la distance a faire*/
  while (comptClicsATTG > (- MetresToClics((0.9 * distance))))
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
    VD = AjustTrajec(VD, - comptClicsATTG, - comptClicsREELD);

    //Acceleration progressive
    RenitClics ();
    if (VG > - 0.9)
    {
      VG -= 0.03;
      VD -= 0.03;
    }
  }

  int clicsRalen = 0;
  int l = 0;
  comptClicsATTG = 0;
  comptClicsREELD = 0;

  //Ralentissement progressif jusqu'a ce qu'il ai fait les 10% restants de la distance
  while(comptClicsATTG >= (- MetresToClics((0.1 * distance))))
  {
    //Compteur de clics
    float leftClic = ENCODER_Read(LEFT);
    float rightClic = ENCODER_Read(RIGHT);
    comptClicsATTG += leftClic;
    comptClicsREELD += rightClic;

    //Ajustement de la trajectoire avec le PID
    VD = AjustTrajec(VD, - comptClicsATTG, - comptClicsREELD);

    RenitClics();

    //Ralentissement progressif
    if (VG < 0.25)
    {
      VG += 0.05;
      VD += 0.05;
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
void ReinitMoteurs(){
  MOTOR_SetSpeed(LEFT, 0.0);
  MOTOR_SetSpeed(RIGHT, 0.0);
  ENCODER_ReadReset(LEFT);
  ENCODER_ReadReset(RIGHT);
}

//Réinitialisation des clics
void RenitClics () {
  ENCODER_ReadReset(LEFT);
  ENCODER_ReadReset(RIGHT);
}

//Ajustement du trajet avec le PID
float AjustTrajec(float vintD, long int comptATT, long int comptREEL) {
 
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

//Transformation de distance (m) en nbr de clics
long int MetresToClics(float distance){
  float circonference, diametre = 0.077;
  long int distClics;

  circonference = PI * diametre;
  distClics = distance / circonference * 3200;

  return distClics;
}

void virage2moteurs(double angle, int direction, int optimisation){
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

float AjustTrajecViragesD(float vintD, long int comptATT, long int comptREEL) {
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

float AjustTrajecViragesG(float vintD, long int comptATT, long int comptREEL) {
 
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