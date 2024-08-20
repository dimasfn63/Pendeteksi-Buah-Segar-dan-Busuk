//library loadcell
//#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include<HX711_ADC.h>
#include<EEPROM.h>
#include<LiquidCrystal_I2C.h>

//library TCS230
#include <tcs3200.h>
#define S0 4
#define S1 5
#define S2 6
#define S3 7
#define sensorOut 8

//variabel loadcell
LiquidCrystal_I2C lcd(0x27,16,2);
const int HX711_dout = A1;
const int HX711_sck = A0;
HX711_ADC LoadCell(HX711_dout, HX711_sck);
const int calVal_eepromAdress = 0;
long t;


//Variabel TCS2300
int redFrequency = 0;


int redMatang = 0;
int BeratMatang = 0; 
int redBusuk = 0;
int BeratBusuk = 0; 

void setup() {
  Serial.begin(9600);
  lcd.begin();
  
  //pendefinisian loadcell
  pinMode(HX711_dout, INPUT);
  
  //pendefinisian TCS2300
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  digitalWrite(S0,HIGH);
  digitalWrite(S1,HIGH);
  
  delay(10);
  Serial.println();
  Serial.println("Memulai...");
  
  //
  lcd.begin();
  LoadCell.begin();
     
  //Pemanggilan hasil kalibrasi loadcell di eeprom
  float calibrationValue; 
  calibrationValue = 696.0; 
  EEPROM.get(calVal_eepromAdress, calibrationValue);
  long stabilizingtime = 2000; 
  boolean _tare = true; 
  LoadCell.start(stabilizingtime, _tare);
  if (LoadCell.getTareTimeoutFlag()) {
    Serial.println("Timeout, cek kabel MCU>HX711 pastikan sudah tepat");
    while (1);
  }
  else {
    LoadCell.setCalFactor(calibrationValue);
    Serial.println("Startup selesai");
  }
}

void loop() {
  //Pemanggilan data loadcell
  static boolean newDataReady = 0;
  const int serialPrintInterval = 0; 
  
  if (LoadCell.update()) newDataReady = true;
  if (newDataReady) {
    if (micros() > t + serialPrintInterval) {
      int i = LoadCell.getData();
      if(i<0){
        i=0;
      }
      tampil(i);
      newDataReady = 0;
      t = micros();
    }
  }
  
  lcd.setCursor(0,0);
  lcd.print("Brt: ");
  lcd.setCursor(6,0);
  lcd.print("");
  lcd.setCursor(7,0);
  lcd.print("gr");
  
  //Pemanggilan data TCS3200
  lcd.begin();
  
  lcd.setCursor(0,1);
  lcd.print("Wrn: ");
  lcd.setCursor(6,1);
  lcd.print("");  

  redFrequency = pulseIn(sensorOut, LOW);
  lcd.print(redFrequency);
  lcd.print(" ");

  //Pendefinisian Variabel if
  redMatang = map(redFrequency, 67, 96, 255,0);
  BeratMatang = map(HX711_dout, 6, 10, 255,0);
  redBusuk = map(redFrequency, 63, 109, 255,0);
  BeratBusuk = map(HX711_dout, 4, 10, 255,0);
  
  
  if(redMatang > redBusuk ){
      Serial.println("Buah Matang");
    lcd.setCursor(10, 1);
   lcd.print("Matang  ");
  }
  
  if(redBusuk > redMatang ){
    Serial.println("Buah Busuk");
    lcd.setCursor(10, 1);
   lcd.print("Busuk  "); 
  }
}
  
  
  

void tampil(int j){
  //Pendefinisian dari loadcell ke lcd
  lcd.setCursor(4,0);
  lcd.print("     ");
  if(j<10){
    lcd.setCursor(6,0);  
  }else if(j<100 && j>=10){
    lcd.setCursor(5,0);
  }else if(j<1000 && j>=100){
    lcd.setCursor(4,0);
  }
  lcd.print(j);
}
