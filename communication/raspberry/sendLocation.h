#ifndef SEND_LOCATIONS_H
#define SEND_LOCATIONS_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <semaphore.h>
#include <time.h>

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "../auxiliarFunctions.h"
#include "marvelmind/marvelmind.h"

#define ERROR               -1
#define REMOTE_PORT         3000
#define REMOTE_IP           "127.0.0.1"
#define MAX_BUFFER_LENGTH   80

void sendStartVoiture();
void sendStopVoiture();
void sendCoordinates();

#endif