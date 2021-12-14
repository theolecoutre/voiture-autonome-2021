#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#ifdef WIN32
#include <windows.h>
#else
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <semaphore.h>
#include <time.h>
#endif // WIN32

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "sendLocation.h"

#define SERVER_PATH "socketLocation"
#define BUFFER_LENGTH 250
#define FALSE 0

bool terminateProgram=false;

#ifdef WIN32
BOOL CtrlHandler( DWORD fdwCtrlType )
{
    if ((fdwCtrlType==CTRL_C_EVENT ||
            fdwCtrlType==CTRL_BREAK_EVENT ||
            fdwCtrlType==CTRL_CLOSE_EVENT ||
            fdwCtrlType==CTRL_LOGOFF_EVENT ||
            fdwCtrlType==CTRL_SHUTDOWN_EVENT) &&
            (terminateProgram==false))
    {
        terminateProgram=true;
        return true;
    }
    else return false;
}
#else
void CtrlHandler(int signum)
{
    terminateProgram=true;
}
#endif

#ifdef WIN32
void sleep(unsigned int seconds)
{
    Sleep (seconds*1000);
}
#endif


#ifdef WIN32
HANDLE ghSemaphore;
DWORD dwSemWaitResult;
void semCallback()
{
    ReleaseSemaphore(
        ghSemaphore,  // handle to semaphore
        1,            // increase count by one
        NULL);
}
#else
// Linux
static sem_t *sem;
struct timespec ts;
void semCallback()
{
	sem_post(sem);
}
#endif // WIN32


int establishConnectionServer()
{
    int sock;
    struct sockaddr_in adrServer;
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
    // get port name from command line arguments (if specified)
    const char * ttyFileName;
    if (argc==2) ttyFileName=argv[1];
    else ttyFileName=DEFAULT_TTY_FILENAME;

    int rc;

    int socketServer, socketPython;
    struct PositionValue currentLocation;
    char buffer[MAX_BUFFER_LENGTH + 1];

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
#ifdef WIN32
    SetConsoleCtrlHandler( (PHANDLER_ROUTINE) CtrlHandler, TRUE );
#else
    signal (SIGINT, CtrlHandler);
    signal (SIGQUIT, CtrlHandler);
#endif

#ifdef WIN32
    ghSemaphore = CreateSemaphore(
        NULL, // default security attributes
        10,  // initial count
        10,  // maximum count
        NULL);          // unnamed semaphore
    if (ghSemaphore == NULL)
    {
        printf("CreateSemaphore error: %d\n", (int) GetLastError());
        return 1;
    }
#else
	// Linux
	sem = sem_open(DATA_INPUT_SEMAPHORE, O_CREAT, 0777, 0);
#endif

    // Main loop
    socketServer = establishConnectionServer();
    socketPython = establishConnectionPythonServer();
    while ((!terminateProgram) && (!hedge->terminationRequired))
    {
        //sleep (3);
        #ifdef WIN32
        dwSemWaitResult = WaitForSingleObject(
            ghSemaphore,   // handle to semaphore
            1000); // time-out interval
        #else
        // Linux
        if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
		{
			printf("clock_gettime error");
			return -1;
		}
		ts.tv_sec += 2;
		sem_timedwait(sem,&ts);
        #endif


        getPositionFromMarvelmindHedge(hedge, &currentLocation);
        bzero(buffer, sizeof(buffer));
        sprintf (buffer, "Address: %d, X: %.3d, Y: %.3d, Z: %.3d, Angle: %.1f  at time T: %u",
                            currentLocation.address, currentLocation.x, currentLocation.y, currentLocation.z, currentLocation.angle, currentLocation.timestamp);
        write(socketServer, buffer, sizeof(buffer));
        fflush(stdout);
        printf ("%s\n", buffer);


        // sending to python program
        rc = send(socketPython, &currentLocation, sizeof(currentLocation), 0);
        if (rc < 0)
        {
            perror("send() failed");
        }
        
        // printStationaryBeaconsPositionsFromMarvelmindHedge (hedge, true);

        // printRawDistancesFromMarvelmindHedge(hedge, true);

        // printRawIMUFromMarvelmindHedge(hedge, true);

        // printFusionIMUFromMarvelmindHedge(hedge, true);

        // printTelemetryFromMarvelmindHedge(hedge, true);

        // printQualityFromMarvelmindHedge(hedge, true);
    }

    // Exit
    stopMarvelmindHedge (hedge);
    destroyMarvelmindHedge (hedge);
    return 0;
}
