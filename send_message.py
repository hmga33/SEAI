import time
import serial
import RPi.GPIO as GPIO

#define typeOfMessage 1 to speed_error
#define typeOfMessage 2 to direction_error
#define typeOfMessage 3 to temperature

# sending messages to Arduino
def sendMessage(typeofMessage, error):
    time.sleep(1) 
    if(typeOfMessage == 1):
        values = [typeOfMessage, error]
    if(typeofMessage == 2):
        values = [typeofMessage, error]
    if(typeofMessage == 3):
        values = typeofMessage

    ser.write(values)

# receive message from Arduino
def receiveMessage(): 
    read_ser=ser.readline()
    print(read_ser)

ser = serial.Serial('/dev/ttyUSB0', 9600)
ser.baudrate = 9600
time.sleep(5)


while 1:
    ser.write("1\n")
    ser.write("-25\n")
    time.sleep(0.5)
    #ser.write("1\n")
    #ser.write("15\n")
    #time.sleep(1)
    ser.write("2\n")
    ser.write("-15\n")
    time.sleep(1)
    ser.write("2\n")
    ser.write("0\n")
    time.sleep(1)
