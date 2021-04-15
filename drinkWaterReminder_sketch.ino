#include <HX711_ADC.h> // https://github.com/olkal/HX711_ADC
#include <Wire.h>
#include <LiquidCrystal_I2C.h> // LiquidCrystal_I2C library
#include "RTClib.h"

HX711_ADC scale(4, 5); // pins are in ports 4 and 5
LiquidCrystal_I2C lcd(0x27,16,2); // 0x27 is the address, 16 characters, 2 lines
RTC_Millis rtc;

DateTime now;

/////////FUNCTIONS TO HELP DEBUG//////////////////

void debugMessage(String message, String message2 = "") {
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print(message);
  lcd.setCursor(0,1);
  lcd.print(message2);
  delay(5000);
  lcd.clear();
  lcd.noBacklight();
}

/////////FUNCTIONS TO HELP DEBUG//////////////////

long millisToTheHour() {
  now = rtc.now();
  int minutesToWait = 60-now.minute();
  int secondsToWait = 60-now.second();
  long millisecondsToWait = ((minutesToWait*60000) + (secondsToWait*1000) + 60000);
  return millisecondsToWait;
}

float checkScaleWeight() {
  float weight;
  for(int i = 0; i < 10000; i++) {
    scale.update();
    weight = scale.getData();
  }
  return weight;
}

void scrollPrint(String text, int character = 0, int row = 0) {
  lcd.setCursor(character,row);
  int stringLengthOffScreen = text.length()-16;
  lcd.print(text);
  if(stringLengthOffScreen > 0) {
    delay(1000);
    for(int i = 0; i < stringLengthOffScreen; i++) {
      lcd.scrollDisplayLeft();
      delay(500);
    }
  }
}

void giveReminder(String reminderText) {
  for(int i=0; i<5; i++) { // blink 5 times
    lcd.backlight();
    delay(100);
    lcd.noBacklight();
    delay(100);
  }
  lcd.backlight();
  scrollPrint(reminderText); // print out the text
  delay(3000); // wait 5 seconds 
}

void checkWeight(int requiredWeight, String reminderText) {
  scale.update();
  if(!(checkScaleWeight() > 200 && checkScaleWeight() < requiredWeight)) { // if the scale is not in the acceptable limits
    while(checkScaleWeight() < 200) { // if nothing is on the scale
      giveReminder("Put the bottle on the scale");
      lcd.clear();
      scale.update();
    }
    while(checkScaleWeight() > requiredWeight) {
      giveReminder(reminderText); // go nuts until it's picked up
      lcd.clear();
      scale.update();
    }
    lcd.noBacklight();
    delay(60000); // it's been picked up, so wait a minute
    checkWeight(requiredWeight, reminderText);
  }
}

void setup() {
  // set up the scale
  scale.begin(); // begins connection to HX711
  scale.start(2000); // load cells gets 2000ms of time to stabilize
  scale.setCalFactor (453.5); // calibration factor for load cell => strongly dependent on your individual setup

  // set up the lcd
  lcd.init();
  lcd.begin(16, 2); // begins connection to the LCD module

  // configure the clock
  rtc.begin(DateTime(F(__DATE__), F(__TIME__)));
  lcd.backlight();
  lcd.print("Hello.");
  delay(3000);
  lcd.clear();
  lcd.noBacklight();
}

void loop() {
  now = rtc.now();
//  debugMessage("time: " + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()));
  if(now.hour() == 9) {
    checkWeight(2049, "Good morning. Drink up!");
  }
  else if(now.hour() == 10) {
    checkWeight(1892, "HYDRATE YOURSELF");
  }
  else if(now.hour() == 11) {
    checkWeight(1793, "glug glug glug glug glug");
  }
  else if(now.hour() == 12) {
    checkWeight(1634, "Keep sippin'");
  }
  else if(now.hour() == 13) {
    checkWeight(1518, "PUT WATER IN YOU");
  }
  else if(now.hour() == 14) {
    checkWeight(1372, "#DRINK-WATER-REMINDER");
  }
  else if(now.hour() == 15) {
    checkWeight(1262, "drink.");
  }
  else if(now.hour() == 16) {
    checkWeight(1099, "ALMOST FINISHED");
  }
  else if(now.hour() == 17) {
    checkWeight(996, "Now just a few more sips");
  }
  now = rtc.now();
//  debugMessage("mins til hr: " + String(millisToTheHour()/60000), "time: " + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()));
  delay(millisToTheHour());
}
