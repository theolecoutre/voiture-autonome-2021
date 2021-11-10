#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "sendLocation.h"

#define SERVER_PATH "../testdesocketici"
#define BUFFER_LENGTH 250
#define FALSE 0


bool terminateProgram=false;


void CtrlHandler(int signum)
{
    terminateProgram=true;
}

// Linux
static sem_t *sem;
struct timespec ts;
void semCallback()
{
	sem_post(sem);
}

int establishConnectionServer()
{
    int sock;
    struct sockaddr_in adrServer;
    int choice;
    char *errorStr;

    sock = socket(AF_INET, SOCK_STREAM, 0);

    check_error("Error opening socket\n", sock, ERROR);

    adrServer.sin_family      = AF_INET;
    adrServer.sin_port        = htons(REMOTE_PORT);
    adrServer.sin_addr.s_addr = inet_addr(REMOTE_IP);

    check_error("Connect Error\n", connect(sock, (const struct sockaddr *) &adrServer, sizeof(adrServer)), ERROR);

    return sock;
}

int establishConnectionPythonServer()
{
    int sd = -1, rc;
    char buffer[BUFFER_LENGTH];
    struct sockaddr_un serveraddr;

    //Création du socket

    sd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sd < 0)
    {
        perror("socket() failed");
    }

    //Configuration du socket

    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sun_family = AF_UNIX;
    strcpy(serveraddr.sun_path, SERVER_PATH);

    //Connexion au serveur
    rc = connect(sd, (struct sockaddr *)&serveraddr, SUN_LEN(&serveraddr));
    if (rc < 0)
    {
        perror("connect() failed");
    } else {
        printf("Client connecté au serveur.\n");
    } 

    return sd; 

}

int main (int argc, char *argv[])
{

    char buffer [MAX_BUFFER_LENGTH + 1]; 
    int sock, sockPython;
    int rc;
    // "Address: %d, X: %.3f, Y: %.3f, Z: %.3f, Angle: %.1f  at time T: %u\n"
    location l;

    // get port name from command line arguments (if specified)
    const char * ttyFileName;
    if (argc==2) ttyFileName=argv[1];
    else ttyFileName=DEFAULT_TTY_FILENAME;

    // Init
    struct MarvelmindHedge * hedge=createMarvelmindHedge ();
    if (hedge==NULL)
    {
        puts ("Error: Unable to create MarvelmindHedge");
        return -1;
    }

    hedge->ttyFileName=ttyFileName;
    hedge->verbose=true; // show errors and warnings
    hedge->anyInputPacketCallback= semCallback;
    startMarvelmindHedge (hedge);

    // Set Ctrl-C handler
    signal (SIGINT, CtrlHandler);
    signal (SIGQUIT, CtrlHandler);

	sem = sem_open(DATA_INPUT_SEMAPHORE, O_CREAT, 0777, 0);

    // TCP
    // sock = establishConnectionServer();
    sockPython = establishConnectionPythonServer();

    // Main loop
    while ((!terminateProgram) && (!hedge->terminationRequired))
    {
        //sleep (3);
        
        // Linux
        if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
		{
			printf("clock_gettime error");
			return -1;
		}
		ts.tv_sec += 2;
		sem_timedwait(sem,&ts);


        // "Address: %d, X: %.3f, Y: %.3f, Z: %.3f, Angle: %.1f  at time T: %u\n"
        printPositionFromMarvelmindHedge (hedge, true, &l.address, &l.x, &l.y, &l.z, &l.angle, &l.time);   
        
        //sending to server (ordinateur)
        bzero(buffer, sizeof(buffer));
        sprintf (buffer, "Address: %d, X: %.3f, Y: %.3f, Z: %.3f, Angle: %.1f  at time T: %u\n", l.address, l.x, l.y, l.z, l.angle, l.time);
        sendABuffer(sock, buffer);

        //sending to python program
        rc = send(sockPython, &l, sizeof(l), 0);
        if (rc < 0)
        {
            perror("send() failed");
        }
        usleep(10000);


    }

    //Exit
    stopMarvelmindHedge (hedge);
    destroyMarvelmindHedge (hedge);
    return 0;
}
