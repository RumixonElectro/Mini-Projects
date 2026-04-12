const int SENSOR_PIN = 2;
const int RELAY_PIN = 13;
volatile long pulseCount = 0;

void pulseCounter() {
  pulseCount++;
}

void setup() {
  Serial.begin(9600);
  pinMode(SENSOR_PIN, INPUT_PULLUP);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), pulseCounter, FALLING);
}

void loop() {
  pulseCount = 0;
  delay(500);

  if (pulseCount > 2) {
    digitalWrite(RELAY_PIN, LOW);
    Serial.println("Water is flowing");
  } else {
    digitalWrite(RELAY_PIN, HIGH);
    Serial.println("No water flow");
  }
}