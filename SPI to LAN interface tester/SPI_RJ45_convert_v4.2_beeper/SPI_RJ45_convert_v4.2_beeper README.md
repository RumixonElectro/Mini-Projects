# **SPI\_RJ45\_convert\_v4.2\_beeper**

A high-speed, sequential hardware validation fixture designed to diagnose W5500 Ethernet modules. This version features a single global timeout configuration, a standardized ![][image1] beep/gap alarm array, and finite fail-safe alerting loops instead of infinite buzzes.

## **🔌 Hardware Wiring Diagram**

| Peripheral / Module | Pin (W5500 Board) | Target Arduino Pin | Description |
| :---- | :---- | :---- | :---- |
| **W5500 Transceiver** | VCC | 3V3 | Power Supply (![][image2]) |
|  | GND | GND | Common Ground |
|  | SCS / CS | D10 | SPI Chip Select |
|  | MOSI | D11 | SPI Master Out Slave In |
|  | MISO | D12 | SPI Master In Slave Out |
|  | SCLK | D13 | SPI Clock |
|  | RST | D3 | Hardware Reset Control Pin |
| **Active Buzzer** | \+ (Anode) | D4 | Active-High Diagnostic Buzzer |
| **Green LED** | \+ (Anode) | D2 | Success Indicator LED |
| **LCD Display (I2C)** | SDA | A4 | I2C Data Line |
|  | SCL | A5 | I2C Clock Line |

## **🚦 Sequential Testing Pipeline & Sensory Alarm States**

\[ Power ON \] ──\> \[ Boot Beep (100ms) \] ──\> \[ Hardware Reset (D3) \]  
                                                   │  
                                                   ▼  
\[ Halt (5 Beeps) \] \<─── \[ Fail: SPI Timeout \] \<─── \[ Phase 1: SPI Check \]  
                                                   │ (Success)  
                                                   ▼  
\[ Halt (2 Beeps) \] \<─── \[ Fail: DHCP Timeout \] \<── \[ Phase 2: DHCP Check \]  
                                                   │ (Success)  
                                                   ▼  
                                            \[ Success State \]  
                                      (Green LED ON \+ Double Beep)

### **1\. Initial Power Verification**

* **Buzzer Feedback:** Emits a single, ultra-fast boot beep for exactly ![][image1].  
* **LCD Display:** V4 Jig Booting...  
* **Hardware Action:** Pulls reset pin (D3) LOW for ![][image3], then releases it to HIGH for ![][image4] to cleanly reboot the W5500 controller.

### **2\. Phase 1: SPI Hardware Verification (Step 1\)**

* **Initialization Resolution:** Boots the SPI registers using a dummy static IP address first (192.168.1.200) to wake up the bus.  
* **Timeout Window:** Defined by the global variable TIMEOUT\_MS (![][image5]).  
* **On Failure:** LCD prints SPI: FAILED / CHIP NOT FOUND. The buzzer sounds exactly **5 times** (![][image1] beep / ![][image1] gap) and halts execution.

### **3\. Phase 2: Active DHCP Allocation Check (Step 2\)**

* **Timeout Configuration:** Calls Ethernet.begin(mac, TIMEOUT\_MS, responseTimeout). The response timeout is automatically computed as half of the global timeout limit (minimum ![][image6]).  
* **On Failure:** LCD prints DHCP: FAILED / NO IP ASSIGNED. The buzzer sounds exactly **2 times** (![][image1] beep / ![][image1] gap) and halts execution.

### **4\. Initialization Validation (Success State)**

* **Visual Action:** The green success indicator LED (pin D2) switches **HIGH**.  
* **Buzzer Action:** Sounds a rapid, clean double-success beep (![][image1] beep / ![][image1] gap / ![][image1] beep).  
* **LCD Display:** Displays the leased Local IP Address on line 1, and Module is OK on line 2\.

### **5\. Runtime Passive Mesh Monitoring**

* **Port / Protocol:** UDP Port 8888\.  
* **Action:** Listens for subnet broadcasts. If a packet starts with the token signature TG\_NODE:, the LCD outputs the payload, the buzzer triggers a fast ![][image1] acknowledgment beep, and the interface automatically restores to idle state after ![][image7].

## **📂 Code Architecture**

* **Unified Configuration Variable:**  
  const unsigned long TIMEOUT\_MS \= 2000; // Configures both SPI and DHCP timeouts in milliseconds

