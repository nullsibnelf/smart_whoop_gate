
#define StatusLed       13
#define Taster01        11
#define Beeper           4

int   T1_state = 1;
int   T1_counter = 0;



void setup() 
{
  pinMode(StatusLed,   OUTPUT);
  pinMode(Beeper,      OUTPUT);
  pinMode(Taster01,    INPUT_PULLUP);
  Serial.begin(9600);                         //  setup serial for debugging
}




void loop()
{
  check_inputs();

  chirp(3);
    
  delay(10000);
}





void check_inputs (void)
{
  T1_state = digitalRead(Taster01);    
}



void chirp (int amount)
{  
  for (int i=0; i<amount; i++)
  {
  digitalWrite(Beeper, HIGH);
  delay(10);
  digitalWrite(Beeper, LOW);
  delay(140);
  }
}

