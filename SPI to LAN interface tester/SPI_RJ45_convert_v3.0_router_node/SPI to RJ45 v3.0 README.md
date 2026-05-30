# **W5500 Automated DHCP Test Jig Firmware (v3.0)**

Version 3.0 transitions the test platform from passive physical link monitoring (v2.x) to active network layer diagnostics. It introduces automated DHCP address allocation testing, randomized MAC address generation, and structured buzzer alert patterns to give immediate pass/fail feedback to line operators.

## **🔌 Hardware Wiring Diagram**

| Peripheral / Module | Pin (W5500 Board) | Target Arduino Pin | Description |
| :---- | :---- | :---- | :---- |
| **W5500 Transceiver** | VCC | 3V3 | Power Supply (![][image1]) |
|  | GND | GND | Common Ground |
|  | SCS / CS | D10 | SPI Chip Select |
|  | MOSI | D11 | SPI Master Out Slave In |
|  | MISO | D12 | SPI Master In Slave Out |
|  | SCLK | D13 | SPI Clock |
|  | RST | D3 | Hardware Reset Pin |
| **Active Buzzer** | \+ (Anode) | D4 | Diagnostic Piezo Buzzer |
| **Green LED** | \+ (Anode) | D2 | Success Indicator LED |
| **LCD Display (I2C)** | SDA | A4 | I2C Data Line |
|  | SCL | A5 | I2C Clock Line |

## **🚦 Diagnostic Sequence & Sensory Alarm States**

On power delivery, the firmware performs an active reset of the controller and runs through sequential validation phases with unique sensory profiles:

\[ Power ON \] ──\> \[ Boot Beep (500ms) \] ──\> \[ Hardware Reset (D3) \]  
                                                   │  
                                                   ▼  
\[ Halt (Solid Buzz) \] \<── \[ Fail: SPI Timeout \] \<── \[ Phase 1: SPI Check \]  
                                                   │ (Success)  
                                                   ▼  
\[ Loop (Slow Pulse) \] \<── \[ Fail: DHCP Timeout \] \<─ \[ Phase 2: DHCP Check \]  
                                                   │ (Success)  
                                                   ▼  
                                            \[ Success State \]  
                                      (Green LED ON \+ Double Beep)

### **1\. Startup Initialization**

* **Buzzer Feedback:** Emits a single, long power-on confirmation beep for exactly ![][image2].  
* **Hardware Action:** Pulls the physical W5500 reset pin (D3) LOW for ![][image3], then releases it to HIGH for ![][image4] to guarantee a clean chip boot.

### **2\. Phase 1: SPI Hardware Verification**

* **Check:** Queries internal W5500 registry status over the SPI bus.  
* **Timeout Window:** ![][image5].  
* **On Failure:** LCD prints SPI: FAILED / CHIP NOT FOUND. The buzzer turns **constantly HIGH** (solid tone) to signal a missing, unpowered, or shorted chip.

### **3\. Phase 2: Active DHCP Allocation Check**

* **Check:** Requests a dynamic IP lease from the local network gateway.  
* **Timeout Window:** ![][image5].  
* **On Failure:** LCD prints DHCP: FAILED / NO IP ASSIGNED. The buzzer sounds a slow, distinct pulsing alarm pattern (![][image6] **ON / ![][image7] OFF**) indefinitely.

### **4\. Initialization Pass State**

* **Indicator Actions:** The green status LED (pin D2) switches **HIGH** and the buzzer emits a dual-tone success chirp (![][image6] **beep / ![][image7] gap / ![][image6] beep**).  
* **LCD Display:** Clears and prints the newly leased DHCP IP address on line 1, and Module is OK on line 2\.

## **📂 Code Architecture**

* **Automated Collision Prevention (generateRandomMAC())**: Uses atmospheric electrical noise harvested from floating analog inputs A0 and A1 to seed the random number generator, ensuring every tested module gets a unique MAC address and doesn't conflict on the DHCP table.  
* **Hardware-Assisted Recovery**: Integrates a dedicated physical hardware reset pin cycle directly in the setup routine to eliminate software lockups on hot-plugged modules.  
* **Continuous UDP Mesh Listener**: Once verified, the controller remains active on UDP Port 8888, parsing broadcast packets looking for the target signature TG\_NODE: to handle live mesh feedback.

