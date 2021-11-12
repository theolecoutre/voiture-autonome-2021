#ifndef SEND_COMMANDS_H
#define SEND_COMMANDS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "../auxiliarFunctions.h"

#define ERROR               -1
#define REMOTE_PORT         3000
#define REMOTE_IP           "127.0.0.1"
#define MAX_BUFFER_LENGTH   80

void sendStartVoiture();
void sendStopVoiture();
void sendCoordinates();

#endif