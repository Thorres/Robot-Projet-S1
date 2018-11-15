/*
Projet: Déplacement Robot A Défi parcours
Equipe: P-10
Auteurs: Simon Torres, 
Description: Permet de compléter le défi du parcours
Date: 2018-09-27
*/

//Librairies
#include <LibRobus.h> // Essentielle pour utiliser RobUS
#include <string.h>
//Variables Globales
 float kP = 0.00085, kI = 0.00004;
 int test;
 #define opti45LEFT2 175
 #define opti45RIGHT2 100
 #define opti90LEFT2 417
 #define opti90RIGHT2 237
 #define opti180LEFT2 300
 #define opti180RIGHT2 500
 #define ECARTMIL 0.25
 #define ECARTEXT 0.4
 #define DISTIRMUR 300
 #define DISTSOMUR 5
 #define DISTAPVIRAGE 0.2
 #define DROITE 1
 #define AVANT 2
 #define GAUCHE 3
 #define ARRIERE 4
 int etatMoteurs = 0;
 int tempsRandom;
 long int comptClicsATTG = 0, comptClicsREELD = 0;
 char tableauSuiveur [6] = "00000";
 char tableauSuiveurTemp [6] = "00000";
 float deplacementLabi[100];
 float VG = 0.3;
 float VD = 0.3;
/* ******************************************************************  SETUP  ************************************************** */
void setup()
{
  BoardInit();
  pinMode(47, INPUT);
  delay(500);
  for (int i = 0; i < 100; i++){
    deplacementLabi[i] = 0;
  }
  Serial.println("GO!");
  /*while(ROBUS_IsBumper(3) == 0){
    delay(100);
  }
  delay(1000);*/
}
/* ******************************************************************   LOOP   ************************************************** */
void loop() 
{
  ENCODER_Reset(LEFT);
  /*
  //while(detection de l'objet == faux){
    int etape = 0;

    while(DetectionIntersection() == 0){
      ActionSuiveur();
    }

    EnregistrerDistance(etape);
    etape++;
    EnregistrerVirage(&etape);
    etape++;
  //}
  */

  //Test du suiveur de lignes
  if(ROBUS_IsBumper(1) == 1){
    virage2moteurs(90, RIGHT, 0);
    ActionSuiveurDist(30);
    virage2moteurs(180, RIGHT, 0);
    ActionSuiveurDist(30);
  }

  //Test du detecteur d'intersection
  if(ROBUS_IsBumper(2) == 1){
    int compteurClics = 0, clicsMoment = 0;
    for(int i = 0; i < 10; i++)
      clicsMoment = ENCODER_Read(0);
      compteurClics += clicsMoment;
      AvancerCorriger(0.3);
    Serial.println(compteurClics);
  //Le robot avance tant qu'il ne detecte pas une intersection
  
    //int etape = 0;
    /*Serial.println(DetectionIntersection());
    while(DetectionIntersection() == 0){
      AvancerCorrigerIndefini();
    }
    ReinitMoteurs();*/
  

  //Test de l'enregistrement du virage et de la prise de Decision
    /*deplacementLabi[etape] = 0;
    EnregistrerVirage(&etape);
    Serial.print(deplacementLabi[etape]);*/
    
  
  //Test des senseurs de facon independante
   /* Serial.print("Le capteur IR gauche = ");
    Serial.println(distanceIR(3));
    Serial.println(ROBUS_ReadIR(3));
    Serial.print("Le capteur IR droite = ");
    Serial.println(distanceIR(2));
    Serial.println(ROBUS_ReadIR(2));*/
   
  //Test d'affiliation des bons capteur aux bons cotes de robot (droite = droite) 
    /*Serial.print("Les options de virage sont : ");
    if(distanceIR(2) == 1)
      Serial.print("DROITE  ");
    if(distanceIR(3) == 1)
      Serial.print("GAUCHE  ");
    if(distanceSO(0) == 1)
      Serial.println("DEVANT");
    else
      Serial.println("Je suis dans un cul de sac");
    delay(200);*/
  }

  //Test de l'enregistrement des distances
  if(ROBUS_IsBumper(0) == 1){
    float distance = 0.5;
    int etape = 0;
    Serial.print(deplacementLabi[0]);
    Serial.print("\t");
    Serial.print(deplacementLabi[1]);
    Serial.print("\t");
    Serial.println(deplacementLabi[2]);
    for (int i = 0; i < 3; i++){
      AvancerCorriger(distance);
      EnregistrerDistance(etape);
      distance += 0.1;
      etape ++;
      Serial.print(deplacementLabi[i]);
      Serial.print("\t");
    }
    Serial.println("");
    delay(2000);

    //Ajout du test de la gestion d'un CDS 
    //Le robot devrait refaire la derniere distance effectuee et remettre la valeur dans le tableau a 0
    /*
    gererCDS(&etape);
    Serial.print(deplacementLabi[i]);
    Serial.print("\t");
    */
  }

  //Test de l'enregitrement des virages
  /*if(ROBUS_IsBumper(4) == 1){
    
  }

  */

  //5 secs pour rammasser le robot en appuyant sur le bumper arriere
  if(ROBUS_IsBumper(3) == 1){
    ReinitMoteurs();
    delay(5000);
  }
  // SOFT_TIMER_Update(); // A decommenter pour utiliser des compteurs logiciels
    
  delay(10);// Delais pour décharger le CPU
}

