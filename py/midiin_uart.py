import serial
import time

ser = serial.Serial('/dev/ttyAMA0', baudrate=38400)
old = time.time()
print(old)

i = 0
while True:
    try:
        data = ord(ser.read(1)) # read a byte
    except serial.serialutil.SerialException:
        print('not ready exception')
        continue

    i += 1
    if ((i % 24) == 0):
        i = 0
        new = time.time()
        dif = new - old
        old = new
        bpm = 60 / dif
        bpm = round(bpm, 1)
        print (bpm)
ser.close()
