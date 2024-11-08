#define PIN_BTN 2

void setup() 
{
  pinMode(PIN_BTN, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
}


void loop() 
{
  unsigned long cas_ted = millis();
  static unsigned long cas_predtim = cas_ted;
  static int stav_led = LOW;
  static int rezim = 0;

  if (tlacitkoStisknuto()) 
  {
    ++rezim;
    if (rezim > 2) rezim = 0;
  }
  switch (rezim)
  {
  case 0:
    if(cas_ted >= cas_predtim + 1000) 
    {
      stav_led = !stav_led;
      cas_predtim = cas_ted;
    }
    digitalWrite(13, stav_led);
  
  case 1:
    digitalWrite(LED_BUILTIN, HIGH);
  break;

  case 2:
  break;
  }
}





//FUNKCE
int tlacitkoStisknuto() 
{
  static int stav_tlacitka_predchozi;
  int stav_tlacitka = digitalRead(PIN_BTN);
  int _stisknuto = 0;

  if (stav_tlacitka_predchozi == 0 && stav_tlacitka == 1) {
    _stisknuto = 1;
  }
  stav_tlacitka_predchozi = stav_tlacitka;
  return _stisknuto;
}