#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include "time.h"

// --- OLED Display Setup ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// --- Pin Definitions ---
const int BTN_UP = D0;      // D0
const int BTN_DOWN = D1;    // D1
const int BTN_SELECT = D2;  // D2
const int BTN_BACK = D3;    // D3

// --- Wi-Fi Credentials ---
const char* ssid     = "Galaxy A25";
const char* password = "31153115";

// --- Time Settings (NTP) ---
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 19800; // Offset for India (UTC +5:30). Change for your timezone.
const int   daylightOffset_sec = 0;

// --- System States ---
enum WatchState {
  STATE_WATCH_FACE,
  STATE_MENU,
  STATE_STOPWATCH,
  STATE_INFO
};

WatchState currentState = STATE_WATCH_FACE;

// --- Menu Variables ---
const char* menuItems[] = {"Stopwatch", "System Info", "Sync Time"};
const int menuLength = 3;
int currentMenuIndex = 0;

// --- Stopwatch Variables ---
unsigned long stopwatchStart = 0;
bool stopwatchRunning = false;
unsigned long elapsedStopwatch = 0;

// --- Button Handling Struct ---
struct Button {
  int pin;
  bool lastState;
  unsigned long lastDebounceTime;
  
  void init() {
    pinMode(pin, INPUT_PULLUP);
    lastState = HIGH;
    lastDebounceTime = 0;
  }

  bool isPressed() {
    int reading = digitalRead(pin);
    if (reading != lastState) {
      lastDebounceTime = millis();
    }
    lastState = reading;
    
    if ((millis() - lastDebounceTime) > 50) { // 50ms debounce
       if (reading == LOW) { // Button pressed (Active LOW)
         return true;
       }
    }
    return false;
  }
};

Button bUp = {BTN_UP};
Button bDown = {BTN_DOWN};
Button bSelect = {BTN_SELECT};
Button bBack = {BTN_BACK};

// =================================================================
// SETUP
// =================================================================
void setup() {
  Serial.begin(115200);

  // 1. Initialize Buttons
  bUp.init();
  bDown.init();
  bSelect.init();
  bBack.init();

  // 2. Initialize OLED
  Wire.begin(); 
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  // 3. Show Boot Screen
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10, 20);
  display.println("XIAO OS");
  display.display();
  delay(1000);

  // 4. Connect to WiFi & Sync Time
  display.setTextSize(1);
  display.setCursor(0, 50);
  display.println("Connecting WiFi...");
  display.display();
  
  WiFi.begin(ssid, password);
  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 10) {
      delay(500);
      Serial.print(".");
      retry++;
  }
  
  if(WiFi.status() == WL_CONNECTED) {
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  }
}

// =================================================================
// MAIN LOOP
// =================================================================
void loop() {
  // Clear display buffer at start of loop
  display.clearDisplay();

  // 1. Handle Input based on Global State
  handleInput();

  // 2. Render Screen based on Global State
  switch (currentState) {
    case STATE_WATCH_FACE:
      drawWatchFace();
      break;
    case STATE_MENU:
      drawMenu();
      break;
    case STATE_STOPWATCH:
      drawStopwatch();
      break;
    case STATE_INFO:
      drawInfo();
      break;
  }

  display.display();
  delay(50); // Small delay to save power and ease processing
}

// =================================================================
// LOGIC HANDLERS
// =================================================================

void handleInput() {
  // Flag to ensure we don't handle multiple presses too fast
  static unsigned long lastPress = 0;
  if (millis() - lastPress < 200) return; 

  if (bUp.isPressed()) {
    lastPress = millis();
    if (currentState == STATE_MENU) {
      currentMenuIndex--;
      if (currentMenuIndex < 0) currentMenuIndex = menuLength - 1;
    }
  }

  if (bDown.isPressed()) {
    lastPress = millis();
    if (currentState == STATE_MENU) {
      currentMenuIndex++;
      if (currentMenuIndex >= menuLength) currentMenuIndex = 0;
    }
  }

  if (bSelect.isPressed()) {
    lastPress = millis();
    if (currentState == STATE_WATCH_FACE) {
      currentState = STATE_MENU; // Enter Menu
    } else if (currentState == STATE_MENU) {
      executeMenuAction();       // Select Item
    } else if (currentState == STATE_STOPWATCH) {
      if (stopwatchRunning) {
        elapsedStopwatch += millis() - stopwatchStart;
        stopwatchRunning = false;
      } else {
        stopwatchStart = millis();
        stopwatchRunning = true;
      }
    }
  }

  if (bBack.isPressed()) {
    lastPress = millis();
    if (currentState != STATE_WATCH_FACE) {
      currentState = STATE_WATCH_FACE; // Always return home
    }
  }
}

void executeMenuAction() {
  switch (currentMenuIndex) {
    case 0: currentState = STATE_STOPWATCH; break;
    case 1: currentState = STATE_INFO; break;
    case 2: // Force Sync
       configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
       currentState = STATE_WATCH_FACE; 
       break;
  }
}

// =================================================================
// DRAWING FUNCTIONS
// =================================================================

void drawWatchFace() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    display.setCursor(0,0);
    display.print("No Time Sync");
    return;
  }

  // Draw Time
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(10, 20);
  display.printf("%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);

  // Draw Seconds (Small)
  display.setTextSize(1);
  display.setCursor(105, 35);
  display.printf(":%02d", timeinfo.tm_sec);

  // Draw Date
  display.setCursor(20, 50);
  display.printf("%02d/%02d/%d", timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
}

void drawMenu() {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("--- MENU ---");

  for (int i = 0; i < menuLength; i++) {
    int yPos = 15 + (i * 12);
    display.setCursor(10, yPos);
    
    if (i == currentMenuIndex) {
      display.print("> "); // Cursor
    } else {
      display.print("  ");
    }
    display.print(menuItems[i]);
  }
}

void drawStopwatch() {
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("STOPWATCH");
  
  display.setTextSize(2);
  display.setCursor(10, 25);
  
  unsigned long currentDisplayTime = elapsedStopwatch;
  if (stopwatchRunning) {
    currentDisplayTime += (millis() - stopwatchStart);
  }
  
  float seconds = currentDisplayTime / 1000.0;
  display.print(seconds, 1);
  display.print(" s");

  display.setTextSize(1);
  display.setCursor(0, 55);
  display.print("[SEL] Start/Stop");
}

void drawInfo() {
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("SYSTEM INFO");
  
  display.setCursor(0, 15);
  display.print("IP: ");
  display.println(WiFi.localIP());
  
  display.setCursor(0, 25);
  display.print("WiFi: ");
  display.println(WiFi.status() == WL_CONNECTED ? "OK" : "Disc");
  
  display.setCursor(0, 45);
  display.println("[BACK] to Exit");
}void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
