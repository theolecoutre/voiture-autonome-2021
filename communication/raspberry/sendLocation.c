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

int establishConnection()
{
    int sock;
    struct sockaddr_in adrRBPI;
    int choice;
    char *errorStr;

    char buffer [MAX_BUFFER_LENGTH + 1];

    sock = socket(AF_INET, SOCK_STREAM, 0);

    check_error("Error opening socket\n", sock, ERROR);

    adrRBPI.sin_family      = AF_INET;
    adrRBPI.sin_port        = htons(REMOTE_PORT);
    adrRBPI.sin_addr.s_addr = inet_addr(REMOTE_IP);

    check_error("Connect Error\n", connect(sock, (const struct sockaddr *) &adrRBPI, sizeof(adrRBPI)), ERROR);

    return sock;
}

int main (int argc, char *argv[])
{

    char buffer [MAX_BUFFER_LENGTH + 1]; 
    int sock;
    // "Address: %d, X: %.3f, Y: %.3f, Z: %.3f, Angle: %.1f  at time T: %u\n"
    int address;
    float x, y, z, angle;
    uint32_t time;

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
    sock = establishConnection();

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
        printPositionFromMarvelmindHedge (hedge, true, &address, &x, &y, &z, &angle, &time);   
        bzero(buffer, sizeof(buffer));
        sprintf (buffer, "%d;%.3f;%.3f;%.3f;%.1f;%u", address, x, y, z, angle, time);
        sendABuffer(sock, buffer);
    }

    // Exit
    stopMarvelmindHedge (hedge);
    destroyMarvelmindHedge (hedge);
    return 0;
}
