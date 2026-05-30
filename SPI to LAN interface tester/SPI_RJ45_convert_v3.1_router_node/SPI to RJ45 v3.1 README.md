# **SPI to RJ45 Auto-Mesh Node Firmware (v3.1)**

An intelligent, self-configuring peer-to-peer mesh networking firmware. It allows any number of test nodes to dynamically discover each other on a local switch, auto-negotiate IP addresses, and exchange test payloads via filtered UDP broadcasts.

## **🔌 Hardware Wiring Diagram**

Wire each test node according to this standard schematic:

| Peripheral / Module | Pin (W5500 Board) | Target Arduino Pin | Description |
| :---- | :---- | :---- | :---- |
| **W5500 Transceiver** | VCC | 3V3 | Power Supply (![][image1]) |
|  | GND | GND | Common Ground |
|  | SCS / CS | D10 | SPI Chip Select |
|  | MOSI | D11 | SPI Master Out Slave In |
|  | MISO | D12 | SPI Master In Slave Out |
|  | SCLK | D13 | SPI Clock |
| **Test Button** | Button | D2 | Mesh Broadcast Trigger |
| **LCD Display (I2C)** | SDA | A4 | I2C Data Line |
|  | SCL | A5 | I2C Clock Line |

## **🚦 Dual-Protocol Boot & Mesh Workflow**

Each node independently boots and establishes its presence within the mesh dynamically:

\[ Power ON \] ──\> \[ Random MAC Generated \]  
                        │  
                        ▼  
             \[ Attempt DHCP Lease \]  
             /                    \\  
     (Success)                    (Fails)  
        /                          \\  
       ▼                            ▼  
\[ Dynamic IP Assigned \]      \[ Random Fallback Static IP \]  
                             (e.g., 192.168.1.137)  
       \\                            /  
        ▼                          ▼  
      \[ Bind UDP Port 8888: Mesh Ready \]  
        │  
        ├─\> \[ Button Pressed \] ──\> Broadcasts "TG\_NODE:Ethernet test OK" to 255.255.255.255  
        │  
        └─\> \[ Packet Received \] ─\> Signature Matches "TG\_NODE:"?   
                                    ├── Yes ──\> LCD shows clean message for 4 seconds  
                                    └── No  ──\> Ignores background network noise

### **1\. Dynamic Identity & Auto-Negotiation**

* **Automated Seed:** Uses atmospheric noise harvested from floating analog pins A0 and A1 to formulate a unique MAC address on boot.  
* **Smart IP Allocation:** First requests an IP from the router via DHCP. If no router is present (e.g., on an unmanaged local switch), it automatically falls back to static IP routing $192.168.1.x$, where $x$ is randomized between ![][image2] and ![][image3] to avoid address collisions.

### **2\. Multi-Node Mesh Broadcasting (TX)**

* Pressing the button on D2 triggers a global subnet broadcast targeted at $255.255.255.255$.  
* This delivers the diagnostic packet to every node connected to the active switch segment simultaneously.

### **3\. Signature Filtering (RX)**

* Prevents raw network noise from disrupting the tester LCD by verifying that incoming packets start with the header signature: TG\_NODE:.  
* When verified, the payload message is displayed on the LCD for ![][image4] before returning to the active idle monitoring state.

## **📂 Code Architecture**

* **Collision Minimization:** No hardcoded IPs or MACs. Each node determines its position on the network completely at run-time, making it an entirely scalable mesh test environment.  
* **Low-Latency Listening:** The UDP buffer parser automatically drops foreign background computer or router chatter without freezing the visual interface.

