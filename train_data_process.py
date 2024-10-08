import serial

my_serial_port = serial.Serial('COM8', 115200)
file_path = r'd:\Edge caculation\magic_wand\train_data\train_data_other.txt'

try:
    with open(file_path, 'w') as f:
        while True:
            data = my_serial_port.readline().decode('utf-8')
            if data:
                f.write(data)
except Exception as e:
    print(f"Error: {e}")
finally:
    my_serial_port.close()