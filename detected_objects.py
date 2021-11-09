import logging
from pickle import STRING
from typing import Match
import config
import time

"""
object in {"stop", "zone_30", "fin_zone_30", "limit_50", "limit_80", "limit 110","passage_pieton"}
"""

class DetectedObject:

    def __init__(self, object: str, dx, dy, frameWidth, voitureAutonome):
        self.object = object
        self.isCloseBy = dx/frameWidth > config.PROXIMITY_RATE
        self.voitureAutonome = voitureAutonome
        self.takeDecision()

    def takeDecision(self):
        if (self.object.startswith("zone") or self.object.startswith("limit")): #il s'agit d'un panneau de vitesse
            vitesse = int(self.object.split("_")[1]) #on découpe l'object selon le _ et on récupère la valeur de vitesse
            self.voitureAutonome.setVitesse(vitesse)

        elif(self.object == "stop"): #il s'agit d'un panneau stop
            self.voitureAutonome.attenteStart = time.time()
            self.voitureAutonome.attente = True
            logging.warning("On arrête la voiture")

        #elif(self.object == "passage_pieton"):
            #self.voitureAutonome.vitesse 
            #réduire la vitesse
            #tester si la réduction de vitesse affecte l'instance voiture autonome et non une copie
        
        elif(self.object == "fin_zone_30"): #il s'agit du panneau fin de zone 30
            if self.voitureAutonome.vitesse == 30: #on modifie la vitesse seulement si la voiture roulait à la limitation
                self.voitureAutonome.setVitesse(self.voitureAutonome.vitessePrecedente)