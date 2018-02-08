
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

int node_nr = 1;          // <------------------------------- Muss später aus dem Eprom geholt und gespeichert werden

int level = 0;
int pull_timer = 0;
const int pull_timer_max = 100;




void setup() 
{
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_HIGH);
  radio.stopListening();
  
  pinMode(Taster, INPUT_PULLUP);
  
  pinMode(NeoPixelVCC, OUTPUT);
  digitalWrite(NeoPixelVCC, HIGH);
  pinMode(NeoPixelPin, OUTPUT);  
  pixels.begin();                             // Initialisierung der NeoPixel
  pixels.setPixelColor(0, 5, 5, 5);
  pixels.show();

}




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
              
              if (pull_timer == pull_timer_max)
              { while(digitalRead(Taster) == 0);
                level++;  pull_timer = 0;  }
                
              break;            
    case 1:                                               // Node-Config Menu
              for (int i=0; i<NeoPixelAmount; i++)        // background for submenu in blue
              { pixels.setPixelColor(i, 0, 0, 5);  }

              pixels.setPixelColor(node_nr, 5, 0, 0);     // Node Number in red
                
              break;
  }

  pixels.show();
  delay(10);  
}




void show_bar (int amount)
{
  amount = map(amount, 0, pull_timer_max, 0, NeoPixelAmount);
  for (int i=0; i<=amount; i++)
    pixels.setPixelColor(i, 5, 0, 0);
}

