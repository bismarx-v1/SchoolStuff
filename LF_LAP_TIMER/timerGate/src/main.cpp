#include <Arduino.h>
#include "LiquidCrystal_I2C.h"


#define DEBUG                     0
#define DEBUG_MSPF                1
#define DISPLAY_TYPE              1  // 0 - soft, 1 - real.
#define GATE_RETRIGGER_BLOCK_TIME 5000

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
#define DISPLAY_TEXT_OFFSET  16


/**
 * @param STUFF The stuff in the if.
 * @brief Sets the last state and does some stuff.
 */
#define STATE_ON_ENTER(STATE_CUR, STATE_PREV, STUFF)                                                                           \
  {                                                                                                                            \
    if(STATE_CUR != STATE_PREV) {                                                                                              \
      STATE_PREV = STATE_CUR;                                                                                                  \
      STUFF                                                                                                                    \
    }                                                                                                                          \
  }

/**
 * @param STUFF The stuff in the if.
 * @brief Sets the last state and does some stuff.
 */
#define STATE_ON_EXIT(STATE_CUR, STATE_PREV, STUFF)                                                                            \
  {                                                                                                                            \
    if(STATE_CUR != STATE_PREV) {                                                                                              \
      STUFF                                                                                                                    \
    }                                                                                                                          \
  }


LiquidCrystal_I2C lcd(DISPLAY_ADDRESS, DISPLAY_COLUMNS, DISPLAY_ROWS);


void checkPin(uint8_t pin, bool* val, bool* change) {
  *change     = 0;
  bool pinVal = digitalRead(pin);
  if(pinVal != *val) {
    *val    = pinVal;
    *change = 1;
  }
}

int32_t deltaTime() {
  static uint32_t loopTime[2];
  static uint8_t  loopTimeIdx = 0;
  loopTimeIdx                 = loopTimeIdx ^ 1;
  loopTime[loopTimeIdx]       = millis();
  return (loopTime[loopTimeIdx] - loopTime[loopTimeIdx ^ 1]);
}


bool gateVal;
bool gateChange;
bool armVal;
bool stopVal;
bool resetVal;
char displayBuffer[DISPLAY_CHARS];
char stupidDisplayBuffer[DISPLAY_ROWS][DISPLAY_COLUMNS];

void hotfixGateUpdate() {
  static uint32_t timeLast = 0;
  Serial.print("Interrupt");
  uint32_t timeNow = millis();
  
  *(&gateChange) = 0;
  if(timeLast + 500 < timeNow) {
    bool pinVal    = digitalRead(2);
    Serial.print((pinVal?"RISING":"FALLING"));
    if(pinVal != *(&gateVal)) {
      *(&gateVal)    = pinVal;
      *(&gateChange) = 1;
    }
    timeLast = timeNow;
  }
  Serial.print('\n');
}

void setup() {
  Serial.begin(115200);
#if DEBUG == 1
  Serial.begin(115200);
#endif
  pinMode(PIN_GATE, INPUT);
  pinMode(PIN_BTN_ARM, INPUT);
  pinMode(PIN_BTN_STOP, INPUT);
  pinMode(PIN_BTN_RESET, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), hotfixGateUpdate, CHANGE);

  gateVal  = digitalRead(PIN_GATE);
  armVal   = digitalRead(PIN_BTN_ARM);
  stopVal  = digitalRead(PIN_BTN_STOP);
  resetVal = digitalRead(PIN_BTN_RESET);

#if DISPLAY_TYPE == 1
  lcd.init();
  lcd.backlight();
  lcd.clear();
#elif DEBUG != 1
  Serial.begin(115200);
#endif
#if DEBUG == 1
  Serial.print("\x1b[?25l");                     // Hide cursor.
  Serial.print("\x1b[48;5;111m\x1b[38;5;202m");  // Terminal colours.
#endif
  memset(displayBuffer, 32, DISPLAY_CHARS);  // Clear display.
  deltaTime();
}


