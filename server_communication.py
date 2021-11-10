import logging
import socket
from ctypes import *

class Location(Structure):
    _fields_ = [("address", c_int),
                ("x", c_float),
                ("y", c_float),
                ("z", c_float),
                ("angle", c_float),
                ("time", c_uint32)
                ]


logging.basicConfig(level=logging.DEBUG)


class ServerCommunication :

    def __init__(self):
        self.PORT = 2300
        self.MAX_BUFFER_LENGTH = 80

        self.server_address = ('localhost', self.PORT)
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        self.client_socket = None
        self.client_address = None
    
    def bindListenAccept(self):
        try:
            self.socket.bind(self.server_address)
            logging.debug("Socket binded\n")
        except Exception as e:
            logging.debug(f"Excpetion {e}")

        try:
            self.socket.listen(1)
            logging.debug("Socket listening\n")
        except Exception as e:
            logging.debug(f"Excpetion {e}")

        try:
            self.client_socket, self.client_address = self.socket.accept()
            logging.debug("Socket accepted\n")
        except Exception as e:
            logging.debug(f"Excpetion {e}")
    
    def receiveLocation(self):
        buffer = self.client_socket.recv(self.MAX_BUFFER_LENGTH)
        location_in = Location.from_buffer_copy(buffer)
        logging.debug(f"Received: address:{location_in.address}, x:{location_in.x}, y:{location_in.y}, z:{location_in.z}, angle:{location_in.angle}, time:{location_in.time}\n")
        return location_in