* **Dynamic MAC Generation:** Employs analog noise harvested from floating pins A0 and A1 to formulate unique MAC addresses, preventing collision-based DHCP delays on high-throughput assembly lines.

[image1]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAD0AAAAZCAYAAACCXybJAAADP0lEQVR4Xu1WO2hUQRTNEgW/iMiyZD9v9oeLhRBYFRG0EsFCEbQQFW0Eg1hFoqCNKClMGQNCuiDbaGVhpZhABKN2glpYiGElVVLFwoV1PWdn7nKdvJfVpEiUd+Dy5p25d+498+bNTE9PjBgx/mvkLc76vKBQKFSMMSOwcdi5bDa72fchxz7nM8IY32fNgcL2wK7CXsKaED3h+xDoOw37iP7+ZDK5De17sOfFYnGH+LBNjn30oS9jGKvHWnNQNIo7hechWD1MdKlUyqHvM+y8cEEQ7MT7O9g1xd0kxz7hGAP7hC+eEm7dAIX1wb6GiXaFf4eYqqIT4GqwKX5VmQQ/PpfL7Qe/iOdJza8LdBE9GiKae8AE+DnwRWN/k3k/njGMhQ1rXsAJy2QyWayEYxyjWq1uxATt4+rjCnNuCfw6uzHWGfrgvVePkUqltoI/DruOcY5wosvlclL7hGI50U5clOg2L+L8+CheQHHo/wZrGbv5PTJ2Zd1mHOyy4wfhO+B8xxC6gfHGTvYr9B3Esw/5LsJm/VpDwQATItptWlMswB/IE30C7ZYf3000wS8Nn7pR/36lUtmO92nYD3y5w+KL92HWyXrd+yCshmZC+dz1aw0FB+FgfnFu6bwQcbpPizZ2ea1ItOSGjQqnJnuaEyB8YDfLjmiMewXtJuwBJuwA41gzfxOJiYQkDitOi4vio8RF8RqSm4KEU6LbG6Xwvmh3TD4x9vdoG3LdX7VoY5dUlOg6l2dgN7M5P15Ew25pXkNyr0S0QyJvcQn8JOwn2gOqPxyS2C+a4HGDviYSHhUOfpvAPaOxrYpsv4sfY8A1dKwPyb0S0XgO6eOQXxj5H4fpWAJJbLxNgcCX3IVB3sLuCIfrZtnYy0zn2oqCLsBmwRccxbOcN7cZfXPzoTayzmroInre2KOr/U6Rajkz55iewCVwX4IJuRm0nC1ioPcQtlf8eHaC/wL/G4E9L3kR+e3fYRt9D9E3mbdHEQV/QLtffHzk7THUkNycXE4e2guqngXHvVZcw8UOoe8NnjOwcXBP8awtN8l/Be6KvERQkLo4+OhcJHhR+KMNZRVIp9Nb8OhlHh53elXEiBEjRowY/zB+AV7nP65qAOOtAAAAAElFTkSuQmCC>

[image2]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACkAAAAWCAYAAABdTLWOAAADAUlEQVR4Xu2VO2hUQRSG72KEiC+Crks2u3t3N4trQFDZQiIiIj5ioYVYRCwtFAmKWAiCEtHGQgLRShQVCT4KLcRoGbQJBiIWIaCmUHxUGhQUFTbx+/fODbPjzWbtUuwPP3fmPGbOnDlnruc10MA8RaFQWJbJZI77vn81m82ey+fzqxHHXLso4NMKz8gXnmxvb0+7NjbYpw+7Mpy2+CCRSCy2bI6Fuoogl8utYzJEcFtRtvA9zPyPNvTmCBT7PdgNag0OGmd8Hv6C+1xbG6VSaSH73DeBRNoi3wRHw0k/LKfT6b2aK1DmI/AL7KjytMAmzegfw0m4QTJ884w/w7FkMrnS9bGBf7cJ8g7Tpln0lysTBpeM8SHNi8XiUsbP4XdlqMrTggnyISzrFiRra2tLMf8AJ/BNOC5VkB67cWOfc9RNyK5z6M2VmVJvTr1AczK61g+yMxSPx5fYni4UKIGt8ExZMO/Cbwpe8yKy4yCG3RU4rRKzFcg64CN6Y7ktr0ANhHKAE7zHcb2rrwVlEZ9n+I9o7OqjwD7bdSi+T1Op1KJQjqwH2Snb1lNnIbyr4DCYYLNdnsnsXKCbV/lBeejaRk2J1Gy4EMoUPsNwUjcomYJVosJ5JAhwDUaf4G37WagH+GyDP1mjV2Xk6qMgWz+48l7NGXcyvqdSckyroFoZMI5HXGUtWE1X5lZ2uPooWD0wrMzid5Z9u2cMzHt1QrRPrnowQd6aMXagBU0WDnrW9cpHvv/U1CzQ9aom8ZnC9wDfJ77d7ShLCH6IGofycCPYH8oUFKdOeiagTPCQy+YdbJVMrwHjIXPAqo6tBR3UrPWRdfs8u6b1C0PxBsXNsN31pLDBC996pCVj/Ar+hp2SmQN+hRfCWyArBT+o5/F6O1zAPucHjadsdrl6GeyGb1FeVC34wV/km+ShjcnQIPrXdK9vxKrdo3AMnvaDbLw0a/3X8wViHPoGviN8W1xlBSzanA3+3fsJYku9nSnoBvDZKV9TMnU9XS50eGLY6MobaGC+4S8p5NckReOqbQAAAABJRU5ErkJggg==>

