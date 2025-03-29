/**
 * DEBUG: Program freezes when going to "running" mode.
 */
#include <Arduino.h>  
#include "LiquidCrystal_I2C.h"

#define DEBUG 1

#define PIN_GATE      2
#define PIN_BTN_ARM   5
#define PIN_BTN_STOP  6
#define PIN_BTN_RESET 7

#define DISPLAY_ADDRESS 0x27
#define DISPLAY_COLUMNS 16
#define DISPLAY_ROWS    2
#define DISPLAY_CHARS   (DISPLAY_COLUMNS * DISPLAY_ROWS)

#define STATE_WAITING 0
#define STATE_ARMED   1
#define STATE_RUNNING 2
#define STATE_END     3

#define MS_PER_LOOP 20

#define DISPLAY_TIMER_OFFSET 0

#define DISPLAY_TYPE 1  // 0 - soft, 1 - real.



LiquidCrystal_I2C lcd(DISPLAY_ADDRESS, DISPLAY_COLUMNS, DISPLAY_ROWS);


void checkPin(uint8_t pin, bool* val, bool* change) {
  *change     = 0;
  bool pinVal = digitalRead(pin);
  if(pinVal != *val) {
    *val    = pinVal;
    *change = 1;
  }
}

int64_t deltaTime() {
  static uint64_t loopTime[2];
  static uint8_t  loopTimeIdx = 0;
  loopTimeIdx                 = loopTimeIdx ^ 1;
  loopTime[loopTimeIdx]       = millis();
  return (loopTime[loopTimeIdx] - loopTime[loopTimeIdx ^ 1]);
}


bool gateVal;
bool armVal;
bool stopVal;
bool resetVal;
char displayBuffer[DISPLAY_CHARS];

void setup() {
  #if DEBUG == 1
  Serial.begin(115200);
  #endif
  pinMode(PIN_GATE, INPUT);
  pinMode(PIN_BTN_ARM, INPUT);
  pinMode(PIN_BTN_STOP, INPUT);
  pinMode(PIN_BTN_RESET, INPUT);

  gateVal  = digitalRead(PIN_GATE);
  armVal   = digitalRead(PIN_BTN_ARM);
  stopVal  = digitalRead(PIN_BTN_STOP);
  resetVal = digitalRead(PIN_BTN_RESET);

#if DISPLAY_TYPE == 1
  lcd.init();
  lcd.backlight();
  lcd.clear();
#else
  Serial.begin(115200);
#endif
  memset(displayBuffer, 32, DISPLAY_CHARS);  // Clear display.
  deltaTime();
}


void loop() {
  
  static uint8_t  state        = 0;
  static int64_t  deltaTimeVar = 0;
  bool            gateChange;
  bool            armChange;
  bool            stopChange;
  bool            resetChange;
  static uint32_t timerStartTime = 0;
  static uint32_t timerTime;
  uint16_t        timerMs;
  uint8_t         timerS;
  uint8_t         timerM;
  bool            displayBufferChanged = 0;


  
  checkPin(PIN_GATE, &gateVal, &gateChange);
  checkPin(PIN_BTN_ARM, &armVal, &armChange);
  checkPin(PIN_BTN_STOP, &stopVal, &stopChange);
  checkPin(PIN_BTN_RESET, &resetVal, &resetChange);



  switch(state) {
    case STATE_WAITING:
      if(armChange == 1 && armVal == 0) {  // Arm button.
        state = STATE_ARMED;
      }

      for(uint8_t idx = 0; idx < 7; idx++) {
        displayBuffer[idx] = "WAITING"[idx];
      }
      displayBufferChanged = 1;

      break;

    case STATE_ARMED:
    if(gateChange == 1 && gateVal == 0) {
        state          = STATE_RUNNING;
        timerStartTime = millis();
      }

      for(uint8_t idx = 0; idx < 7; idx++) {
        displayBuffer[idx] = "ARMED  "[idx];
      }
      displayBufferChanged = 1;
      
      break;
    case STATE_RUNNING:

      if(armChange == 1 && armVal == 0) {
        state = STATE_WAITING;
      } else if(resetChange == 1 && resetVal == 0) {
        state = STATE_ARMED;
      }

      timerTime = timerStartTime - millis();

      timerMs = timerTime % 1000;
      timerTime -= timerMs;
      timerS = timerTime / 1000 % 60;
      timerTime -= timerS;
      timerM = timerTime / 60000;

      for(uint8_t idx = 0; idx < 9; idx++) {
        displayBuffer[idx + DISPLAY_TIMER_OFFSET] = "000:00:00"[idx];
      }


      for(uint8_t idx = 2; idx >= 0; idx--) {
        displayBuffer[idx + DISPLAY_TIMER_OFFSET] = (timerM / (10 ^ idx)) % (10 ^ (idx + 1));
      }

      for(uint8_t idx = 1; idx >= 0; idx--) {
        displayBuffer[idx + DISPLAY_TIMER_OFFSET + 4] = (timerS / (10 ^ idx)) % (10 ^ (idx + 1));
      }
      
      for(uint8_t idx = 1; idx >= 0; idx--) {
        displayBuffer[idx + DISPLAY_TIMER_OFFSET + 6] = (timerMs / (10 ^ idx)) % (10 ^ (idx + 1));
      }

      displayBufferChanged = 1;

      if(gateChange == 1 && gateVal == 0) {
        if(timerTime > 1000) {
          state = STATE_END;
        }      
      }

      break;

    case STATE_END:
      if(resetChange == 1 && resetVal == 0) {
        state = STATE_ARMED;
      } else if(stopChange && stopVal == 0) {
        state = STATE_WAITING;
      }

      break;

    default:
      state = 0;
      break;
    }

  if(displayBufferChanged == 1) {
#if DISPLAY_TYPE == 1
    lcd.setCursor(0, 0);
    lcd.print(displayBuffer);
#else
    Serial.print("\x1b[1;1H");
    Serial.print(displayBuffer);
#endif
  }
  
  #if DEBUG == 1
  Serial.print("Arm: ");
  Serial.print(digitalRead(PIN_BTN_ARM));
  Serial.print("\tRst: ");
  Serial.print(digitalRead(PIN_BTN_RESET));
  Serial.print("\tStp: ");
  Serial.print(digitalRead(PIN_BTN_STOP));
  Serial.print("\tGte: ");
  Serial.print(digitalRead(PIN_GATE));
  Serial.print("\tTime: ");
  Serial.println(timerTime);
  #endif

  deltaTimeVar = deltaTime();
  if(MS_PER_LOOP - deltaTimeVar > 0) {
    delay(MS_PER_LOOP - deltaTimeVar);
  }
  deltaTime();
}
