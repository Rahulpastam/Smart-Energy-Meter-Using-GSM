#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include "EmonLib.h"
#include<Wire.h>
#include <RTClib.h>
RTC_DS3231 rtc;
bool hasRun = false;

SoftwareSerial mySerial(6, 7);
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);
EnergyMonitor emon1;

const int Sensor_Pin = A1;
int sensitivity = 185;
int offsetvoltage = 2542;
const char* monthNames[] = {
  "January", "February", "March", "April", "May", "June",
  "July", "August", "September", "October", "November", "December"
};


void setup() {
  emon1.current(A1, sensitivity);
  emon1.voltage(A0, 187, 1.7);

  mySerial.begin(9600);
  Serial.begin(9600);
  Wire.begin();
  rtc.begin();
  lcd.begin(20, 4);
  rtc.adjust(DateTime(F(__DATE__),F(__TIME__)));

  
  delay(300);
  lcd.setCursor(1, 1);
  lcd.print("Smart Energy Meter");
  lcd.setCursor(1, 2);
  lcd.print("Using GSM Module");
  delay(700);
  lcd.clear();
  lcd.setCursor(2, 1);
  lcd.print("Initializing ");
  lcd.setCursor(2, 2);
  lcd.print("Energy meter..");
  delay(700);
  lcd.clear();
  
}

void loop() {
  DateTime now = rtc.now();
  emon1.calcVI(20, 2000);
  int Voltage = emon1.Vrms;

  unsigned int temp = 0;
  float maxpoint = 0;
  for (int i = 0; i < 500; i++) {
    if (temp = analogRead(Sensor_Pin), temp > maxpoint) {
      maxpoint = temp;
    }
  }

  float ADCvalue = maxpoint;
  double eVoltage = (ADCvalue / 1024.0) * 5000;
  double Current = ((eVoltage - offsetvoltage) / sensitivity);
  double AC_Current = (Current) / (sqrt(2));

  int Power = (Voltage * AC_Current);

  long milisec = millis();
  long time = milisec / 1000;
  
  float Energy = (Power * time) / 3600;
  float energy_kwh = Energy / 1000;

  if(!hasRun){
    FirstMessage();
    hasRun = true;
  } 

  lcd.setCursor(0, 0);
  lcd.print(now.day(), DEC);
  lcd.print("-");
  lcd.print(now.month(), DEC);
  lcd.print("-");
  lcd.print(now.year(), DEC);
  lcd.setCursor(0, 2);
  lcd.print("Energy(in Units)");
  lcd.print(": ");
  lcd.setCursor(0, 3);
  lcd.print(energy_kwh, 4);
 
  float startEnergy = energy_kwh;
  const int tempo = 2*60;
  // float TotalEnergy = 0.0;
  float TotalEnergy = energy_kwh;
  if(Serial.available() > 0){
    char rbit = Serial.read();
  if (rbit == 'a' ) {
    Datefunction();
    CostFunction(energy_kwh);  
  }
  else if (rbit == 'b') {
    Datefunction();
    NextBillGen();
  }
  else if (rbit == 'c') {
    Datefunction();
    Consump24(energy_kwh);
  }
  else if (rbit == 'd') {
    Datefunction();
   LastBill();
  } 
  else{
    Datefunction();
    Error();
  }
  }
}

void FirstMessage(){

  mySerial.println();
  mySerial.print("^^SMART ENERGY METER^^");
  mySerial.println();
  mySerial.println();
  Datefunction();
  mySerial.println("These are the services we provide: ");
  mySerial.println("Please type your option below list...");
  mySerial.println();
  mySerial.println("For Total Bill and Total Power(in Units) Consumed for this Month --->  Message 'a' ");
  mySerial.println("            - - -     ");
  mySerial.println("For Next Bill Generation Date --->  Message 'b' ");
  mySerial.println("            - - -     ");
  mySerial.println("For Consumption in Last 24 hour --->  Message 'c' ");
  mySerial.println("            - - -     ");
  mySerial.println("For The Details of the last month Bill --->  Message 'd' ");
  mySerial.println("          ----------------------------------------------------------");
  mySerial.println();
}

