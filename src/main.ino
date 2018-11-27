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
//#include <Pixy2.h>
#include <Pixy2I2C.h>


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
#define DISTSOMUR 17.00
#define DISTAVVIRAGE 0.11
#define DISTAPVIRAGE 0.265
#define DROITE 1
#define AVANT 2
#define GAUCHE 3
#define ARRIERE 4
int etatMoteurs = 0;
int fromage =0;
int etape = 0;
int tempsRandom;
long int comptClicsATTG = 0, comptClicsREELD = 0;
char tableauSuiveur [6] = "00000";
char tableauSuiveurTemp [6] = "00000";
float deplacementLabi[100];
float VG = 0.15;
float VD = 0.15;
int maxX =  170;
int minX = 160;
int maxH =  60;
int minH = 50;
int go = 0;
Pixy2I2C pixy;
/* ******************************************************************  SETUP  ************************************************** */
void setup()
{
  BoardInit();
  Serial.begin(9600);
  Serial1.begin(115200);//////////////////////--------TestBras
  pinMode(47, INPUT);
  delay(500);
  Serial1.write('1');
  for (int i = 0; i < 100; i++){
    deplacementLabi[i] = 0;
  }
  pixy.init();
  delay(3000);
  
  Serial.println("GO!");
 /* while(1)
  {
    while(ROBUS_IsBumper(3) == 1)
    {
    Serial1.write('1');
    Serial1.write('2');
    printit();
    delay(1000);
    
    }
    while(ROBUS_IsBumper(3) == 0)
    {
      delay(100);
    }
    
  }*/
}
/* ******************************************************************   LOOP   ************************************************** */
void loop() 
{
  ENCODER_Reset(LEFT);
  if(ROBUS_IsBumper(3) == 1)
  {
    while(fromage == 0)
    {

    detectionObjet();
    //pixy.ccc.getBlocks();
    Serial.println("Je debute un deplacement");

    //Tant que le robot ne detecte pas d'intersection ou de mur devant lui ou d'objet il suit la ligne
    while(DetectionIntersection() == 0 && SONAR_GetRange(0) > 8.00 && fromage == 0){
      detectionObjet();
      ActionSuiveur();
      
    }
      MOTOR_SetSpeed(LEFT, 0);
      MOTOR_SetSpeed(RIGHT, 0);
      if(DetectionIntersection() == 1)
        Serial.println("Je detecte une intersection");
      else if(fromage == 1)
        Serial.println("Je detecte un objet");
      /*Serial.print("Distance du sonar avant : ");
      Serial.println(SONAR_GetRange(0));
      Serial.print("Distance IR droite : ");
      Serial.println(ROBUS_ReadIR(2));
      Serial.print("Distance IR gauche : ");
      Serial.println(ROBUS_ReadIR(3));*/
      //A l'intersection ou lorsqu'il detecte un objet il enregistre la distance qu'il a parcouru
      if(fromage == 0)
        EnregistrerDistance();

    //if(!pixy.ccc.numBlocks){
      //Si le robot n'est pas dans un cul de sac il avance jusqu'au centre de l'intersection
      if(DetectionIntersection() == 1 && fromage == 0){
        detectionObjet();
        ActionSuiveurDist(DISTAVVIRAGE);
      }

      //Si c'est un cul de sac il le gere et efface les donnees de sa memoire
      if(DetectionIntersection() == 0 && distanceSO(0) == 1 && fromage == 0){
        gererCDS();
        Serial.println("Jai fini de gerer le cul de sac");
      }
      
      //Prise de la decision du bon virage a effectuer
      if (fromage == 0)
        EnregistrerVirage();
      Serial.println("---------------------------------------------");
      RenitClics();
    }
    Serial.println("Je detecte un objet et donc je vais m'aligner dessus");
    centrerObjet();
  }
  
  if(ROBUS_IsBumper(1) == 1){
    for( int i = 0; i < etape; i++)
    {
      Serial.print(deplacementLabi[i]);
      Serial.print("\t");
      if(i != 0){
        if(i % 10 == 0){
          Serial.print("\n");
        }
      }
    }
  for (int i = 0; i < 100; i++){
      deplacementLabi[i] = 0;
  }
    etape = 0;
    ReinitMoteurs();
  }
  if(ROBUS_IsBumper(0) == 1){
    Serial.print("Distance du sonar avant : ");
      Serial.println(SONAR_GetRange(0));
      Serial.print("Distance IR droite : ");
      Serial.println(ROBUS_ReadIR(2));
    Serial.print("Distance IR gauche : ");
    Serial.println(ROBUS_ReadIR(3));
    Serial.println("--------------------------");
  }
  //S'il detecte l'objet a ramasser il effectue la sequence d'alignement et de prise de l'objet
    //ALIGNEMENT AVEC PIXY
    //RAMASSAGE DE L'objet

    //Se reajuste pour etre dans la direction inverse a celle qu'il est arrive
    
  delay(10);// Delais pour décharger le CPU
}

