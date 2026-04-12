void setup() {
  Serial.begin(9600);
  pinMode(3, INPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
}


void loop() {
  int F = digitalRead(3);
  if(F==0)
  {
    digitalWrite(4, LOW);
    digitalWrite(5, HIGH);
  }
  else{
    digitalWrite(4, HIGH);
    digitalWrite(5, LOW);
  }
  Serial.println(F);
  delay(500);
}

