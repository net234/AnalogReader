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
      V1.1 P.HENRY  06/03/2020
      Choix de l'entree ADC0 ... ADC7
      Meilleur documentation de l'initialisation ADC
 **   V1.2 P.HENRY  25/05/2020
 **    Multi instances (A0..A7)
 *************************************************
 *************************************************/
#pragma once
#include "Arduino.h"
#include "TimerOne.h"


// la frequence d'interuption  permet de lire regulierement l'AD
// la frequence du timer est egale a FREQUENCE_TIMER * nombre d'instance
// pour 8 capteurs : 50 * 8 = 400Hz
// elle ne doit pas etre superieur a 10.000Hz sinon le convertisseur AD
// ne sera pas pret a fournir la nouvelle valeur.

#define  FREQUENCE_TIMER 50  //  frequence d'interuption en Hz par defaut (A changer via set frequence)

// la frequence est ajusté a chaque creation/destruction d'instance
// begin / end   ne touche pas a la frequence le capteur est lu mais la donnée n'est pas prise en compte
// setFreqence() permet d'ajuster la frequence a tout moment, la premiere valeure lue de chaque instance peut etre invalide

class AnalogReader {
  public:

    // Constructeur
    AnalogReader(const byte pin = 0, const byte lissage = 1);
    ~AnalogReader();

    // Methodes public
    void   begin(const int pin = -1, const int lissage = -1);        // Activation de la lecture cyclique sur la ADC (0..7)
    void   end();                 // Arret de la lecure cyclique
    bool   ready();               // Une lecture prete ?
    short  read();                // Recuperation de la valeur lue
    short  getMissedRead();       // Nombre de lecture non recupérée
    bool   setFrequence(const long frequence = FREQUENCE_TIMER);        // Changement de la frequence de base (50HZ)
    // Devrais etre privé mais utilisé par les interuptions
    void   _putValue(const int aValue);
    byte   _pin {0};                  // AD Pin
    AnalogReader* _next{NULL};
    // Variables privée
  private:
    void   _startTimer();
    void   _stopTimer();
    bool   _active {false};          // activé/desactivé par un begin/end
    byte   _lissage {1};
    bool   _valueChanged {false};    // Une nouvelle Valeur est presente dans l'objet
    short  _value {0};               // Valeur lue sur le convertisseur
    short  _missedRead{0};           // nombre de valeur differents non lue depuis le dernier read()
    // valeur sous interuption  (volatil?)
    short __ADValue {0};             // Valeur lue
    bool  __ADNewValue {false};      // Presence d'une Nouvelle valeur
    short __ADMissed {0};            // Nombre de valaur non recuperée

    

};

// debug
