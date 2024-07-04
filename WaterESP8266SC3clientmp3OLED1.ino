
/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp8266-client-server-wi-fi/  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/



/////////// Library MP3 Module ///////////////
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
 //  for smooth sound use resistor 1k to pin TX and RX
SoftwareSerial mySoftwareSerial(13,15); // (uno 10:RX, 11:TX ) 
//(ESP8266 RX=15=D8 ,TX=13=D7 )RX = 3 , TX = 2
DFRobotDFPlayerMini myDFPlayer; // MP3 Module can use Vcc = 3.3V

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti WiFiMulti;
const char* ssid = "ESP8266-Access-Point";
const char* password = "123456789";
//Your IP address or domain name with URL path
const char* serverNameTemp = "http://192.168.4.1/waterc1"; // change variable of server program
 //const char* serverNameHumi = "http://192.168.4.1/angleyr"; // change variable of server program
const char* serverNamePres = "http://192.168.4.1/mq2c1"; // change variable of server program


/////////////// OLED //////////////////
#include <Wire.h>
// library OLEDtest2 
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

/* Uncomment the initialize the I2C address , uncomment only one, If you get a totally blank screen try the other*/
#define i2c_Address 0x3c //initialize with the I2C addr 0x3C Typically eBay OLED's
//#define i2c_Address 0x3d //initialize with the I2C addr 0x3D Typically Adafruit OLED's

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1   //   QT-PY / XIAO
 ///////////// show logo adafruit  ///////////////  
   Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16

// End library OLEDtest2  
static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000
};
 char test1[10] = {'0','1','2','3','4','5','6','7','8','9'} ;

//////////////  Variable for detect water Level ///////////
String temperature , waterc ;
String humidity , angleyc ;
String pressure , gasc , smokec ;
unsigned long previousMillis = 0;
const long interval = 2000; // 5000
int waterc2, smokec2, gasc2 ;

void setup() {
Serial.begin(115200);  
  display.begin(i2c_Address, true); // Address 0x3C default
 //display.setContrast (0); // dim display 
  display.display();
  delay(2000);
  // Clear the buffer.
  display.clearDisplay();

  // draw a single pixel
 // display.drawPixel(10, 10, SH110X_WHITE);
  // Show the display buffer on the hardware.
  // NOTE: You _must_ call display after making any drawing commands
  // to make them visible on the display hardware!
 // display.display();
 // delay(2000);
 // display.clearDisplay();

 // End setting OLED 
  
  ///////////////    test  display menu  ///////////
 // text display tests
 /// 1.settextsize , 2.setcolour , 3. setcursor , 
 //  4. println or print , 5. display.display() ,6.delay or clear  
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0,0);
  display.println("Detect");
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.println("Water");
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.println("Gas, Smoke");
  display.display();
  delay(1000);
  display.clearDisplay();
 
///////// SETUP for MP3 Module  //////////////
mySoftwareSerial.begin(9600);  
  pinMode(D4,OUTPUT) ;
Serial.println();
Serial.println(F("DFRobot DFPlayer Mini Demo"));
Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

if (!myDFPlayer.begin(mySoftwareSerial)) { //Use softwareSerial to communicate with mp3.
Serial.println(F("Unable to begin:"));
Serial.println(F("1.Please recheck the connection!"));
Serial.println(F("2.Please insert the SD card!"));
  // while (true) {
//  delay(0); // Code to compatible with ESP8266 watch dog.
//  }
}
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Connected to WiFi");
Serial.println(F("DFPlayer Mini online.")); 
delay(1000) ;
myDFPlayer.volume(25); //ตั้งระดับความดังของเสียง 0-30
delay(100) ;
  Serial.print(" Open Music in Folder 1 number 23");
   Serial.print("\n");
      myDFPlayer.playFolder(1, 23); 
      delay(2300) ;

///////////// End SETUP LOOP  //////////////////
}

