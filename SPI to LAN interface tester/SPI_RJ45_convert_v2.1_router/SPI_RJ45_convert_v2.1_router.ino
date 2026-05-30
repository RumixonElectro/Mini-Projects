#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define BUTTON_PIN 2    // Button connected between D2 and GND
#define CS_PIN 10       // W5500 Chip Select Pin

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Generic static configuration for basic link verification
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x10 };
IPAddress ip(192, 168, 1, 99); 

bool isNetworkInitialized = false;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print(F("Jig V2 Ready"));
  delay(1000);
  
  Ethernet.init(CS_PIN);
}

void loop() {
  // --- 1. HARDWARE (SPI) DIAGNOSTIC TRACKING ---
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    isNetworkInitialized = false; 
    lcd.clear();
    lcd.print(F("SPI: ERROR"));
    lcd.setCursor(0, 1);
    lcd.print(F("CHIP NOT FOUND"));
    
    while (Ethernet.hardwareStatus() == EthernetNoHardware) {
      delay(200); 
    }
  }

  // --- 2. INITIALIZE NETWORK STACK ONCE SPI IS OK ---
  if (!isNetworkInitialized) {
    Ethernet.begin(mac, ip);
    isNetworkInitialized = true;
  }

  // --- 3. ROUTER / SWITCH (LAN) LINK TRACKING ---
  if (Ethernet.linkStatus() == LinkOFF) {
    lcd.clear();
    lcd.print(F("SPI: OK"));
    lcd.setCursor(0, 1);
    lcd.print(F("LAN: DISCONNECTED"));
    
    while (Ethernet.linkStatus() == LinkOFF) {
      if (Ethernet.hardwareStatus() == EthernetNoHardware) return; 
      delay(200); 
    }
  }

  // --- 4. PASS / READY SCREEN ---
  // If the code reaches here, both SPI communication and Router Link are successful
  lcd.clear();
  lcd.print(F("SPI: OK"));
  lcd.setCursor(0, 1);
  lcd.print(F("LAN: LINK PASSED"));
  
  // Hold until the module is pulled out or link drops
  while (Ethernet.hardwareStatus() == EthernetHardwareStatus() && Ethernet.linkStatus() == LinkON) {
    delay(200);
  }
}`