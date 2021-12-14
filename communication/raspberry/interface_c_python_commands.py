import socket
import logging
import os
from ctypes import *

class Coordinate(Structure):
    _fields_ = [
                ("x", c_int),
                ("y", c_int),
                ("z", c_int)
                ]

class SocketCommands:

    serverAddress = "socketCommands"

    def __init__(self):
        if os.path.exists("socketCommands"):
            os.remove("socketCommands")
        self.newCoord = None
        self.command = " "
        self.sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        self.sock.bind(self.serverAddress)
    



    def run(self):
        self.sock.listen(1)
        logging.info("Serveur Commands local à l'écoute...")

        self.connection, self.client_address = self.sock.accept()
        
        while(1):
            try:
                data = self.connection.recv(250)
                if data:
                    if (data.decode()[0] == '0'):
                        self.command = data.decode()
                        print (f'received {self.command}')
                    else:
                        self.newCoord = Coordinate.from_buffer_copy(data)
                        print (f'received coordinates: ({self.newCoord.x}, {self.newCoord.y}, {self.newCoord.z})')

            except Exception as e:
                logging.debug (f"Exception {e}")