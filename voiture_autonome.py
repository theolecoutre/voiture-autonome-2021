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

from detected_objects import DetectedObject

logging.basicConfig(level=logging.DEBUG)

class VoitureAutonome :

    test="salut"

    def __init__(self, show_video: bool):
        logging.info("VoitureAutonome instanciée !")
        self.__SHOW_VIDEO = show_video

    def initVideoServer(self):
        server_socket = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        socket_address = (config.SERVER_IP,config.SERVER_PORT)
        server_socket.bind(socket_address)
        logging.debug("Socket créée sur %s:%i.", config.SERVER_IP,config.SERVER_PORT)
        server_socket.listen(5)
        logging.info("Attente de la connexion du client...")
        self.client_socket,addr = server_socket.accept()

    def initObjectDetection(self):
        startTime = time.time()
        self.interpreter = tflite.Interpreter(model_path=config.PATH_TO_MODEL, experimental_delegates=[tflite.load_delegate(config.TPU_LIB)])
        with open(config.PATH_TO_LABELS, 'r') as f:
            self.labels = [line.strip() for line in f.readlines()]
        endTime=time.time()
        logging.info("Modèle chargé en {} secondes".format(endTime-startTime))

        self.interpreter.allocate_tensors()
        self.inputDetails = self.interpreter.get_input_details()
        self.outputDetails = self.interpreter.get_output_details()
        
        self.videoStream = cv2.VideoCapture(0)
        self.videoWidth = int(self.videoStream.get(3))
        self.videoHeight = int(self.videoStream.get(4))

        logging.debug("VideoStream lancé, dimensions : %i x %i", self.videoWidth, self.videoHeight)

    def processObjectDetectionOnFrame(self, frame, boxes, classes, scores):
        detectedObjects = []
        for i in range(len(scores)):
                if ((scores[i] > config.MIN_CONF_THRESH) and (scores[i] <= 1.0)):

                    ymin = int(max(1,(boxes[i][0] * self.videoHeight)))
                    xmin = int(max(1,(boxes[i][1] * self.videoWidth)))
                    ymax = int(min(self.videoHeight,(boxes[i][2] * self.videoHeight)))
                    xmax = int(min(self.videoWidth,(boxes[i][3] * self.videoWidth)))

                    object = DetectedObject(self.labels[int(classes[i])], xmax-xmin, ymax-ymin, self.videoWidth)

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

    def invokeModelOnFrame(self, frame):
        frameRGB = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        frameResized = cv2.resize(frameRGB, (self.inputDetails[0]['shape'][2], self.inputDetails[0]['shape'][1]))
        inputData = np.expand_dims(frameResized, axis=0)

        self.interpreter.set_tensor(self.inputDetails[0]['index'], inputData)

        self.interpreter.invoke()

        boxes = self.interpreter.get_tensor(self.outputDetails[0]['index'])[0]
        classes = self.interpreter.get_tensor(self.outputDetails[1]['index'])[0]
        scores = self.interpreter.get_tensor(self.outputDetails[2]['index'])[0]
        return (boxes, classes, scores)



    def drive(self):
        if self.__SHOW_VIDEO:
            self.initVideoServer()
        self.initObjectDetection()
        while self.videoStream.isOpened():
            ret, frame = self.videoStream.read()
            
            boxes, classes, scores = self.invokeModelOnFrame(frame)

            frame = self.processObjectDetectionOnFrame(frame, boxes, classes, scores)


            if self.__SHOW_VIDEO:
                toSend = cv2.resize(frame, (0,0), fx=0.35, fy=0.35) 
                a = pickle.dumps(toSend)
                message = struct.pack("Q", len(a))+a
                self.client_socket.sendall(message)

            """ if self.__SHOW_VIDEO:
                cv2.imshow("Détection d'objets",frame)

                if cv2.waitKey(1) == ord('q'):
                    break """