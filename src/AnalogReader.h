/*************************************************
 *************************************************
 **  AnalogReader.cpp  Objet d'interface pour lecture analogique
 **   Pierre HENRY  05/03/2020
 **
 **   Lecture sous interuption en "vol de cycle"
 **   Utilisation du timer1 a 1000Hz pour lire puis relancer le
 **   converisseur AD.
 **
 **   Merci a :
 **     Stephan Maugars pour la maquette
 **     amandaghassaei  pour le tuto AD
 **
 **
 **
 **
 **   V1.0 P.HENRY  05/03/2020
 **   From scratch with arduino.cc totorial :)
 *    V1.1 P.HENRY  06/03/2020
 *    Choix de l'entree ADC0 ... ADC7
 *    Meilleur documentation de l'initialisation ADC
 *************************************************
 *************************************************/
#pragma once
#include "Arduino.h"
#include "TimerOne.h"


// la frequence d'interuption  permet de lire regulierement l'AD
// elle ne doit pas etre superieur a 10.000Hz sinon le convertisseur AD
// ne sera pas pret a fournir la nouvelle valeur.
const int  FrequenceTimer = 1000;  //  frequence d'interuption en Hz

class AnalogReader {
  public:
    // Constructeur
    AnalogReader()  {
      _ADValue = 0;
      _ADValueChanged = false;
      _MissedADRead = 0;
    }
    //   AnalogReader(const byte aPinNumber ) {  // constructeur
    //    AnalogReader();
    //}
    // Methodes public
    void   begin(const byte pin);         // Activation de la lecture cyclique sur la ADC (0..7)
    void   end();           // Arret de la lecure cyclique
    bool   ADReady();    // Une lecture prete ?
    int    getADValue();      // Recuperation de la valeur lue
    int    getMissedADRead(); // Nombre de lecture non recupérée
    // Variables privée
  private:
    bool   _ADValueChanged;       // Une nouvelle Valeur est presente dans l'objet
    int    _ADValue;              // Valeur lue sur le convertisseur 
    int    _MissedADRead;         // nombre de valeur non lue depuis le dernier getValue()

};