[image1]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACkAAAAWCAYAAABdTLWOAAADAUlEQVR4Xu2VO2hUQRSG72KEiC+Crks2u3t3N4trQFDZQiIiIj5ioYVYRCwtFAmKWAiCEtHGQgLRShQVCT4KLcRoGbQJBiIWIaCmUHxUGhQUFTbx+/fODbPjzWbtUuwPP3fmPGbOnDlnruc10MA8RaFQWJbJZI77vn81m82ey+fzqxHHXLso4NMKz8gXnmxvb0+7NjbYpw+7Mpy2+CCRSCy2bI6Fuoogl8utYzJEcFtRtvA9zPyPNvTmCBT7PdgNag0OGmd8Hv6C+1xbG6VSaSH73DeBRNoi3wRHw0k/LKfT6b2aK1DmI/AL7KjytMAmzegfw0m4QTJ884w/w7FkMrnS9bGBf7cJ8g7Tpln0lysTBpeM8SHNi8XiUsbP4XdlqMrTggnyISzrFiRra2tLMf8AJ/BNOC5VkB67cWOfc9RNyK5z6M2VmVJvTr1AczK61g+yMxSPx5fYni4UKIGt8ExZMO/Cbwpe8yKy4yCG3RU4rRKzFcg64CN6Y7ktr0ANhHKAE7zHcb2rrwVlEZ9n+I9o7OqjwD7bdSi+T1Op1KJQjqwH2Snb1lNnIbyr4DCYYLNdnsnsXKCbV/lBeejaRk2J1Gy4EMoUPsNwUjcomYJVosJ5JAhwDUaf4G37WagH+GyDP1mjV2Xk6qMgWz+48l7NGXcyvqdSckyroFoZMI5HXGUtWE1X5lZ2uPooWD0wrMzid5Z9u2cMzHt1QrRPrnowQd6aMXagBU0WDnrW9cpHvv/U1CzQ9aom8ZnC9wDfJ77d7ShLCH6IGofycCPYH8oUFKdOeiagTPCQy+YdbJVMrwHjIXPAqo6tBR3UrPWRdfs8u6b1C0PxBsXNsN31pLDBC996pCVj/Ar+hp2SmQN+hRfCWyArBT+o5/F6O1zAPucHjadsdrl6GeyGb1FeVC34wV/km+ShjcnQIPrXdK9vxKrdo3AMnvaDbLw0a/3X8wViHPoGviN8W1xlBSzanA3+3fsJYku9nSnoBvDZKV9TMnU9XS50eGLY6MobaGC+4S8p5NckReOqbQAAAABJRU5ErkJggg==>

[image2]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAD0AAAAZCAYAAACCXybJAAADlklEQVR4Xu1WTUiUQRh20cD+iIhtUXd3dtelpVPC9g91iiCiCOoQFXUoykMnwwI79IeH7KZC4c3CSx2iQ6cEJaPUDnXoDwoiMTzZKQ8Ktj3PN/Ou745uigRL8T3w8M08884788w338xXVRUiRIj/DinAGHOioaEhjmo1qrXpdHpLMpk8z7KOhZ5DbAfYwz7xeHylbieosc3FdLCPH1NxYGK7wWmwoDgFHvTijoIfsBBN0Wh0Dcq3wGeZTGadxLBMjW2MYSz7sK/OVXHgjebdxD6BbzHRG3jW6ZjGxsYEtM/gSdVvPeqvwYtKu0KNbaKxD/gRbzwmWsXhTHf5uoab+BRjlRyB1gcO8q3KImDRelVMVSKR2Ab9J56HtV5RLNF05wKmeR70Qp+AnsFzMzjpm3b5+bm0a13ABeN5gp2wnzny+fwKLNBW5DnCHebCIvh0NiHXMcagXq1zxGKx1dAPgJeQZy8XOpvNRnVMCdykHoP3wS+oj4HX9CHlzJUzHehirpxpXxfQHNq/G3uW8PB7YOzOusp+4DmntyC22cV2o2sN+xu72C/QthPPOox32nkomWsJ3IRHYDLLOlZ9AxKMciCuuju0BjkBP5Fn+hDKBd/cYqYJvmnEjBv17edyubWoD4HTeHN7JBb1dvAbDbp6C9iHYkTF3PTnWgIa4/bQGjq1cTBwl9s6/Zy4n0ibNnZ7Lcs0DTgjnaKpxR7iAoietIdl0TTyXkB5FuzCgm1nP86ZvqTPkuASF8AW1rU5Hee96QXNldM1xDTHFU2ZDg5K0X3T7pp85OYbEGPdLmuaW9nYa+cdDo2NootpmYSxW6qc6XFuz6Q9zCZ8c2IabNO6Bg2YZZp2iKQszkAfAH+h3Kza5yCDGc80JwgW5JrhE/VZDLhPYpC0FtpTkmU1yaAucewDbUb39SHzWI5pPFv1dcg3jPEf+ouvUYMk9xCwQwRuF9SfI1m//G3J4QZelzgefIgZh3ZcNOQ6BY5BTzuJdzn/3Ib1n5sPdZAVd8MipieNvbqCOk2q7cwxu/UCzgM+fsQEp+Qd8Cz4Bnzp/sWL4N0J/SuSXU7a+5I/IiXfDstou4u2gZS9imj4PcpNOpdGyl5DM2buexzl4qH8QzSWnfZKaTOubyvaRvAcBnugPcGz70+LHICT5aXuzMy7/AU8FfkTQUPqx8FH8UeCOcseKH8J9fX1q/Codh5ieleECBEiRIgQ/zB+Ax6OVGY9NBuCAAAAAElFTkSuQmCC>

