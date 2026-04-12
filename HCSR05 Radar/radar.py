import serial
import serial.tools.list_ports
import turtle
import math
import _tkinter

# --- CONFIGURATION ---
BAUD = 9600
MAX_DIST = 100 # Adjust this to match your sensor's max range (cm)

# --- AUTO-DETECT PORT ---
def find_arduino_port():
    ports = list(serial.tools.list_ports.comports())
    for p in ports:
        # Common Arduino identifiers
        if any(name in p.description for name in ["Arduino", "USB Serial", "CH340", "CP210x"]):
            return p.device
    return ports[0].device if ports else None

PORT = find_arduino_port()
if not PORT:
    print("Error: No serial ports found. Is your Arduino plugged in?")
    exit()

try:
    ser = serial.Serial(PORT, BAUD, timeout=1)
    print(f"Connected to {PORT}")
except Exception as e:
    print(f"Error: Could not open {PORT}. {e}")
    exit()

# --- TURTLE UI SETUP ---
screen = turtle.Screen()
screen.setup(1000, 700)
screen.bgcolor("#0a0a0a")
screen.title("Pro Radar System - HC-SR05")
screen.tracer(0) 

# Grid Turtle (Static)
grid = turtle.Turtle()
grid.hideturtle()
grid.speed(0)
grid.color("#004400") # Dim green for the grid

def draw_static_grid():
    grid.clear()
    grid.width(1)
    # Draw arcs
    for r in [100, 200, 300, 400]:
        grid.penup()
        grid.goto(r, -250)
        grid.setheading(90)
        grid.pendown()
        grid.circle(r, 180)
    
    # Draw radial lines
    for angle in range(30, 181, 30):
        grid.penup()
        grid.goto(0, -250)
        grid.setheading(angle)
        grid.pendown()
        grid.forward(420)
        # Labels
        grid.penup()
        grid.forward(20)
        grid.write(f"{angle}°", align="center", font=("Arial", 10, "normal"))

draw_static_grid()

# Radar Sweep Line
sweep = turtle.Turtle()
sweep.hideturtle()
sweep.speed(0)
sweep.color("#00ff00")
sweep.width(3)

# Detected Objects
bits = turtle.Turtle()
bits.hideturtle()
bits.speed(0)
bits.penup()

# Text Display
info = turtle.Turtle()
info.hideturtle()
info.speed(0)
info.color("white")
info.penup()

def update_radar(angle, distance):
    sweep.clear()
    
    # Distance text update
    info.clear()
    info.goto(-450, 300)
    info.write(f"Angle: {angle}° | Distance: {distance} cm", font=("Courier", 16, "bold"))
    
    angle_rad = math.radians(angle)
    # Scale: 400 pixels = MAX_DIST cm
    scale = 400 / MAX_DIST
    pixel_dist = min(distance * scale, 400)
    
    # Clear old dots occasionally to simulate "fading"
    if angle == 15 or angle == 165:
        bits.clear()

    # Draw Sweep Line
    sweep.penup()
    sweep.goto(0, -250)
    sweep.pendown()
    sweep.setheading(angle)
    sweep.forward(400)
    
    # Draw Object if in range
    if distance < 50: # Only show dots for close objects
        tx = pixel_dist * math.cos(angle_rad)
        ty = pixel_dist * math.sin(angle_rad) - 250
        bits.goto(tx, ty)
        bits.dot(10, "#ff3333") # Glowing red dot
    
    screen.update()

print("Radar Activated. Starting sweep...")

try:
    while True:
        if ser.in_waiting > 0:
            data = ser.read_until(b'.').decode('utf-8', errors='ignore').strip()
            if ',' in data:
                try:
                    parts = data.replace('.', '').split(',')
                    if len(parts) >= 2:
                        angle_val = int(parts[0])
                        dist_val = int(parts[1])
                        # Check if window is still open
                        if screen._canvas: 
                            update_radar(angle_val, dist_val)
                        else:
                            break
                except (ValueError, IndexError, turtle.Terminator, _tkinter.TclError):
                    continue
except (KeyboardInterrupt, turtle.Terminator, _tkinter.TclError):
    print("\nRadar window closed or interrupted.")
finally:
    ser.close()
