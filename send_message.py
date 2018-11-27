import time
import serial

ser = serial.Serial('/dev/ttyACM0', 9600)
time.sleep(5)
ser.write('5')