[image3]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAADMAAAAZCAYAAACclhZ6AAADEUlEQVR4Xu2WT0hUURTGlTHoLxllg86ML8doEIKEKSKhaBFBiCEZIRm0CHTTooIK2lSUC4sIahNCRIEQFESLIMiFFAglWAuNKCEKsV0RWNBC7Pc599j1+UT7A9PiffBx7/nuOfee8+6fmZKSGDFi/DbWgSAIDqZSqTRmAnNxTU3Npurq6nb1fV/0HL6XYLdi0un0En+86CCpBvgDTnr8BhtDfi3wNQXWV1RULKd/AT7JZrMrfb+igh3IK0n4Br4i2fO0lb5PbW1tBu0dbPPiVmEPwKO+b1Hhirke1n2oCO2WfD25FK0H9mmnPL14WGAx1yKK0X27jf4JPevrBsbLuWcB403cr5TuINwpmzu62rklNC96s+4vdqk3RSnHeANyBzzMWnWwwRufCVfMA3gHjmB/hGf9y+2SnquYWbqB8eOMfwkK97ATv1tore7jfIYtaHdhO/2TmovxYy5cO38FXtTjRFvH2EOtOWMRH0oEPif59bL1xQh4QXB3Pp9f5C57X1TS8xUjZDKZLfh8x+exfSCX3Cgcs3VBGfZ9raU12dEk/UHF21wqSAWaPQtKOJlMLvM1As4EhRdum8Zoe6OSXkgxbuf1xTtMw66EH2BPiXes3HxTxVRVVa2hPwyHmOOQPoByzeVyK8x/QSD4dFA4Gidkz5X0XLoPK4a2yTQrJnxk/GKcX6NiXS7iCD71fsw0dKSCwvM6xPO71nQrRq1s+p1RSbvFR90PbiT+phinTT0asIuxr+Fcp2GThh2CwjGb5Lzula0We4KEdpmPFkF7JKpvehh/Wox8ZPuFkcd29LHwRzWUMXCDoK0m6Bcd+ylBvfbrbo8CPGd+urj4jKK1mhYFewDgPtPmKWbY3Rf5DKsAG6e/UXnomffjpqHfAfAMXoZH4EvYHz46TLQZ/T3Fn4L76Q8wcZcupe/nA7+r+E0Ev878vaDwBI+bxhxvadtca37jxB6g7Q8K10BxN+Eg3BNeZwaUEEXtcEnWISXCPoJeNvx2s3Cz/uKEx/8xEuzQUnVYr9xdg8i8YsSIESPGf4mflooKAIBAyKcAAAAASUVORK5CYII=>

