import serial
import tkinter as tk
import math
import threading

# Global variable to track the current angle of the radar
current_angle = 0

# Dictionary to store the distance values for each angle
distance_data = {}

# ============================
# Function Definitions
# ============================

"""
@brief Reads data from the serial port and updates the radar interface.

This function continuously listens to the specified serial port for incoming data.
When data is received, it is parsed as an integer representing the distance and
used to update the radar visualization.

@param port The serial port to listen to (e.g., '/dev/ttyACM0').
@param baudrate The baud rate for the serial communication.
@param canvas The Tkinter canvas object used for drawing the radar.
@param radar_center A tuple representing the center coordinates of the radar.
@param radar_radius The radius of the radar visualization.
"""
def read_serial(port, baudrate, canvas, radar_center, radar_radius):
    global current_angle, distance_data
    try:
        with serial.Serial(port, baudrate, timeout=1) as ser:
            print("Listening on serial port...")
            while True:
                if ser.in_waiting > 0:  # Check if data is available
                    data = ser.readline().decode('utf-8').strip()  # Read and decode the data
                    try:
                        distance = int(data)  # Convert the data to an integer
                        # Store the distance for the current angle
                        distance_data[current_angle] = distance
                        # Update the radar visualization for the current angle
                        update_radar(canvas, radar_center, radar_radius, current_angle, distance)
                        # Increment the angle for the next update
                        current_angle = (current_angle + 1) % 360  # Increment by 1 degree, wrap around at 360
                    except ValueError:
                        print(f"Invalid data: {data}")  # Handle invalid data
    except serial.SerialException as e:
        print(f"Error: {e}")  # Handle serial port errors

"""
@brief Updates the radar visualization for a specific angle.

This function draws or updates a point at the end of the radar line for the given angle and distance.
It also draws a rotating radius in a different color to indicate the current angle and distance.
Additionally, it displays the distance value (in centimeters) near the point.

@param canvas The Tkinter canvas object used for drawing the radar.
@param radar_center A tuple representing the center coordinates of the radar.
@param radar_radius The radius of the radar visualization.
@param angle The angle at which to draw the radar point.
@param distance The distance value to represent at the given angle.
"""
def update_radar(canvas, radar_center, radar_radius, angle, distance):
    # Remove the previous point, radius, and text for this angle, if they exist
    point_tag = f"radar_point_{angle}"
    radius_tag = "radar_radius"
    text_tag = f"radar_text_{angle}"
    canvas.delete(point_tag)
    canvas.delete(radius_tag)
    canvas.delete(text_tag)

    # Calculate the x and y coordinates for the point
    x = radar_center[0] + distance * math.cos(math.radians(angle))
    y = radar_center[1] - distance * math.sin(math.radians(angle))

    # Draw the radar point for the current angle
    canvas.create_oval(
        x - 2, y - 2, x + 2, y + 2,  # Small circle to represent the point
        fill="green", outline="green", tags=point_tag
    )

    # Draw the rotating radius in red
    canvas.create_line(
        radar_center[0], radar_center[1], x, y,
        fill="red", width=1, tags=radius_tag
    )

    # Remove the previous distance text
    canvas.delete("distance_text")

    # Display the distance value (in centimeters) in the top-right corner
    canvas.create_text(
        radar_center[0] + radar_radius - 10, 10, 
        text=f"{distance} cm", fill="white", font=("Arial", 20), tags=("distance_text", text_tag), anchor="ne"
    )

"""
@brief Creates the radar interface using Tkinter.

This function initializes a Tkinter window with a radar visualization. It also starts
a separate thread to read data from the serial port and update the radar in real-time.

@param serial_port The serial port to listen to (e.g., '/dev/ttyACM0').
@param baud_rate The baud rate for the serial communication.
"""
def create_radar_interface(serial_port, baud_rate):
    root = tk.Tk()
    root.title("Radar Interface")

    # Canvas dimensions and radar properties
    canvas_width = 710
    canvas_height = 710
    radar_radius = 350
    radar_center = (canvas_width // 2, canvas_height // 2)

    # Create a Tkinter canvas for the radar
    canvas = tk.Canvas(root, width=canvas_width, height=canvas_height, bg="black")
    canvas.pack()

    # Draw the radar circle
    canvas.create_oval(
        radar_center[0] - radar_radius,
        radar_center[1] - radar_radius,
        radar_center[0] + radar_radius,
        radar_center[1] + radar_radius,
        outline="green"
    )
    # Draw the crosshairs
    canvas.create_line(radar_center[0], radar_center[1] - radar_radius, radar_center[0], radar_center[1] + radar_radius, fill="green")
    canvas.create_line(radar_center[0] - radar_radius, radar_center[1], radar_center[0] + radar_radius, radar_center[1], fill="green")

    # Start reading from the serial port in a separate thread
    threading.Thread(target=read_serial, args=(serial_port, baud_rate, canvas, radar_center, radar_radius), daemon=True).start()

    # Start the Tkinter main loop
    root.mainloop()

# ============================
# Main Execution
# ============================

"""
@brief Main entry point of the program.

This script initializes the radar interface and starts reading data from the serial port.
Replace the `serial_port` and `baud_rate` variables with the appropriate values for your setup.
"""
if __name__ == "__main__":
    # Replace '/dev/ttyACM0' with your serial port and adjust the baud rate
    serial_port = '/dev/ttyACM0'
    baud_rate = 15200
    create_radar_interface(serial_port, baud_rate)