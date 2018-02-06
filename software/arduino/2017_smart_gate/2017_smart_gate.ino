
#include <Adafruit_NeoPixel.h>
#include "fastADC.h"

#define BAT_ad_ch        2
#define RSSI_ad_ch       1
#define StatusLed       13
#define BT_Vcc          12
#define Taster01        11
#define Beeper          10
#define NeoPixelPin      9
#define VRX_CH3          8
#define VRX_CH2          7
#define VRX_CH1          6
#define NeoPixelAmount  64

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NeoPixelAmount, NeoPixelPin, NEO_GRB + NEO_KHZ800);

int   state = 1;

int   T1_state = 1;
int   T1_counter = 0;

int   blink_counter_init = 4;                           // Anzahl der Zustände der Blink-Sequenz
int   blink_counter = blink_counter_init;
int   switch_delay_init = 50;
int   switch_delay = switch_delay_init;

int   RSSI_raw = 0;
int   RSSI_Limit = 240;

float u_bat;
int   u_bat_raw;
int   low_bat[7] = {25, 0, 0, 750, 5.05, 100, 0};       // r, g, b, raw_limit, ref-level, counter_max, counter

unsigned long time;
unsigned long last_lap;
unsigned long this_lap;




void setup() 
{
  pinMode(StatusLed,   OUTPUT);
  pinMode(NeoPixelPin, OUTPUT);
  pinMode(VRX_CH1,     OUTPUT);
  pinMode(VRX_CH2,     OUTPUT);
  pinMode(VRX_CH3,     OUTPUT);
  pinMode(Beeper,      OUTPUT);
  pinMode(Taster01,    INPUT_PULLUP);
  pixels.begin();                             // Initialisierung der NeoPixel
  Serial.begin(9600);                         //  setup serial for debugging
//  pinMode(BT_Vcc,      OUTPUT);
//  digitalWrite(BT_Vcc, HIGH);
  set_channel(0, 35);
}




void loop()
{
//  check_inputs();
  check_battery();
  check_rssi();

  switch (state)
  {
    case 1:
            cls();
            show_circle(0,5,0, 8);
            
            if (RSSI_raw > RSSI_Limit)
              state = 2;
            
            break;
            
    case 2:
            if (blink_counter == 0)
              blink_counter = blink_counter_init;

            cls();
            
            switch (blink_counter)
            {
              case 4:
                      show_circle(5,5,5, 8);
                      switch_delay_counter();
                      break;
              case 3:
                      show_circle(5,5,5, 6);
                      switch_delay_counter();
                      break;
              case 2:
                      show_circle(5,5,5, 4);
                      switch_delay_counter();
                      break;
              case 1:
                      show_circle(5,5,5, 2);
                      switch_delay_counter();
                      break;
            }

            if ((blink_counter == 0) and (RSSI_raw < RSSI_Limit))
              { state = 1; last_lap = time; time = millis(); this_lap = time - last_lap; Serial.print("lap time: "); Serial.println(this_lap/1000.0); }
              
            break;
  }

  show_low_bat();
  pixels.show();
  delay(2);
}




void switch_delay_counter (void)
{
  if (switch_delay > 0)
    switch_delay --;
  else
  {
    blink_counter --;
    switch_delay = switch_delay_init;
  }
}




void check_rssi (void)
{
  RSSI_raw = analogRead(RSSI_ad_ch);
  Serial.println(RSSI_raw);
}




void check_inputs (void)
{
  T1_state = digitalRead(Taster01);    
}




void check_battery (void)
{
  int amount = 5;
  int ad_value[amount];
  for (int i=0; i<amount; i++)
    ad_value[i] = analogRead(BAT_ad_ch);
  u_bat_raw = 0;
  for (int i=0; i<amount; i++)
    u_bat_raw = u_bat_raw + ad_value[i];
  u_bat_raw = u_bat_raw / amount;
  /* Batterie-Spannung ausgeben
  Serial.println(u_bat_raw);                    // digital
  u_bat = (low_bat[4] * u_bat_raw / 1024.0);    // analog
  Serial.println(u_bat);
  // 740 -> 3,5V
  // 880 -> 4,2V
  */
}




void show_low_bat (void)
{
  if (u_bat_raw < low_bat[3])
  {
    low_bat[6]++;
    if (low_bat[6] > low_bat[5])
    {
      pixels.setPixelColor(0, pixels.Color(low_bat[0], low_bat[1], low_bat[2]));
      pixels.setPixelColor(7, pixels.Color(low_bat[0], low_bat[1], low_bat[2]));
      pixels.setPixelColor(63, pixels.Color(low_bat[0], low_bat[1], low_bat[2]));
      pixels.setPixelColor(56, pixels.Color(low_bat[0], low_bat[1], low_bat[2]));
      
      u_bat = (low_bat[4] * u_bat_raw / 1024.0);
      Serial.print("Low Battery: ");
      Serial.print(u_bat);
      Serial.println("V");
    }
  }
  else
    low_bat[6] = 0;
}




