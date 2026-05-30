#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// --- PIN DEFINITIONS ---
#define CS_PIN 10
#define RESET_PIN 3
#define BUZZER_PIN 4
#define GREEN_LED_PIN 2

LiquidCrystal_I2C lcd(0x27, 16, 2);
EthernetUDP Udp;

unsigned int localPort = 8888; 
unsigned int remotePort = 8888;
byte mac[6];

const char* NODE_SIGNATURE = "TG_NODE:";

void generateRandomMAC() {
  randomSeed(analogRead(A0) + analogRead(A1));
  mac[0] = 0xDE; mac[1] = 0xAD;
  mac[2] = random(0x00, 0xFE);
  mac[3] = random(0x00, 0xFE);
  mac[4] = random(0x00, 0xFE);
  mac[5] = random(0x00, 0xFE);
}

void setup() {
  pinMode(RESET_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);

  lcd.init();
  lcd.backlight();
  lcd.print(F("V4 Jig Booting..."));

  // 1. POWER ON BEEP: Exactly 200ms
  digitalWrite(BUZZER_PIN, HIGH);
  delay(200);
  digitalWrite(BUZZER_PIN, LOW);

  // Hardware Reset W5500
  digitalWrite(RESET_PIN, LOW);  delay(50);
  digitalWrite(RESET_PIN, HIGH); delay(500); 

  generateRandomMAC();
  Ethernet.init(CS_PIN);

  // CRITICAL RESOLUTION: Wake SPI bus with dummy static configuration first
  IPAddress dummyIP(192, 168, 1, 200);
  Ethernet.begin(mac, dummyIP);

  // 2. SEQUENTIAL CHECK STEP 1: SPI Hardware Check (10-second timeout)
  unsigned long spiStartTime = millis();
  while (Ethernet.hardwareStatus() == EthernetNoHardware) {
    if (millis() - spiStartTime >= 10000) {
      lcd.clear();
      lcd.print(F("SPI: FAILED"));
      lcd.setCursor(0, 1);
      lcd.print(F("CHIP NOT FOUND"));
      
      // Hardware missing -> Buzzer remains CONSTANTLY HIGH
      while (true) {
        digitalWrite(BUZZER_PIN, HIGH);
      }
    }
    delay(100); 
  }

  // 3. SEQUENTIAL CHECK STEP 2: DHCP Network Check (Only if SPI passes)
  lcd.clear();
  lcd.print(F("SPI: OK"));
  lcd.setCursor(0, 1);
  lcd.print(F("Fetching IP..."));

  unsigned long dhcpStartTime = millis();
  bool ipAssigned = false;

  // Run dynamic configuration handshake
  while (ipAssigned == false) {
    if (Ethernet.begin(mac) != 0) {
      ipAssigned = true; 
    } else {
      if (millis() - dhcpStartTime >= 10000) {
        lcd.clear();
        lcd.print(F("DHCP: FAILED"));
        lcd.setCursor(0, 1);
        lcd.print(F("NO IP ASSIGNED"));
        
        // No IP -> Continuous 300ms beep / 200ms gap loop
        while (true) {
          digitalWrite(BUZZER_PIN, HIGH); delay(300);
          digitalWrite(BUZZER_PIN, LOW);  delay(200);
        }
      }
    }
  }

  // 4. INITIALIZATION SUCCESS STATE
  digitalWrite(GREEN_LED_PIN, HIGH);
  lcd.clear();
  lcd.print(Ethernet.localIP());
  lcd.setCursor(0, 1);
  lcd.print(F("Module is OK"));

  // Double beep success trigger (300ms beep, 200ms gap)
  digitalWrite(BUZZER_PIN, HIGH); delay(300);
  digitalWrite(BUZZER_PIN, LOW);  delay(200);
  digitalWrite(BUZZER_PIN, HIGH); delay(300);
  digitalWrite(BUZZER_PIN, LOW);

  Udp.begin(localPort);
}

void loop() {
  int packetSize = Udp.parsePacket();
  if (packetSize > 0) {
    char packetBuffer[64] = {0};
    Udp.read(packetBuffer, sizeof(packetBuffer) - 1);
    String incomingData = String(packetBuffer);

    if (incomingData.startsWith(NODE_SIGNATURE)) {
      String cleanMessage = incomingData.substring(strlen(NODE_SIGNATURE));
      
      lcd.clear();
      lcd.print(F("MESH MSG RCVD:"));
      lcd.setCursor(0, 1);
      lcd.print(cleanMessage);
      
      // Fast dynamic validation beep (50ms alert)
      digitalWrite(BUZZER_PIN, HIGH); delay(50);
      digitalWrite(BUZZER_PIN, LOW);
      
      delay(3000); 
      
      lcd.clear();
      lcd.print(Ethernet.localIP());
      lcd.setCursor(0, 1);
      lcd.print(F("Module is OK"));
    }
  }
}