/* ******************************************************************FONCTIONS PERSONNELLES************************************************** */

int gererCDS(int *numEtape){
  //Pour prendre la position d'une distance
  *numEtape--;
  //Le robot fait un U-Turn et refait la derniere distance enregistree
  virage2moteurs(180, RIGHT, opti180RIGHT2);
  //ActionSuiveurDist(deplacementLabi[*numEtape]);
  AvancerCorriger(deplacementLabi[*numEtape]);
  deplacementLabi[*numEtape] = 0;
  //Decrementation de l'etape pour modifier la valeur du virage
  *numEtape--;
}

int EnregistrerVirage(int *numEtape){
  Serial.println(deplacementLabi[*numEtape]);
  Serial.println(distanceSO(0));
  if(deplacementLabi[*numEtape] == 0){

  //Test d'abord sil peut tourner a droite
    if(distanceIR(2) == 1){
      virage2moteurs(90, RIGHT, opti90RIGHT2);
      deplacementLabi[*numEtape] = DROITE;
      ActionSuiveurDist(DISTAPVIRAGE);
    }
  
  //Test s'il peut aller tout droit
    else if(distanceSO(0) == 1){
      deplacementLabi[*numEtape] = AVANT;
      ActionSuiveurDist(DISTAPVIRAGE);
    }

  //Test s'il peut tourner a gauche
    else if(distanceIR(3) == 1){
      virage2moteurs(90, LEFT, opti90LEFT2);
      deplacementLabi[*numEtape] = GAUCHE;
      ActionSuiveurDist(DISTAPVIRAGE);
    }

  //Sinon c'est qu'il est dans un cul de sac
    else{
      deplacementLabi[*numEtape] = ARRIERE;
    }
  }

  //Si la valeur attitrer a la case du virage n'est pas 4 et pas 0
  //Le robot a alors deja effectuer des virages a partir de la meme intersection
  else if(deplacementLabi[*numEtape] != 4){

    if(distanceIR(0) == 1){
      virage2moteurs(90, RIGHT, opti90RIGHT2);
      deplacementLabi[*numEtape] + 1;
      ActionSuiveurDist(DISTAPVIRAGE);
    }
    else if(distanceSO(0) == 1){
      deplacementLabi[*numEtape] + 2;
      ActionSuiveurDist(DISTAPVIRAGE);
    }
    else if(distanceIR(1) == 1){
      virage2moteurs(90, LEFT, opti90LEFT2);
      deplacementLabi[*numEtape] + 3;
      ActionSuiveurDist(DISTAPVIRAGE);
    }
  }
  else{
    gererCDS(numEtape);
  }
  return 0;
}

void EnregistrerDistance(int numEtape){
  int clicsFaits = ENCODER_Read(LEFT);
  Serial.println(clicsFaits);

  deplacementLabi[numEtape] = clicsFaits;
}

int DetectionIntersection(){
  //Si les detecteurs IR detectes une distance plus grande que la distance du mur alors la condition deviens vrai
  if (distanceIR(3) == 1 || distanceIR(2) == 1){
   // Serial.println("Je detecte une intersection");
    AvancerCorriger(DISTAPVIRAGE);
    return 1;
  }
  else{
    //Serial.println("Je ne detecte pas une intersection");
    return 0;
  }
}

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

int distanceIR (int id) {
  float ir = ROBUS_ReadIR(id);
  
  if(ir < DISTIRMUR)
    return 1;
  else
    return 0;
}

