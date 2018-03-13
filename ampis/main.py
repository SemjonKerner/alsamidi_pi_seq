# simple gpio tutorial program with python
# https://tutorials-raspberrypi.de/raspberry-pi-gpio-erklaerung-beginner-programmierung-lernen/
#
# pip install RPi.GPIO
#
# use a switch between gpio 24 and 3.3v with pulldown resistor 10kOhm
# use an LED + R:330Ohm between GND and gpio 23

import RPi.GPIO as GPIO
import time

GPIO.setmode(GPIO.BCM)

GPIO.setup(23, GPIO.OUT)
GPIO.setup(24, GPIO.IN)

for i in range(5):
    GPIO.output(23, GPIO.HIGH)
    time.sleep(0.5)
    GPIO.output(23, GPIO.LOW)
    time.sleep(0.5)

while True:
    if GPIO.input(24) == 0:
        GPIO.output(23, GPIO.LOW)
    else:
        GPIO.output(23, GPIO.HIGH)
