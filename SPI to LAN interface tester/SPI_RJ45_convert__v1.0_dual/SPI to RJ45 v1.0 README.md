# **SPI to RJ45 Conversion Test Firmware (v1.0)**

A lightweight peer-to-peer validation firmware designed to test data communication between two Arduino Nano nodes using W5500 Ethernet modules. This code allows two matching hardware setups to exchange UDP strings over an Ethernet link to verify physical layer integrity, wiring, and module performance.

## **🔌 Hardware Wiring Diagram**

Each node requires an identical hardware configuration. Wire your components to the Arduino Nano according to the table below:

| Peripheral / Module | Pin (W5500 Board) | Target Arduino Pin | Description |
| :---- | :---- | :---- | :---- |
| **W5500 Transceiver** | VCC | 3V3 or 5V (Match module specs) | Power Supply |
|  | GND | GND | Common System Ground |
|  | SCS / CS | D10 | SPI Chip Select |
|  | MOSI | D11 | SPI Master Out Slave In |
|  | MISO | D12 | SPI Master In Slave Out |
|  | SCLK | D13 | SPI Clock |
| **Tactile Button** | Pin 1 | D2 | Transmit Action Button |
|  | Pin 2 | GND | Ground Link |
| **LCD Display (I2C)** | SDA | A4 | I2C Data Line |
|  | SCL | A5 | I2C Clock Line |

## **🚦 Communication Workflow**

The system uses a direct cross-over mapping via Static IP routing to verify dual-channel transmission:

\[ Node 1 (IP: .1.10) \] ──(UDP Packet: Port 8888)──\> \[ Node 2 (IP: .1.11) \]  
\[ Node 1 (IP: .1.10) \] \<──(UDP Packet: Port 8888)── \[ Node 2 (IP: .1.11) \]

### **1\. Hardcoded ID Selection**

Before flashing your two hardware setups, you must toggle the compile-time token DEVICE\_ID inside the code:

* **Node 1 Setup:** Set \#define DEVICE\_ID 1 (Assigns IP 192.168.1.10 and targets .1.11).  
* **Node 2 Setup:** Set \#define DEVICE\_ID 2 (Assigns IP 192.168.1.11 and targets .1.10).

### **2\. Startup Diagnostics**

* On boot, the LCD prints Eth Node: \[1 or 2\].  
* The code checks if the Ethernet module is properly responding over the SPI interface. If missing, the screen locks on No Eth Hardware.  
* If successfully initialized, the LCD displays Ready to Test....

### **3\. Transmission (TX)**

* Pressing the button connected to D2 triggers an active-low switch read.  
* The system buffers a dedicated payload string: "Ethernet test OK".  
* The payload is shot across the network using EthernetUDP towards the target node's IP address on port 8888\.  
* The local display provides instant feedback printing TX Success\! or TX Failed.

### **4\. Reception (RX)**

* Both nodes constantly poll their internal network buffers for data packets using Udp.parsePacket().  
* When a valid packet arrives, the data is dumped into a buffer array and printed directly onto the LCD screen (RX Packet: Ethernet test OK).

## **📂 Code Architecture**

* **Conditional Directives (\#if DEVICE\_ID \== 1\)**: Used to prevent maintaining two separate project files. Changing a single digit dynamically remaps the local MAC address, local IP, and target IP mapping during compilation.  
* **Debounce Logic (delay(50))**: A minor blocking layout filter used to stabilize manual physical mechanical button bouncing during test runs.  
* **Dedicated Listening Matrix**: Operating loop checks for data every cycle, ensuring incoming strings clear out the UI layout dynamically to prioritize incoming telemetry.