
#/usr/bin/python
import serial

import echuart
import io
import time

if __name__ == "__main__":
    try:
            ser=None
            vuart=echuart.uart()
            vuart.close()
            if(ser ==None):
                ser = serial.Serial(port="/dev/ttyAMA0", baudrate=115200, xonxoff=True, timeout=1)
            fin=False
            while(not fin):
                print("entrer un no de test ")
                ent = int(input())
                if(ent==0):
                    fin=True
                if(ent==1):
                    payrol="abcd\""
                    bpayrol=payrol.encode()
                    nb=vuart.ser.write(bpayrol)
                    print( "envoi de {0} bytes  payrol={1}".format(nb, bpayrol))
                elif(ent==2):
                    # if(vuart.ser.isOpen()):
                    # vuart.close()
                    if (not ser.isOpen()):
                        ser.open()
                    # resul = vuart.ser.read_until('\n')
                    print("readable={0}".format(ser.readable()))

                    print("baudrate={0}".format(ser.baudrate))
                    ser.xonxoff = True
                    print("mis xonxoff a " + str(ser.xonxoff))
                    print("setting = {0}".format(ser.get_settings()))
                    b="abc\n".encode()
                    nb=ser.write(b)
                    #time.sleep(1)
                    #vuart.ser.flush()
                    #vuart.close()
                    print("b= {0} nb= {1}".format(b, nb))
                    ser.close()
                else:
                    print("test inconnu")
            print("the end")
    except Exception as ee:
             print("erreur mycable " +str(ee))
             a=input()