/* ******************************************************************FONCTIONS PERSONNELLES************************************************** */
void detectionObjet()
{
    // grab blocks!
  pixy.ccc.getBlocks();
  
  // If there are detect blocks, print them!
  if (pixy.ccc.numBlocks)
  {
    Serial.println("Detected ");
    fromage = 1;
    VG = 0.1;
    VD = 0.1;
  }
}
void printit()
{
    int i = 0;
    float x =0;
    float y =0;
    float h =0;
    float w =0;
    pixy.ccc.getBlocks();
      if (pixy.ccc.numBlocks)
      {
          for ( i=0; i<pixy.ccc.numBlocks; i++)
          {
            w = pixy.ccc.blocks[i].m_width;
            h = pixy.ccc.blocks[i].m_height;
            x = pixy.ccc.blocks[i].m_x;
            y = pixy.ccc.blocks[i].m_y;
          }
          Serial.print(" x: ");
        Serial.print(x);
        Serial.print(" y: ");
        Serial.print(y);
        Serial.print(" w: ");
        Serial.print(w);
        Serial.print(" H: ");
        Serial.println(h);
      }
}
//Fonction de communication avec pixy pour se centrer sur l'objet
void centrerObjet()
{
    int i = 0;
    float x =0;
    float y =0;
    float h =0;
    float w =0;
 
    int whichDirection = 0;
    
    for (i=0; i<pixy.ccc.numBlocks; i++)
    {
      x = pixy.ccc.blocks[i].m_x;
      y = pixy.ccc.blocks[i].m_y;
      h = pixy.ccc.blocks[i].m_height;
      w = pixy.ccc.blocks[i].m_width;
    }
    if(go == 0)
    {
      while(h < minH && go == 0)//--------------------------------Si l'objet est trop loin
      {
        delay(10);
        while(h < minH && h > 5)                              // Si trop loin
        {                                         
          Serial.print("Avancer, h: ");   
          Serial.println(h);
          ActionSuiveur();
          pixy.ccc.getBlocks();
          if (pixy.ccc.numBlocks)
          {
            for (i=0; i<pixy.ccc.numBlocks; i++)
            {
              h = pixy.ccc.blocks[i].m_height;
            }
          }             
        }
        while(h > maxH)                         // Si trop proche
        {                                         
          Serial.print("Reculer, h: ");   
          Serial.println(h);    
          MOTOR_SetSpeed(RIGHT, -0.1);            // Recule
          MOTOR_SetSpeed(LEFT,  -0.1);             
          pixy.ccc.getBlocks();
          if (pixy.ccc.numBlocks)
          {
            for (i=0; i<pixy.ccc.numBlocks; i++)
            {
              h = pixy.ccc.blocks[i].m_height;
            }
          }   
        }
          pixy.ccc.getBlocks();
          if (pixy.ccc.numBlocks)
          {
            for (i=0; i<pixy.ccc.numBlocks; i++)
            {
              h = pixy.ccc.blocks[i].m_height;
            }
          }
      }
    }
    EnregistrerDistance();
    if(go == 0)
    {
      while(x > minX &&  x <maxX && go == 0)//--------------------------------Si l'objet est centré!!!
      {
          
          MOTOR_SetSpeed(RIGHT, 0);
          MOTOR_SetSpeed(LEFT, 0);
          Serial.println("Arrêt");
          delay(1000);
          Serial1.write('1');
          Serial1.write('2');
          delay(7000);
          Serial.print("height ");
          Serial.print(h);
          Serial.print(" widht ");
          Serial.println(w);
          Serial.println("b1");
          virage2moteurs(180,RIGHT, 0);
          RevenirLabi();
          Serial1.write('3');
          go =1;
      }
    }
    if(go == 0)
    {
      float tourne = 0.05;
      while(x < minX || x >maxX)//--------------------------------Si l'objet est pas centré.....
      {
        delay(10);
        if(x < minX)                               // Si trop à gauche 
        {
          Serial.print("Tourne à gauche, x: ");   
          Serial.print(x);
          Serial.print(" minX: ");   
          Serial.println(minX);
          MOTOR_SetSpeed(RIGHT, tourne);            // Tourne à droite
          MOTOR_SetSpeed(LEFT, -tourne);
          whichDirection = 0;
        }
        else if (x > maxX)                                  // Si trop à droite
        {
          Serial.print("Tourne à droite, x: ");   
          Serial.print(x);
          Serial.print(" maxX: ");   
          Serial.println(maxX);
          MOTOR_SetSpeed(RIGHT, -tourne);            // Tourne à gauche
          MOTOR_SetSpeed(LEFT,   tourne);
          whichDirection = 1;
        }
        else
        {
          Serial.print("Arrêt, x: ");   
          Serial.println(x);
          MOTOR_SetSpeed(LEFT,0);
          MOTOR_SetSpeed(RIGHT, 0);
        }
    
        pixy.ccc.getBlocks();
        if (pixy.ccc.numBlocks)
        {
            for (i=0; i<pixy.ccc.numBlocks; i++)
            {
              x = pixy.ccc.blocks[i].m_x;
              y = pixy.ccc.blocks[i].m_y;
              h = pixy.ccc.blocks[i].m_height;
              w = pixy.ccc.blocks[i].m_width;
            }
          Serial.print("Correction,      x: ");   
          Serial.print(x);
          Serial.print(" minX: ");   
          Serial.print(minX);
          Serial.print(" maxX: ");   
          Serial.println(maxX);
        }
        
      }
    }
    if(go == 0)
    {
      while(x > minX &&  x <maxX && go ==0)//--------------------------------Si l'objet est centré!!!
      {
          
          MOTOR_SetSpeed(RIGHT, 0);
          MOTOR_SetSpeed(LEFT, 0);
          Serial.println("Arrêt");
          delay(1000);
          Serial1.write('2');
          delay(7000);
          Serial.print("height: ");
          Serial.print(h);
          Serial.print(", widht: ");
          Serial.println(w);
          Serial.println("b2");
          virage2moteurs(180,RIGHT, 0);
          RevenirLabi();
          Serial1.write('3');
          go =1;
      }
    }
}

