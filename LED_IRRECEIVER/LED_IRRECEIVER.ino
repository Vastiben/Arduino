/************************************************** 
 *        Part for the upload trhough wifi        *
 **************************************************/
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#ifndef STASSID
#define STASSID "kln-71032"
#define STAPSK  "melanieetbastien"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;
int start_time = millis();
bool bool_OTA = false;

/************************************************** 
*        Real code for the led stripe        *
**************************************************/

#include <IRremote.h>
#include <FastLED.h>
#include <Arduino.h>

#define RECV_PIN  5 // D1
IRrecv irrecv(RECV_PIN);
decode_results results;

#define ANZAHL_LEDS 100//180
#define LED_PIN 4 // D2
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
int delay_LED = 200; //Delay in milliseconds
int brightness_LED = 10;
int start_count = millis();
int count_rainbow = 0;

void setup(){
/*****************************************************
 * Part in the setup for the upload through the wifi *
 *****************************************************/
  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  ArduinoOTA.setPassword("admin");
  
   ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

/***********************************************
 *    Part in the setup for the led stripe     *
 ***********************************************/

  irrecv.enableIRIn();
  //irrecv.blink13(true);

  //Setup Leds
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, ANZAHL_LEDS);
  FastLED.setBrightness(brightness_LED);

  orange();
  delay(2000);
  off();
}

void loop(){
/***************************************************************
 *         Part in the void loop for the led stripe            *            
 ***************************************************************/
 
  if (irrecv.decode(&results)){
    Serial.println("HOME");
    push = "";
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
    } else if (results.value == 0xFF10EF){ //Button 4
      push = "4";
      mode = "4";
    } else if (results.value == 0xFF38C7){ //Button 5
      push = "5";
      mode = "5";
    } else if (results.value == 0xFF5AA5){ //Button 6
      push = "6";
      mode = "6";
    } else if (results.value == 0xFF42BD){ //Button 7
      push = "7";
      mode = "7";
    } else if (results.value == 0xFF4AB5){ //Button 8
      push = "8";
      mode = "8";
    } else if (results.value == 0xFF52AD){ //Button 9
      push = "9";
      mode = "9";
    } else if (results.value == 0xFF02FD){ //Button >>
      push = "faster";
      delay_LED -= 50;
    } else if (results.value == 0xFF22DD){ //Button <<
      push = "slower";
      delay_LED += 50;
    } else if (results.value == 0xFFC23D){ //Button play/pause 
      push = "reset_delay"; 
      delay_LED   = 200;
    } else if (results.value == 0xFFA857){ //Button +
      push = "brighter"; 
      brightness_LED += 10;
      FastLED.setBrightness(brightness_LED);
      led_stripe_show();
    } else if (results.value == 0xFFE01F){ //Button -
      push = "darker"; 
      brightness_LED -= 10;
      FastLED.setBrightness(brightness_LED);
      led_stripe_show();
    } else if (results.value == 0xFF906F){ //Button EQ
      push = "reset_brightness"; 
      brightness_LED = 10;
      FastLED.setBrightness(brightness_LED);
      led_stripe_show();
      
      // Permet d'activer la réception d'un nouveau programme en appuyant sur le bouton CH-
    } else if (results.value == 0xFFA25D){ //Button CH-
      push = "CH-";
      mode = "upload"; 
      start_time = millis();
    }
    
    delay(100);
    irrecv.resume(); 
    Serial.print("Push "); 
    Serial.println(push);
  } else  if (mode == "off"){
    off();
    mode="";
  } else if (mode == "1"){
    rainbow();
  } else if (mode == "2"){
    blue();
    mode="";
  } else if (mode == "3"){
    red();
    mode="";
  } else if (mode == "4"){
    violet();
    mode="";
  } else if (mode == "5"){
    orange();
    mode="";
  } else if (mode == "upload"){
    while ((millis()-start_time) < 15000){
      ArduinoOTA.handle();
      delay(10);
      wait_upload();
    }
    off();   
  }       
}
  
void off() {
  FastLED.clear(true);  // clear all pixel data
  led_stripe_show();
}

void rainbow() {
  if (millis()> (start_count + delay_LED)) {
    count_rainbow = count_rainbow +1;
    if (count_rainbow == 7){
      count_rainbow = 0;
    }
    for (int k = 0; k < 7; k++) {
      if (k+count_rainbow < 7) {
        new_Colors[k]= Colors[k+count_rainbow];
      } else {
        new_Colors[k]= Colors[k+count_rainbow-7];
      }
    }
    for (int i = 0; i < int(ANZAHL_LEDS/4) ; i++) {
      for (int j = 0; j< 4 ; j++ ) {
        leds[i*4+j] = new_Colors[int(i-7*round(int (i/7)))]; 
      }
    }
    led_stripe_show();
    start_count = millis();
  }
  Serial.println("Rainbow");
}

void blue() {
  for (int i = 0; i < int(ANZAHL_LEDS) ; i++) {
      leds[i] = CRGB(0,0,255);   
  }
  led_stripe_show();
  Serial.println("Blue");
}

void red() {
  for (int i = 0; i < int(ANZAHL_LEDS) ; i++) {
      leds[i] = CRGB(255,0, 0);   
  }
  led_stripe_show();
  Serial.println("Red");
}

void violet() {
  for (int i = 0; i < int(ANZAHL_LEDS) ; i++) {
      leds[i] = Violet;   
  }
  led_stripe_show();
  Serial.println("Violet");
}

void orange() {
  for (int i = 0; i < int(ANZAHL_LEDS) ; i++) {
      leds[i] = Orange;   
  }
  led_stripe_show();
  Serial.println("Orange");
}

void wait_upload(){
    for (int i = 0; i < int(ANZAHL_LEDS) ; i++) {
      leds[i] = CRGB(255,255,255);
      leds[i+1] = CRGB(0,0,0);
      i+=1;   
  }
  led_stripe_show();
  Serial.println("Wait for upload");
}

void led_stripe_show(){
  for (int i = 0; i< 4; i++){
    FastLED.show();
  }
}
