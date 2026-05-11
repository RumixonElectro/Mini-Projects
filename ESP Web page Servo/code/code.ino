#include <WiFi.h>
#include <ESP32Servo.h>

const int numServos = 6;
Servo joints[numServos];
int pins[numServos] = {13, 12, 14, 27, 26, 25}; 

const char* ssid     = "Airtel_balu_2470_ex";
const char* password = "air38583";

WiFiServer server(80);
String header;

void setup() {
  Serial.begin(115200);

  // ESP32 Timer allocation
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  for (int i = 0; i < numServos; i++) {
    joints[i].setPeriodHertz(50);
    joints[i].attach(pins[i], 500, 2400);
    joints[i].write(90);
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  
  Serial.println("\nWiFi connected.");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    String currentLine = "";
    unsigned long currentTime = millis();
    unsigned long previousTime = currentTime;
    
    while (client.connected() && currentTime - previousTime <= 2000) {
      currentTime = millis();
      if (client.available()) {
        char c = client.read();
        header += c;
        if (c == '\n') {
          if (currentLine.length() == 0) {
            // Send Headers
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // Web Page Content
            client.println("<!DOCTYPE html><html><head>");
            client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<style>body{text-align:center;font-family:sans-serif;background:#222;color:white;} .s{width:80%;height:25px;}</style>");
            client.println("</head><body><h1>Arm Control</h1>");

            // Generate 6 Sliders
            for(int i=0; i<6; i++) {
              client.print("<div>Joint "); client.print(i);
              client.print(": <span id=\"v"); client.print(i); client.print("\">90</span><br>");
              client.print("<input type=\"range\" min=\"0\" max=\"180\" value=\"90\" class=\"s\" ");
              client.print("oninput=\"move("); client.print(i); client.print(",this.value)\"></div><br>");
            }

            // JavaScript (Pure JS - No jQuery needed)
            client.println("<script>function move(j,a){");
            client.println("document.getElementById('v'+j).innerHTML=a;");
            client.println("fetch('/?j='+j+'&a='+a);}</script>");
            client.println("</body></html>");

            // Parse Command: GET /?j=0&a=90
            if(header.indexOf("GET /?j=") >= 0) {
              int jIdx = header.indexOf("j=") + 2;
              int jVal = header.substring(jIdx, header.indexOf("&")).toInt();
              int aIdx = header.indexOf("a=") + 2;
              int aVal = header.substring(aIdx, header.indexOf(" ", aIdx)).toInt();
              
              if(jVal >= 0 && jVal < 6) {
                joints[jVal].write(aVal);
                Serial.printf("Moving Joint %d to %d\n", jVal, aVal);
              }
            }
            break;
          } else { currentLine = ""; }
        } else if (c != '\r') { currentLine += c; }
      }
    }
    header = "";
    client.stop();
  }
}