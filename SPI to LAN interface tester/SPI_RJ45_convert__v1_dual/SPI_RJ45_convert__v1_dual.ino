#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// --- CONFIGURATION ---
#define DEVICE_ID 1  // Change this to 2 for the second Arduino Nano

#define BUTTON_PIN 2
#define CS_PIN 10

LiquidCrystal_I2C lcd(0x27, 16, 2);
EthernetUDP Udp;
unsigned int localPort = 8888;
unsigned int remotePort = 8888;

// Network Settings based on Device ID
#if DEVICE_ID == 1
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x01 };
IPAddress ip(192, 168, 1, 10);
IPAddress targetIP(192, 168, 1, 11);  // Points to Node 2
#else
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x02 };
IPAddress ip(192, 168, 1, 11);
IPAddress targetIP(192, 168, 1, 10);  // Points to Node 1
#endif

bool lastButtonState = HIGH;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();
  lcd.print(F("Eth Node: "));
  lcd.print(DEVICE_ID);

  Ethernet.init(CS_PIN);
  Ethernet.begin(mac, ip);

  lcd.setCursor(0, 1);
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    lcd.print(F("No Eth Hardware"));
    while (true)
      ;
  }

  Udp.begin(localPort);
  delay(1000);

  lcd.clear();
  lcd.print(F("Ready to Test..."));
}

void loop() {
  // --- 1. RECEIVE LOGIC ---
  int packetSize = Udp.parsePacket();
  if (packetSize > 0) {
    char packetBuffer[32] = { 0 };
    Udp.read(packetBuffer, sizeof(packetBuffer) - 1);

    lcd.clear();
    lcd.print(F("RX Packet:"));
    lcd.setCursor(0, 1);
    lcd.print(packetBuffer);
  }

  // --- 2. TRANSMIT LOGIC (Button Press) ---
  bool currentButtonState = digitalRead(BUTTON_PIN);
  if (currentButtonState == LOW && lastButtonState == HIGH) {
    delay(50);  // Simple Debounce

    lcd.clear();
    lcd.print(F("Sending..."));

    Udp.beginPacket(targetIP, remotePort);
    Udp.write("Ethernet test OK");

    if (Udp.endPacket() == 1) {
      lcd.setCursor(0, 1);
      lcd.print(F("TX Success!"));
    } else {
      lcd.setCursor(0, 1);
      lcd.print(F("TX Failed"));
    }
    delay(1000);
    lcd.clear();
    lcd.print(F("Ready to Test..."));
  }
  lastButtonState = currentButtonState;
}