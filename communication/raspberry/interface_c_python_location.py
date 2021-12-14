import socket
import logging

from ctypes import *

class Location(Structure):
    _fields_ = [("address", c_uint8),
                ("timestamp", c_uint32),
                ("x", c_int32),
                ("y", c_int32),
                ("z", c_int32),
                ("angle", c_double),
                ("highResolution", c_bool),
                ("ready", c_bool),
                ("processed", c_bool)
                ]

class SocketLocation:

    serverAddress = "socketLocation"

    def __init__(self):
        self.location = None
        self.sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        self.sock.bind(self.serverAddress)

        
    def run(self):
        self.sock.listen(1)
        logging.info("Serveur Location local à l'écoute...")

        self.connection, self.client_address = self.sock.accept()
        
        while(1):
            try:
                data = self.connection.recv(250)
                if data: 
                    self.location = Location.from_buffer_copy(data)
                    logging.debug(f"Received: address:{self.location.address}, x:{self.location.x}, y:{self.location.y}, z:{self.location.z}, angle:{self.location.angle}, time:{self.location.timestamp}\n")
            except Exception as e:
                logging.debug (f"Exception {e}")


