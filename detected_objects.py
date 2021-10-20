import logging
import config

class DetectedObject:

    def __init__(self, object, dx, dy, frameWidth):
        self.object = object
        self.isCloseBy = dx/frameWidth > config.PROXIMITY_RATE