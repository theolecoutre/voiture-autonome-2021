import voiture_autonome
#from server_communication import ServerCommunication
a = voiture_autonome.VoitureAutonome(show_video=True)
# #a.initObjectDetection()
a.drive()

# s = ServerCommunication()
# s.bindListenAccept()
# s.receiveLocation()
