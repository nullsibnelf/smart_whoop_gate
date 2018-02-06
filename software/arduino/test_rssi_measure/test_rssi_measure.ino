
#define StatusLed       13
#define Taster01        11
#define RSSI_ad_ch       1
#define CH1              2
#define CH2              3
#define CH3              4
#define Beeper          10

int   T1_state = 1;
int   T1_counter = 0;

int RSSI_raw;



void setup() 
{
  pinMode(StatusLed,   OUTPUT);
  pinMode(CH1,         OUTPUT);
  pinMode(CH2,         OUTPUT);
  pinMode(CH3,         OUTPUT);
  pinMode(Beeper,      OUTPUT);
  pinMode(Taster01,    INPUT_PULLUP);
  Serial.begin(9600);                         //  setup serial for debugging
}




void loop()
{
  for (int i=0; i<8; i++)
  {
    set_channel(i);
    delay(2);
    RSSI_raw = analogRead(RSSI_ad_ch);
    //Serial.println(i);
    Serial.println(RSSI_raw);

    if (RSSI_raw > 300)
      chirp(1);
  
    delay(10);
  }  
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


void set_channel (byte channel)
{
  digitalWrite(CH1, (channel & (1<<0)));
  digitalWrite(CH2, (channel & (1<<1)));
  digitalWrite(CH3, (channel & (1<<2)));
}