[image3]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAADMAAAAZCAYAAACclhZ6AAADEUlEQVR4Xu2WT0hUURTGlTHoLxllg86ML8doEIKEKSKhaBFBiCEZIRm0CHTTooIK2lSUC4sIahNCRIEQFESLIMiFFAglWAuNKCEKsV0RWNBC7Pc599j1+UT7A9PiffBx7/nuOfee8+6fmZKSGDFi/DbWgSAIDqZSqTRmAnNxTU3Npurq6nb1fV/0HL6XYLdi0un0En+86CCpBvgDTnr8BhtDfi3wNQXWV1RULKd/AT7JZrMrfb+igh3IK0n4Br4i2fO0lb5PbW1tBu0dbPPiVmEPwKO+b1Hhirke1n2oCO2WfD25FK0H9mmnPL14WGAx1yKK0X27jf4JPevrBsbLuWcB403cr5TuINwpmzu62rklNC96s+4vdqk3RSnHeANyBzzMWnWwwRufCVfMA3gHjmB/hGf9y+2SnquYWbqB8eOMfwkK97ATv1tore7jfIYtaHdhO/2TmovxYy5cO38FXtTjRFvH2EOtOWMRH0oEPif59bL1xQh4QXB3Pp9f5C57X1TS8xUjZDKZLfh8x+exfSCX3Cgcs3VBGfZ9raU12dEk/UHF21wqSAWaPQtKOJlMLvM1As4EhRdum8Zoe6OSXkgxbuf1xTtMw66EH2BPiXes3HxTxVRVVa2hPwyHmOOQPoByzeVyK8x/QSD4dFA4Gidkz5X0XLoPK4a2yTQrJnxk/GKcX6NiXS7iCD71fsw0dKSCwvM6xPO71nQrRq1s+p1RSbvFR90PbiT+phinTT0asIuxr+Fcp2GThh2CwjGb5Lzula0We4KEdpmPFkF7JKpvehh/Wox8ZPuFkcd29LHwRzWUMXCDoK0m6Bcd+ylBvfbrbo8CPGd+urj4jKK1mhYFewDgPtPmKWbY3Rf5DKsAG6e/UXnomffjpqHfAfAMXoZH4EvYHz46TLQZ/T3Fn4L76Q8wcZcupe/nA7+r+E0Ev878vaDwBI+bxhxvadtca37jxB6g7Q8K10BxN+Eg3BNeZwaUEEXtcEnWISXCPoJeNvx2s3Cz/uKEx/8xEuzQUnVYr9xdg8i8YsSIESPGf4mflooKAIBAyKcAAAAASUVORK5CYII=>

