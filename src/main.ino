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
 #define opti45LEFT2 175
 #define opti45RIGHT2 100
 #define opti90LEFT2 417
 #define opti90RIGHT2 237
 #define opti180LEFT2 300
 #define opti180RIGHT2 500
 #define ECARTMIL 0.25
 #define ECARTEXT 0.4
 #define DISTIRMUR 300
 #define DISTSOMUR 12
 #define DISTAVVIRAGE 0.25
 #define DISTAPVIRAGE 2
 #define DROITE 1
 #define AVANT 2
 #define GAUCHE 3
 #define ARRIERE 4
 int etatMoteurs = 0;
 int etape = 0;
 long int comptClicsATTG = 0, comptClicsREELD = 0;
 char tableauSuiveur [6] = "00000";
 float deplacementLabi[100];
 float VG = 0.1;
 float VD = 0.1;
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
}
/* ******************************************************************   LOOP   ************************************************** */
void loop() 
{
  ENCODER_Reset(LEFT);
  //Tant que l'objet n'est pas detecte (ici c'est le sifflet de 5kH)
    while(DetectionSifflet == 0){
      Serial.println("Je debute un deplacement");

      //Tant que le robot ne detecte pas d'intersection, il suit la ligne
      while(DetectionIntersection() == 0 && distanceSO(0) == 1){
        ActionSuiveur();
      }

      Serial.print("Intersection rencontree a l'etape ");
      Serial.println(etape);
      Serial.print("Apres une distance de ");
      Serial.println(ENCODER_Read(LEFT));

      //A l'intersection il enregistre la distance qu'il a parcouru
      EnregistrerDistance(&etape);
      Serial.print("La distance enregistree est de ");
      Serial.println(deplacementLabi[etape]);

      //Se place au milieu de l'intersection
      ActionSuiveurDist(DISTAVVIRAGE);

      //Incrementation d'etape pour changer de position dans le tableau de memoire du chemin
      etape++;

      //Prise de la decision du bon virage a effectuer
      EnregistrerVirage(&etape);
      Serial.print("Le virage effectuer et enregistrer est de ");
      Serial.println(deplacementLabi[etape]);

      //Incrementation d'etape pour changer de position dans le tableau de memoire du chemin
      etape++;

      RenitClics();
    }

    for( int i = 0; i < etape; i++){
      Serial.print(deplacementLabi[i]);
      Serial.print("\t");
      if(i % 10 == 0){
        Serial.print("\n");
      }
    }

    //S'il detecte l'objet a ramasser il effectue la sequence d'alignement et de prise de l'objet
    //ALIGNEMENT AVEC PIXY
    //RAMASSAGE DE L'objet

    /*
    //Se reajuste pour etre dans la direction inverse a celle qu'il est arrive
    virage2moteurs(180, RIGHT, opti180RIGHT2)

    //Fait le trajet enregistrer en chemin inverse
    RevenirLabi();
    }
    */
  }
  delay(10);// Delais pour décharger le CPU
}

/* ******************************************************************FONCTIONS PERSONNELLES************************************************** */

