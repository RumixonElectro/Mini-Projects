#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int MQ135_PIN = A0;
const int RELAY_PIN = 3;
const int THRESHOLD = 300; // Updated threshold

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);
  pinMode(RELAY_PIN, OUTPUT);
  
  // Active LOW: HIGH is OFF at startup
  digitalWrite(RELAY_PIN, HIGH); 
  
  lcd.init();
  lcd.backlight(); // Ensure backlight is ON
  lcd.display();   // Ensure display is active
  
  lcd.setCursor(0, 0);
  lcd.print("Threshold: 500");
  delay(1000);
  lcd.clear();
}

void loop() {
  int val = analogRead(MQ135_PIN);
  
  // Data sent to Serial for your computer
  Serial.println(val);

  lcd.setCursor(0, 0);
  lcd.print("AQI: ");
  lcd.print(val);
  lcd.print("      ");

  lcd.setCursor(0, 1);
  if (val >= THRESHOLD) {
    digitalWrite(RELAY_PIN, LOW); // Active LOW: ON
    lcd.print("FAN: ON         ");
  } else {
    digitalWrite(RELAY_PIN, HIGH); // Active LOW: OFF
    lcd.print("FAN: OFF        ");
  }
  
  delay(500);
}