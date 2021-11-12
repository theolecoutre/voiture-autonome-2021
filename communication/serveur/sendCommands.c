#include "sendCommands.h"
#include "../auxiliarFunctions.c"


void printMenu ()
{
    printf ("Bienvenue a la voiture\n");
    printf ("1-Envoyer une coordonee destination\n");
    printf ("2-Arreter voiture\n");
    printf ("3-Demarrer voiture\n");
    printf ("Choix : ");
}

char* sendCoordinates(char *buffer)
{   
    char x[10],y[10],z[10];
    float X,Y,Z;
    printf("X=");
    fgets(x, 11, stdin);
    X = atof(x);
    printf("Y=");
    fgets(y, 11, stdin);
    Y = atof(y);
    printf("Z=");
    fgets(z, 11, stdin);
    Z = atof(z);
    sprintf(buffer,"Changer la destination:X=%f,Y=%f,Z=%f.\n",X,Y,Z);
    return buffer;
}

char* sendStopVoiture(char *buffer)
{
    buffer = "Stop!\n";
}

char* sendStartVoiture(char *buffer)
{
    buffer = "Start!\n";
}

int main ()
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

    for (;;)
    {
        printMenu();
        fgets(buffer, MAX_BUFFER_LENGTH + 1, stdin);
        

        choice = (int) strtol(buffer, &errorStr, 10);
        
        /*tell RaspberryPi which option the user chose, to listen correctly*/
        sendABuffer(sock, buffer);
        clearBreakLine(buffer);
        switch(choice)
        {
            case 1:
                printf("1\n");
                strcpy(buffer,sendCoordinates(buffer));
                // buffer = sendCoordinates(buffer);
                sendABuffer(sock, buffer);
                break;

            case 2:
                strcpy(buffer,sendStopVoiture(buffer));
                // buffer = sendStopVoiture(buffer);
                sendABuffer(sock, buffer);
                break;

            case 3:
                strcpy(buffer,sendStartVoiture(buffer));
                // buffer = sendStartVoiture(buffer);
                sendABuffer(sock, buffer);
                break;

            // default:

        }


    }


    return 0;
}
