#define BUTTON1 2
#define LED_BUILTIN 13

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
}
void loop()
{
  static int LAST_BUTTON_STATE = 0;
  int BUTTON_STATE = digitalRead(BUTTON1);

  if(BUTTON_STATE == 1 && )
  {
    
  }
}