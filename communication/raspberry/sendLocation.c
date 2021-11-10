#include "sendLocation.h"

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
    int sock;
    struct sockaddr_in adrServer;

    bzero(&adrServer, sizeof(adrServer));

    // inet_pton(AF_INET, PYTHON_IP, &adrServer.sin_addr);
    adrServer.sin_addr.s_addr = inet_addr(PYTHON_IP);
    adrServer.sin_port = htons(PYTHON_PORT);
    adrServer.sin_family = AF_INET;


    sock = socket(AF_INET, SOCK_STREAM, 0);
    check_error("Error opening socket\n", sock, ERROR);

    check_error("Connect Error\n", connect(sock, (const struct sockaddr *) &adrServer, sizeof(adrServer)), ERROR);

    printf ("Connected to python server\n");

    return sock;

}

int main (int argc, char *argv[])
{

    char buffer [MAX_BUFFER_LENGTH + 1]; 
    int sock, sockPython;
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
        bzero(buffer, sizeof(buffer));
        sprintf (buffer, "Address: %d, X: %.3f, Y: %.3f, Z: %.3f, Angle: %.1f  at time T: %u\n", l.address, l.x, l.y, l.z, l.angle, l.time);
        // sendABuffer(sock, buffer);

        //sending to python program
        sendMessage(sockPython, &l, sizeof(l));
        usleep(10000);


    }

    // Exit
    stopMarvelmindHedge (hedge);
    destroyMarvelmindHedge (hedge);
    return 0;
}
