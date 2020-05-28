/*************************************************
 *************************************************
 **  AnalogReader.cpp  Objet d'interface pour lecture analogique
 **   Pierre HENRY  27/05/2020
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
 **    From scratch with arduino.cc totorial :)
 **   V1.1 P.HENRY  06/03/2020
 **    Choix de l'entree ADC0 ... ADC7
 **    Meilleur documentation de l'initialisation ADC
 **   V1.2 P.HENRY  25/05/2020
 **    Multi instances (A0..A7)
 **   V1.2.1 P.HENRY 27/05/2020
 **    Ajout de PulseReader en derivé d'AnalogReader
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



//AnalogReader est un oblet specialisé pour lire un port analogique
//les port sont lu en tache de fond via une interuption chaque port est lu a 50HZ
// begin(pin,lissage) permet d'initialiser le port et une moyenne flotante pour eviter les parasites (1 = pas de lissage)
// end()  arrete la lecture virtuellement  pour arreter la lecture reelement il faut detruire l'instance
// ready() est vrai chaque fois q'une *nouvelle* valeur est lue : une valeur differente de la precedente
// read()  ramene la derniere valeur lue sans delais.

class AnalogReader {
  public:

    // Constructeur
    AnalogReader(const byte pin = 0, const byte lissage = 1);
    ~AnalogReader();

    // Methodes public
    void     begin(const int pin = -1, const int lissage = -1);        // Activation de la lecture cyclique sur la ADC (0..7)
    void     end();                 // Arret de la lecure cyclique
    virtual  bool ready();          // Une nouvelle valeur ?  (Valeur differente de la precedente)
    uint16_t read();                // Recuperation de la nouvelle valeur
    uint16_t getMissedRead();       // Nombre de lecture non recupérée
    uint16_t getADValue();          // Lecture de la derniere valeur AD lue
    bool     setFrequence(const long frequence = FREQUENCE_TIMER);        // Changement de la frequence de base (50HZ)
    // Devrais etre privé mais utilisé par les interuptions
    virtual void _putValue(const int aValue);
    byte   _pin {0};                 // AD Pin
    AnalogReader* _next{NULL};
    // Variables privée
  protected:
    void   _startTimer();
    void   _stopTimer();
    bool   _active {false};          // activé/desactivé par un begin/end
    byte   _lissage {1};
    bool   _valueChanged {false};    // Une nouvelle Valeur est presente dans l'objet
    short  _value {0};               // Valeur lue sur le convertisseur
    short  _missedRead{0};           // nombre de valeur differents non lue depuis le dernier read()
    // valeur sous interuption  (volatil?)
    volatile uint16_t __ADNewValue {
      false
    };      // Presence d'une Nouvelle valeur
    volatile uint16_t __ADValue = 0;           // Valeur lue
    volatile uint16_t __ADMissed = 0;          // Nombre de valeurs non recuperée
};

//PulseReader est un oblet specialisé pour detecter des pulses sur un port analogique
//l'objet retourne la frequence et le BPM du pulse
class PulseReader : public AnalogReader {
  public:
    // Constructeur
    PulseReader(const byte pin, const byte highlevel = 200, const byte lowlevel = 50) : AnalogReader{pin}  {
      _pulseInterval = 0;
      _highLevel = highlevel;
      _lowLevel = lowlevel;
    }
    // Methodes public
    void   begin(const int pin = -1, const int highlevel = -1, const int lowlevel = -1);   // Activation de la lecture cyclique sur la ADC (0..7)
    //    void   end();           // Arret de la lecure cyclique
    virtual bool  ready();
    //    int   read();
    int   getLength();
    int   getBPM();
    //    int   getMissedRead();
    virtual void   _putValue(const int aValue);

  protected:
   
    uint16_t _pulseLength = 0;       // duree en frequence timer unit
    uint16_t _pulseInterval = 0;      // duree en frequence timer unit 65 sec max
    uint8_t _highLevel = 200;
    uint8_t _lowLevel = 50;

    // Valeur sous interuption
    //Variables Static Locales aux interuptions (volatil ?)
    volatile bool     __pulseStarted = false;
    volatile uint16_t __pulseLengthCompteur = 0;
    volatile uint16_t __pulseLength = 0;
    volatile uint16_t __pulseLevel {0};
    volatile uint16_t __pulseIntervalCompteur{0};
    volatile uint16_t __pulseIntervalLatch{0};
    volatile uint16_t __pulseInterval{0};
    volatile uint16_t __pulseMaxLevel{0};

};




// debug
