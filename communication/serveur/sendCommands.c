#include "sendCommands.h"


void printMenu ()
{
    printf ("Bienvenue à la voiture\n");
    printf ("1-Envoyer les coordonées de la destination\n");
    printf ("2-Arreter la voiture\n");
    printf ("3-Démarrer la voiture\n");
    printf ("Faites votre choix : ");
}

void sendCoordinates()
{

}

void sendStopVoiture()
{
    
}

void sendStartVoiture()
{
    
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
        clearBreakLine(buffer);

        choice = (int) strtol(buffer, &errorStr, 10);

        /*tell RaspberryPi which option the user chose, to listen correctly*/
        sendABuffer(sock, buffer);

        switch(choice)
        {
            case 1:
                sendCoordinates();
                break;

            case 2:
                sendStopVoiture();
                break;

            case 3:
                sendStartVoiture();
                break;

            default:

        }


    }


    return 0;
}
