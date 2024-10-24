/*
* Name: clock and temp project
* Author: Alexander Bohre
* Date: 2024-10-10
* Description: This project uses a ds3231 to measure time and displays the time to an 1306 oled display,
* it also measures temprature with a analog temprature module and displays a mapped value to a 9g-servo-motor
* and lastly it uses the ds3231 to display a minute with a Adafruit_neoPixel.
*/

// Include Libraries
#include <RTClib.h>
#include <Wire.h>
#include "U8glib.h"
#include <Servo.h>

#include <Adafruit_NeoPixel.h>
 
#define PIN 6


// Init constants

// Init global variables

const int tempPin = A1;
float temp = 0;
char t[32];

// construct objects
Adafruit_NeoPixel strip = Adafruit_NeoPixel(24, PIN, NEO_GRB + NEO_KHZ800);
RTC_DS3231 rtc;
Servo myservo;

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);	// Display which does not send AC


void setup() {
  // init communication
  strip.begin();
  strip.setBrightness(90); 
  Serial.begin(9600);

  // Init Hardware
  myservo.attach(9);
  pinMode(tempPin, INPUT);
  Wire.begin();
  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__),F(__TIME__)));

  u8g.setFont(u8g_font_unifont);
  pinMode(9, INPUT);
  pinMode(8, INPUT);
}

void loop() {
 
  oledWrite("Time: " + getTime());
  servoWrite(getTemp());
  colorLight(strip.Color(255, 0, 0), strip.Color(0, 0, 255));
  delay(1000);
}



/*
*This function reads time from an ds3231 module and package the time as a String
*Parameters: Void
*Returns: time in hh:mm:ss as String
*/
String getTime(){
  DateTime now = rtc.now();
  return((String(now.hour()) +":"+ String(now.minute()) +":"+ String(now.second())));
  delay(1000);
}

/*
*This function reads seconds from an ds3231 module and package the time as a int
*Parameters: Void
*Returns: second as int
*/
int getSecond(){
  DateTime now = rtc.now();
  return(now.second());
}
/*
* This function reads an analog pin connected to an analog temprature sensor and calculates the corresponding temp
*Parameters: Void
*Returns: temprature as float
*/
float getTemp(){
  // put your main code here, to run repeatedly:
  float R1 = 10000;  // value of R1 on board
  float logR2, R2, T;
  float c1 = 0.001129148, c2 = 0.000234125, c3 = 0.0000000876741;  //steinhart-hart coeficients for thermistor
  int Vo = analogRead(tempPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);  //calculate resistance on thermistor
  logR2 = log(R2);
  T = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));  // temperature in Kelvin
  T = T - 273.15;                                              //convert Kelvin to Celcius
  return T;
}

/*
* This function takes a string and draws it to an oled display
*Parameters: - text: String to write to display
*Returns: void
*/
void oledWrite(String text) {
  // picture loop
  u8g.firstPage();
  do {
    u8g.drawStr(12,64, text.c_str());
  } while( u8g.nextPage());

}

/*
* takes a temprature value and maps it to corresponding degree on a servo
*Parameters: - value: temprature
*Returns: void
*/
void servoWrite(float value){
   float ang = map(value, 20, 30, 0, 180);
   myservo.write(ang);
}

/*
* This function colors the LED strip based on the current second, mapping seconds to LEDs. 
*Parameters: 
*c - uint32_t for the color red 0-255 (for the current LED)
*b - uint32_t for the color blue 0-255 (for the previous LED)
*Returns: void
*/
void colorLight(uint32_t c,uint32_t b) {
  for (int i = 0; i <= map(getSecond(), 0, 59, 0, 23); i += 1){
    strip.setPixelColor(i, c);
    if(i == 0){
      strip.setPixelColor(23,b);
    }
    else{
      strip.setPixelColor((i-1),b);
    }
  }
 
  strip.show();
  }