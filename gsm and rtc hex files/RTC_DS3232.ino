// include LCD library code
#include <LiquidCrystal.h>

// LCD module Pins(RS, E, D4, D5, D6, D7)
 LiquidCrystal lcd(7,  6,  5,  4,  3,  2);

// include Wire library code (needed for I2C protocol devices)
#include<Wire.h>

// include RTC library code
#include <RTClib.h>
RTC_DS3231 rtc;

void setup()
{
  lcd.begin(20,4);  //Start the LCD with Columns and Rows
  lcd.setCursor(0,0);
  lcd.print("  The Bright Light");
  
  Wire.begin();
  rtc.begin(); // load the time from your computer.
  rtc.adjust(DateTime(F(__DATE__),F(__TIME__)));
}
void loop()
{
  DateTime now = rtc.now();
  
  lcd.setCursor(0,1);
  lcd.print(now.hour(), DEC);
  lcd.print(':');
  lcd.print(now.minute(), DEC);
  lcd.print(':');
  lcd.print(now.second(), DEC);
  lcd.print("  ");
  lcd.print(now.day(), DEC);
  lcd.print('/');
  lcd.print(now.month(), DEC);
  lcd.print('/');
  lcd.print(now.year(), DEC);
  delay(100);
}