[image1]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACkAAAAWCAYAAABdTLWOAAADAUlEQVR4Xu2VO2hUQRSG72KEiC+Crks2u3t3N4trQFDZQiIiIj5ioYVYRCwtFAmKWAiCEtHGQgLRShQVCT4KLcRoGbQJBiIWIaCmUHxUGhQUFTbx+/fODbPjzWbtUuwPP3fmPGbOnDlnruc10MA8RaFQWJbJZI77vn81m82ey+fzqxHHXLso4NMKz8gXnmxvb0+7NjbYpw+7Mpy2+CCRSCy2bI6Fuoogl8utYzJEcFtRtvA9zPyPNvTmCBT7PdgNag0OGmd8Hv6C+1xbG6VSaSH73DeBRNoi3wRHw0k/LKfT6b2aK1DmI/AL7KjytMAmzegfw0m4QTJ884w/w7FkMrnS9bGBf7cJ8g7Tpln0lysTBpeM8SHNi8XiUsbP4XdlqMrTggnyISzrFiRra2tLMf8AJ/BNOC5VkB67cWOfc9RNyK5z6M2VmVJvTr1AczK61g+yMxSPx5fYni4UKIGt8ExZMO/Cbwpe8yKy4yCG3RU4rRKzFcg64CN6Y7ktr0ANhHKAE7zHcb2rrwVlEZ9n+I9o7OqjwD7bdSi+T1Op1KJQjqwH2Snb1lNnIbyr4DCYYLNdnsnsXKCbV/lBeejaRk2J1Gy4EMoUPsNwUjcomYJVosJ5JAhwDUaf4G37WagH+GyDP1mjV2Xk6qMgWz+48l7NGXcyvqdSckyroFoZMI5HXGUtWE1X5lZ2uPooWD0wrMzid5Z9u2cMzHt1QrRPrnowQd6aMXagBU0WDnrW9cpHvv/U1CzQ9aom8ZnC9wDfJ77d7ShLCH6IGofycCPYH8oUFKdOeiagTPCQy+YdbJVMrwHjIXPAqo6tBR3UrPWRdfs8u6b1C0PxBsXNsN31pLDBC996pCVj/Ar+hp2SmQN+hRfCWyArBT+o5/F6O1zAPucHjadsdrl6GeyGb1FeVC34wV/km+ShjcnQIPrXdK9vxKrdo3AMnvaDbLw0a/3X8wViHPoGviN8W1xlBSzanA3+3fsJYku9nSnoBvDZKV9TMnU9XS50eGLY6MobaGC+4S8p5NckReOqbQAAAABJRU5ErkJggg==>

[image2]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABUAAAAYCAYAAAAVibZIAAABsklEQVR4Xu2TTyhEURTG34QiStI0ad7Mmz9KVtSIDWUhSwkLmY1sbGyYIkqRLNix0uwkOwubWYjFrM3exkKk7CwUu2n8vpl3eWYesbCbr77eOd/5zrn3vXufZdXxb3AcpwtuwmwsFtu2bbu72iPE4/EePPvywTl8LdWeMjAOYriMRCIjxH3EOViCGcoB4yOfhjcs2h8MBtuId9SXSCTaPeMsSytFo9FzigukDdLC4XAnjddor9RS0pLJZIT8FqZNL7UO8gJcMloZTuW17+GLdunRN5zKblfcPO1dxEUA7RTmtfMPNZVKNbGrA8wXWsDo5Gsaqqdy4kOfoRa9x+hP6Amv7odGjGewSNOoBLf5u6E1eg0wDskIs3oT91Dyfs2/GqqTxHQFT0KhUKs0PV2tpvnHodoRhSNM65Z7CzD3cnjjbrzr1+wOfeTG2F5d0ClmMCwrNiL5IvqUYu7wBHGRoWOeerNTudM5xUYXAgjzFN60Ik0PhuTPPIdlMncXbplG/XXqQZs1WhnO5z0t+fDLVWG3A2h3aKtwhrjAwD19Ou/MP0OHpu/MsEn9ZdX1Our4B7wDvjuBV76XvUIAAAAASUVORK5CYII=>

[image3]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAB8AAAAZCAYAAADJ9/UkAAACbElEQVR4Xu2VO2iUQRDHvw8jKFFE5DzCPfYe4pFK5TDBoMFCBBFFfCDoFT4KxU6vEAMpQkhjoUQDhiCIWAhqYZNCFLRTDKTyUVkYhFRJIUZE0PM33k5ussmhpLDxBv7szn/+M7M7t/kSRS37b8051wH6wVgulxtIp9ObQk02mz1GvCeRSKzBjYvF4ka4CvqtVkfuanQnpBa4ms/nSza+wAh2IXqayWR2sd/CfhzUQJVw7GVtNHngeYuHhUJhndaSvdQCg3JIORj7d+CIauZNTsnpHxM8g7tCuFQqtYGk13BzxMqqxb8N3sBNsT4C+zVHjdhl+AnW9crhnwTvuVjSanXcH8FnubXh+1z9ZpcMd9MeJjRpKI05+F3LM9Ht8F9YD1o+KpfLKxEPk/hEDqK8v0FNVuX+1Jx4J5gJm0sO/BwYsnwza3P1sf6g0G4l8UcodJ11EnwCL8E2jWuTZs1DfklD1O1POiaTUZ4id4hdifzvjF9BMysP1vsH8Gthk79u7l/rM3AvmUy221ipVFobmQfGw0z7CdyP6tPat+zmckuEowivyV9BGA/NNR7rB3nJzZo04+dNG9uxktBJ0b2yhz+N/5P1vOaY5oIO8gus02ET8+D6LK8WE6iSdFH2SuKfgz8s+6x//ba5GfsL+aAIZA/G0a1SHbl74L7LqpxajPAUwa9SCMGUAn+WdaeI2O8AI/YBkncc7pszXy/0FZ+b95RcbBC8sl/C3+Yao6stgWkZpSlSBc/BWXADzIALEtN6/ue7JToOd8jVG7/NBd//ZRn/TDJSVN7Cops0LCa2mUMcRddrp9WylrXsn9svvEjHa6LYotQAAAAASUVORK5CYII=>

