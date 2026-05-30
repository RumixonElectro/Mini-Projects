# **SPI to RJ45 Router/Cable Tester Firmware (v2.1)**

An advanced, auto-recovering diagnostic testing firmware designed to continuously monitor both the physical SPI chip connection and the active RJ45 cable link status of a W5500 Ethernet micro module.

Unlike previous versions, v2.1 does not permanently lock or halt the system on error. It dynamically monitors and adapts in real time, automatically clearing error states and proceeding as soon as hardware issues are corrected.

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
| **Test Button** | Button | D2 | Active Low Input (Reserved) |

## **🚦 Verification Sequence & Dynamic State Machine**

The firmware executes a state-based loop that allows the testing process to run continuously on the assembly line:

                  ┌──────────────────────┐  
                  │   Power ON Boot     │  
                  └──────────┬───────────┘  
                             │  
                             ▼  
                 ┌───────────────────────┐  
         ┌──────\>│ Check SPI Connection  ├───────┐  
         │       └───────────┬───────────┘       │  
         │                   │ (Detected)        │ (Missing)  
         │                   ▼                   ▼  
         │       ┌───────────────────────┐   ┌───────────────────────┐  
         │       │ Initialize Network    │   │ LCD: "SPI: ERROR"     │  
         │       └───────────┬───────────┘   │      "CHIP NOT FOUND" │  
         │                   │               └───────────┬───────────┘  
         │                   ▼                           │  
         │       ┌───────────────────────┐               │ (Loop until  
         │       │ Check LAN Cable Link  │\<──────────────┘  SPI connected)  
         │       └───────────┬───────────┘  
         │                   │ (Connected)  
         │                   ▼  
         │       ┌───────────────────────┐   ┌───────────────────────┐  
         │       │ LCD: "SPI: OK"        │\<──┤ LCD: "SPI: OK"        │  
         │       │      "LAN: PASSED"    │   │      "LAN: DISCONN."  │  
         │       └───────────┬───────────┘   └───────────▲───────────┘  
         │                   │                           │ (Loop until  
         └───────────────────┴───────────────────────────┘  cable connected)  
                      (If SPI or LAN drops)

### **1\. SPI Hardware Verification**

* **Check:** Verifies physical contact with W5500 over SPI.  
* **On Failure:** Immediately resets network status, displays SPI: ERROR / CHIP NOT FOUND, and waits. Once the module is connected, it automatically initializes the network stack and proceeds.

### **2\. Network Initialization**

* **Check:** Boots network registers using generic static IP configuration (192.168.1.99) exactly once upon successful SPI discovery.

### **3\. LAN Link Verification**

* **Check:** Monitors physical connection to a switch/router.  
* **On Failure:** Displays SPI: OK / LAN: DISCONNECTED and polls the link. If the W5500 is pulled out during this state, it instantly breaks out and loops back to the SPI verification step.

### **4\. Consolidated Pass State**

* **Check:** Reaches the final diagnostic layer.  
* **On Success:** Displays SPI: OK / LAN: LINK PASSED and holds this state until a hardware cable drop or SPI disconnect occurs, resetting the loop.

## **📂 Code Architecture**

* **Non-Blocking Dynamic Looping:** Prevents the unit from needing hard resets between tests. Simply plug in a bad module to see the error, and plug in a good module to instantly see the pass screen.  
* **State Flag Preservation (isNetworkInitialized):** Tracks the TCP/IP stack configuration status, preventing redundant network initialization routines when the module link bounces.  
* **SPI Disconnect Safeguard:** The LAN link loop continuously polls hardwareStatus(). If the chip is suddenly disconnected, it breaks out of the cable loop and restarts SPI registration.