//Fonction permettant de faire le chemin inverse
void RevenirLabi(){
  for(int i = etape ; i >= 0; i--){

    //Si i est pair, c'est une distance
    if(i % 2 == 0){
      Serial.print("J'avance de la distance enregistree a l'etape ");
      Serial.println(i);
      ActionSuiveurDist(deplacementLabi[i]);
      if(i != 0){
        ActionSuiveurDist(DISTAPVIRAGE);
      }
    }

    //Si i est impair, c'est un virage
    else {
      if(deplacementLabi[i] == DROITE){
        Serial.print("Le virage enregistrer est de ");
        Serial.print(deplacementLabi[i]);
        Serial.println(" donc j'effectue un virage a gauche.");
        virage2moteurs(90, LEFT, opti90LEFT2);
        ActionSuiveurDist(DISTAVVIRAGE);
      }

      else if(deplacementLabi[i] == AVANT){
        Serial.print("Le virage enregistrer est de ");
        Serial.print(deplacementLabi[i]);
        Serial.println(" donc je continu tout droit.");
        ActionSuiveurDist(DISTAVVIRAGE);
      }

      else if(deplacementLabi[i] == GAUCHE){
        Serial.print("Le virage enregistrer est de ");
        Serial.print(deplacementLabi[i]);
        Serial.println(" donc j'effectue un virage a droite.");
        virage2moteurs(90, RIGHT, opti90LEFT2);
        ActionSuiveurDist(DISTAVVIRAGE);
      }
    }
  }
}

