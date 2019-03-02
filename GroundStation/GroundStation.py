import serial
import serial.tools.list_ports
import threading
import time
import binascii
import sys

print("---------------------------------------")
print(" Daedalus Serial Communication Utility");
print("---------------------------------------")

ports = serial.tools.list_ports.comports()
index = 0
if len(ports) > 1:
    print("Select a serial port:")
    for i in range(len(ports)):
        print("{}) ".format(i + 1) + ports[i][0])
    index = int(input(":"))- 1
elif len(ports) == 1:
    index = 0
else:
    print("No serial ports are available!")
    exit()
    
ser = serial.Serial(ports[index][0], 9600)

print("Connected! Serial port: " + ports[index][0] + "\n")

current = 0
exit = False

def handle_response():
    while True:
        line = ser.readline().decode()
        print(line)


thread = threading.Thread(target=handle_response)
thread.start()

while not exit:
    command = input("#{:<2}>".format(current))
    if command == "exit":
        exit = True
        sys.exit()
    else:
        space = command.find(' ')
        if space == -1:
            command += ';'
        else:
            comList = list(command)
            comList[space] = ';'
            command = "".join(comList)
        command = str(current) + ";" + command + ";"
    
        crc = str(binascii.crc32(command.encode()))
        ser.write((command + crc + '\n').encode())
        
        current += 1
        time.sleep(0.2)
