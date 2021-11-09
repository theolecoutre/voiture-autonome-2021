import serial,time
import logging

logging.basicConfig(level=logging.DEBUG)


class SerialCommunicator :

    
    def __init__(self, port="/dev/ttyS0"):
        logging.info("SerialCommunicator instanciée !")
        self.arduino = serial.Serial("/dev/ttyS0", 9600, timeout=1)
        time.sleep(0.1) #wait for serial to open

    def sendMessage(self, message):
        if self.arduino.isOpen():
            print(f"{self.arduino.port} connected to Arduino for writing")
            try:
                    self.arduino.write(("0" + message).encode())
                    time.sleep(0.1) #wait for arduino to answer
            except Exception as e:
                print (f"Excpetion {e}")
    
    def readMessage(self):
        if self.arduino.isOpen():
            print(f"{self.arduino.port} connected to Arduino fo reading")
            if  self.arduino.inWaiting()>0: 
                        answer=self.arduino.readline()
                        print(answer)
                        self.arduino.flushInput()