//Fonction permettant de faire le chemin inverse
void RevenirLabi(){
  for(int i = etape; i > 0; i--){
    
    //Si i est pair, c"est une distance
    if(i % 2 == 0){
      ActionSuiveurDist(deplacementLabi[i]);
      ActionSuiveurDist(DISTAVVIRAGE);
    }

    //Si i est impair c'est un virage
    else{
      if(deplacementLabi[i] == DROITE){
        virage2moteurs(90, LEFT, opti90LEFT2);
        ActionSuiveurDist(DISTAPVIRAGE);
      }

      else if(deplacementLabi[i] == AVANT){
        ActionSuiveurDist(DISTAPVIRAGE);
      }

      else if(deplacementLabi[i] == GAUCHE){
        virage2moteurs(90, RIGHT, opti90LEFT2);
        ActionSuiveurDist(DISTAPVIRAGE);
      }
    }
}

//Fonction ou le robot gere un cul de sac et elimine les donnes de distance de sa memoire
int gererCDS(int *numEtape){
  //Pour prendre la position d'une distance
  *numEtape -= 1;
  //Le robot fait un U-Turn et refait la derniere distance enregistree
  virage2moteurs(180, RIGHT, opti180RIGHT2);
  ActionSuiveurDist(deplacementLabi[*numEtape]);
  Serial.print("Pour l'etape ");
  Serial.print(*numEtape);
  Serial.print("   Le nombre de clic enregistrer a la derniere position --> ");
  Serial.print(deplacementLabi[*numEtape]);

  //ActionSuiveurDist(deplacementLabi[*numEtape]);
  //AvancerTestAvecEnre(deplacementLabi[*numEtape], *numEtape);
  deplacementLabi[*numEtape] = 0;
  //Decrementation de l'etape pour modifier la valeur du virage
  *numEtape--;
}

//Fonction qui permet d'enregistrer le nombre de clics effectues dans la chaine representant son parcours.
void EnregistrerDistance(int *numEtape){
  deplacementLabi[*numEtape] = ENCODER_Read(LEFT);
}

//Fonction de prise de decision du robot et de lenregistrement de cette position.
int EnregistrerVirage(int *numEtape){
  /*Serial.print(distanceIR(2));
  Serial.print("\t");
  Serial.print(distanceSO(0));
  Serial.print("\t");
  Serial.print(distanceIR(3));
  Serial.print("\t");*/
  if(deplacementLabi[*numEtape] == 0){

  //Test d'abord sil peut tourner a droite
    if(distanceIR(2) == 1){
      Serial.println("Virage a droite");
      virage2moteurs(90, RIGHT, opti90RIGHT2);
      deplacementLabi[*numEtape] = DROITE;
      ActionSuiveurDist(DISTAPVIRAGE);
    }
  
  //Test s'il peut aller tout droit
    else if(distanceSO(0) == 1){
      Serial.println("Aller tout droit");
      deplacementLabi[*numEtape] = AVANT;
      ActionSuiveurDist(DISTAPVIRAGE);
    }

  //Test s'il peut tourner a gauche
    else if(distanceIR(3) == 1){
      Serial.println("Virage a gauche");
      virage2moteurs(90, LEFT, opti90LEFT2);
      deplacementLabi[*numEtape] = GAUCHE;
      ActionSuiveurDist(DISTAPVIRAGE);
    }

  //Sinon c'est qu'il est dans un cul de sac
    else{
      Serial.println("Cul de sac");
      gererCDS(numEtape);
      deplacementLabi[*numEtape] = ARRIERE;
    }
  }

  //Si la valeur attitrer a la case du virage n'est pas 4 et pas 0
  //Le robot a alors deja effectuer des virages a partir de la meme intersection
  else if(deplacementLabi[*numEtape] != 4 && deplacementLabi[*numEtape] != 0 ){

    if(distanceIR(2) == 1){
      Serial.println("Virage a droite 2");
      virage2moteurs(90, RIGHT, opti90RIGHT2);
      deplacementLabi[*numEtape] + 1;
      //ActionSuiveurDist(DISTAPVIRAGE);
      AvancerCorriger(DISTAPVIRAGE);
    }
    else if(distanceSO(0) == 1){
      Serial.println("Aller tout droit 2");
      deplacementLabi[*numEtape] + 2;
      //ActionSuiveurDist(DISTAPVIRAGE);
      AvancerCorriger(DISTAPVIRAGE);
    }
    else if(distanceIR(3) == 1){
      Serial.println("Virage a gauche 2");
      virage2moteurs(90, LEFT, opti90LEFT2);
      deplacementLabi[*numEtape] + 3;
      //ActionSuiveurDist(DISTAPVIRAGE);
      AvancerCorriger(DISTAPVIRAGE);
    }
  }
  else{
    Serial.println("Cul de sac 2");
    gererCDS(numEtape);
  }
  return 0;
}

//Si un des deux senseurs IR lateraux ne detecte plus de mur, la fonction retourne 1
int DetectionIntersection(){
  //Si les detecteurs IR detectes une distance plus grande que la distance du mur alors la condition deviens vrai
  if (distanceIR(3) == 1 || distanceIR(2) == 1){
   // Serial.println("Je detecte une intersection");
    return 1;
  }
  else{
    //Serial.println("Je ne detecte pas une intersection");
    return 0;
  }
}

//Fonction qui retourne la valeur de la PIN analog du detecteur de sifflet soit 1 ou 0
int DetectionSifflet(){
  int pinMicro = 47;
  int Siffler = 0;

  Siffler = digitalRead(pinMicro);
  return Siffler;
}

//Generateur de int aleatoire
int randomInt(int a, int b){
  return rand() % (b - a) + a;
}

//Fonction qui retourne 1 si le capteur infrarouge detecte un mur plus pres que DISTIRMUR
int distanceIR (int id) {
  float ir = ROBUS_ReadIR(id);
  
  if(ir < DISTIRMUR)
    return 1;
  else
    return 0;
}

//Fonction qui retourne 1 lorsque le sonar detecte un mur plus pres que DISTSOMUR
int distanceSO (int id) {
  id = 0;
  float so = SONAR_GetRange(id);

  if (so > DISTSOMUR) 
    return 1;
  return 0;
}

//Fonction qui envoie les correction moteurs pour que le robot suive la ligne pour une distance precise
float ActionSuiveurDist(float distance){
  //Noir = 1 et blanc = 0
  if(distance < 100){
    distance = MetresToClics(distance);
  }
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
      //Serial.println("Correction gauche");
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
     // Serial.println("Garder les meme vitesses");
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

//Fonction qui envoie les correction moteurs pour que le robot suive la ligne pour une distance indeterminee
float ActionSuiveur(){
  //Noir = 1 et blanc = 0

    UpdateSuiveur();

    //Correction a Gauche minime
    if (memcmp(tableauSuiveur, "11100", 5 ) == 0 || memcmp(tableauSuiveur, "01100", 5 ) == 0)
    {
      //1 1 1 0 0 : 0 1 1 0 0
      //Serial.println("Correction gauche");
      MOTOR_SetSpeed(RIGHT, VD + correctionMinime);
      MOTOR_SetSpeed(LEFT, VG);
      correctionMinime -= 0.002;
    }

    //Correction a gauche importante
    else if (memcmp(tableauSuiveur, "11000", 5 ) == 0 || memcmp(tableauSuiveur, "10000", 5 ) == 0)
    {
      //1 1 0 0 0 : 1 0 0 0 0
      //Serial.println("Correction gauche importante");
      MOTOR_SetSpeed(RIGHT, VD + correctionImportante);
      MOTOR_SetSpeed(LEFT, VG);
      correctionImportante -= 0.002;
    }
    
    //Correction a droite minime
    else if(memcmp(tableauSuiveur, "00111", 5 ) == 0 || memcmp(tableauSuiveur, "00110", 5 ) == 0)
    {
      //0 0 1 1 1 : 0 0 1 1 0 : 0 0 0 1 1 : 0 0 0 0 1
      //Serial.println("Correction droite");
      MOTOR_SetSpeed(RIGHT, VD);
      MOTOR_SetSpeed(LEFT, VG + correctionMinime);
      correctionMinime -= 0.002;
    }

    //Correction a droite importante
    else if(memcmp(tableauSuiveur, "00011", 5 ) == 0|| memcmp(tableauSuiveur, "00001", 5 ) == 0)
    {
      // 0 0 0 1 1 : 0 0 0 0 1
      //Serial.println("Correction importante droite");
      MOTOR_SetSpeed(RIGHT, VD);
      MOTOR_SetSpeed(LEFT, VG + correctionImportante);
      correctionImportante -= 0.002;
    }
    else if(memcmp(tableauSuiveur, "00000", 5 ) == 0|| memcmp(tableauSuiveur, "01000", 5 ) == 0|| memcmp(tableauSuiveur, "00100", 5 ) == 0|| memcmp(tableauSuiveur, "00010", 5 ) == 0){
     // Serial.println("Garder les meme vitesses");
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

//Fonction qui met a jour letat du tableau representant les senseurs
void UpdateSuiveur(){
  float Vout1 = vOutSuiveurExtr(), Vout2 = vOutSuiveurMilieu();

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
  
}

//Fonction retournant la valeur de tension a la sortie desdeux senseurs exterieurs du circuit du suiveur de ligne
float vOutSuiveurExtr(){
  float sensorA1 = A1, vOutExtr = 0 ;

  vOutExtr = analogRead(sensorA1) / 200.00;
  delay(50);
  return vOutExtr;
}

//Fonction retournant la valeur de tension a la sortie des trois senseurs centraux du circuit du suiveur de ligne
float vOutSuiveurMilieu(){
  float sensorA0 = A0, vOutMilieu = 0 ;

  vOutMilieu = analogRead(sensorA0) / 200.00;
  delay(50);
  return vOutMilieu;
}

//Avancer avec le PID pour une distance precise
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

//Avancer avec le PID pour une distance indeterminee
//A retester le fonctionnement.......
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

//Fonction permettant de reculer avec le PID, l'acceleration et la deceleration
void ReculerCorriger(float distance){
  float VG = -0.01;
  float VD = -0.01;
  long int comptClicsATTG = 0, comptClicsREELD = 0;
  int clics = - MetresToClics((0.9 * distance));
  
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
    if (VG > - 0.4)
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
    if (VG < - 0.25)
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

//Fonction faisant tourner le robot suur lui-meme avec un PID
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

//Fonction du PID pour le virage a droite
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

//Fonction du PID pour le virage a gauche
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