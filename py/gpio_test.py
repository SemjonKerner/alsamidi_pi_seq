import RPi.GPIO as GPIO

def button_callback(channel):
    print("Button was pushed!")


GPIO.setwarnings(False)
GPIO.setmode(GPIO.BOARD)
GPIO.setup(10, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)

#while True:
#    if GPIO.input(10) == GPIO.HIGH:
#        print("Button was pushed!")

GPIO.add_event_detect(10, GPIO.FALLING, callback=button_callback)

message = input("Press enter to quit\n\n")
GPIO.cleanup()
