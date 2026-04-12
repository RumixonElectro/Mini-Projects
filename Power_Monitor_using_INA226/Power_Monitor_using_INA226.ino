#include "INA226.h"
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#define Buzzer 12
#define Temp 2

INA226 INA(0x40);
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup()
{
  pinMode(Buzzer, OUTPUT);        // Sets pin 12 as an output for the buzzer [17]
  pinMode(Temp, INPUT_PULLUP);    // Sets pin 2 as an input with internal pull-up [15]
  lcd.init();                      // initialize the lcd 
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Welcome To Power");
  lcd.setCursor(0,1);
  lcd.print("Monitor");
  delay(2000);
  lcd.setCursor(0,0);
  lcd.print("Welcome To Power");
  
  
  Serial.begin(115200);
  Serial.println();
  // lcd.clear();
  Serial.println(__FILE__);
  Serial.print("INA226_LIB_VERSION: ");
  Serial.println(INA226_LIB_VERSION);
  Serial.println();

  lcd.setCursor(0, 0);
  lcd.print(__FILE__);
  delay(1000);
  lcd.clear();
  lcd.print("INA226_LIB_VERSION: ");
  delay(1000);
  lcd.clear();
  lcd.println(INA226_LIB_VERSION);
  delay(1000);
  lcd.clear();

  Wire.begin();
  if (!INA.begin() )
  {
    Serial.println("could not connect. Fix and Reboot");
  }
  INA.setMaxCurrentShunt(1, 0.01);
}


void loop()
{
  Serial.println("\nBUS\tSHUNT\tCURRENT\tPOWER");
  for (int i = 0; i < 20; i++)
  {
    Serial.print(INA.getBusVoltage(), 3);
    Serial.print("\t");
    Serial.print(INA.getShuntVoltage_mV(), 3);
    Serial.print("\t");
    Serial.print(INA.getCurrent_mA(), 3);
    Serial.print("\t");
    Serial.print(INA.getPower_mW(), 3);
    Serial.println();


    if(digitalRead(Temp)==LOW){
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("High Temperature");
      lcd.setCursor(0, 1);
      lcd.print("!!--Alert--!!");
      digitalWrite(Buzzer, HIGH);
    }
    else{
      digitalWrite(Buzzer, LOW);
      lcd.clear();
    
      lcd.setCursor(0, 0);
      lcd.print("V:");
      lcd.setCursor(2, 0);
      lcd.print(INA.getBusVoltage());

      lcd.setCursor(6, 0);
      lcd.print(" SV:");
      lcd.setCursor(10, 0);
      lcd.print(INA.getShuntVoltage_mV());
    

      lcd.setCursor(0, 1);
      lcd.print("A:");
      lcd.setCursor(2, 1);
      lcd.print(INA.getCurrent_mA());
      lcd.setCursor(6, 1);
      lcd.print(" P:");
      lcd.setCursor(9, 1);
      lcd.print(INA.getPower_mW());


    }
    delay(1000);
  }
}