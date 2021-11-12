import socket
import logging

from ctypes import *

class Location(Structure):
    _fields_ = [("address", c_int),
                ("x", c_float),
                ("y", c_float),
                ("z", c_float),
                ("angle", c_float),
                ("time", c_uint32)
                ]

class SocketServer:

    serverAddress = "testdesocketici"

    def __init__(self):
        self.sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)

        self.sock.bind(self.serverAddress)

        self.sock.listen(1)
        logging.info("Serveur local à l'écoute...")

        self.connection, self.client_address = self.sock.accept()
        
        while(1):
            try:
                data = self.connection.recv(250)
                if data:
                    print(f'Reçu : {data}')
                    location_in = Location.from_buffer_copy(data)
                    logging.debug(f"Received: address:{location_in.address}, x:{location_in.x}, y:{location_in.y}, z:{location_in.z}, angle:{location_in.angle}, time:{location_in.time}\n")
            except Exception as e:
                logging.debug (f"Exception {e}")

            

    def receiveLocation(self):
        try:
            data = self.connection.recv(250)
            if data:
                print(f'Reçu : {data}')
                location_in = Location.from_buffer_copy(data)
                logging.debug(f"Received: address:{location_in.address}, x:{location_in.x}, y:{location_in.y}, z:{location_in.z}, angle:{location_in.angle}, time:{location_in.time}\n")
        except Exception as e:
            logging.debug (f"Exception {e}")
        finally:
            self.connection.close()
        
        return location_in

    def receiveCommands(self):
        try:
            data = self.connection.recv(250)
            if data:
                print(f'Reçu : {data.decode()}')

        except Exception as e:
            logging.debug (f"Exception {e}")

        finally:
            self.connection.close()
        return data