[image4]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAFMAAAAZCAYAAABNcRIKAAAEhklEQVR4Xu2YW2iURxTHN6wFLy31FlNz2dlcMF5aaom9oiJSi2IjoogFRR8V6Yspaan0paJIKUFrfUpbgg8iqC9iBcGCgqI2gk8RQSzFUpS2EFFqEKSmv//Oma/DrtldSXxJvz/82W/Obc6cb+bMl2QyKVKkSDH+kc/nJ7a2ts7iMVusGzVaWlpezuVyfY2Nja8V68YTVEDn3AAchrfh7GKb0aKGN/U5gR9S0I5i5TiE1vvVcykmgd8m6OD/qJgZ1vnZmBdTx5uA38HetJijQw3BuuB6C15NMWt4AXPYzdvgVnzmwfdiA/puG3E+Rb5Dz7HOkNU8+H8pm+bm5teR1cQGDQ0NM7DZgH6X5ZRcFPhNxcch7yR+gy4TuExj+UVhEli8Tvw+sPuhpJh1dXVTGK+Cn2C3tKmp6c22trbaOM6IwOEtHHs6OjpeqLKYKn4P3ENyjSokizgJD0upOMi+gFetWCt5/lXJyVc2yF5h/BM8rSIyXsvzY7MpzIHvFsVQgVQ08DU8K1+LsZPxPecvkb3Y9yH7iOeDcFDrsliFeMg+hn/ALrP7Hv7iomI6vykuon9HMuUAf6tQDw873r045zWuppgkWYfNNb2xILMkeux5HXwA3430hYXg224vrY/xDcWSXgVj/E+IgX4Fz/eYY0mIIT/sjiE/xU6cJJlyYDyE/Zkgsxf8OzwYfPHbqJzieKDkAnL+hB6RLhgx3l2uHgEKthPDDUFQTTHr6+tnYnMdDmC3Wclroe3t7S8RbyLy09LLLvhEi+5UbM0RLzbjd+I0frMjxRAsv8f8LrZxIRY+24KNCgNv5+2k1NbWvsj4vPMnRXMksHhJMRXH+Zf6rXa2fHXstb7YrwQ4vOHseAdZNcUUsFktO+ePmHiLRBYqKUvur5zffbrQEspGBZWP5iqOK9jO1/E7r8XEOstP83XZuFBMxQw2IYdQzDAuEy8ppp3U4zZHgdq9FYuJ0fac7wcJcf7bgvwJL9hfB0+FdhBcpsmwvQ8HeJ7r/K4t2QUBlYoZ7fySGFExN9l4TItp0IkVtiI/B5+oVpG+OljwsjtTEyvRODH1IuR3+F3E7yF4lxgtsZ9uf5hDp9tEPe0E4glBbz1xIY865kdloxbyX4TC3Hud76WvalxNMfXSXfm2EffMbmKvCfrQp0OsZwLBdsGh+HIphiV7PW7mWhwT9usThQQXOH/M92XsU8aS+sY+kdQf9ck0BJdHMfRC9kjvfAsaxG5z0OuzRnPAA7Ixn0IvhuuCneWXFFPI+QvtEbKNQUYu05FdcNGLV3FVvOhYK5dDkge/isD4feeP9rBRTfjS0465JXsJXnW+v/wAr8FVwca+Qfud//tX/VIXQKK3z6du+AC7k87vxF71rGBDTvORXYY/2hw6+t1hoej3O5/nsFG5KGZoVep3N+3/DHqBHzp/Io6r0Pyesl/ZKs4O+WP3M79XnO/xyu1InNdYI8txmawHJpta7r8u0utCGamBSy697Ip1AZViPCOyyjfMp1YV52/ryoa8intsihQpUqRIkSJFior4FyTbnNK7Ji3VAAAAAElFTkSuQmCC>