[image4]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAD0AAAAZCAYAAACCXybJAAADn0lEQVR4Xu1XO2hUQRTdJRH8IiLLYrL7ZnezuKQysP4KFQsRLBRRC1FREcEUNiZEMSL4IYUpBGNASSchjVpokUIUEohg1FotLESJpEoKUcFAXM95M3e5GfOyGItFeQcuM3Pm3vfm3Jk3My+RiBEjxn8JY8w62GXYQC6Xu5rJZIq+D5HP50vw6aUf7Aj8lvk+5NjnfHoZ4/vUHRjUZgzuaTab3Y76BtSHYRVYJ7qT4of2QdhbJKUtlUqtRP064wqFwmrxYZ0c++hDX8YwVnzqDs5KEASPMKhTaDaQa25uXovBvgL3DX1lci0tLVm038OOSiz61qD9GnZWcRfIsU84xsDeIaFp4eoKY5f1R9gXzrLiu42d7Q7X5sCrSXBIghuCjXJWJQlI2D3lk8AK2gT+K8p9mq8byuXyEgzyFgb8hAkQ3s1YhSXbqPfNIzpBgeAnwRdQtsKmfNGMYSysR/MCJgyrK4Ok7+YzOCYkaCOes58rzLkl8emsx7MO0SfhVqUgnU6vAL8H1onn7GCii8ViSvvUQiOCH8Jm8eKdJJy4KNEhL+KiRPu8gOLQ/9nYlcXNb9DYlXWJcbDTju+Ab7vz7UdoI+ONTfZz9G1FuQ7vOw775I91QSB4i3vZALPuNq1Rcv6DPNF7Ua/44mqJJjjT8Jkw6tsvlUqr0B6D/eAmK75o9xj7SYYrE2UHbCgxd9O95o81Em73fQYb5JIh55YOuQVFG7u8FiWaApyQPuFUsseYAOED++lVReO5Z1Cfhd3mScQ4jpkTJjGRoBMeeBfBN/3zV4uL4qPERfEaIpqChFOiw41SeF+0m6gHxn4eoeFdN2qKFsFwvphwmwSCW7m5uDqXVJToCS7PwG5mk744EQ3r1rwGBVDIYkQ7JHMWJ8CPwH6i3q76fwOPnk44nWNdyJxdNgdY53GD+ixeuEv1LzX2IjPMuhpk2BY/xoCb0bE+KIBCFiMaZZc+Dt2JdN9PvgYzdBKB32ETgd31QkN7GuU2OsmFBXZFAnlVZQy4w8LB/5iLzTuKCeXNbVzf3Hyojay6GmqInjL26ArbFKmWM9/ZrxM4B8yWsVmrzGPh+Su+PDvBfQB3PrDnJS8ic74d95ncQd9Izh5FFPwG9Tbx8ZGzx9CMvJfJZfJQn1Zj4QSQe6G4GRfbhb6XKMeN/Xd4jHJooST/Ebgr8junIHVx8FG9SPCiUHND+Us0NTUtR9HA9/C406siRowYMWLE+IfxC378VmIczTeoAAAAAElFTkSuQmCC>

