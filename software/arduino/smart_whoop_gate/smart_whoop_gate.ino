
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Adafruit_NeoPixel.h>

#define NeoPixelVCC    2
#define NeoPixelPin    3
#define NeoPixelAmount 8
#define Pushbutton     9

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NeoPixelAmount, NeoPixelPin, NEO_GRB + NEO_KHZ800);
RF24 radio(7, 8);                                                 // CE, CSN of wifi module

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
  pixels.setPixelColor(0, 5, 5, 5);
  pixels.show();
  
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
              { for (int i=0; i<NeoPixelAmount; i++)              // background for submenu is 
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
              for (int i=0; i<NeoPixelAmount; i++)                // background for submenu is
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
              for (int i=0; i<NeoPixelAmount; i++)                // background for submenu is
              { pixels.setPixelColor(i, 0, 0, 0);  }              // black
              pixels.setPixelColor(my_node_nr, 0, 5, 0);          // show own node number in green

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
              for (int i=0; i<NeoPixelAmount; i++)                //
              { if (my_node_nr > 0)                               // all gates are
                {  pixels.setPixelColor(i, 5, 0, 0);  }           // red
                else                                              //
                {  pixels.setPixelColor(i, 0, 5, 0);  }  }        // first gate is green
              pixels.setPixelColor(node_nr, 5, 5, 5);             // gate number is white
              level++;                                            //
                                        
              break;
    case 4:                                                       // rennen starten - Zeitmessung mit Überflug starten
                          
              break;
  }

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
  amount = map(amount, 0, pull_timer_max, 0, NeoPixelAmount);
  for (int i=0; i<=amount; i++)
    pixels.setPixelColor(i, 5, 0, 0);
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