[image4]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAD0AAAAZCAYAAACCXybJAAADlklEQVR4Xu1WTUiUQRh20cD+iIhtUXd3dtelpVPC9g91iiCiCOoQFXUoykMnwwI79IeH7KZC4c3CSx2iQ6cEJaPUDnXoDwoiMTzZKQ8Ktj3PN/Ou745uigRL8T3w8M08884788w338xXVRUiRIj/DinAGHOioaEhjmo1qrXpdHpLMpk8z7KOhZ5DbAfYwz7xeHylbieosc3FdLCPH1NxYGK7wWmwoDgFHvTijoIfsBBN0Wh0Dcq3wGeZTGadxLBMjW2MYSz7sK/OVXHgjebdxD6BbzHRG3jW6ZjGxsYEtM/gSdVvPeqvwYtKu0KNbaKxD/gRbzwmWsXhTHf5uoab+BRjlRyB1gcO8q3KImDRelVMVSKR2Ab9J56HtV5RLNF05wKmeR70Qp+AnsFzMzjpm3b5+bm0a13ABeN5gp2wnzny+fwKLNBW5DnCHebCIvh0NiHXMcagXq1zxGKx1dAPgJeQZy8XOpvNRnVMCdykHoP3wS+oj4HX9CHlzJUzHehirpxpXxfQHNq/G3uW8PB7YOzOusp+4DmntyC22cV2o2sN+xu72C/QthPPOox32nkomWsJ3IRHYDLLOlZ9AxKMciCuuju0BjkBP5Fn+hDKBd/cYqYJvmnEjBv17edyubWoD4HTeHN7JBb1dvAbDbp6C9iHYkTF3PTnWgIa4/bQGjq1cTBwl9s6/Zy4n0ibNnZ7Lcs0DTgjnaKpxR7iAoietIdl0TTyXkB5FuzCgm1nP86ZvqTPkuASF8AW1rU5Hee96QXNldM1xDTHFU2ZDg5K0X3T7pp85OYbEGPdLmuaW9nYa+cdDo2NootpmYSxW6qc6XFuz6Q9zCZ8c2IabNO6Bg2YZZp2iKQszkAfAH+h3Kza5yCDGc80JwgW5JrhE/VZDLhPYpC0FtpTkmU1yaAucewDbUb39SHzWI5pPFv1dcg3jPEf+ouvUYMk9xCwQwRuF9SfI1m//G3J4QZelzgefIgZh3ZcNOQ6BY5BTzuJdzn/3Ib1n5sPdZAVd8MipieNvbqCOk2q7cwxu/UCzgM+fsQEp+Qd8Cz4Bnzp/sWL4N0J/SuSXU7a+5I/IiXfDstou4u2gZS9imj4PcpNOpdGyl5DM2buexzl4qH8QzSWnfZKaTOubyvaRvAcBnugPcGz70+LHICT5aXuzMy7/AU8FfkTQUPqx8FH8UeCOcseKH8J9fX1q/Codh5ieleECBEiRIgQ/zB+Ax6OVGY9NBuCAAAAAElFTkSuQmCC>

[image5]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAEgAAAAZCAYAAACSP2gVAAAEP0lEQVR4Xu2XT4hVVRzH72MUTJOMmgbnzzvvzYTDgKHwTCkyXEQ4iCHmQkswWihImxQLBaFQFwoGuagQJEJaZC1s4SISMgSJAleOLixkYlAsDBR1Icj0+b77+82cOb47Tio5i/uFL+ec7/n97j3nd37n3HOzrESJEiWmAUII8+FueLhWq33c3d39fGoj1Ov1fmwOyA6+hd0TqY009ZnNAfmkNqDCe5bRfwh+BlehtaVG0wJMYCkD/LGnp2c59UXUT8BRuJ3uitvRfhOeZ2KL29vbn6S+R369vb1PuY3q0tQnG9nKR75uAyrVavUD+DN6vaur6xnKr+HhRqMxM7J7/NBqM9DjDO7dzFZQA2Ziv6Ldoq8hra+vr4f2Rfi2+9L3NO3f4HuR9qE09bkmH3iB4HeYTYP2VcpXIr9etGHeu9K1aYGQb61heEPZE+m7Qp5F26ytSY4FzFAJ+cqfUrZ4wJjkV5FNRma+iH6T8g21qe+zd853m/7+/rm0T/OML7Moax87lNJM6FMG9kM8YMuEUZVqh/ysSAOUKRjoVywDBuC1NECWMbcUGPpmhXwLTwiQbdlTIcm+CG1k8XMEeSE2g8p8ZSS2qylf9a3Z0dExh/brYrz1DRW0BYxhC9xk4305sZkSZuD4HbzLg1ZIsEAUBaipeyCKAiQ9CkRRgCboDnznwWMaExyhvp9yD+V6yjPwJPUNlEel2bj+jnaFsv0g3MsR0k05gM336VinhFr+ddGKNw/NaPD3C9Bq6qPpS+MAhfEt/Z8C5AjjW3/s3KO+1t57zDPJMlqZ3dwByjbaZ7XdIz9l0EFvTwn2FToJjypdpak0bdIAUQ7aQAsDZAP9IzxggDRh+q/BgUhrLgxc65qeoWd5gDo7O5+lPQTPoW1UFimYOvvc576QA85f8JBP0vtNHIgiPQ5EbBPrRYEo0lNYgCbYeIBUuqZ+2XmATFulccjW+DtjWuz9k8KDg8POzD73PGBAh53V9eUpCtCIVqRqaV0UILgrGz/figJ0erJVfZgACYxtFlxRy8+w6/CcDv/YphUqGG7H6X3VXaS9JVja6hNN/S4vfC3q9y/SCdWjSTbbbicftDvua5OcsE2iLXDItVZ40ACprYXTGN1GF2P0y+mip6jg+A6Gt+EIxn86af9TtcucXx7hR+6o3xH5oK13DfuN5ls3ScHXjfsX/+y28rPB/gVfcq0VQn5INzPWtckCFCzg1h7Se9xGVwbNifF0uXYPogeNtmDzfuO2PHAJ2qVq/puwLuSXwv3x74Ft1c/p+4m+NSEPzlAt2esh//LoWZtr+Z1EvyNbs4JLoraBJhON7Q7cgfZNyD/90nQtUXsHvOm28mNxXgj5dUA3/2/hEXgWDqbveij4RUyT1+9H2m9oXsgURGzHLnEplJVadVH1tP8Ro41tPFsVxj7Pzp3p+XNcokSJEiVKlPg/8C+oh58E80p2bAAAAABJRU5ErkJggg==>

