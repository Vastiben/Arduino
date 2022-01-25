#include <IRremote.h>
#include <FastLED.h>
#include <Arduino.h>

#define RECV_PIN  8
IRrecv irrecv(RECV_PIN);
decode_results results;

#define ANZAHL_LEDS 10//180
#define LED_PIN 4
CRGB leds[ANZAHL_LEDS];
CRGB Violet = CRGB(255,0,200);
CRGB Indigo= CRGB(102,0,255);
CRGB Blue = CRGB(0,0,255);
CRGB Green = CRGB(0,255,0);
CRGB Yellow = CRGB(255,255,0);
CRGB Orange = CRGB(255,50,0);
CRGB Red = CRGB(255,0,0);
CRGB Colors[7] = {Violet, Indigo, Blue, Green, Yellow, Orange, Red};
CRGB new_Colors[7];
bool on;
String mode;
String push;
int delay_LED = 50;

void setup(){
  Serial.begin(9600);
  irrecv.enableIRIn();
  irrecv.blink13(true);

  //Setub Leds
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, ANZAHL_LEDS);
  FastLED.setBrightness(10);
  Serial.println("Ready");
}

void loop(){
  if (irrecv.decode(&results)){
    push = "";
    mode = "";
    Serial.println(results.value, HEX);
    Serial.println(results.decode_type);
    if (results.value == 0XFF6897){ // Button 0
      push = "0";
      mode = "off";
    } else if (results.value == 0xFF30CF){ //Button 1
      push = "1";
      mode = "1";
    } else if (results.value == 0xFF18E7){ //Button 2
      push = "2";
      mode = "2";
    } else if (results.value == 0xFF7A85){ //Button 3
      push = "3";
      mode = "3";
    } else if (results.value == 0xFF02FD){ //Button >>
      push = "faster";
      delay_LED -= 10;
    } else if (results.value == 0xFF22DD){ //Button <<
      push = "slower";
      delay_LED += 10;
    } else if (results.value == 0xFF906F){ //Button EQ
      push = "reset_delay"; 
      delay_LED = 50;
    } 
    delay(100);
    irrecv.resume(); 
    Serial.print("Push "); 
    Serial.println(push);
  }
  
  if (mode == "off"){
      off();
    } else if (mode == "1"){
      rainbow();
    } else if (mode == "2"){
      blue();
    } 
}

void off() {
  FastLED.clear(true);  // clear all pixel data
  FastLED.show();
}

void rainbow() {
  noInterrupts();
  for (int bob = 0; bob < 7; bob++) {
    for (int k = 0; k < 7; k++) {
      if (k+bob < 7) {
        new_Colors[k]= Colors[k+bob];
      } else {
        new_Colors[k]= Colors[k+bob-7];
      }
    }
    for (int i = 0; i < int(ANZAHL_LEDS/4) ; i++) {
      for (int j = 0; j< 4 ; j++ ) {
        leds[i*4+j] = new_Colors[int(i-7*round(int (i/7)))]; 
      }
    }
    FastLED.show();
    delay(delay_LED);    
  }
  FastLED.setBrightness(50);
  interrupts();
}

void blue() {
  for (int i = 0; i < int(ANZAHL_LEDS) ; i++) {
      leds[i] = CRGB(0,0,255);   
  }
  FastLED.show();
  FastLED.setBrightness(50);
}