[image5]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAF0AAAAZCAYAAABTuCK5AAAFHElEQVR4Xu2YW2gdVRSGc0iFeKnXxJjb2ScXjaEKhXijXigi0qKVeAHjBfskERUfKrG0T9XahwoWsSISFOlDENuAFNGHGrS2orWF1kraglakEpQWRIT6oJLE78+sXVcnM0mqkUqYH35m9lpr1t77nz177XOqqgoUKFCgwFlGqamp6bK6uroL0o5/jUqC3rQ9orW1tTOE8BIcgA83Nzefm46Zb2CeT8EJsVwur077/xFI1gWfhB/DMUTfko4R8N0PD+NfrDfO/Xr4UVtb20Xp2PkGFteVzPXEnIqOkD1cl8DRLNHb29tb8H0LH4k2BnAJ7X3waR87H8EcG+CxORM9IibOEl1iw9/otNuZS9gG4c7/ZK/7H+Fsif5qhuiqAVuw/4S9zds96uvrzydmOXyWmnBbS0vL9R0dHXXRr7rA83eT63m4LKtOqIgR8yg5NsB7ef5C7+/u7j5HufGtJW6F+kz56uEN5F+qttWuHptPtUsVUS0ffICcXfTfnCF6ia31KvL0wZWKg0ucf2ZMJ7qJmyf6FHuEDeQz4m5SfuIegz/EePN/hX8NL6Od+9fhQQQKlqKkZ7Ad5/o411b4ITwiIVyOyW1ONnI9yP1heLv8UpdnPw1JMdzD/RtcV1ne7+CAXoT1pxesHHvhdthL3DPE7Ia/O9H1lb8MX7QXoi1a8VO0mxYhR3QrmjtDhrizEH0VHOS25GwvKN7lfQfzAvm0Imkf0YTUJv+Nyh+sbtCu4f49eFwnKWrN5dyPwPW+j3LyVehFLTJT3ArHybEsxtFeC3+GXWrrUIB/F9zqX0S6kNo49+urdbn08ifHPWuEHNFtexgOGeLOJDr+PvxjcLM+bwmtfJoQz9xhIvS5R07Vidra2oXEvB2cKJazJtYQyz+uXH+nmBS9W+OCG6LNxnqosbGx1sWt9uNHxHtC8kWcOjAIIbWnK4dywRG9YK12zamzs3Ohf25GxMRp0YU8cfPsEVo5+LfZRCbJMxtNdE1Y7R0hOfd79tvE9CUcgw3p3AL2N0NG/0704bi/21hPK/pp0eOYuK6IMYL61zii6Ga7y/qYMB6lj8X+uRkRE2eJHpICNmVyNpHRuL/moFRJsJLYT0Kyup+oJKtUgz1tVUW47Wc60VXg/2Rct3i7E32oyrauuRZdIGcNXAo34v8VjmjL8zHTIibW4NI+++zGyu4zVofYPhB17+Mj8PXr2di2k8NW9YH9Gvy/BLcFxBj6uZnbBcStCxmiqtCyzzbh75VI8D7vV7yeC+43xCxFz9rypoiutvL5XMznVuw/phfmtIiJQ6rwCTqyVZKKvi7amHRHSH5M5f5toEFKZFeUtGe/ZoPXF/CKBsr16viMBk97jfwSl/tviBmKR0nlshyLNGn8OzL60K9lv+pirdjt99206OqD9vvk2+V+aWucD2EfD7ZATKtDGmvMpUUkjbQYoi0X9nZHQ1LwJownSfA1Ca6NcSS9Dvv3xD9XTs6v+yq2P/t8HsT0E/sl1z1wgPjtXAfjhEzA/pAUy2343+X6lv9rwc7Ce7EflQ8OV9wJRGd2bJvhAfVRTo6HQ8RcIb/mEJKj4YTxZDk53XzhbH/ATRZ/qV4i7YPKF5KveRM8YbEHGJNyfh6So6pqlsa1Hy6P45ozqCix+u5kUD36ayDtT4NieB6X6vgDJe+Xa/ST9+K0L0I+xeS95Eqy3TXk9XGmUB7XX7W+Gpe72uY2OS77orJ+ZBUoUKBAgQIFChQocKb4Cy3Z08uHCm21AAAAAElFTkSuQmCC>

[image6]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAD0AAAAZCAYAAACCXybJAAADYklEQVR4Xu1WO2hUQRTNEgW/iMi6mP3M7mZxsRAi66/RSgQLRYiCqGgjmMLGSBRjI0oKAzYxIMRCRNJoZWGlkEBEo9ZqYSHKSkBIqigYiPGcnXvXu6MvwWixyjtweTNn7p2ZM3PfzLS0xIgR479HsVhck8vlbmcymc1hW6FQKDvn+mFDsKPwWR76kGOb+PQzJvRpNiTy+fxFTPYzhFdsA7hO2Gu0dySTyVUoX4U94iKpD8vk2EYf+jKGsbavpgImuQMTnApFt7e3Z8G9hR1TDu1rUX8JO2O4C+TYphxjYG+w4ynlmgayS7ecT8sG0TLxcPcT4IZho9xVXQQs3B3j05LNZreBn8b3gOWbARTQDeuU3QpFD4QcQYHgJ8AX8d0EmwxFM4axsD7LK7hg6XQ6g0zYyz4qlcpSLNBW9HOQGSZuCWzKRvR1iD6ot9o+UqnUSvD7YOfQz24udKlUSlqfnwDH7Qi4zgF/JVrERYmu8SouSnTIKygO7R9hc85n2V3nM+sS42CnhO+Gb5f4DiJ0CeOdX+wnaNuJ7waMdwL2IZxrAySthxCUZz0ULYfWqOUUgej9KM+F4hYSTXCn4VN15t8vl8urUR+DfcXO7VJf1Ptg7ylQ6szQYRQTxudKOFcLntZn4XBYiVC0pM7jhUQ7n16LEk0BImRAObPYY1wA5WV+ddHo9zTKs7AbzFjGcc7MWo1pABy3OElr5ULRhBWnXMhHiYviLVQ0x1bOiK4dlMqHoiVT7zv/e9QMY12LFI3GrpzP/7ohaFqCP8HGcJisdz6lokRXmZ45f5hNhOJUNKzX8hbuD0QLmLHESfAjsG/UZtrnh3TaIJDXDbhZcHuUQ6fLwD2ksWwmWaurH2PAzdjYEBRAIYsRjW+PvQ65wxj/Xrj48wKd9MK+8NhXDju5Dp28gF1WDs/NEvyq4I4ohwkdl2wpCMWrkC+3cftyC2EOsno2LCB60vmrq1anSJPOHHPQLmAkZEeY0nNiPByeMr3ZzrsT9XfwO5/z9yUfIg3/jlx5N9E2kvdXEQW/Qrnjx0iNYBrCZ0bH5eJy8Zx/GepcpoR7ZrgZie1B23N8x52/hR7gOzzfIv8WeCryEUFB5uEQov6Q4EMh8kD5S2hra1uBTyvH4XVnsyJGjBgxYsT4h/Ed5h5P2uIl3H4AAAAASUVORK5CYII=>