//Fonction ou le robot gere un cul de sac et elimine les donnes de distance de sa memoire
void gererCDS(){
  //Pour prendre la position d'une distance
  etape --;
  
  Serial.println("Je suis dans un cul de sac");

  //Le robot fait un U-Turn et refait la derniere distance enregistree
  virage2moteurs(180, RIGHT, opti180RIGHT2);
  ActionSuiveurDist(deplacementLabi[etape]);
  ActionSuiveurDist(DISTAPVIRAGE);
  deplacementLabi[etape] = 0;
  etape --;
}

//Fonction qui permet d'enregistrer le nombre de clics effectues dans la chaine
void EnregistrerDistance(){
  deplacementLabi[etape] = ClicsToMetres(ENCODER_Read(LEFT));
  Serial.print("Etape ");
  Serial.print(etape);
  Serial.print(": La distance enregistree est de ");
  Serial.println(deplacementLabi[etape]);
  ENCODER_Reset(LEFT);
  etape ++;
}

int EnregistrerVirage(){
  Serial.print("Etape ");
  Serial.print(etape);
  Serial.print(" = ");
  Serial.print(deplacementLabi[etape]);
  Serial.print(" Effecue --> ");

  if(deplacementLabi[etape] == 0 && fromage == 0)
  {
  //Test d'abord sil peut tourner a droite
    if(distanceIR(2) == 1){
      Serial.println("Virage a droite");
      virage2moteurs(90, RIGHT, opti90RIGHT2);
      deplacementLabi[etape] = DROITE;
      detectionObjet();
      if(fromage == 0)
        ActionSuiveurDist(DISTAPVIRAGE);
    }
  
  //Test s'il peut aller tout droit
    else if(distanceSO(0) == 0){
      Serial.println("Aller tout droit");
      deplacementLabi[etape] = AVANT;
      detectionObjet();
      if(fromage == 0)
        ActionSuiveurDist(DISTAPVIRAGE);
    }

  //Test s'il peut tourner a gauche
    else if(distanceIR(3) == 1){
      Serial.println("Virage a gauche");
      virage2moteurs(90, LEFT, opti90LEFT2);
      deplacementLabi[etape] = GAUCHE;
      detectionObjet();
      if(fromage == 0)
        ActionSuiveurDist(DISTAPVIRAGE);
    }
  }

  //Si la valeur attitrer a la case du virage est 1, 2 ou 3
  //Le robot a alors deja effectuer des virages a partir de la meme intersection
  else if(deplacementLabi[etape] == 1 || deplacementLabi[etape] == 2 || deplacementLabi[etape] == 3 && fromage == 0)
  {
    if(distanceIR(2) == 1){
      Serial.println("Virage a droite 2");
      virage2moteurs(90, RIGHT, opti90RIGHT2);
      deplacementLabi[etape] += 1;
      detectionObjet();
      if(fromage == 0)
        ActionSuiveurDist(DISTAPVIRAGE);
    }
    else if(distanceSO(0) == 1){
      Serial.println("Aller tout droit 2");
      deplacementLabi[etape] += 2;
      detectionObjet();
      if(fromage == 0)
        ActionSuiveurDist(DISTAPVIRAGE);
    }
    else if(distanceIR(3) == 1){
      Serial.println("Virage a gauche 2");
      virage2moteurs(90, LEFT, opti90LEFT2);
      deplacementLabi[etape] += 3;
      detectionObjet();
      if(fromage == 0)
        ActionSuiveurDist(DISTAPVIRAGE);
    }
  }
  etape ++;
  return 0;
}

//Si un des deux senseurs IR lateraux ne detecte plus de mur, la fonction retourne 1
int DetectionIntersection(){
  //Si les detecteurs IR detectes une distance plus grande que la distance du mur alors la condition deviens vrai
  if (distanceIR(3) == 1 || distanceIR(2) == 1){
    return 1;
  }
    return 0;
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
  int ir = ROBUS_ReadIR(id);
  //Serial.println(ir);
  if(ir < DISTIRMUR)
    return 1;
  else
    return 0;
}

//Fonction qui retourne 1 lorsque le sonar detecte un mur plus pres que DISTSOMUR
int distanceSO (int id) {
  float so = SONAR_GetRange(id);
  //Serial.println(so);
  if (so <= DISTSOMUR) 
    return 1;
  return 0;
}

