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

// Fallback IP configurations if DHCP server/Router is missing on the switch
byte mac[6];
IPAddress fallbackIP(192, 168, 1, 200); 

// Unique signature to identify our testing devices from regular network traffic
const char* NODE_SIGNATURE = "TG_NODE:";
bool lastButtonState = HIGH;

void generateRandomMAC() {
  // Generate unique MAC using internal Arduino analog noise to prevent conflicts
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
  lcd.print(F("Auto-Mesh Init.."));

  generateRandomMAC();
  Ethernet.init(CS_PIN);

  lcd.setCursor(0, 1);
  // Attempt to get IP from router automatically
  if (Ethernet.begin(mac) == 0) {
    lcd.print(F("DHCP Fail->Static"));
    // Fallback to static if connected to an unmanaged switch without a router
    fallbackIP[3] = random(20, 250); // Randomize last octet to prevent collision
    Ethernet.begin(mac, fallbackIP);
  } else {
    lcd.print(F("DHCP IP Assigned"));
  }
  
  delay(1500);
  
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    lcd.clear();
    lcd.print(F("W5500 HW Error"));
    while (true);
  }

  Udp.begin(localPort);
  showIdleScreen();
}

void loop() {
  // --- 1. NETWORK LISTENER (AUTO-FILTER TRAFFIC) ---
  int packetSize = Udp.parsePacket();
  if (packetSize > 0) {
    char packetBuffer[64] = {0};
    Udp.read(packetBuffer, sizeof(packetBuffer) - 1);
    String incomingData = String(packetBuffer);

    // Check if the packet starts with our unique identifier token
    if (incomingData.startsWith(NODE_SIGNATURE)) {
      // Strip away the signature token to read the raw test message
      String cleanMessage = incomingData.substring(strlen(NODE_SIGNATURE));
      
      lcd.clear();
      lcd.print(F("NODE MSG RCVD:"));
      lcd.setCursor(0, 1);
      lcd.print(cleanMessage);
      
      delay(4000); // Hold message on display
      showIdleScreen();
    }
    // Foreign traffic from computers or routers is ignored automatically here
  }

  // --- 2. BROADCAST TRANSMITTER (BUTTON TRIGGER) ---
  bool currentButtonState = digitalRead(BUTTON_PIN);
  if (currentButtonState == LOW && lastButtonState == HIGH) {
    delay(50); // Debounce
    
    lcd.clear();
    lcd.print(F("Broadcasting..."));
    
    // 255.255.255.255 sends the packet to EVERY device on the current local network switch
    IPAddress broadcastIP(255, 255, 255, 255);
    
    Udp.beginPacket(broadcastIP, remotePort);
    Udp.write(NODE_SIGNATURE); // Inject identity token
    Udp.write("Ethernet test OK");
    
    if (Udp.endPacket() == 1) {
      lcd.setCursor(0, 1);
      lcd.print(F("Sent to Mesh!"));
    } else {
      lcd.setCursor(0, 1);
      lcd.print(F("Tx Error"));
    }
    delay(2000);
    showIdleScreen();
  }
  lastButtonState = currentButtonState;
}

void showIdleScreen() {
  lcd.clear();
  lcd.print(Ethernet.localIP()); // Displays current IP so you can monitor it
  lcd.setCursor(0, 1);
  lcd.print(F("Mesh Mesh Ready"));
}