#define BTN 2
#define LED 13


void setup() {
  pinMode(LED, OUTPUT);
  pinMode(BTN, INPUT);
}

void loop() {
  static int rezim = 0;
  if (tlacitkoStisknuto()) {
    ++rezim;
    if (rezim > 2) rezim = 0;
  }
  switch (rezim) {
    case 0:
      digitalWrite(LED_BUILTIN, LOW);
      break;

    case 1:
      digitalWrite(LED_BUILTIN, HIGH);
      break;

    case 2:
      break;
  }
}

int tlacitkoStisknuto() {
  static int stav_tlacitka_predchozi;
  int stav_tlacitka = digitalRead(BTN);
  int _stisknuto = 0;

  if (stav_tlacitka_predchozi == 0 && stav_tlacitka == 1) {
    _stisknuto = 1;
  }
  stav_tlacitka_predchozi = stav_tlacitka;
  return _stisknuto;
}
