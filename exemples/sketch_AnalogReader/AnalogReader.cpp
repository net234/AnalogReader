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
#include  "AnalogReader.h"


char Sprint1H( byte aByte) {
  aByte &= 0xF;
  Serial.print((char)aByte + (aByte <= 9 ? '0' : 'A' -  10));
}
char Sprint2H( byte aByte) {
  Sprint1H(aByte >> 4);
  Sprint1H(aByte);
}
char Sprint4H( word aWord) {
  Sprint2H(aWord >> 8);
  Sprint2H(aWord);
}

void  SprintH(String string, long aValue) {
  Serial.print(string);
  Sprint4H(aValue);
  Serial.println();
}
void  Sprint(String string, long aValue) {
  Serial.print(string);
  Serial.println(aValue);
}



//====== Variable globale
static AnalogReader* __AnalogReaderFirst = NULL;     // First instance
static AnalogReader* __AnalogReaderCurrent = NULL;   // Current reading Instance

//====== interuption de gestion de l'AD lancéee a FrequenceTimer Hertz (50)
void __callback_AnalogReader(void) {

  if (__AnalogReaderCurrent != NULL) {

    __AnalogReaderCurrent->putValue(ADCH);
    __AnalogReaderCurrent = __AnalogReaderCurrent->next;
  }
  if (__AnalogReaderCurrent == NULL) {
    __AnalogReaderCurrent = (AnalogReader*) __AnalogReaderFirst;
  }
  if (__AnalogReaderCurrent != NULL) {
    ADMUX  = (ADMUX & 0xF0) | (__AnalogReaderCurrent->_pin & 0xF); // choix de l'entree (0 a 7)
    ADCSRA |= (1 << ADSC);     // Relance le convetisseur  (le bit ADSC de ADSRA est mis a 1)
  }
}


// constructor
AnalogReader::AnalogReader(const byte pin, const byte lissage) {
  _pin = pin;
  if (lissage > 0 && lissage <= 100) {
    _lissage = lissage;
  } else {
    _lissage = 1;
  }
  _active = false;
  _valueChanged = false;
  _value = 0;
  next = NULL;
  //Serial.begin(115200);
  Sprint("Construct = ", (long)pin);
  Sprint("Valeur de __AnalogReaderFirst = ", (long)__AnalogReaderFirst);
  _stopTimer();
  if (__AnalogReaderFirst == NULL) {
    __AnalogReaderFirst = this;
  } else {
    AnalogReader* aPtr = __AnalogReaderFirst;
    Sprint("Valeur de aPtr = ", (long)aPtr);
    delay(500);
    while ( aPtr->next != NULL) {
      aPtr = aPtr->next;
      Sprint("Valeur de aPtr = ", (long)aPtr);
    };
    aPtr->next = this;
    Sprint("Valeur de aPtr finale = ", (long)aPtr);
  }
  _startTimer();
  //  Sprint("Valeur de __AnalogReaderFirst = ",(long)__AnalogReaderFirst);
  //  Sprint("Valeur de __AnalogReaderFirst->next = ",(long)__AnalogReaderFirst->next);
  //
}

AnalogReader::~AnalogReader() {
  _stopTimer();
  if (__AnalogReaderFirst == this) {
    __AnalogReaderFirst = this->next;
  } else {
    AnalogReader* aPtr = __AnalogReaderFirst;
    while (aPtr != NULL && aPtr->next != this) {
      aPtr = aPtr->next;
    };
    if (aPtr != NULL) {
      aPtr->next = this->next;
    };
  }
  if (__AnalogReaderFirst != NULL) {
    _startTimer();
  }
}


// Le numero de la pin est 0 a 7 (ADC0 a ADC7)
void  AnalogReader::begin(const int pin, const int lissage) {
  if ( pin >= 0) {
    _pin = pin;
  }
  if ( lissage >= 0 ) {
    _lissage = lissage;
  }
  _active = true;
  Serial.print("begin pin = ");
  Serial.println(_pin);
}

