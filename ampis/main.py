# simple gpio tutorial program with python
#
# sudo pip3 install RPi.GPIO
#
# use a switch between gpio 10 and 3.3v with pulldown resistor 220 Ohm
# use an LED + R:330 Ohm between GND and gpio 23

import RPi.GPIO as GPIO

state = 0

def button_callback(channel):
    global state
    if state == 0:
        GPIO.output(11, GPIO.HIGH)
        state = 1
    else:
        GPIO.output(11, GPIO.LOW)
        state = 0
    print("Button was pushed!")

GPIO.setwarnings(False)

GPIO.setmode(GPIO.BOARD)
GPIO.setup(11, GPIO.OUT)
GPIO.setup(10, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)

GPIO.add_event_detect(10, GPIO.FALLING, callback=button_callback)

message = input("Press enter to quit\n\n")
GPIO.output(11, GPIO.LOW)
GPIO.cleanup()
