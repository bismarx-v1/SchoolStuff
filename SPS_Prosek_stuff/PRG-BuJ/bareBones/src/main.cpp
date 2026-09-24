#include <Arduino.h>

/* HW: 1 LED, 1 BUTTON
Requirment: one button press starts the first function
first function - blinking with interval_1 = 500ms, second button press starts the second fucntion, 
second function: SOS-type blinking, third press makes the LED go from LOW to HIGH but in a smooth manner,
fourth press turns it off. 
*/

// goto = instruction that jumps to a label (it skips the code similar to switch() but without a condition) 

//instructions end with ;
//functions have ()



//USEFULL KEYBINDS: CTRL B for {}, CTRL D for copying line, HOME/END for start and end of a line 


/* goto syntax:  
goto fucntion_1;
 fucntion_1:
 */

// label = name that ends with :


#define LED 2 
#define BUTTON 3
int functions = 0;
const int INTERVAL_1 = 500;
const int INTERVAL_2 = 200;
unsigned long time_start = 0;
int led_state = 0;
int button_state = 0;
int button_state_last = 0;
int num_of_cycles_200;
int num_of_cycles_500;


void time_interval() {
  time_start = millis();
}

int time_500() {
  unsigned long current_time = millis();
  if (time_start + INTERVAL_1 <= current_time) {
    return 1; 
  }
  else {
    return 0;
  }
}

int time_200() {
  unsigned long current_time = millis();
  if (time_start + INTERVAL_2 <= current_time) {
    return 1; 
  }
  else {
    return 0;
  }
}



void setup() {
  pinMode(LED, OUTPUT); //function with 2 params
  pinMode(BUTTON, INPUT_PULLUP);
}

void loop() {
  button_state = digitalRead(BUTTON);
  if (button_state != button_state_last) {
    button_state_last = button_state;
    functions++; 
  }

  switch(functions) {
    case 0: // off case
      led_state = 0;
      digitalWrite(LED, led_state);
    break;

    case 1: //labels end with : 
      fucntion_1(); //500ms blink
      digitalWrite(LED, led_state);
    break;

    case 2: 
      function_2();
      digitalWrite(LED, led_state);
    break;

    case 3: 
      function_3();
      digitalWrite(LED, led_state);
    break;
    
    default: 
      functions = 0;
    break;
  }
}


void fucntion_1() {
  if(time_500()) {
    time_interval();
    led_state = 1 - led_state;
  }
}

void function_2() {
  if (num_of_cycles_200 < 5) {
    if(time_200()) {
      time_interval();
      led_state = 1 - led_state;
      num_of_cycles_200++;
    }
  }
  else if (num_of_cycles_500 < 7) {
    if(time_500()) {
      time_interval();
      led_state = 1 - led_state;
    }
  }
  else {num_of_cycles_200 = 0; 
    num_of_cycles_500 = 0;
  }
}

void function_3() {
  int direction = 0;
  int intesnsity = 0;
  if (intesnsity >= 255) {  //= must be on the right side: <= not =<
    direction = 1;
  }

  else if (intesnsity <= 0) {
    direction = 0;
  }

  if (direction == 0) { //rising 0-255
    intesnsity++;
  }  else { // falling 255-0
    intesnsity--;
  }
  analogWrite(LED, intesnsity);
}