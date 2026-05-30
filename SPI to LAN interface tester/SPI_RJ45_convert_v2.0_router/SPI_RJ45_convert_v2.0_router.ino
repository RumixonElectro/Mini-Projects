#include <SPI.h>
#include <Ethernet.h> // Supports W5500 specific checks like link status
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); 

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177); // Dynamic or static IP for testing

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.print("W5500 Tester ZIG");
  delay(1500);
  lcd.clear();

  // Initialize Ethernet
  Ethernet.init(10); // CS pin
  Ethernet.begin(mac, ip);

  // 1. Check Hardware SPI connection
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    lcd.print("SPI: FAILED");
    lcd.setCursor(0, 1);
    lcd.print("No W5500 Found");
    while (true); // Halt
  }
  lcd.print("SPI: OK");
  delay(1000);

  // 2. Check Cable Link Status
  lcd.setCursor(0, 1);
  if (Ethernet.linkStatus() == LinkOFF) {
    lcd.print("LAN: NO CABLE");
  } else {
    lcd.print("LAN: LINK OK");
  }
}

void loop() {
  // Continuous status loop

  // 1. Constantly check Hardware SPI connection
  lcd.clear();
  lcd.setCursor(0, 0);
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    lcd.print("SPI: FAILED");
    while (true); // Halt
  }
  else
  // 2. Constantly Check Cable Link Status
  lcd.setCursor(0, 1);
  if (Ethernet.linkStatus() == LinkOFF) {
    lcd.print("LAN: NO CABLE");
  } else {
    lcd.print("LAN: LINK OK");
  }
  delay(1000);
}