void show_circle (int r, int g, int b, int size_)
{
  switch ( size_ )
  {
    case 8:
            pixels.setPixelColor(1, pixels.Color(r, g, b));
            pixels.setPixelColor(2, pixels.Color(r, g, b));
            pixels.setPixelColor(3, pixels.Color(r, g, b));
            pixels.setPixelColor(4, pixels.Color(r, g, b));
            pixels.setPixelColor(5, pixels.Color(r, g, b));
            pixels.setPixelColor(6, pixels.Color(r, g, b));
            pixels.setPixelColor(8, pixels.Color(r, g, b));
            pixels.setPixelColor(15, pixels.Color(r, g, b));
            pixels.setPixelColor(16, pixels.Color(r, g, b));
            pixels.setPixelColor(23, pixels.Color(r, g, b));
            pixels.setPixelColor(24, pixels.Color(r, g, b));
            pixels.setPixelColor(31, pixels.Color(r, g, b));
            pixels.setPixelColor(32, pixels.Color(r, g, b));
            pixels.setPixelColor(39, pixels.Color(r, g, b));
            pixels.setPixelColor(40, pixels.Color(r, g, b));
            pixels.setPixelColor(47, pixels.Color(r, g, b));
            pixels.setPixelColor(48, pixels.Color(r, g, b));
            pixels.setPixelColor(55, pixels.Color(r, g, b));
            pixels.setPixelColor(57, pixels.Color(r, g, b));
            pixels.setPixelColor(58, pixels.Color(r, g, b));
            pixels.setPixelColor(59, pixels.Color(r, g, b));
            pixels.setPixelColor(60, pixels.Color(r, g, b));
            pixels.setPixelColor(61, pixels.Color(r, g, b));
            pixels.setPixelColor(62, pixels.Color(r, g, b));
            break;
    case 6:
            pixels.setPixelColor(10, pixels.Color(r, g, b));
            pixels.setPixelColor(11, pixels.Color(r, g, b));
            pixels.setPixelColor(12, pixels.Color(r, g, b));
            pixels.setPixelColor(13, pixels.Color(r, g, b));
            pixels.setPixelColor(17, pixels.Color(r, g, b));
            pixels.setPixelColor(22, pixels.Color(r, g, b));
            pixels.setPixelColor(25, pixels.Color(r, g, b));
            pixels.setPixelColor(30, pixels.Color(r, g, b));
            pixels.setPixelColor(33, pixels.Color(r, g, b));
            pixels.setPixelColor(38, pixels.Color(r, g, b));
            pixels.setPixelColor(41, pixels.Color(r, g, b));
            pixels.setPixelColor(46, pixels.Color(r, g, b));
            pixels.setPixelColor(50, pixels.Color(r, g, b));
            pixels.setPixelColor(51, pixels.Color(r, g, b));
            pixels.setPixelColor(52, pixels.Color(r, g, b));
            pixels.setPixelColor(53, pixels.Color(r, g, b));
            break;
    case 4:
            pixels.setPixelColor(19, pixels.Color(r, g, b));
            pixels.setPixelColor(20, pixels.Color(r, g, b));
            pixels.setPixelColor(26, pixels.Color(r, g, b));
            pixels.setPixelColor(29, pixels.Color(r, g, b));
            pixels.setPixelColor(34, pixels.Color(r, g, b));
            pixels.setPixelColor(37, pixels.Color(r, g, b));
            pixels.setPixelColor(43, pixels.Color(r, g, b));
            pixels.setPixelColor(44, pixels.Color(r, g, b));
            break;
    case 2:
            pixels.setPixelColor(27, pixels.Color(r, g, b));
            pixels.setPixelColor(28, pixels.Color(r, g, b));
            pixels.setPixelColor(35, pixels.Color(r, g, b));
            pixels.setPixelColor(36, pixels.Color(r, g, b));
            break;
  }
}




void cls (void)
{
  for (int i=0; i< NeoPixelAmount; i++)
    pixels.setPixelColor(i, pixels.Color(0,0,0));
}




void set_channel (byte channel, int wait_time)
{
  digitalWrite(VRX_CH1, (channel & (1<<0)));
  digitalWrite(VRX_CH2, (channel & (1<<1)));
  digitalWrite(VRX_CH3, (channel & (1<<2)));
  delay(wait_time);
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