int distanceSO (int id) {
  id = 0;
  //SONAR
  Serial.print("Sonar: ");
  Serial.println(SONAR_GetRange(0));
  float so = SONAR_GetRange(id);

  if (so > DISTSOMUR) 
    return 1;
  return 0;
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

float ActionSuiveurDist(float distance){
  //Noir = 1 et blanc = 0
  distance = MetresToClics(distance);
  RenitClics();
  //Serial.println(distance);
  long int comptClics = 0;
  float correctionMinime = 0.04, correctionImportante = 0.07;

  while(comptClics < distance)
  {
    //Compteur de clics 
    float leftClic = ENCODER_Read(LEFT);
    comptClics += leftClic;
    for(int i = 0; i < 6; i++){
      tableauSuiveurTemp[i] = tableauSuiveur[i];
    }
    UpdateSuiveur();

    //Correction a Gauche minime
    if (memcmp(tableauSuiveur, "11100", 5 ) == 0 || memcmp(tableauSuiveur, "01100", 5 ) == 0)
    {
      //1 1 1 0 0 : 0 1 1 0 0
     // Serial.println("Correction gauche");
      MOTOR_SetSpeed(RIGHT, VD + correctionMinime);
      MOTOR_SetSpeed(LEFT, VG);
      correctionMinime -= 0.002;
      //Serial.println(correctionMinime * 1000);
      //delay(50);
    }

    //Correction a gauche importante
    else if (memcmp(tableauSuiveur, "11000", 5 ) == 0 || memcmp(tableauSuiveur, "10000", 5 ) == 0)
    {
      //1 1 0 0 0 : 1 0 0 0 0
      //Serial.println("Correction gauche importante");
      MOTOR_SetSpeed(RIGHT, VD + correctionImportante);
      MOTOR_SetSpeed(LEFT, VG);
      correctionImportante -= 0.002;
      //Serial.println(correctionImportante * 1000);
      //delay(50);
    }
    
    //Correction a droite minime
    else if(memcmp(tableauSuiveur, "00111", 5 ) == 0 || memcmp(tableauSuiveur, "00110", 5 ) == 0)
    {
      //0 0 1 1 1 : 0 0 1 1 0 : 0 0 0 1 1 : 0 0 0 0 1
      //Serial.println("Correction droite");
      MOTOR_SetSpeed(RIGHT, VD);
      MOTOR_SetSpeed(LEFT, VG + correctionMinime);
      correctionMinime -= 0.002;
      //Serial.println(correctionMinime * 1000);
      //delay(50);
    }

    //Correction a droite importante
    else if(memcmp(tableauSuiveur, "00011", 5 ) == 0|| memcmp(tableauSuiveur, "00001", 5 ) == 0)
    {
      // 0 0 0 1 1 : 0 0 0 0 1
      //Serial.println("Correction importante droite");
      MOTOR_SetSpeed(RIGHT, VD);
      MOTOR_SetSpeed(LEFT, VG + correctionImportante);
      correctionImportante -= 0.002;
      //Serial.println(correctionImportante * 1000);
      //delay(50);
    }
    else if(memcmp(tableauSuiveur, "00000", 5 ) == 0|| memcmp(tableauSuiveur, "01000", 5 ) == 0|| memcmp(tableauSuiveur, "00100", 5 ) == 0|| memcmp(tableauSuiveur, "00010", 5 ) == 0){
      //Serial.println("Garder les meme vitesses");
    }

    //Les cas ou le robot doit avancer en ligne droite
    else /*if(memcmp(tableauSuiveur, "11111", 5 ) == 0 || memcmp(tableauSuiveur, "01110", 5 ) == 0)*/{
      //Serial.println("Avancer tout droit");
      MOTOR_SetSpeed(LEFT, VG);
      MOTOR_SetSpeed(RIGHT, VD);
    }
    
  }
  ReinitMoteurs();
}

float ActionSuiveur(){
  //Noir = 1 et blanc = 0

  UpdateSuiveur();
  if(ROBUS_IsBumper(2) == 1){
    virage2moteurs(90, RIGHT, opti90RIGHT2);
  }

  else{
    //Correction a Gauche
    if (memcmp(tableauSuiveur, "11000", 5 ) == 0 || memcmp(tableauSuiveur, "11100", 5 ) == 0 || memcmp(tableauSuiveur, "01100", 5 ) == 0 || memcmp(tableauSuiveur, "10000", 5 ) == 0){
      //1 1 1 0 0 : 0 1 1 0 0  : 1 1 0 0 0 : 1 0 0 0 0
      Serial.println("Correction gauche");
      MOTOR_SetSpeed(RIGHT, VD + 0.05);
      MOTOR_SetSpeed(LEFT, VG);
    }

    //Correction a droite
    else if(memcmp(tableauSuiveur, "00011", 5 ) == 0|| memcmp(tableauSuiveur, "00111", 5 ) == 0 || memcmp(tableauSuiveur, "00110", 5 ) == 0 || memcmp(tableauSuiveur, "00001", 5 ) == 0){
      //0 0 1 1 1 : 0 0 1 1 0 : 0 0 0 1 1 : 0 0 0 0 1
      Serial.println("Correction droite");
      MOTOR_SetSpeed(RIGHT, VD);
      MOTOR_SetSpeed(LEFT, VG + 0.05);
    }

    //Les cas ou le robot doit avancer en ligne droite
    else {
      MOTOR_SetSpeed(RIGHT, VD);
      MOTOR_SetSpeed(LEFT, VG);
    }
  }
}

void UpdateSuiveur(){
  float Vout1 = vOutSuiveurExtr(), Vout2 = vOutSuiveurMilieu();
  /*Serial.print("V Extr : ");
  Serial.println(Vout1);
  Serial.print("V Milieu : ");
  Serial.println(Vout2);*/
  //Capteur en position 1
  if((Vout1 >= 2.9 - ECARTEXT && Vout1 <= 2.9 + ECARTEXT) ||
     (Vout1 >= 0 - ECARTEXT && Vout1 <= 0 + ECARTEXT))
    tableauSuiveur[0] = '1';
  else
    tableauSuiveur[0] = '0';

    
  /*Serial.print("Extreme gauche : ");
  Serial.println(tableauSuiveur[0]);*/
  
  //Capteur en position 2
  if((Vout2 >= 3.28 - ECARTMIL && Vout2 <= 3.28 + ECARTMIL) || 
    (Vout2 >= 2.6 - ECARTMIL && Vout2 <= 2.6 + ECARTMIL) ||
    (Vout2 >= 0 - ECARTMIL && Vout2 <= 0 + ECARTMIL))
    tableauSuiveur[1] = '1';
  else 
    tableauSuiveur[1] = '0';
    /*Serial.print("Gauche : ");
    Serial.println(tableauSuiveur[1]);*/

  //Capteur en position 3
  if((Vout2 >= 0 - ECARTMIL && Vout2 <= 0 + ECARTMIL) ||
    (Vout2 >= 2.6 - ECARTMIL && Vout2 <= 2.6 + ECARTMIL) ||
    (Vout2 >= 1.28 - ECARTMIL && Vout2 <= 1.28 + ECARTMIL))
    tableauSuiveur[2] = '1';
  else 
    tableauSuiveur[2] = '0';
    /*Serial.print("Centre : ");
    Serial.println(tableauSuiveur[2]);*/

  //Capteur en position 4
  if((Vout2 >= 0 - ECARTMIL && Vout2 <= 0 + ECARTMIL) ||
    (Vout2 >= 2.0 - ECARTMIL && Vout2 <= 2.0 + ECARTMIL) ||
    (Vout2 >= 1.28 - ECARTMIL && Vout2 <= 1.28 + ECARTMIL))
    tableauSuiveur[3] = '1';
  else 
    tableauSuiveur[3] = '0';
    /*Serial.print("Droite : ");
    Serial.println(tableauSuiveur[3]);*/

  //Capteur en position 5
  if((Vout1  >= 1.6 - ECARTEXT && Vout1 <= 1.6 + ECARTEXT) ||
    (Vout1 >= 0 - ECARTEXT && Vout1 <= 0 + ECARTEXT))
    tableauSuiveur[4] = '1';
  else 
    tableauSuiveur[4] = '0';
    /*Serial.print("Extreme droite : ");
    Serial.println(tableauSuiveur[4]);*/
    //Serial.println(tableauSuiveur);
  
}

float vOutSuiveurExtr(){
  float sensorA1 = A1, vOutExtr = 0 ;

  vOutExtr = analogRead(sensorA1) / 200.00;
  //Tension = analogread / 200
  /*Serial.print("La tension est de : ");
  Serial.println(vOutExtr);*/ 
  delay(50);
  return vOutExtr;
}

float vOutSuiveurMilieu(){
  float sensorA0 = A0, vOutMilieu = 0 ;

  vOutMilieu = analogRead(sensorA0) / 200.00;
  //Tension = analogread / 200
  /*Serial.print("La tension est de : ");
  Serial.println(vOutMilieu);*/
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