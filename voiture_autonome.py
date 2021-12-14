import logging
from numpy.__config__ import show
import tflite_runtime.interpreter as tflite
import time
import config
import cv2
import numpy as np
import socket
import struct
import pickle
from numpy import dot

import threading

from detected_objects import DetectedObject
import serial_communicator

from communication.raspberry.interface_c_python_commands import SocketCommands
from communication.raspberry.interface_c_python_location import SocketLocation


logging.basicConfig(level=logging.DEBUG)


class VoitureAutonome :

    def __init__(self, show_video: bool):
        logging.info("VoitureAutonome instanciée !")
        self.__SHOW_VIDEO = show_video
        self.vitesse = 10 #on initialise la vitesse à 0, la voiture reste à l'arrêt
        self.direction = 90 #on initialise l'angle de braquage à 90, le point milieu
        self.attente = False #variable indiquant si la voiture est en attente (v=0 en attendant)
        self.attenteStart = 0 #le timestamp auquel l'attente a débuté
        
        self.serial_communicator = serial_communicator.SerialCommunicator()


        self.interfaceLocation = SocketLocation()
        self.interfaceCommands = SocketCommands()
        self.lancerCommunicationC()


    def lancerCommunicationC(self):
        t1 = threading.Thread(target=self.interfaceLocation.run)
        t2 = threading.Thread(target=self.interfaceCommands.run)

        t1.start()
        t2.start()

        
    def initVideoServer(self): #démarre la socket vidéo et attend qu'un client se connecte
        server_socket = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        socket_address = (config.SERVER_IP,config.SERVER_PORT)
        server_socket.bind(socket_address)
        logging.debug("Socket créée sur %s:%i.", config.SERVER_IP,config.SERVER_PORT)
        server_socket.listen(5)
        logging.info("Attente de la connexion du client...")
        self.client_socket,addr = server_socket.accept()

    def initObjectDetection(self): #charge la modèle en cache et démarre le flux vidéo
        startTime = time.time()
        self.interpreter = tflite.Interpreter(model_path=config.PATH_TO_MODEL, experimental_delegates=[tflite.load_delegate(config.TPU_LIB)])
        with open(config.PATH_TO_LABELS, 'r') as f:
            self.labels = [line.strip() for line in f.readlines()]
        endTime=time.time()
        logging.info("Modèle chargé en {} secondes".format(endTime-startTime))

        self.interpreter.allocate_tensors()
        self.inputDetails = self.interpreter.get_input_details()
        self.outputDetails = self.interpreter.get_output_details()
        
        self.videoStream = cv2.VideoCapture(-1)
        self.videoWidth = int(self.videoStream.get(3))
        self.videoHeight = int(self.videoStream.get(4))

        logging.debug("VideoStream lancé, dimensions : %i x %i", self.videoWidth, self.videoHeight)

    def processObjectDetectionOnFrame(self, frame, boxes, classes, scores): #traite les détections faites sur une image
        detectedObjects = []
        for i in range(len(scores)):
                if ((scores[i] > config.MIN_CONF_THRESH) and (scores[i] <= 1.0)):

                    ymin = int(max(1,(boxes[i][0] * self.videoHeight)))
                    xmin = int(max(1,(boxes[i][1] * self.videoWidth)))
                    ymax = int(min(self.videoHeight,(boxes[i][2] * self.videoHeight)))
                    xmax = int(min(self.videoWidth,(boxes[i][3] * self.videoWidth)))

                    object = DetectedObject(self.labels[int(classes[i])], xmax-xmin, ymax-ymin, self.videoWidth, self)

                    detectedObjects.append(object)

                    if self.__SHOW_VIDEO:

                        cv2.rectangle(frame, (xmin,ymin), (xmax,ymax), (10, 255, 0), 2)

                        objectName = self.labels[int(classes[i])]
                        label = "%s : %d%%" % (objectName, int(scores[i]*100))
                        labelSize, baseLine = cv2.getTextSize(label, cv2.FONT_HERSHEY_SIMPLEX, 0.7, 2)
                        labelYmin = max(ymin, labelSize[1] + 10)
                        cv2.rectangle(frame, (xmin, labelYmin-labelSize[1]-10), (xmin+labelSize[0], labelYmin+baseLine-10), (255, 255, 255), cv2.FILLED)
                        cv2.putText(frame, label, (xmin, labelYmin-7), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 0, 0), 2)
        if (len(detectedObjects)>0):
            print("Objets détectés :")
            for i in range(len(detectedObjects)):
                print("Nom : ", detectedObjects[i].object)
                print("Proche : ",detectedObjects[i].isCloseBy)
            print("----------")
        return frame

    def invokeModelOnFrame(self, frame): #
        frameRGB = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        frameResized = cv2.resize(frameRGB, (self.inputDetails[0]['shape'][2], self.inputDetails[0]['shape'][1]))
        inputData = np.expand_dims(frameResized, axis=0)

        self.interpreter.set_tensor(self.inputDetails[0]['index'], inputData)

        self.interpreter.invoke()

        boxes = self.interpreter.get_tensor(self.outputDetails[0]['index'])[0]
        classes = self.interpreter.get_tensor(self.outputDetails[1]['index'])[0]
        scores = self.interpreter.get_tensor(self.outputDetails[2]['index'])[0]
        return (boxes, classes, scores)
    
    def afficherInfosVoiture(self):
        logging.debug("--- INFORMATIONS VOITURE ---")
        logging.debug("Vitesse : " + str(self.vitesse))
        logging.debug("Angle de braquage : " + str(self.direction))
        logging.debug("Attente : " + str(self.attente))
        logging.debug("Attente start : " + str(self.attenteStart))
        logging.debug("Time : " + str(time.time()))


    def setVitesse(self, vitesse : int):
        if vitesse >= 0: #on vérifie que la vitesse est positive
            if vitesse != self.vitesse: #on vérifie que la vitesse n'est as déjà celle programmée
                if (self.vitesse != 0):
                    self.vitessePrecedente = self.vitesse #on enregistre la vitesse précédente en cas de fin de limitation future
                self.vitesse = vitesse #

    def nextStepConduite(self):
        if (self.interfaceCommands.command[0] == '0'):
                self.interfaceCommands.command = " "
                self.setVitesse(0)
        if self.attente :
            self.setVitesse(0)
            if (time.time() - self.attenteStart > 3): #si l'attente a démarré il y a plus de 3 secondes
                self.setVitesse(self.vitessePrecedente)
                self.attente = False
        return

    def sendInfosToArduino(self):
        msg = '{:0>2}{:0>3}'.format(self.vitesse, self.direction)
        self.serial_communicator.sendMessage(msg)

    def calculatePossibleDirs(self, pannel):
        #to do. 
        location = self.interfaceLocation.location
        possible_dirs = None
        if (pannel == "ALL"):
          possible_dirs.append([self.direction]) 
        elif(pannel == "LEFT"):
          possible_dirs.append([self.direction]) 
            
        elif(pannel == "RIGHT"):
          possible_dirs.append([self.direction]) 
        
        elif(pannel == "FWD_LEFT"):
          possible_dirs.append([self.direction]) 
        
        elif(pannel == "FWD_RIGHT"):
          possible_dirs.append([self.direction]) 


    def chooseADirection(self, pointA, pointB, possible_dirs):
        #pointA, pointB : [x, y]
        #possible_dirs : [[x, y],
        #                 [x, y]...]
        # vector distance = (yb- ya)^2 + (xb - xa)^2
        # d(distance)/dxa = -2(xb - xa)
        # d(distance)/dya = -2(yb - ya)
        max_dot = 0
        best_dir = []
        gradient = [-2*(pointB[0] - pointA[0]), -2*(pointB[1] - pointA[1])]
        
        for dir in possible_dirs:
            aux = dot(gradient, dir)
            if (aux > max_dot):
                max_dot = aux
                best_dir = dir
        
        return best_dir


    def drive(self):
        if self.__SHOW_VIDEO:
            self.initVideoServer()
        self.initObjectDetection()
        while self.videoStream.isOpened():
            ret, frame = self.videoStream.read()

            frame = cv2.rotate(frame, cv2.ROTATE_180)
            
            boxes, classes, scores = self.invokeModelOnFrame(frame)

            frame = self.processObjectDetectionOnFrame(frame, boxes, classes, scores)

            self.nextStepConduite()
            self.sendInfosToArduino()
            


            if self.__SHOW_VIDEO:
                toSend = cv2.resize(frame, (0,0), fx=0.35, fy=0.35) 
                a = pickle.dumps(toSend)
                message = struct.pack("Q", len(a))+a
                self.client_socket.sendall(message)

            """ if self.__SHOW_VIDEO:
                cv2.imshow("Détection d'objets",frame)

                if cv2.waitKey(1) == ord('q'):
                    break """
            self.afficherInfosVoiture()