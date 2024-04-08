
#/usr/bin/python
import time
import RPi.GPIO as GPIO



if __name__ == "__main__":
    try:
        GPIO.setmode(GPIO.BOARD)
        GPIO.setup(16,GPIO.OUT)
        GPIO.output(16,GPIO.HIGH)
        print("the end")
    except Exception as ee:
             print("erreur gpioev " +str(ee))
             a=input()


