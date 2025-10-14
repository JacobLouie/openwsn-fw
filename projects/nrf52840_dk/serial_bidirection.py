import serial
import threading

# Function to read data from the serial port
def read_from_port(ser):
    while True:
        data = ser.readline().decode().strip()  # Read a line of data and decode it from bytes to string
        print("\n\rReceived:", data)

        # Check for a specific condition to exit the loop
        if data == 'exit':
            break

# Function to write data to the serial port
def write_to_port(ser):
    while True:
        message = input("Enter message to send (or 'exit' to quit): ")
        ser.write((message + '\n').encode())  # Encode the string to bytes and write to the port

        # Check for a specific condition to exit the loop
        if message == 'exit':
            break

# Open the serial port
port = serial.Serial('COM11', 115200)  # Replace with the appropriate port and baud rate

# Create and start the reading thread
reading_thread = threading.Thread(target=read_from_port, args=(port,))
reading_thread.start()

# Start the writing process
write_to_port(port)

# Wait for the reading thread to finish
reading_thread.join()

# Close the serial port
port.close()