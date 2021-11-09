#Chemin du modèle .tflite
PATH_TO_MODEL = "model/output_tflite_graph_edgetpu.tflite"

#Chemin du fichier de labels
PATH_TO_LABELS = "model/labels.txt"

#Score minimal pour une reconnaissance avérée
MIN_CONF_THRESH = 0.7

#Pourcentage de proximité
PROXIMITY_RATE = 0.5

#TPU libs :
#macOS : libedgetpu.1.dylib
#linux : libedgetpu.so.1

TPU_LIB = "libedgetpu.1.dylib"

#Adresse du serveur de vidéo
SERVER_IP = '172.31.69.100'
SERVER_PORT = 8000