[image7]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAD0AAAAZCAYAAACCXybJAAADpElEQVR4Xu1XPWgUQRTeIwrxH9HjMLm72bscngFB5VRsFAtRLBTBFKKihYIpBDEhCtr4Q4oEbGJAsFLCgWhlYaWQQASj1mqhIko0VRRFCwPx/L7MvOVl4iYShKjsB4+d+ea9ufe9nZ2ZC4IECRL8lyiVSkvz+fwpY8z1MAwvFovF1aBTvl+hUCjDp5t+sIPZbHaB70OOY86nmzG+z5wDSa1DcgMQux3Cl+N5Av0xWHughKO/H/Yc4+vT6fRitC/D7qNAy8SHbXIcow99GcNY8fkrgIR6YOO5XG4v+xSO/lPYKKyZXFNTUw7tl7BDEqf8TiruLDmOCccY2AsUNyPcnAMJXYHVYMfYL5fLS9AehH3hKnA+TPwbxFRUaApcFTbAtypFwNu9qXwCFHMT+K9S1L8ClUplfkNDw0o069hHcmuR5CcRQ87Y1eCLDigQ/Aj4Ip7NsFFfNGMYC+vUvIC/0djYmEWBd3IO5oMcNmKefVxhzi3FfQZztdAncLkKMpnMIvC7Ye2YZxsLjX0qrX1iwQ0NgVVM/o7fo/BOXJzoCV7ExYn2eQHFYfy9sauNm1+fsSvrPONgxx3fBt9W59uL0HmMN7bYDzG2Bc9V+L0jzN/PdQpYKTjdojMCX2OCXYGrptu0BpiAP5Eneg/aNV/cTKIJvmn4DBv17avP7Dve3FbxRb8T9pYCXb8NVg0mb7qX/FynBZJbg6APsD4Wwy2dByLO841EG7u8ZiWaApyQHuFUsQdZAOHzdrOMRIf2tBmHXUXBNjOOOfMzkZjfgWxQFNBKQovTjt6b/qW4OF5DRFOQcEp0tLcQvmh3TN5hvmL4ra5Y0RyAw2madnITR2/N2CUVJ3qYyzNvN7MRX5yIhp3TvAYFUMhsRDukQouj4PthP0L3wqZAJTRJkBNTM2658bhBexw+O5RPPbh7NLZVkhN98WMMuDEd64MCzCxF49mhj0P3Im/7xY8glw5MdENuVnhrKxDwxNhja4PmYBckFtfNEsaHwR0QDvMcztvNsOAofiq8uQ3pm5sPtZFFq2EG0dHFiX2KVCuVv9mrCzgFxm5ArxDYRQHGvr3P5LUfz05wbzDZmbw9L3kRmfTtsI2xaxjrD+1RRMHPQnX8+QjtMcRrb43G4rJ4aH8Ujm3HPVLcmIvtwNhjPIeM/e9wF8/qdEWeABzrQ3v3buHhHrcJcFfkJYKC1MXBR3SRmG6uPwVcrBbiUcff4XGnV0WCBAkSJEjwD+MnJd5Z12Ma7A4AAAAASUVORK5CYII=>