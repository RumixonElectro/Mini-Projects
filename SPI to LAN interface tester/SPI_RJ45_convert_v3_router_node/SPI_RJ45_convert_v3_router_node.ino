#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define BUTTON_PIN 2
#define CS_PIN 10

LiquidCrystal_I2C lcd(0x27, 16, 2);
EthernetUDP Udp;

unsigned int localPort = 8888; 
unsigned int remotePort = 8888;

byte mac[6];
// Updated fallback subnet matching the Windows ICS gateway (192.168.137.1)
IPAddress fallbackIP(192, 168, 137, 200); 

const char* NODE_SIGNATURE = "TG_NODE:";
bool lastButtonState = HIGH;

void generateRandomMAC() {
  randomSeed(analogRead(A0) + analogRead(A1));
  mac[0] = 0xDE;
  mac[1] = 0xAD;
  mac[2] = random(0x00, 0xFE);
  mac[3] = random(0x00, 0xFE);
  mac[4] = random(0x00, 0xFE);
  mac[5] = random(0x00, 0xFE);
}

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  lcd.init();
  lcd.backlight();
  lcd.print(F("ICS Mesh Init..."));

  generateRandomMAC();
  Ethernet.init(CS_PIN);

  lcd.setCursor(0, 1);
  // Request an IP from your Windows Desktop
  if (Ethernet.begin(mac) == 0) {
    lcd.print(F("DHCP Timeout"));
    delay(1000);
    // Use the 192.168.137.x fallback if Windows is slow to respond
    fallbackIP[3] = random(20, 250); 
    Ethernet.begin(mac, fallbackIP);
  }
  
  Udp.begin(localPort);
  showIdleScreen();
}

void loop() {
  // --- 1. RECEIVER (Listen for packets from PC) ---
  int packetSize = Udp.parsePacket();
  if (packetSize > 0) {
    char packetBuffer[64] = {0};
    Udp.read(packetBuffer, sizeof(packetBuffer) - 1);
    String incomingData = String(packetBuffer);

    if (incomingData.startsWith(NODE_SIGNATURE)) {
      String cleanMessage = incomingData.substring(strlen(NODE_SIGNATURE));
      
      lcd.clear();
      lcd.print(F("PC TEST MSG:"));
      lcd.setCursor(0, 1);
      lcd.print(cleanMessage);
      
      delay(4000); 
      showIdleScreen();
    }
  }

  // --- 2. TRANSMITTER (Send broadcast to PC on Button Press) ---
  bool currentButtonState = digitalRead(BUTTON_PIN);
  if (currentButtonState == LOW && lastButtonState == HIGH) {
    delay(50); // Debounce
    
    lcd.clear();
    lcd.print(F("Sending to PC..."));
    
    // Broadcast over the Windows network bridge
    IPAddress broadcastIP(255, 255, 255, 255);
    
    Udp.beginPacket(broadcastIP, remotePort);
    Udp.write(NODE_SIGNATURE); 
    Udp.write("Ethernet test OK");
    
    if (Udp.endPacket() == 1) {
      lcd.setCursor(0, 1);
      lcd.print(F("TX: SUCCESS"));
    } else {
      lcd.setCursor(0, 1);
      lcd.print(F("TX: FAILED"));
    }
    delay(2000);
    showIdleScreen();
  }
  lastButtonState = currentButtonState;
}

void showIdleScreen() {
  lcd.clear();
  lcd.print(Ethernet.localIP()); 
  lcd.setCursor(0, 1);
  lcd.print(F("V3 Desktop Ready"));
}