//Fonction qui envoie les correction moteurs pour que le robot suive la ligne pour une distance precise
long int ActionSuiveurDist(float distance){
  //Noir = 1 et blanc = 0
  distance = MetresToClics(distance);
  RenitClics();
  //Serial.println(distance);
  long int comptClics = 0;
  float correctionMinime = 0.04, correctionImportante = 0.07;

  while(comptClics < distance)
  {
    //Compteur de clics
    int leftClics = ENCODER_Read(LEFT);
    ENCODER_Reset(LEFT);
    comptClics += leftClics;
    //Serial.print("Compteur de clics : ");
    //Serial.println(comptClics);

    UpdateSuiveur();

    //Correction a Gauche minime
    if (memcmp(tableauSuiveur, "11100", 5 ) == 0 || memcmp(tableauSuiveur, "01100", 5 ) == 0)
    {
      //1 1 1 0 0 : 0 1 1 0 0
      //Serial.println("Correction gauche");
      MOTOR_SetSpeed(RIGHT, VD + correctionMinime);
      MOTOR_SetSpeed(LEFT, VG);
      //correctionMinime -= 0.002;
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
      //correctionImportante -= 0.002;
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
      //correctionMinime -= 0.002;
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
      //correctionImportante -= 0.002;
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

//Fonction qui envoie les correction moteurs pour que le robot suive la ligne pour une distance indeterminee
float ActionSuiveur(){
  int facteurFonctionSuiveur = 0.002;
  //Noir = 1 et blanc = 0
  float correctionMinime = 0.04, correctionImportante = 0.06;
  UpdateSuiveur();


  //Correction a Gauche minime
  if (memcmp(tableauSuiveur, "11100", 5 ) == 0 || memcmp(tableauSuiveur, "01100", 5 ) == 0)
  {
    //1 1 1 0 0 : 0 1 1 0 0
    //Serial.println("Correction gauche");
    MOTOR_SetSpeed(RIGHT, VD + correctionMinime);
    MOTOR_SetSpeed(LEFT, VG);
    if(correctionMinime > 0.005){
     correctionMinime -= facteurFonctionSuiveur;
    //Serial.println(correctionMinime * 1000);
    //delay(50);
    }
  }

  //Correction a gauche importante
  else if (memcmp(tableauSuiveur, "11000", 5 ) == 0 || memcmp(tableauSuiveur, "10000", 5 ) == 0)
  {
    //1 1 0 0 0 : 1 0 0 0 0
    //Serial.println("Correction gauche importante");
    MOTOR_SetSpeed(RIGHT, VD + correctionImportante);
    MOTOR_SetSpeed(LEFT, VG);
    if(correctionImportante > 0.005){
     correctionImportante -= facteurFonctionSuiveur;
    //Serial.println(correctionMinime * 1000);
    //delay(50);
    }
  }
  
  //Correction a droite minime
  else if(memcmp(tableauSuiveur, "00111", 5 ) == 0 || memcmp(tableauSuiveur, "00110", 5 ) == 0)
  {
    //0 0 1 1 1 : 0 0 1 1 0 : 0 0 0 1 1 : 0 0 0 0 1
    //Serial.println("Correction droite");
    MOTOR_SetSpeed(RIGHT, VD);
    MOTOR_SetSpeed(LEFT, VG + correctionMinime);
    if(correctionMinime > 0.005){
     correctionMinime -= facteurFonctionSuiveur;
    //Serial.println(correctionMinime * 1000);
    //delay(50);
    }
  }

  //Correction a droite importante
  else if(memcmp(tableauSuiveur, "00011", 5 ) == 0|| memcmp(tableauSuiveur, "00001", 5 ) == 0)
  {
    // 0 0 0 1 1 : 0 0 0 0 1
    //Serial.println("Correction importante droite");
    MOTOR_SetSpeed(RIGHT, VD);
    MOTOR_SetSpeed(LEFT, VG + correctionImportante);
    if(correctionImportante > 0.01){
     correctionImportante -= facteurFonctionSuiveur;
    //Serial.println(correctionMinime * 1000);
    //delay(50);
    }
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
//Fonction qui met a jour letat du tableau representant les senseurs
void UpdateSuiveur(){
  float Vout1 = vOutSuiveurExtr(), Vout2 = vOutSuiveurMilieu();
  /*Serial.print("V Extr : ");
  Serial.println(Vout1);
  Serial.print("V Milieu : ");
  Serial.println(Vout2);*/
  //Capteur en position 1
  if((Vout1 >= 2.92 - ECARTEXT && Vout1 <= 2.92 + ECARTEXT) ||
     (Vout1 >= 0 - ECARTEXT && Vout1 <= 0 + ECARTEXT))
    tableauSuiveur[0] = '1';
  else
    tableauSuiveur[0] = '0';

    
  /*Serial.print("Extreme gauche : ");
  Serial.println(tableauSuiveur[0]);*/
  
  //Capteur en position 2
  if((Vout2 >= 2.76 - ECARTMIL && Vout2 <= 2.76 + ECARTMIL) || 
    (Vout2 >= 4.04 - ECARTMIL && Vout2 <= 4.04 + ECARTMIL) ||
    (Vout2 >= 0 - ECARTMIL && Vout2 <= 0 + ECARTMIL))
    tableauSuiveur[1] = '1';
  else 
    tableauSuiveur[1] = '0';
    /*Serial.print("Gauche : ");
    Serial.println(tableauSuiveur[1]);*/

  //Capteur en position 3
  if((Vout2 >= 0 - ECARTMIL && Vout2 <= 0 + ECARTMIL) ||
    (Vout2 >= 0.76 - ECARTMIL && Vout2 <= 0.76 + ECARTMIL) ||
    (Vout2 >= 2.76 - ECARTMIL && Vout2 <= 2.76 + ECARTMIL))
    tableauSuiveur[2] = '1';
  else 
    tableauSuiveur[2] = '0';
    /*Serial.print("Centre : ");
    Serial.println(tableauSuiveur[2]);*/

  //Capteur en position 4
  if((Vout2 >= 0 - ECARTMIL && Vout2 <= 0 + ECARTMIL) ||
    (Vout2 >= 0.76 - ECARTMIL && Vout2 <= 0.76 + ECARTMIL) ||
    (Vout2 >= 2.12 - ECARTMIL && Vout2 <= 2.12 + ECARTMIL))
    tableauSuiveur[3] = '1';
  else 
    tableauSuiveur[3] = '0';
    /*Serial.print("Droite : ");
    Serial.println(tableauSuiveur[3]);*/

  //Capteur en position 5
  if((Vout1  >= 1.5 - ECARTEXT && Vout1 <= 1.5 + ECARTEXT) ||
    (Vout1 >= 0 - ECARTEXT && Vout1 <= 0 + ECARTEXT))
    tableauSuiveur[4] = '1';
  else 
    tableauSuiveur[4] = '0';
    /*Serial.print("Extreme droite : ");
    Serial.println(tableauSuiveur[4]);*/
    //Serial.println(tableauSuiveur);
  
}

//Fonction retournant la valeur de tension a la sortie desdeux senseurs exterieurs du circuit du suiveur de ligne
float vOutSuiveurExtr(){
  float sensorA0 = A0, vOutExtr = 0 ;

  vOutExtr = analogRead(sensorA0) / 200.00;
  //Tension = analogread / 200
  /*Serial.print("La tension est de : ");
  Serial.println(vOutExtr);*/ 
  delay(50);
  return vOutExtr;
}

//Fonction retournant la valeur de tension a la sortie des trois senseurs centraux du circuit du suiveur de ligne
float vOutSuiveurMilieu(){
  float sensorA1 = A1, vOutMilieu = 0 ;

  vOutMilieu = analogRead(sensorA1
  ) / 200.00;
  //Tension = analogread / 200
  /*Serial.print("La tension est de : ");
  Serial.println(vOutMilieu);*/
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
    if (VG < 0.1)
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
  ENCODER_Reset(LEFT);
  ENCODER_Reset(RIGHT);
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

//Transformation de clics en metres
float ClicsToMetres(float nbrClics){
  float circonference, diametre = 0.077;
  float distM;

  distM = nbrClics / 3200 * PI * diametre;
  //Serial.print("distance en metres : ");
  //Serial.println(distM);
  return distM;
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