void  AnalogReader::end() {
  _active = false;
}




void AnalogReader::_startTimer() {
  _stopTimer();

  // Initialisation de l'ADC (Analog Digital Converter)
  // voir page 217 a 220 ATMega328 DataSheeet
  ADMUX  = 0;
  ADCSRA = 0;
  ADCSRB = 0;


  ADMUX  |= (_pin & 0xF << MUX0); // choix de l'entree (0 a 7)
  ADMUX  |= (1 << REFS0); // reference voltage : Interne sur AVCC (0..3) 1=Vcc 3=1,1v
  ADMUX  |= (1 << ADLAR); //alignement a droite : mode 8 bit  (0/1)

  // l'horloge de l'ad doit etre entre 100Kh et 200Khz (800Khz pour 8 bit)
  // valeur possible devant MUX0 (page 2
  //  1    // div 2   - 16mHz/2   = 8MHz
  //  2    // div 4   - 16mHz/4   = 4MHz
  //  3    // div 8   - 16mHz/8   = 2MHz
  //  4    // div 16  - 16mHz/16  = 1MHz
  //  5    // div 32  - 16mHz/32  = 500KHz  50µS
  //  6    // div 64  - 16mHz/64  = 250KHz  100µS
  //  7    // div 128 - 16mHz/128 = 128KHz  200µS
  ADCSRA |= (6 << ADPS0) ; //set ADC clock div 64 (1 a 7)
  ADCSRA |= (1 << ADEN);  //Active l'ADC

  //Initialisation Timer1 on ajuste la frequence en fonction du nombre d'element;

  AnalogReader* aPtr = (AnalogReader*)__AnalogReaderFirst;
  byte N = 0;
  while (aPtr != NULL) {
    aPtr = aPtr->next;
    N++;
  }

  Serial.print("Nbr reader = ");
  Serial.println(N);
  Serial.print("Timer = ");
  Serial.println(1000000 / FrequenceTimer / N);

  __AnalogReaderCurrent = NULL;  // il sera mis en place par le callback
  Timer1.initialize(1000000 / FrequenceTimer / N); // Timer reglé en microsecondes
  Timer1.attachInterrupt(__callback_AnalogReader);    // attaches __callback_AnalogReader() pour gerer l'interuption
}

void  AnalogReader::_stopTimer() {
  Serial.println("Timer OFF ");
  Timer1.stop();
  Timer1.detachInterrupt();
  // Arret AD
  ADMUX  = 0;
  ADCSRA = 0;
  ADCSRB = 0;
  __AnalogReaderCurrent = NULL;
}


bool  AnalogReader::ready() {
  // On a deja une valeur non lue dans l'objet
  if (!_valueChanged) {
    // On regarde si une valeur a ete lue par l'AD
    noInterrupts();
    if (__ADNewValue) {
      _value = __ADValue;
      __ADNewValue = false;
      _valueChanged = true;
      _missedRead += __ADMissed;
      __ADMissed = 0;
    }
    interrupts();
  }
  return (_valueChanged);
}

short AnalogReader::read() {
  if ( !ready()) {
    return (-1);
  };
  _valueChanged = false;
  return (_value);
}

short AnalogReader::getMissedRead() {
  short result = _missedRead;
  _missedRead = 0;
  return (result);
}

// est appelee sous interuption
void volatile AnalogReader::putValue(int aValue) {
  if (_active) {
    if ( _lissage > 1) {
      aValue = (aValue + __ADValue * (_lissage - 1) ) / _lissage;        // Avec un lissage
    }
    if (__ADValue != aValue) {
      __ADValue = aValue;
      if ( __ADValue != _value) {
        if ( __ADNewValue) {
          __ADMissed++;            // Comptage des valeurs non lues
        } else {
          __ADNewValue = true;   // Signale a l'objet que la lecture a eut lieu
        }
      }
    }
  }
}