[image6]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAEgAAAAZCAYAAACSP2gVAAAD8klEQVR4Xu1XTUiUQRjexQL7I6Nsyf2Z3TVahCRhi0gqOkQkYoR1sBK6FUhXEzwF1kHBi4cCIULCQ9alg6ekDCGiwENoHSqikKKiQ6AXQex5dt7Xxmk/VzJiD98DD/PNM+878877zcw3XyQSIkSIEGWEtEWbrysymUzOGNMHDoLnE4nEBt+GGtvEpo8+vg0QxTgH0T4A3gSboVX4RmUBBFcHdoCPwQUEPuTbEGg7A75Ge0N1dfVmPPeAj7LZ7Fa14TM1ttGGtvShr9NVNJVKXQWfQs/E4/HtKIfBwXw+v96xKw8wQZjIaZSN4EyxBNXW1ibR9ha8oBomuA31l+AVR+uixjbV6AO+wUqKiU0e9a8oDzt+WWgfMfZJ1coOCHCXBPlHgmSSc5ycI0eNffPjXC2aMN8/mUwegD6L8hTreL7BcTie2uRyuS2oT6CPO6hGl5zLCSUSxLPCTxDPrCHoX2QFcKv+8P1lxcwxMWirRDnqJ0i27LjxVp+DCqzinUjyXtg08ZzjioRtC8qjujVjsdgm1E+Q7tYXRKHtQQyXwYsSb6NnEwwGzMD9CRKSiKAEFXRNhO/v6k4ighK0TFfAtwocQduCscdAL8oelG0on4FjeD6H8i41ies7ErVPuuBq7wev48xLGHusPPRjXREMjAH6Tk7wpRLUgudF399NkI4hXHWCFGjr5hjGOffw3CrjjuhKkhXNld3FOlcb6pPc7o4fV1C/1kuCgTFAf4JcttDHTIkEoWySQAMTJIG+5zgcT21WmyBOGO0/wDpHK7wYsFU19sG+NEE1NTU7UJ8Gp6C1cxUxmTz71KcktFN/goSbiCDdTYRr4+pBiQjSfUiCltlogliqpnPRBInWzDhoK3yHmBq0vSS0U3+C0sYvT1CCZvhGUrKsfX9NENiN6jqUDzgOx1MbJ0ETK73VtSSIQGyV4LG0PcN+glM8/F2bQGin/gQJfqLRtoABj6vGwYz9Io3y2Zlkoa529IE2r74yyWXbxNkCA6oVw98miHVdwWqDOR2B/tl/6YHQTsHhiHcX4W0XA7wAr6mGT+1uY78oS78mGKwd/AQ9I1LU2Bv3c/3sFvOTYL+Bh1QrBmMP6cKKVW2lBBlJuNSnOY7a8MrAOSGeuGpFIW94xthP6KJwFs6v4FyvduhwP/QPKfubcNbYS2Gv+3vAZ7TdQtuTtL2dMznTaW+vG/vlYV+X0vZOwt+RjkjAJZHbgJNx4psHO6HdM7/j5m8S653grNrSDy+n3tjrAG/+98Hb4CTY5I+1JuhFjJPn74ffLihcyJhE9xLng6uSb53ks9/+j1GBbbyRD4i9Ss6d8vw5DhEiRIgQIUL8D/wCkSmIUGjOPqsAAAAASUVORK5CYII=>

