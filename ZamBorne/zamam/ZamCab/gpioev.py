
#/usr/bin/python
import time
import RPi.GPIO as GPIO

if __name__ == "__main__":
    try:
        GPIO.setmode(GPIO.BOARD)
        # set 3
        GPIO.setup(35,GPIO.OUT,)
        GPIO.setup(36, GPIO.OUT)
        GPIO.output(35, GPIO.LOW)
        print(" GPIO 19 LOW")
        GPIO.output(36, GPIO.LOW)
        print(" GPIO 16 LOW")
        time.sleep(.5)
        GPIO.output(35,GPIO.HIGH)
        print(" GPIO 19 HIGH")
        GPIO.output(36, GPIO.HIGH)
        print(" GPIO 16 HIGH")
        """
        time.sleep(.5)
        #set 2
        GPIO.output(35, GPIO.HIGH)
        print(" GPIO 19 HIGH")
        j=10
        while(j>0):
            time.sleep(1)
            GPIO.output(37, GPIO.LOW)
            print(" GPIO 26 LOW")
            j=j-1
        """
        print("the end")
    except Exception as ee:
             print("erreur gpioev " +str(ee))
             a=input()


