
#/usr/bin/python
import echuart
import io
import serial

if __name__ == "__main__":
    try:
        fin=False
        ser=None
        vuart=echuart.uart(baudrate=9600,timeout=1)
        vuart.close()
        if(ser==None):
            ser = serial.Serial(port="/dev/ttyAMA0", baudrate=115200, timeout=10)
        while(not fin):
            print("entrer un no de test ")
            ent = int(input())
            if(ent==0): fin=True
            elif(ent==1):
                nok=True
                while(nok):
                    try:
                        r = vuart.ser.readline()
                        resul = r.decode().strip("\"")
                        print(" lecture =" + str(resul))
                    except TimeoutError as t:
                        nok = True
            elif(ent==2):
                #if(vuart.ser.isOpen()):
                    #vuart.close()
                if(not ser.isOpen()):
                    ser.open()
                #resul = vuart.ser.read_until('\n')
                print("readable={0}".format(ser.readable()))


                print("baudrate={0}".format(ser.baudrate))
                ser.xonxoff=True
                print("mis xonxoff a " + str(ser.xonxoff))
                print("setting = {0}".format(ser.get_settings()))
                i=1
                while(i>0):
                    resul=ser.readline()
                    print(" resul={0}".format(resul))
                    i=i-1
                #result=ser.inWaiting()
                #print(" resul={0}".format(str(resul)))
                #result=ser.read(3)
                #print(" resul={0}".format(resul))
            elif(ent==3):

                ser = serial.serial_for_url('loop://', timeout=1)
                sio = io.TextIOWrapper(io.BufferedRWPair(ser, ser))

                sio.write("hello\n")
                sio.flush()  # it is buffering. required to get the data out *now*
                hello = sio.readline()
                print(hello == "hello\n")



    except Exception as ee:
             print("erreur mycable " +str(ee))
             a=input()


