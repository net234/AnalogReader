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
AnalogReader MonLecteurAnalogique1(1);




void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println(APP_NAME);
  
  //  Serial.println(ADC1);
  delay(2000);
  //pinMode(A0, INPUT); 
  // Choix de l'entree ADC
  MonLecteurAnalogique1.begin();


  Serial.println("Bonjour");

}

void loop() {
  // put your main code here, to run repeatedly:

 // if (MonLecteurAnalogique1.ADReady()) {
    static int V1Old = -1; 
 //   int V = MonLecteurAnalogique.getADValue() - 128;  Si l'entree est polarisee a VCC/2
    int V1 = MonLecteurAnalogique1.getADValue();
 //     int V1 = analogRead(A1);
    int M1 = MonLecteurAnalogique1.getMissedADRead();
    if (V1 != V1Old)  { // si V > 0,1V
      V1Old = V1;
      Serial.print("V1:");
      Serial.print(V1);
      Serial.print(" M1:");
      Serial.println(M1);
 //   }
  }
}