void Datefunction(){
    DateTime t = rtc.now();
  int month = t.month();
  int day = t.dayOfTheWeek();
  const char* dayNames[] = {
  "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
};

  mySerial.print("                    ");
  if(day == 0){
    mySerial.print(dayNames[0]);
  }
  else if(day>0){
    mySerial.print(dayNames[day]);
  }
  mySerial.print(", ");
  mySerial.print(monthNames[month-1]);
   DateTime n = rtc.now();
   mySerial.print("-");
   mySerial.print(n.day(),DEC);
   mySerial.print(" ");
   mySerial.print(n.hour(), DEC);
   mySerial.print(":");
   mySerial.print(n.minute(), DEC);
  mySerial.println();
  mySerial.println();
}

void CostFunction(float Energy) {
  float Unitcost;
  float Totalcost;
  if(Energy < 30.0){
    Unitcost = 4.22;
    Totalcost = (Unitcost*Energy)+40;
    mySerial.print("Total Bill: ");
    mySerial.println(Totalcost);
  }
  else if (Energy >30.0 && Energy < 100.0) {
    Unitcost = 5.02;
    Totalcost = (Unitcost*Energy)+40;
    mySerial.print("Total Bill: ");
    mySerial.println(Totalcost);
  }
  else if(Energy >100.0 && Energy < 150.0){
    Unitcost = 6.00;
    Totalcost = (Unitcost*Energy)+40;
    mySerial.print("Total Bill: ");
    mySerial.println(Totalcost);
  }
 
  mySerial.print("The Energy consumed(in units): ");
  mySerial.println(Energy, 4);
  mySerial.println("          ----------------------------------------------------------");
  mySerial.println();
}

void NextBillGen(){
  DateTime now = rtc.now();
  int month = now.month();

  mySerial.print("Next Bill will be Generated on: ");
  mySerial.print(now.day(), DEC);
  mySerial.print("-");
  if(month == 12){
    mySerial.print(1);
    mySerial.print(monthNames[0]);
  }
  else{
    mySerial.print(monthNames[month]);
  }
  mySerial.print("-");
  mySerial.println(now.year(),DEC);
  mySerial.println("          ----------------------------------------------------------");
  mySerial.println();
}

void Consump24(float Energy){
  mySerial.print("The last 24hr Consumption (In units) : ");
  mySerial.println(Energy,4);
  mySerial.println("          ----------------------------------------------------------");
  mySerial.println();
}


void LastBill(){
  DateTime now = rtc.now();
  int month = now.month();
  float Energy = 130.02;
  float Unitcost,Totalcost;
   if(Energy < 30.0){
    Unitcost = 4.22;
    Totalcost = (Unitcost*Energy)+40;
  }
  else if (Energy >30.0 && Energy < 100.0) {
    Unitcost = 5.02;
    Totalcost = (Unitcost*Energy)+40;
  }
  else if(Energy >100.0 && Energy < 150.0){
    Unitcost = 6.00;
    Totalcost = (Unitcost*Energy)+40;
  }
  mySerial.print("Your Bill details for the month of ");
  mySerial.print(monthNames[month - 2]);
  mySerial.print("-");
  mySerial.print(now.year(), DEC);
  mySerial.print(" are: ");
  mySerial.println();
  mySerial.print("Total Power Consumed (In Units): ");
  mySerial.println(Energy);
  mySerial.print("Bill Paid (In Rupees): ");
  mySerial.println(Totalcost);
  mySerial.println("          ----------------------------------------------------------");
  mySerial.println();
}

void Error(){
  mySerial.println("Please choose the option from the below list only...");
  FirstMessage();
}