import socket
import logging

class SocketServer:

    serverAddress = "testdesocketici"

    def __init__(self):
        self.sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)

        self.sock.bind(self.serverAddress)

        self.sock.listen(1)
        logging.info("Serveur local à l'écoute...")

        while True:
            # Wait for a connection
            connection, client_address = self.sock.accept()
            try:
                # Receive the data in small chunks and retransmit it
                while True:
                    data = connection.recv(250)
                    print('Reçu : "%s"' % data)
                    if data:
                        msg = b"MESSAGE A ENVOYER"
                        connection.send(msg.encode("UTF-8"))
                    else:
                        print('no more data from', client_address)
                        break
                    
            finally:
                # Clean up the connection
                connection.close()