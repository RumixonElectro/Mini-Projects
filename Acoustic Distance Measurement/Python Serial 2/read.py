import time
import customtkinter
import serial
import random

app = customtkinter.CTk()

app.title("Serial Data Reader by Shriraj Darade (Sandip University)")

def DTread():
    # serial data is read here
    print("Distance : ")
    # ser = serial.Serial("COM7", 9600)
   
    # # Send character 'S' to start the program
    # ser.write(bytearray('S','ascii'))
    
    # # Read line
    # bs = ser.readline()
    bs = random.randint(1, 100)
    print(bs)
    lbl1.configure(text=bs)
    lbl1.after(100, time)

lbl1 = customtkinter.CTkLabel(app, font=("ds-digital", 80, 'bold'), fg_color="purple", text_color="white", anchor='center')

lbl1.grid(padx=10 , pady=10)

DTread()

app.mainloop()