void loop() {
  unsigned long currentMillis = millis();  
  if(currentMillis - previousMillis >= interval) {
     // Check WiFi connection status
    if ((WiFiMulti.run() == WL_CONNECTED)) {
      waterc = httpGETRequest(serverNameTemp); // detect water 
   //   angleyc = httpGETRequest(serverNameHumi);
      gasc = httpGETRequest(serverNamePres);
      gasc2 = gasc.toInt() ;
      waterc2 = waterc.toInt() ;
      smokec2 = gasc2 ;
      Serial.println("WaterC: " + waterc + " - GASC: " + gasc + " - SMOKEC: "+ smokec2 );      
      digitalWrite(D4,HIGH) ; 
      delay(100) ;
      digitalWrite(D4,LOW) ;
      delay(100) ;
   display.clearDisplay();
      // display OLED 
   display.setTextSize(1.2);
      display.setCursor(0,10);
      display.print("Water:");
      if ( waterc2 == 0) {
      display.setTextSize(1);  
      display.setCursor(50, 10); // (front,back) = (Width,Height)
      display.print("Not Full");      
      }
     // display.setTextSize(1);
     // display.cp437(true);
     // display.write(248);
  if ( waterc2 == 1) {
      display.setCursor(50, 10); // (front,back) = (Width,Height)
      display.print("Full");   
     }
      display.setTextSize(1.2);
      display.setCursor(0, 25);
      display.print("Gas: ");
      if ( gasc2 <= 134 ) {
      display.setTextSize(1);
      display.setCursor(50, 30); // (front,back) = (Width,Height)
      display.print("NON");
      display.setCursor(80, 30); // (front,back) = (Width,Height)
      display.print(gasc);
      }
     if ( gasc2 > 134 ) {
      display.setTextSize(1);
      display.setCursor(50, 30); // (front,back) = (Width,Height)
      display.print("Detected");
      display.setCursor(100, 30); // (front,back) = (Width,Height)
      display.print(gasc);
      }
   //   display.setCursor(100, 25);
   //   display.print("ms"); 
      display.setTextSize(1.2);
      display.setCursor(0, 50);
      display.print("Smoke: ");  
 if ( smokec2 < 90 ) { 
      display.setTextSize(1);
      display.setCursor(50, 50);
      display.print("NON");  
      display.setCursor(70, 50);
      display.print(smokec) ;
 }
 if ( smokec2 > 150 ) { 
      display.setTextSize(1);
      display.setCursor(50, 50);
      display.print("NON");  
      display.setCursor(70, 50);
      display.print(smokec) ;
 }
// 90-114
 if ( smokec2 > 90 && smokec2 < 115 ) { 
      display.setTextSize(1);
      display.setCursor(50, 50);
      display.print("Detected");  
      display.setCursor(80, 50);
      display.print(smokec) ;
 }
      display.display();   
/// DFPlayermini MP3  
  if ( waterc2 == 1) {
  Serial.print(" Open Music in Folder 1 number 20");
   Serial.print("\n");
      myDFPlayer.playFolder(1, 20); 
      delay(2300) ;      
     }
     if ( gasc2 > 134 ) {
  Serial.print(" Open Music in Folder 1 number 22");
  Serial.print("\n");
      myDFPlayer.playFolder(1, 22); 
      delay(2300) ;
      }
 if ( smokec2 > 90 && smokec2 < 115 ) { 
  Serial.print(" Open Music in Folder 1 number 21");
  Serial.print("\n");
      myDFPlayer.playFolder(1, 21); 
      delay(2000) ;
 }

      // save the last HTTP GET Request
      previousMillis = currentMillis;
    }
    else {
      Serial.println("WiFi Disconnected");
    }
  }


}  //////////////// End Void Loop ///////////////////


String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;    
  // Your IP address with path or Domain name with URL path 
  http.begin(client, serverName);  
  // Send HTTP POST request
  int httpResponseCode = http.GET();  
  String payload = "--";   
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();
  return payload;
}




