/*************************************************
 *************************************************
 **  sketch AnalogReader Demo V1.2 Exemple d'utilisation de la mini lib Analogue pour lecture analogique
 **   Pierre HENRY  05/03/2020
 **
 **   Lecture sous interuption pour ne pas attendre le temps de conversion
 **   Utilisation du timer1 a 100Hz pour lire puis relancer le
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

#define APP_NAME  "AnalogReader V1.2 Demo"


// Objets d'interface pour le convetisseur AD
AnalogReader MonPotentionmetre = AnalogReader(0, 3);  //A0 avec un lissage sur 3 mesures
AnalogReader MaCelulePhoto = AnalogReader(1);         //A1 sans lissage
PulseReader  MonCapteurPiezzo = PulseReader(2);       //A2 un piezzo mode pulse
PulseReader* MaCeluleModePulse = NULL;                // il sera instancié en A1 : on peux instancier plusieur fois le meme port



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println(APP_NAME);

  // begin( n° du port , lissage)
  MonPotentionmetre.begin();        // On demarre les mesures
  MaCelulePhoto.begin(-1, 5);       // On peut ajuster le lissage ici (-1 pour ne pas changer le port)
  MonCapteurPiezzo.begin(-1, 3, 1);  // detect a 3 le depart pulse   a 1 la fin pulse
  Serial.println("Bonjour");
}

void loop() {

  // lecture classique d'un potentiometre
  // avec un lissage sur 3 mesure il n'y a pas de bruit
  // seul les changements sont affichés

  if (MonPotentionmetre.ready()) {
    int V0 = MonPotentionmetre.read();
    Serial.print("Potentiometre V0:");
    Serial.print(V0);
    int M0 = MonPotentionmetre.getMissedRead();
    if (M0) {
      Serial.print(" Lecture ratée :");
      Serial.print(M0);
    }
    Serial.println();
  }




  // Je detecte ici le clignotement d'une lampe devant une cellule
  // avec ajustement automatique du niveau
  static  bool tictac = false;
  static  int ticLow = 0;
  static  int ticHigh = 255;
  static int ticMed = 128;
  static int V1 = 128;
  if (MaCelulePhoto.ready()) {
    V1 = MaCelulePhoto.read();

    // calcul du point millieu
    ticMed = ticLow + (ticHigh - ticLow) / 2;

    // ajustement a la volée du maxi et mini
    if (V1 <= ticMed) {
      ticLow = min(ticLow, V1);
      if (ticHigh > ticMed) ticHigh --;
    } else {
      ticHigh = max(ticHigh, V1);
      if (ticLow < ticMed) ticLow ++;
    }

    // si je vois un flash j'affiche "Tac"
    if ( tictac != (V1 > ticMed) ) {
      tictac = !tictac;
      if (tictac) {
        Serial.print(F("->Tac"));
        Serial.print(" V1:");
        Serial.print(V1);
        int M1 = MaCelulePhoto.getMissedRead();
        if (M1)  { // si V > 0,1V
          Serial.print(" M1:");
          Serial.print(M1);
        }
        Serial.println();
      }
    }
  }

  // petit affichage de l'etat du maxi mini de la cellule toutes les 10 secondes
  static long T1 = millis();
  if (millis() - T1 > 10000) {
    T1 += 10000;
    int N = MonPotentionmetre.getADValue();
    Serial.print(" Potentionmetre:");
    Serial.print(N);
    N = MaCelulePhoto.getADValue();
    Serial.print(" CelulePhoto:");
    Serial.print(N);
    N = MonCapteurPiezzo.getADValue();
    Serial.print(" CapteurPiezzo:");
    Serial.print(N);

    Serial.print(" Low:");
    Serial.print(ticLow);
    Serial.print(" Highr:");
    Serial.print(ticHigh);
    Serial.print(" Med:");
    Serial.println(ticMed);
  }


  // La cellule est vue par le detecteur de pulse
  //
  if (MaCeluleModePulse && MaCeluleModePulse->ready()) {

    int M2 = MaCeluleModePulse->getMissedRead();
    int L2 = MaCeluleModePulse->getLength();
    int B2 = MaCeluleModePulse->getBPM();
    int V2 = MaCeluleModePulse->read();// a lire en dernier

    Serial.print("CeluleModePulse(1)=");
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



  // un capteur piezzo seul les changements de valeurs sont affiché
  //
  if (MonCapteurPiezzo.ready()) {
    int M2 = MonCapteurPiezzo.getMissedRead();
    int L2 = MonCapteurPiezzo.getLength();
    int B2 = MonCapteurPiezzo.getBPM();
    int V2 = MonCapteurPiezzo.read();   // a lire en dernier

    Serial.print("CeluleModePulse(1)=");
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


  // ajout ou supression d'un capteur
  // la frequence du timer est recalculé pour avoir une lecture a 50Hz de chaque capteur


  if (Serial.available())   {
    char inChar = (char)Serial.read();
    switch (inChar) {
      case 'P':
        if (MaCeluleModePulse == NULL) {
          Serial.println("Creation d'un capteur pulse sur A1 ");
          MaCeluleModePulse = new PulseReader(1, ticMed + 1 , ticMed - 1); //
          MaCeluleModePulse->begin();
        }  else {
          Serial.println("delete  capteur pulse A1");
          delete  MaCeluleModePulse;
          MaCeluleModePulse = NULL;
        }
        break;

    }
  }

}
