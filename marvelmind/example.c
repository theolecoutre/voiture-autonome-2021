#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <semaphore.h>
#include <time.h>
#include "marvelmind.h"

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

int main (int argc, char *argv[])
{
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

	// Linux
	sem = sem_open(DATA_INPUT_SEMAPHORE, O_CREAT, 0777, 0);

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


        printPositionFromMarvelmindHedge (hedge, true);
        usleep(50000);
        
        //printStationaryBeaconsPositionsFromMarvelmindHedge (hedge, true);

        //printRawDistancesFromMarvelmindHedge(hedge, true);

        //printRawIMUFromMarvelmindHedge(hedge, true);

        //printFusionIMUFromMarvelmindHedge(hedge, true);

        //printTelemetryFromMarvelmindHedge(hedge, true);

        //printQualityFromMarvelmindHedge(hedge, true);
    }

    // Exit
    stopMarvelmindHedge (hedge);
    destroyMarvelmindHedge (hedge);
    return 0;
}
