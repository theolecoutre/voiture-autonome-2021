import interface_c_python_location
import interface_c_python_commands
import logging
import os
import threading
logging.basicConfig(level=logging.DEBUG)

if os.path.exists("socketLocation"):
    os.remove("socketLocation")

if os.path.exists("socketCommands"):
    os.remove("socketCommands")
    
interface_loc = interface_c_python_location.SocketLocation()
interface_com = interface_c_python_commands.SocketCommands()

try:

    t1 = threading.Thread(target=interface_loc.run)
    t2 = threading.Thread(target=interface_com.run)

    t1.start()
    t2.start()

    t1.join()
    t2.join()


except Exception as e:
    print (f'Exception {e}')

# socket = interface_c_python_location.SocketServer()
# if os.path.exists("socketCommands"):
#     os.remove("socketCommands")