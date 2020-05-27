/*************************************************
 *************************************************
 **  sketch AnalogReader V1.1 Exemple d'utilisation de la mini lib Analogue pour lecture analogique
 **   Pierre HENRY  05/03/2020
 **
 **   Lecture sous interuption pour ne pas attendre le temps de conversion
 **   Utilisation du timer1 a 1000Hz pour lire puis relancer le
 **   converisseur AD.
 **
 **   Merci a :
 **     Stephan Maugars pour la maquette de depart
 **     amandaghassaei  pour le tuto AD
 **
 **
 **
 **
 **   V1.0 P.HENRY  05/03/2020
 **    From scratch with arduino.cc totorial :)
 **   V1.1 P.HENRY  06/03/2020
 **    Choix de l'entree ADC0 ... ADC7
 **    Meilleur documentation de l'initialisation ADC
 **   V1.2 P.HENRY  25/05/2020
 **    Multi instances (A0..A7)
 **
 *************************************************
 *************************************************/
#include  "Arduino.h"
#include  "AnalogReader.h"

#define APP_NAME  "AnalogReader V1.2"


// Objets d'interface pour le convetisseur AD
AnalogReader* MonLecteurAnalogique0 = NULL;
AnalogReader* MonLecteurAnalogique1 = NULL;
AnalogReader* MonLecteurAnalogique2 = NULL;




void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println(APP_NAME);

  //  Serial.println(ADC1);
  //pinMode(A0, INPUT);
  // Choix de l'entree ADC
  MonLecteurAnalogique0 = new AnalogReader(0,3);
  MonLecteurAnalogique1 = new AnalogReader(1);
  MonLecteurAnalogique2 = new AnalogReader(2);



  MonLecteurAnalogique0->begin();
  MonLecteurAnalogique1->begin();
  MonLecteurAnalogique2->begin();


  Serial.println("Bonjour");

}

void loop() {
  // put your main code here, to run repeatedly:
  static  bool tictac = false;
  static  int ticLow = 0;
  static  int ticHigh = 255;
  static int ticMed = 128;
  static int V1 = 128;
  if (MonLecteurAnalogique1 && MonLecteurAnalogique1->ready()) {
    V1 = MonLecteurAnalogique1->read();
    ticMed = ticLow + (ticHigh - ticLow) / 2;
    if (V1 <= ticMed) {
      ticLow = min(ticLow, V1);
      if (ticHigh > ticMed) ticHigh --;
    } else {
      ticHigh = max(ticHigh, V1);

      if (ticLow < ticMed) ticLow ++;
    }
    if ( tictac != (V1 > ticMed) ) {
      tictac = !tictac;
      if (tictac) {
        Serial.print(F("->Tac"));
        Serial.print(" V1:");
        Serial.print(V1);
        int M1 = MonLecteurAnalogique1->getMissedRead();
        if (M1)  { // si V > 0,1V
          Serial.print(" M1:");
          Serial.print(M1);
        }
        Serial.println();
      }
    }
  }
  static long T1 = 0;
  if (T1++ > 1000000) {
    T1 = 0;
    Serial.print("V1:");
    Serial.print(V1);

    Serial.print(" Low:");
    Serial.print(ticLow);
    Serial.print(" Highr:");
    Serial.print(ticHigh);
    Serial.print(" Med:");
    Serial.println(ticMed);


  }
  if (MonLecteurAnalogique2 && MonLecteurAnalogique2->ready()) {
    static int V2Old = -1;
    int V2 = MonLecteurAnalogique2->read();
    //     int V1 = analogRead(A2);
    int M2 = MonLecteurAnalogique2->getMissedRead();
    if (V2 != V2Old)  { // si V > 0,1V
      V2Old = V2;
      Serial.print("V2:");
      Serial.print(V2);
      Serial.print(" M2:");
      Serial.println(M2);
    }
  }
  if (MonLecteurAnalogique0 && MonLecteurAnalogique0->ready()) {
    static int V0Old = -1;
    int V0 = MonLecteurAnalogique0->read();
    int M0 = MonLecteurAnalogique0->getMissedRead();
    if (V0 != V0Old)  { // si V > 0,1V
      V0Old = V0;
      Serial.print("V0:");
      Serial.print(V0);
      Serial.print(" M0:");
      Serial.println(M0);
    }
  }
  if (Serial.available())   {
    char inChar = (char)Serial.read();
    switch (inChar) {
      case '0':
        if (MonLecteurAnalogique0 == NULL) {
          Serial.println("New Lecteur0");
          MonLecteurAnalogique0 = new AnalogReader(0,3);
          MonLecteurAnalogique0->begin();

        }  else {
          Serial.println("delete Lecteur0");
          delete  MonLecteurAnalogique0;
          MonLecteurAnalogique0 = NULL;
        }
        break;
      case '1':
        if (MonLecteurAnalogique1 == NULL) {
          Serial.println("New Lecteur1");
          MonLecteurAnalogique1 = new AnalogReader(1);
          MonLecteurAnalogique1->begin();
        }  else {
          Serial.println("delete Lecteur1");
          delete  MonLecteurAnalogique1;
          MonLecteurAnalogique1 = NULL;
        }
        break;
      case '2':
        if (MonLecteurAnalogique2 == NULL) {
          Serial.println("New Lecteur2");
          MonLecteurAnalogique2 = new AnalogReader(2);
          MonLecteurAnalogique2->begin();
        }  else {
          Serial.println("delete Lecteur2");
          delete  MonLecteurAnalogique2;
          MonLecteurAnalogique2 = NULL;
        }
        break;

    }
  }

}
