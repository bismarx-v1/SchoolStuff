#include <Arduino.h>
#define LED 2
#define SW 4

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

void setup(){
  pinMode(LED, OUTPUT);
  pinMode(SW, INPUT);
  pinMode(SW, INPUT_PULLUP);
}

void loop() 
{

  unsigned long cas_ted = millis();
  static unsigned long cas_predtim = cas_ted;
  static int stav_led = LOW;
  static int rezim = 0;

  if (tlacitkoStisknuto()) {
    ++rezim;
    if (rezim > 4) rezim = 0;
  }
  switch (rezim){



  case 1:
    digitalWrite(LED, LOW);
  break;


  case 2:
    if(cas_ted >= cas_predtim + 500) {
      stav_led = !stav_led;
      cas_predtim = cas_ted;
    }
    digitalWrite(2, stav_led);
  break;


  case 3:
        if(cas_ted >= cas_predtim + 250) {
      stav_led = !stav_led;
      cas_predtim = cas_ted;
    }
    digitalWrite(2, stav_led);
  break;

  case 4:
    digitalWrite(LED, HIGH);
  break;
  }
}