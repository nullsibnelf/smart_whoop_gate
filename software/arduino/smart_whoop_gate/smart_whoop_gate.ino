
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Adafruit_NeoPixel.h>

#define NeoPixelVCC    2
#define NeoPixelPin    3
#define NeoPixelAmount 8
#define Taster         9

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NeoPixelAmount, NeoPixelPin, NEO_GRB + NEO_KHZ800);
RF24 radio(7, 8);                   // CE, CSN

const byte address[6] = "00001";

int node_nr = 0;
int my_node_nr = 0;
int level = 0;
int pull_timer = 0;
const int pull_timer_max = 100;
int radio_input = 0;




void setup() 
{
  radio.begin();
  radio.setPALevel(RF24_PA_MIN);
  radio.openReadingPipe(0, address);
  radio.startListening();
  
  pinMode(Taster, INPUT_PULLUP);
  
  pinMode(NeoPixelVCC, OUTPUT);
  digitalWrite(NeoPixelVCC, HIGH);
  pinMode(NeoPixelPin, OUTPUT);  
  pixels.begin();                             // Initialisierung der NeoPixel
  pixels.setPixelColor(0, 5, 5, 5);
  pixels.show();
//Serial.begin(9600);                         // setup serial for debugging
}

// 127 -> pairing starten
// 126 -> pairing beenden




void loop()
{

  switch(level)
  {
    case 0:                                               // Start
              if (digitalRead(Taster) == 0)
              { if (pull_timer < pull_timer_max)
                  pull_timer++;  }
              else
              { pull_timer = 0;  }

              if (pull_timer > 0)
              { show_bar(pull_timer);  }
              else
              { for (int i=0; i<NeoPixelAmount; i++)
                { pixels.setPixelColor(i, 5, 5, 5);  }  }

              if (radio.available())
              { radio.read(&radio_input, sizeof(radio_input));
                if (radio_input == 127)
                { level++;  }  }
              
              if (pull_timer == pull_timer_max)
              { while(digitalRead(Taster) == 0);
                level++;  pull_timer = 0;  send_radio(127);  }
              
              break;
    case 1:                                               // Node-Config Menu
              for (int i=0; i<NeoPixelAmount; i++)        // background for submenu in blue
              { pixels.setPixelColor(i, 0, 0, 5);  }

              if (radio.available())
              { radio.read(&radio_input, sizeof(radio_input));
                node_nr = radio_input +1;  }
              
              if (digitalRead(Taster) == 0)
              { send_radio(node_nr);
                my_node_nr = node_nr;
                while(digitalRead(Taster) == 0);
                delay(100);
                level++;  }
              
              break;
    case 2:                                                 // NodeNummer anzeigen und auf finish warten bzw finish senden
              for (int i=0; i<NeoPixelAmount; i++)
              { pixels.setPixelColor(i, 0, 0, 0);  }              
              pixels.setPixelColor(my_node_nr, 0, 5, 0);

              if ((my_node_nr == 0) and (digitalRead(Taster) == 0))
              { send_radio(126);
                while(digitalRead(Taster) == 0);
                level++;  }
              
              if (radio.available())
              { radio.read(&radio_input, sizeof(radio_input));
                if (radio_input == 126)
                { level++;  }  }   
              
              break;
    case 3:                                                 // Gates einfärben
              for (int i=0; i<NeoPixelAmount; i++)
              { if (my_node_nr > 0)
                {  pixels.setPixelColor(i, 5, 0, 0);  }
                else
                {  pixels.setPixelColor(i, 0, 5, 0);  }  } 
              pixels.setPixelColor(node_nr, 5, 5, 5);
              level++;
                                        
              break;
    case 4:                                                 // rennen starten - Zeitmessung mit Überflug starten
                          
              break;
  }

  pixels.show();
  delay(10);  

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

