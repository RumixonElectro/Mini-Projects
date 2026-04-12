Mini Projects: Electronics, IoT & Embedded Systems
A collection of functional prototypes and R&D projects focusing on Industrial IoT, Embedded Systems, and Hardware-Software Integration. This repository includes source code, circuit diagrams, and simulation logic for various microcontroller-based applications.

🚀 Key Project Categories
🌐 Internet of Things (IoT)
Environmental Monitoring: ESP8266-based systems using DHT11, MQ135, and INA226 sensors for real-time data logging.

Web Dashboards: Integration of Web Serial API for browser-based hardware monitoring and real-time simulators.

🤖 Embedded Electronics
Industrial Protection: Three-phase power monitoring systems with overload and Line-to-Line (L-L) fault detection using ACS712 sensors.

Biometrics & Security: ATM prototypes and enrollment systems utilizing R307 fingerprint sensors and RFID (RC522).

Control Logic: Timer-based relay systems and capacitive touch interfaces (MPR121).

🚦 Smart Infrastructure
Emergency Priority Traffic Systems: Smart traffic lights with RFID-based ambulance detection and sirens.

Safety Tech: Public safety prototypes involving sensor-integrated poles and emergency signaling.

🛠 Tech Stack
Microcontrollers: Arduino Nano, ESP8266 (NodeMCU/ESP-01), STM32 (Blue Pill).

Sensors/Modules: ACS712, DHT11, MQ135, INA226, RC522, R307, MPR121, I2C LCDs.

Languages: C++ (Arduino IDE), HTML/JavaScript (Web Serial API), XeLaTeX (Documentation).

Tools: Serial Monitor, Web-based Simulators, Relay Modules.

📁 Repository Structure
Plaintext
├── IoT_Monitoring/         # ESP8266 & Sensor integration
├── Industrial_Protection/  # Fault detection & Relay logic
├── Smart_Traffic/          # RFID & Web Serial simulation
├── Biometrics/             # Fingerprint & Security projects
└── Documentation/          # Project reports and circuit diagrams
📝 How to Use
Navigate to the specific project folder.

Open the .ino file in the Arduino IDE.

Install necessary libraries (e.g., DHT, Adafruit_MPR121, LiquidCrystal_I2C).

Flash the code to your specific microcontroller (Nano/ESP8266/STM32).

Refer to the circuit notes within the folder for wiring.

🤝 Contributions
Feel free to fork this repository, report issues, or submit pull requests for new hardware implementations or optimizations.
