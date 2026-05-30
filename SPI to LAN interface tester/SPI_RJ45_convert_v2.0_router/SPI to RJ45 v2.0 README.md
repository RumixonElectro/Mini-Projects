# **SPI to RJ45 Router/Cable Tester Firmware (v2.0)**

A diagnostic testing firmware designed to continuously monitor both the physical SPI chip connection and the active RJ45 cable link status of a W5500 Ethernet micro module.

## **🔌 Hardware Wiring Diagram**

| Peripheral / Module | Pin (W5500 Board) | Target Arduino Pin | Description |
| :---- | :---- | :---- | :---- |
| **W5500 Transceiver** | VCC | 3V3 | Power Supply |
|  | GND | GND | Common Ground |
|  | SCS / CS | D10 | SPI Chip Select |
|  | MOSI | D11 | SPI Master Out Slave In |
|  | MISO | D12 | SPI Master In Slave Out |
|  | SCLK | D13 | SPI Clock |
| **LCD Display (I2C)** | SDA | A4 | I2C Data Line |
|  | SCL | A5 | I2C Clock Line |

## **🚦 Verification Sequence**

The system continuously loops through two key validation tests:

### **1\. SPI Hardware Test**

* Checks if the W5500 is connected to the Arduino.  
* **On Failure:** LCD displays SPI: FAILED / No W5500 Found and halts program execution.  
* **On Success:** Prints SPI: OK.

### **2\. Physical Link Status Test**

* Evaluates active cable connectivity (whether the RJ45 port is plugged into a router/switch).  
* **Display Output:**  
  * LAN: NO CABLE (No active link detected).  
  * LAN: LINK OK (Physical connection established).

## **📂 Code Architecture**

* **Continuous Monitoring Loop:** Unlike previous versions, the loop dynamically checks both the SPI connection and cable presence every second.  
* **Ethernet.linkStatus() API:** Leverages W5500 registers to determine actual copper transceiver link activity in real time.