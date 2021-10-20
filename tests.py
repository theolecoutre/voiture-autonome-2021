from cv2 import fastAtan2
import voiture_autonome

a = voiture_autonome.VoitureAutonome(show_video=True)
#a.initObjectDetection()
a.drive()