#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// --- PIN DEFINITIONS ---
#define CS_PIN 10
#define RESET_PIN 3
#define BUZZER_PIN 4
#define GREEN_LED_PIN 2
#define TEST_BUTTON_PIN 5 // Tactile button connected between Pin 5 and GND

// --- GLOBAL CONFIGURATION ---
const unsigned long TIMEOUT_MS = 2000; // SPI and DHCP timeout limits in milliseconds

LiquidCrystal_I2C lcd(0x27, 16, 2);
EthernetUDP Udp;

unsigned int localPort = 8888; 
unsigned int remotePort = 8888;
byte mac[6];

const char* NODE_SIGNATURE = "TG_NODE:";
bool testingCompleted = false;

void generateRandomMAC() {
  randomSeed(analogRead(A0) + analogRead(A1));
  mac[0] = 0xDE; mac[1] = 0xAD;
  mac[2] = random(0x00, 0xFE);
  mac[3] = random(0x00, 0xFE);
  mac[4] = random(0x00, 0xFE);
  mac[5] = random(0x00, 0xFE);
}

void showReadyScreen() {
  digitalWrite(GREEN_LED_PIN, LOW);
  testingCompleted = false;
  lcd.clear();
  lcd.print(F("INSERT MODULE"));
  lcd.setCursor(0, 1);
  lcd.print(F("PRESS BTN (D5)"));
}

void setup() {
  pinMode(RESET_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(TEST_BUTTON_PIN, INPUT_PULLUP); // Active LOW with pull-up
  
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);

  lcd.init();
  lcd.backlight();
  
  showReadyScreen();
}

// Executes the entire sequential validation pipeline
bool runDiagnostics() {
  // 1. Initial trigger chirp (100ms)
  digitalWrite(BUZZER_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_PIN, LOW);

  lcd.clear();
  lcd.print(F("Resetting W5500..."));

  // Hardware Reset W5500 to clear previous connection states
  digitalWrite(RESET_PIN, LOW);  delay(50);
  digitalWrite(RESET_PIN, HIGH); delay(500); 

  generateRandomMAC();
  Ethernet.init(CS_PIN);

  // Wake SPI bus with dummy static configuration first
  IPAddress dummyIP(192, 168, 1, 200);
  Ethernet.begin(mac, dummyIP);

  // 2. Phase 1: SPI Hardware Check
  lcd.clear();
  lcd.print(F("Checking SPI..."));
  unsigned long spiStartTime = millis();
  while (Ethernet.hardwareStatus() == EthernetNoHardware) {
    if (millis() - spiStartTime >= TIMEOUT_MS) { 
      lcd.clear();
      lcd.print(F("SPI: FAILED"));
      lcd.setCursor(0, 1);
      lcd.print(F("CHIP NOT FOUND"));
      
      // Hardware missing -> Beep exactly 5 times (100ms beep / 100ms gap)
      for (int i = 0; i < 5; i++) {
        digitalWrite(BUZZER_PIN, HIGH);
        delay(100);
        digitalWrite(BUZZER_PIN, LOW);
        if (i < 4) {
          delay(100);
        }
      }
      return false; // Stop diagnostics
    }
    delay(50); 
  }

  // 3. Phase 2: DHCP Network Check
  lcd.clear();
  lcd.print(F("SPI: OK"));
  lcd.setCursor(0, 1);
  lcd.print(F("Fetching IP..."));

  // Set DHCP timeout, with response timeout as half of TIMEOUT_MS (minimum 1000ms)
  unsigned long responseTimeout = (TIMEOUT_MS / 2 < 1000) ? 1000 : (TIMEOUT_MS / 2);
  if (Ethernet.begin(mac, TIMEOUT_MS, responseTimeout) == 0) { 
    lcd.clear();
    lcd.print(F("DHCP: FAILED"));
    lcd.setCursor(0, 1);
    lcd.print(F("NO IP ASSIGNED"));
    
    // No IP -> Beep exactly 2 times (100ms beep / 100ms gap)
    digitalWrite(BUZZER_PIN, HIGH); delay(100);
    digitalWrite(BUZZER_PIN, LOW);  delay(100);
    digitalWrite(BUZZER_PIN, HIGH); delay(100);
    digitalWrite(BUZZER_PIN, LOW);
    return false; // Stop diagnostics
  }

  // 4. Initialization Success State
  digitalWrite(GREEN_LED_PIN, HIGH);
  lcd.clear();
  lcd.print(Ethernet.localIP());
  lcd.setCursor(0, 1);
  lcd.print(F("Module is OK"));

  // Success double beep (100ms beep, 100ms gap)
  digitalWrite(BUZZER_PIN, HIGH); delay(100);
  digitalWrite(BUZZER_PIN, LOW);  delay(100);
  digitalWrite(BUZZER_PIN, HIGH); delay(100);
  digitalWrite(BUZZER_PIN, LOW);

  Udp.begin(localPort);
  testingCompleted = true;
  return true;
}

void loop() {
  // If the manual test button (D5) is pressed (Active LOW)
  if (digitalRead(TEST_BUTTON_PIN) == LOW) {
    delay(50); // Software debounce
    if (digitalRead(TEST_BUTTON_PIN) == LOW) {
      digitalWrite(GREEN_LED_PIN, LOW); // Turn off LED for the new test run
      runDiagnostics();
      
      // Wait for the user to release the button to prevent double triggers
      while (digitalRead(TEST_BUTTON_PIN) == LOW) {
        delay(10);
      }
    }
  }

  // Only listen for mesh UDP packets if the active module successfully passed test checks
  if (testingCompleted) {
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
        
        // Fast dynamic validation beep (100ms alert)
        digitalWrite(BUZZER_PIN, HIGH); delay(100);
        digitalWrite(BUZZER_PIN, LOW);
        
        delay(3000);
        
        lcd.clear();
        lcd.print(Ethernet.localIP());
        lcd.setCursor(0, 1);
        lcd.print(F("Module is OK"));
      }
    }
  }
}