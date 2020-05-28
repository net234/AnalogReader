/*************************************************
 *************************************************
 **  sketch AnalogReader V1.2.1 Test de developpement de la lin AnalogReader
 **   Pierre HENRY  27/05/2020
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
 **   V1.2.1 P.HENRY 27/05/2020
 **    Ajout de PulseReader en derivé d'AnalogReader
 **
 *************************************************
 *************************************************/
#include  "Arduino.h"
#include  "AnalogReader.h"

#define APP_NAME  "AnalogReader V1.2.1"


// Objets d'interface pour le convetisseur AD
AnalogReader* MonLecteurAnalogique0 = NULL;
AnalogReader* MonLecteurAnalogique1 = NULL;
PulseReader* MonLecteurPulse2 = NULL;




void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println(APP_NAME);
   
  // Choix de l'entree ADC
  MonLecteurAnalogique0 = new AnalogReader(2, 1);
  MonLecteurAnalogique1 = new AnalogReader(1, 5);
  //MonLecteurAnalogique2 = new AnalogReader(2);



  MonLecteurAnalogique0->begin();
  MonLecteurAnalogique1->begin();
  //MonLecteurAnalogique2->begin();


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
    //   Serial.print("V1:");
    //   Serial.println(V1);
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

  if (MonLecteurPulse2 && MonLecteurPulse2->ready()) {
    int M2 = MonLecteurPulse2->getMissedRead();
    int L2 = MonLecteurPulse2->getLength();
    int B2 = MonLecteurPulse2->getBPM();
    int V2 = MonLecteurPulse2->read();

    Serial.print("Pulse level(2)=");
    Serial.print(V2);
    Serial.print(" Len=");
    Serial.print(L2);
    Serial.print(" BPM=");
    Serial.print(B2);
    if (M2) {
    Serial.print(" M2:");
    Serial.print(M2);
    }
    Serial.println();
  }

  if (MonLecteurAnalogique0 && MonLecteurAnalogique0->ready()) {
    int V0 = MonLecteurAnalogique0->read();
    int M0 = MonLecteurAnalogique0->getMissedRead();
    Serial.print("V0:");
    Serial.print(V0);
    Serial.print(" M0:");
    Serial.println(M0);
  }

  if (Serial.available())   {
    char inChar = (char)Serial.read();
    switch (inChar) {
      case '0':
        if (MonLecteurAnalogique0 == NULL) {
          Serial.println("New Lecteur0");
          MonLecteurAnalogique0 = new AnalogReader(0, 3);
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
          MonLecteurAnalogique1 = new AnalogReader(1, 3);
          MonLecteurAnalogique1->begin();
        }  else {
          Serial.println("delete Lecteur1");
          delete  MonLecteurAnalogique1;
          MonLecteurAnalogique1 = NULL;
        }
        break;
      case '2':
        if (MonLecteurPulse2 == NULL) {
          Serial.println("New LecteurPulse2");
          MonLecteurPulse2 = new PulseReader(2,20,2);
          MonLecteurPulse2->begin();
        }  else {
          Serial.println("delete LecteurPulse2");
          delete  MonLecteurPulse2;
          MonLecteurPulse2 = NULL;
        }
        break;
      case 'F':

        if (MonLecteurAnalogique0) {
          Serial.println("Frequence 1000HZ");
          MonLecteurAnalogique0->setFrequence(1000);

        }

        break;
      case 'G':
        if (MonLecteurAnalogique0) {
          Serial.println("Frequence 50HZ");
          MonLecteurAnalogique0->setFrequence(50);

        }
        break;


    }
  }

}
