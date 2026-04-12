int trig=D2;
int echo=D3;
long timeu;
long dcm;

void setup() {
  Serial.begin(9600);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
}

void loop() {
  digitalWrite(trig,LOW);
  delayMicroseconds(2);
  digitalWrite(trig,HIGH);
  delayMicroseconds(10);
  digitalWrite(trig,LOW);
  timeu = pulseIn(echo, HIGH);
  dcm = (timeu/29)/2;
  Serial.println(dcm);
  delay(100);
}