void loop() {
  static uint8_t  state        = 0;
  static uint8_t  statePrev    = 0xff;
  static int32_t  deltaTimeVar = 0;
  bool            armChange;
  bool            stopChange;
  bool            resetChange;
  static uint32_t timerStartTime = 0;
  static uint32_t timerTime;
  uint32_t        timeNow = millis();
  uint16_t        timerMs;
  uint8_t         timerS;
  uint8_t         timerM;
  bool            displayBufferChanged = 0;
  static uint8_t  tempTick1            = 0xff;
  static bool     keepFrame            = 0;


  //checkPin(PIN_GATE, &gateVal, &gateChange);
  checkPin(PIN_BTN_ARM, &armVal, &armChange);
  checkPin(PIN_BTN_STOP, &stopVal, &stopChange);
  checkPin(PIN_BTN_RESET, &resetVal, &resetChange);


  switch(state) {
    case STATE_WAITING:
      STATE_ON_ENTER(
      state, statePrev, for(uint8_t idx = 0; idx < 7; idx++) { displayBuffer[idx] = "WAITING"[idx]; } displayBufferChanged = 1;
      keepFrame = 1;)

      if(armChange == 1 && armVal == 0) {  // Arm button.
        state = STATE_ARMED;
      }

      STATE_ON_EXIT(state, statePrev, keepFrame = 0; displayBufferChanged = 1;)
      break;

    case STATE_ARMED:
      STATE_ON_ENTER(
      state, statePrev, for(uint8_t idx = 0; idx < 7; idx++) { displayBuffer[idx] = "ARMED  "[idx]; } displayBufferChanged = 1;
      keepFrame = 1;)

      if(gateChange == 1 && gateVal == 0) {
        state          = STATE_RUNNING;
        timerStartTime = timeNow;
      } else if(stopChange == 1 && stopVal == 0) {
        state = STATE_WAITING;
      } else if(resetChange == 1 && resetVal == 0) {
        state = STATE_WAITING;
      }

      STATE_ON_EXIT(state, statePrev, keepFrame = 0; displayBufferChanged = 1;)
      break;
    case STATE_RUNNING:
      STATE_ON_ENTER(
      state, statePrev,
      for(uint8_t idx = 0; idx < 9; idx++) { displayBuffer[idx + DISPLAY_TIMER_OFFSET] = "000:00:00"[idx]; } keepFrame = 0;)

      if(armChange == 1 && armVal == 0) {
        state = STATE_WAITING;
      } else if(resetChange == 1 && resetVal == 0) {
        state = STATE_ARMED;
      }

      timerTime = timeNow - timerStartTime;

      timerMs = timerTime % 1000;
      timerS  = (timerTime - timerMs) / 1000 % 60;
      timerM  = (timerTime - timerMs - timerS) / 60000 % 999;


      displayBuffer[DISPLAY_TIMER_OFFSET + 3] = ':';
      displayBuffer[DISPLAY_TIMER_OFFSET + 6] = ':';

      for(uint8_t idx = 0; idx < 3; idx++) {
        displayBuffer[idx + DISPLAY_TIMER_OFFSET + 0] = ((timerM / (uint16_t)pow(10, 2 - idx)) % 10) + '0';
      }

      for(uint8_t idx = 0; idx < 2; idx++) {
        displayBuffer[idx + DISPLAY_TIMER_OFFSET + 4] = ((timerS / (uint16_t)pow(10, 1 - idx)) % 10) + '0';
      }

      for(uint8_t idx = 0; idx < 2; idx++) {
        displayBuffer[idx + DISPLAY_TIMER_OFFSET + 7] = ((timerMs / 10 / (uint16_t)pow(10, 1 - idx)) % 10) + '0';
      }

      if(gateChange == 1 && gateVal == 0) {
        if(timerTime > GATE_RETRIGGER_BLOCK_TIME) {
          state = STATE_END;
        }
      }

      displayBufferChanged = 1;

      STATE_ON_EXIT(state, statePrev, keepFrame = 1;)
      break;

    case STATE_END:
      STATE_ON_ENTER(
      state, statePrev, keepFrame = 1; for(uint8_t idx = 0; idx < 5; idx++) {
        displayBuffer[idx + DISPLAY_TEXT_OFFSET] = "ENDED"[idx];
      } displayBufferChanged = 1;

      )
      if(resetChange == 1 && resetVal == 0) {
        state = STATE_ARMED;
      } else if(stopChange && stopVal == 0) {
        state = STATE_WAITING;
      }

      STATE_ON_EXIT(state, statePrev, keepFrame = 0; displayBufferChanged = 1;)
      break;

    default: state = 0;
#if DEBUG == 1
      Serial.println("STATE ERR");
#endif
      break;
  }
#if DEBUG == 1 || DEBUG_MSPF == 1
  if(tempTick1 >= 32) {
    for(uint8_t idx = 0; idx < 2; idx++) { displayBuffer[idx + 28] = ((deltaTimeVar / (uint16_t)pow(10, 1 - idx)) % 10) + '0'; }
    displayBuffer[30]    = 'm';
    displayBuffer[31]    = 's';
    displayBufferChanged = 1;
    tempTick1            = 0;
  }
  tempTick1++;
#endif

  if(displayBufferChanged == 1) {
    for(uint8_t idx1 = 0; idx1 < DISPLAY_ROWS; idx1++) {
      for(uint8_t idx2 = 0; idx2 < DISPLAY_COLUMNS; idx2++) {
        stupidDisplayBuffer[idx1][idx2] = displayBuffer[idx1 * DISPLAY_COLUMNS + idx2];
      }
    }

#if DISPLAY_TYPE == 1
    lcd.setCursor(0, 0);
    lcd.print(stupidDisplayBuffer[0]);
    lcd.setCursor(0, 1);
    lcd.print(stupidDisplayBuffer[1]);
#else
    Serial.print("\x1b[1;1H");
    Serial.print(stupidDisplayBuffer[0]);
    Serial.print("\x1b[2;1H");
    Serial.print(stupidDisplayBuffer[1]);
#endif
    if(keepFrame != 1) {
      memset(displayBuffer, 32, DISPLAY_CHARS);  // Clear display.
    }
  }

#if DEBUG == 1
  //Serial.print("\x1b[1;1H\x1b[J");
  Serial.print("/-\\|"[(timeNow / 250) % 4]);
  Serial.print("\tArm: ");
  Serial.print(armVal);
  Serial.print(armChange);
  Serial.print("\tRst: ");
  Serial.print(resetVal);
  Serial.print(resetChange);
  Serial.print("\tStp: ");
  Serial.print(stopVal);
  Serial.print(stopChange);
  Serial.print("\tGte: ");
  Serial.print(gateVal);
  Serial.print(gateChange);
  Serial.print("\tTme: ");
  Serial.print(timerTime);
  Serial.print("\tStt: ");
  Serial.print(state);
#endif

  deltaTimeVar = deltaTime();
  if(MS_PER_LOOP - deltaTimeVar > 0) {
    delay(MS_PER_LOOP - deltaTimeVar);
  } else {
#if DEBUG == 1
    Serial.print("\tdTime too long");
#endif
  }
  deltaTime();

#if DEBUG == 1
  Serial.print("\tdTm: ");
  Serial.print(deltaTimeVar, DEC);
  Serial.print("\t");
  Serial.print("\\-/|"[(timeNow / 250) % 4]);
  Serial.print('\n');
#endif
}