[image7]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAFMAAAAZCAYAAABNcRIKAAAE2ElEQVR4Xu2YXYhVVRTH73ALrKw0nSbn4+4zHzhmRsZUVJRI0IfkSEiioOhDD4oE4cgkiC+FEhJSmU9DURFDkb2IDQQFDRT5BT6NBKWIIUqESkNJJDb9/mevfdyd6d47E/N0OX/4c87e62Ovvfbea597S6UCBQoUaHwkSTKru7v7bl7Ledn/Qk9Pzx2VSuUV59wQzl/r6upaSHdTXq+RoAQy3zE4Ac/BBXmdaaOzs/MBHI2SxOUkdC7PzbT/gttLDZ5Q0MR8985YMnGyH17v6OhYpbYSSvsEvATvzes3GpjvjplM5j7nt/pLavf29t7O+7dwXLs2r99omNFk9vX13dza2jq/ZAWYHboEx1fgaHNz8+yceowm1VaVBbhJuxg+Hiu0t7f3EOyr9G/VeywzlJH3qU5LxxbvX6Wlra1tHjprkO+UrmyCDLs52Dj6+/HfpsvEylW/7CI3GcxfP3bPEP+d/5XMlpaW22ivgNvRW0ZOHuZeaY791IUuIhwMM8DPBLU0L4/Q5PyO3k1w7Uok+ofgRxJqgejbBU9Ysp7j/ayCk6106LuH9tdwREmk/QLv10wnHQPbjfKhBClp4E34lWzNxzbnF34C7kH/A/rWOV+6LmPziPlK/dH3MvwFDpjee/CMi5Lp/Kb4Dvmj6lMMyoctZH1oJVD+VEZyjqNnSzU+FQiyBb2TWrHQZ0Hss/fVcBw+FsnTiWDbq2Rr4rR/kC/JlTDa14MP5E/zfoUxngw+ZIfeZ/QfZifeoj7FQPsq+l+GPlvg83B/sMVurWKK/YFJFxDPATgsWVCi/fqUkxkD54swvgA/VpLzckFlAfkpOMYgGxS8Jqp6i/0s+kckt/KRIpp0vwLj/Y94siW/E+fyLFfzIVT8sbzG8wlrp76w2Rx0aC+A5xI7KSpXtEedPykaI4P5y5IpP84v6rva2bJVHjS/2G6q0HEYhhM43pIXBiB/XpOQnvE0+ksVlAX3q+2+oZjSUUJlo4nk/Qq283X8JtVtm7zGG7B2mkz5DDohhpDM0K7hL0um6ijvB22MlNq9dZNpx2abGCuHgEMw1aAdBJdrMPR/g2OJ39natZN2QUC9ZEY7f5KPKJnrrT2jyTTo+Aub6P8G/p3U2FgpQiAWTFYTFITzAcfHMIMGlk4cmGoR/Rd4PsTzALyIz67YTrc/rCDTbaKa9jndNwW5La4uPh3zT6SjEnLDQzr2Hudr6RK1p5LMpH7ZiGvmYPjmFkKdDr6qgp9UHRj/hMMPtb3Vp08HDI8rYPhg3kawYE/FxVyTk50+UfB3n/PH/I2SXWQW1Dv2iaT6qE+mq/CpyIcWZLfkGhteRm9DkIfY4NvSMZu0FsPVQc/iy5IpVPyF9id9a0Mfsdzl/Dd1tvBKrpIXnVTFckD9wa4qnP+eUr3bm/jPBa2gjuyKvG6ABfu987+UVF/ehydjG/sG1aLo96/qpS6ATG6fT4NwHL1Dzu/EobCoAhNYTN8R+IWNoaM/GCaK/C3nL4sJo2KRz99DH75/JGn3l/wCrnT+RBxUonketqd05Wer7NE7xvOo8zVesQ3HcdVEcuNj90WK/7K6xZbdxnG5VS/Yzan1r4vkulCq+VS/5NLLywLq+Zgmyoo3jKdSFcdv8yqHuPI1tkCBAgUKFChQoEBd/AM8zabPEGntKgAAAABJRU5ErkJggg==>