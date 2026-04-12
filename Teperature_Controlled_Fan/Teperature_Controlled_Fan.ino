const int tempSensorPin = 2; // HW-072 Digital Output (DO)
const int relayPin = 3;      // Relay Module

void setup() {
  pinMode(tempSensorPin, INPUT);
  pinMode(relayPin, OUTPUT);
  
  // Project: Send data to computer
  Serial.begin(9600); 
  
  digitalWrite(relayPin, LOW); // Start with relay OFF
}

void loop() {
  // Read the HW-072. Logic: LOW = Temperature above threshold
  int isHot = digitalRead(tempSensorPin);

  if (isHot == LOW) { 
    digitalWrite(relayPin, HIGH); // Turn Relay ON
    Serial.println("TEMP_HIGH");  // Send status to computer
  } else {
    digitalWrite(relayPin, LOW);  // Turn Relay OFF
    Serial.println("TEMP_NORMAL");
  }
  
  delay(200); // Prevent relay chatter near threshold
}