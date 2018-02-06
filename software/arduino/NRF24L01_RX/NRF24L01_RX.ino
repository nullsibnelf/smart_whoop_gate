/*
* Arduino Wireless Communication Tutorial
*       Example 1 - Receiver Code
*                
* by Dejan Nedelkovski, www.HowToMechatronics.com
* 
* Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Adafruit_NeoPixel.h>

#define NeoPixelPin    3
#define NeoPixelVCC    2
#define NeoPixelAmount 8

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NeoPixelAmount, NeoPixelPin, NEO_GRB + NEO_KHZ800);
RF24 radio(7, 8);                     // CE, CSN

const byte address[6] = "00001";




void setup() {  
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_HIGH);
  radio.startListening();
  
  pinMode(NeoPixelPin, OUTPUT);
  pinMode(NeoPixelVCC, OUTPUT);
  digitalWrite(NeoPixelVCC, HIGH);
  delay(100);
  pixels.begin();                             // Initialisierung der NeoPixel
  pixels.setPixelColor(1, 5, 5, 5);
  pixels.show();

}




void loop()
{
  if (radio.available())
  {
    int text = 4;
    radio.read(&text, sizeof(text));

    if (text == 48)
    {      
      Serial.println("null");
      pixels.setPixelColor(1, 0, 5, 0);
    }
    else if (text == 49)
    {
      Serial.println("eins");
      pixels.setPixelColor(1, 5, 0, 0);
    }
    else
    {
      Serial.println(text);
    }
    
    pixels.show();
  }
}
