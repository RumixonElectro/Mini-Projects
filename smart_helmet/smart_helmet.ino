const int irPin = 3;      
const int btnPin = 2;     
const int mqPin = A0;     
const int ledAlc = 4;     // LED 1
const int ledHel = 7;     // LED 2
const int ledStart = 10;  // LED 3
const int relayPin = 8;   

int threshold = 660;      
bool bikeStarted = false; 
bool lastBtnState = HIGH; 

void setup() {
  Serial.begin(9600);
  
  pinMode(irPin, INPUT);
  pinMode(btnPin, INPUT_PULLUP);
  pinMode(ledAlc, OUTPUT);
  pinMode(ledHel, OUTPUT);
  pinMode(ledStart, OUTPUT);
  
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH); // Relay OFF
}

void loop() {
  // 1. READ SENSORS
  bool helmetOn = (digitalRead(irPin) == LOW);
  int alcoholValue = analogRead(mqPin);
  bool isDrunk = (alcoholValue > threshold);
  bool currentBtnState = digitalRead(btnPin);

  // 2. UPDATE STATUS LEDS (LED 1 & 2) - Must be outside any 'if'
  digitalWrite(ledAlc, isDrunk ? HIGH : LOW);   // Alcohol Status
  digitalWrite(ledHel, !helmetOn ? HIGH : LOW); // Helmet Status

  // 3. TOGGLE BUTTON LOGIC (Start/Stop)
  if (lastBtnState == HIGH && currentBtnState == LOW) {
    if (!bikeStarted) {
      // Condition to Start
      if (helmetOn && !isDrunk) {
        bikeStarted = true;
      }
    } else {
      // Stop the bike
      bikeStarted = false;
    }
    delay(100); // Debounce
  }
  lastBtnState = currentBtnState;

  // 4. AUTO-KILL SAFETY
  if (!helmetOn || isDrunk) {
    bikeStarted = false;
  }

  // 5. PHYSICAL OUTPUT (Relay & LED 3)
  if (bikeStarted) {
    digitalWrite(relayPin, LOW);   // Active LOW Relay ON
    digitalWrite(ledStart, HIGH);  // Bike Status LED ON
  } else {
    digitalWrite(relayPin, HIGH);  // Relay OFF
    digitalWrite(ledStart, LOW);   // Bike Status LED OFF
  }

  // 6. DEBUGGING
  Serial.print("H: "); Serial.print(helmetOn ? "OK" : "NO");
  Serial.print(" | Alc: "); Serial.print(alcoholValue);
  Serial.print(" | Bike: "); Serial.println(bikeStarted ? "ON" : "OFF");

  //7. SERVER PRINTING
  // Add this inside your loop() where you do Serial Printing
  Serial.print("{");
  Serial.print("\"helmet\":"); Serial.print(helmetOn ? "true" : "false");
  Serial.print(",\"alcohol\":"); Serial.print(alcoholValue);
  Serial.print(",\"bike\":"); Serial.print(bikeStarted ? "true" : "false");
  Serial.println("}");

  delay(20); 
}