#include <Arduino.h>
#define LED 3
#define SW 4
#define POT A0

const byte pwmLED = 3;

// define directions for LED fade
#define UP 0
#define DOWN 1

// constants for min and max PWM
const int minPWM = 0;
const int maxPWM = 255;

// State Variable for Fade Direction
byte fadeDirection = UP;

// Global Fade Value
// but be bigger than byte and signed, for rollover
int fadeValue = 0;

// How smooth to fade?
byte fadeIncrement = 5;

// millis() timing Variable, just for fading
unsigned long previousFadeMillis;

// How fast to increment?
int fadeInterval = 2000;

//FUNKCE  
int tlacitkoStisknuto() 
{
  static int stav_tlacitka_predchozi;
  int stav_tlacitka = digitalRead(SW);
  int _stisknuto = 0;

  if (stav_tlacitka_predchozi == 0 && stav_tlacitka == 1) {
    _stisknuto = 1;
  }
  stav_tlacitka_predchozi = stav_tlacitka;
  return _stisknuto;
}


void setup() {
  // put pwmLED into known state (off)
  analogWrite(pwmLED, fadeValue);
  pinMode(LED, OUTPUT);
  pinMode(SW, INPUT_PULLUP);
}

void doTheFade(unsigned long thisMillis) {
   // is it time to update yet?
   // if not, nothing happens
   if (thisMillis - previousFadeMillis >= fadeInterval) {
      // yup, it's time!
      if (fadeDirection == UP) {
         fadeValue = fadeValue + fadeIncrement;
         if (fadeValue >= maxPWM) {
            // At max, limit and change direction
            fadeValue = maxPWM;
            fadeDirection = DOWN;
         }
      } else {
         //if we aren't going up, we're going down
         fadeValue = fadeValue - fadeIncrement;
         if (fadeValue <= minPWM) {
            // At min, limit and change direction
            fadeValue = minPWM;
            fadeDirection = UP;
         }
      }
      // Only need to update when it changes
      analogWrite(pwmLED, fadeValue);

      // reset millis for the next iteration (fade timer only)
      previousFadeMillis = thisMillis;
   }
}

void loop() 
{

  unsigned long cas_ted = millis();
  static unsigned long cas_predtim = cas_ted;
  static int stav_led = LOW;
  static int rezim = 0;

  if (tlacitkoStisknuto()) {
    ++rezim;
    if (rezim > 3) rezim = 0;
  }
  switch (rezim){



  case 0:
    digitalWrite(LED, LOW);
  break;


  case 1:
    if(cas_ted >= cas_predtim + 2000) {
      stav_led = !stav_led;
      cas_predtim = cas_ted;
    }
    digitalWrite(2, stav_led);
  break;


  case 2:
        if(cas_ted >= cas_predtim + 250) {
      stav_led = !stav_led;
      cas_predtim = cas_ted;
    }
    digitalWrite(2, stav_led);
  break;
}
   // get the current time, for this time around loop
   // all millis() timer checks will use this time stamp
   unsigned long currentMillis = millis();

   doTheFade(currentMillis);
}