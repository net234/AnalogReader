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
 *************************************************
 *************************************************/
#include  "Arduino.h"
#include  "AnalogReader.h"

#define APP_NAME  "AnalogReader V1.1"


// Objet d'interface pour le convetisseur AD
AnalogReader MonLecteurAnalogique;




void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println(APP_NAME);
  //  Serial.println(ADC1);
  delay(2000);

  // Choix de l'entree ADC
  MonLecteurAnalogique.begin(1);


  Serial.println("Bonjour");

}

void loop() {
  // put your main code here, to run repeatedly:

  if (MonLecteurAnalogique.ADReady()) {
 //   int V = MonLecteurAnalogique.getADValue() - 128;  Si l'entree est polarisee a VCC/2
    int V = MonLecteurAnalogique.getADValue();
    int M = MonLecteurAnalogique.getMissedADRead();
    if ((abs(V) > 5)  ) { // si V > 0,1V
      Serial.print("V:");
      Serial.print(V);
      Serial.print(" M:");
      Serial.println(M);
    }
  }
}
