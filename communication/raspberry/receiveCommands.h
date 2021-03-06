#ifndef RECEIVE_COMMANDS_H
#define RECEIVE_COMMANDS_H

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
#define LOCAL_PORT          3001
#define LOCAL_IP            "127.0.0.1"
#define MAX_BUFFER_LENGTH   80
#define MAX_CONN            1

typedef struct 
{
    int x, y, z;

}coordinates;

void receiveCoordinates();
void receiveStopVoiture();


#endif