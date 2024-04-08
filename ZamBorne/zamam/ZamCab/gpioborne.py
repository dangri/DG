
#/usr/bin/python
import time
import RPi.GPIO as GPIO



if __name__ == "__main__":
    try:
        GPIO.setmode(GPIO.BOARD)
        GPIO.setup(16,GPIO.IN)
        if (GPIO.input(16) ==GPIO.HIGH):
            print(" pin 16 high")
        else:
            print(" pin 16 low")
        print("the end")
    except Exception as ee:
             print("erreur gpioev " +str(ee))
             a=input()


