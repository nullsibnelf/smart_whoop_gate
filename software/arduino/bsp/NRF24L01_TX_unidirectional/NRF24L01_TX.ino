/*
* Arduino Wireless Communication Tutorial
*     Example 1 - Transmitter Code
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
RF24 radio(7, 8);                   // CE, CSN

const byte address[6] = "00001";




void setup() {
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_HIGH);
  radio.stopListening();
  
  pinMode(NeoPixelPin, OUTPUT);
  pinMode(NeoPixelVCC, OUTPUT);
  digitalWrite(NeoPixelVCC, HIGH);
  delay(100);
  pixels.begin();                             // Initialisierung der NeoPixel
  pixels.setPixelColor(1, 5, 5, 5);
  pixels.show();
}




void loop()
{/*
  const char text[] = "Hello World";
  radio.write(&text, sizeof(text));
  delay(200);
  */
  const char text2[] = "1";
  radio.write(&text2, sizeof(text2));
  delay(200);
  
  const char text3[] = "0";
  radio.write(&text3, sizeof(text3));
  delay(200);
}
