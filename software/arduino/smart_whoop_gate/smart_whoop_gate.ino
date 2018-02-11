
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Adafruit_NeoPixel.h>

#define NeoPixelVCC    2
#define NeoPixelPin    3
#define NeoPixelAmount 8
#define Pushbutton     9
#define ad_channel_bat 3

RF24 radio(7, 8);                                                 // CE, CSN of wifi module
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NeoPixelAmount, NeoPixelPin, NEO_GRB + NEO_KHZ800);

float u_bat;
int   u_bat_raw;
int   low_bat[7] = {25, 0, 0, 750, 5.05, 250, 0};                 // r, g, b, intensität, ref-level, treshhold, counter
const byte address[6] = "00001";                                  // wifi net name
int node_nr = 0;
int my_node_nr = 0;
int level = 0;
int pull_timer = 0;
const int pull_timer_max = 100;
int radio_input = 0;




void setup() 
{
  radio.begin();                                                  // wifi module init
  radio.setPALevel(RF24_PA_MAX);
  radio.openReadingPipe(0, address);
  radio.startListening();
  
  pinMode(Pushbutton, INPUT_PULLUP);                              // hw init
  pinMode(NeoPixelVCC, OUTPUT);
  digitalWrite(NeoPixelVCC, HIGH);
  pinMode(NeoPixelPin, OUTPUT);  
  pixels.begin();                                                 // init of NeoPixel
  
//Serial.begin(9600);                                             // setup serial for debugging
}

// 127 -> start pairing
// 126 -> stop pairing
// 125 -> abort pairing


void soft_reset (void)
{
  node_nr = 0;
  my_node_nr = 0;
}

void loop()
{

  switch(level)
  {
    case 0:                                                       // Start Menu
              soft_reset();
              
              if (pull_timer == 0)
              { for (int i=1; i<NeoPixelAmount; i++)              // background for submenu is 
                { pixels.setPixelColor(i, 5, 5, 5);  }  }         // white
              else                                                //
              { show_bar(pull_timer);  }                          // debounce visualisation

              if (radio.available())                              //
              { radio.read(&radio_input, sizeof(radio_input));    //
                if (radio_input == 127)                           // other node is master and forced config-mode
                { level++;  }  }                                  //
              
              if (pull_timer == pull_timer_max)                   //
              { while(digitalRead(Pushbutton) == 0);              // user pressed button to enter config-mode
                level++;  pull_timer = 0;                         //
                send_radio(127);  }                               // force other nodes to enter config-mode
              
              break;
    case 1:                                                       // Node-Config Menu
              for (int i=1; i<NeoPixelAmount; i++)                // background for submenu is
              { pixels.setPixelColor(i, 0, 0, 5);  }              // blue

              if (radio.available())                              //
              { radio.read(&radio_input, sizeof(radio_input));    //
                if (radio_input == 125)                           //
                { level = 0;  }                                   // master node forced abort and return to main menu
                else                                              //
                { node_nr = radio_input +1;  }                    // other node enqued, increment possible own node number
              }
              
              if (digitalRead(Pushbutton) == 0)                   //
              { int i=0;
                while(digitalRead(Pushbutton) == 0)               // debounce
                {  i++; delay(100);  }
                if (i < 10)                                       // short press
                {                                                 //
                  send_radio(node_nr);                            // send my own node number
                  my_node_nr = node_nr;                           //
                  delay(100);                                     //
                  level++;                                        //
                }
                else                                              // long press
                {                                                 //
                  level = 0;                                      // go to main menu
                  send_radio(125);                                // send abort signal to all other nodes
                }                                                 //
              }
              
              break;
    case 2:                                                       // Node number visualisation
              for (int i=1; i<NeoPixelAmount; i++)                // background for submenu is
              { pixels.setPixelColor(i, 0, 0, 0);  }              // black
              pixels.setPixelColor(my_node_nr+1, 0, 5, 0);          // show own node number in green

              if ((my_node_nr == 0) and (digitalRead(Pushbutton) == 0))   //User wants to exit config mode
              { send_radio(126);                                  // send command for exit of config-mode to other nodes
                while(digitalRead(Pushbutton) == 0);              // debounce
                level++;  }                                       //
              
              if (radio.available())                              //
              { radio.read(&radio_input, sizeof(radio_input));    //
                if (radio_input == 126)                           // master-node (1st) forces to exit config-mode and go to race-mode
                { level++;  }                                     //
                else if (radio_input == 125)                      // master-node forces to abort and go to main menu
                { level = 0;  }                                   //
              }
              
              break;
    case 3:                                                       // show gates in prestart condition (with gate-number in white)
              for (int i=1; i<NeoPixelAmount; i++)                //
              { if (my_node_nr > 0)                               // all gates are
                {  pixels.setPixelColor(i, 5, 0, 0);  }           // red
                else                                              //
                {  pixels.setPixelColor(i, 0, 5, 0);  }  }        // first gate is green
              pixels.setPixelColor(node_nr+1, 5, 5, 5);             // gate number is white
              level++;                                            //
                                        
              break;
    case 4:                                                       // rennen starten - Zeitmessung mit Überflug starten
                          
              break;
  }

  check_battery();
  show_low_bat();
  pixels.show();
  delay(10);  
  
  if (digitalRead(Pushbutton) == 0)
  { if (pull_timer < pull_timer_max)
      pull_timer++;  }
  else
  { pull_timer = 0;  }


  // Peeper ?
  // Akku Überwachung implementieren
  // On Off über Schalter (Unterbrechung vor StepUp)
  // Abbruch / Master-Reset
}




void show_bar (int amount)
{
  amount = map(amount, 0, pull_timer_max, 0, NeoPixelAmount-1);
  for (int i=0; i<=amount; i++)
    pixels.setPixelColor(i+1, 5, 0, 0);
}



void send_radio (int buffer_)
{
  radio.stopListening();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MAX);
  radio.write(&buffer_, sizeof(buffer_));  
  radio.openReadingPipe(0, address);
  radio.startListening();
}




void check_battery()
{
  int amount = 5;
  int ad_value[amount];
  for (int i=0; i<amount; i++)
    ad_value[i] = analogRead(ad_channel_bat);
  u_bat_raw = 0;
  for (int i=0; i<amount; i++)
    u_bat_raw = u_bat_raw + ad_value[i];
  u_bat_raw = u_bat_raw / amount;
  // Serial.println(u_bat_raw);
  // 740 -> 3,5V
  // 880 -> 4,2V
}




void show_low_bat()
{
  if (u_bat_raw < low_bat[3])
  {
    low_bat[6]++;
    if (low_bat[6] > low_bat[5])
    {      
     pixels.setPixelColor(0, pixels.Color(low_bat[0], low_bat[1], low_bat[2]));
     u_bat = (low_bat[4] * u_bat_raw / 1024.0);
     Serial.println(u_bat);
    }
  }
  else
    low_bat[6] = 0;
}

