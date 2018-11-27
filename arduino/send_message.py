import time
import serial

ser = serial.Serial('/dev/ttyUSB0', 9600)
a = 10
time.sleep(5)
i = 1

while 1 :

    i = 1
    #print(a)
    while i <= 10:
        #time.sleep(1)
        ser.write(str(a))
        time.sleep(1)
        i +=1
        print(i)

    while 1:
        ser.write('-10')
        #print(a)
        time.sleep(1)
        #ser.write(str(a))
        print('-10')
        #time.sleep(1)   
