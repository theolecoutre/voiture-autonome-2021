#include "sendCommands.h"


void printMenu ()
{
    printf ("Bienvenue a la voiture\n");
    printf ("1-Envoyer une coordonee destination\n");
    printf ("2-Arreter voiture\n");
    printf ("Choix ");
}

void sendCoordinates(int sock)
{
    char buffer[MAX_BUFFER_LENGTH + 1];
    
    printf ("X= ");
    bzero(buffer, sizeof(buffer));
    fgets(buffer, sizeof(buffer), stdin);
    clearBreakLine(buffer);
    sendABuffer(sock, buffer);
    

    printf ("Y= ");
    bzero(buffer, sizeof(buffer));
    fgets(buffer, sizeof(buffer), stdin);
    clearBreakLine(buffer);
    sendABuffer(sock, buffer);


    printf ("Z= ");
    bzero(buffer, sizeof(buffer));
    fgets(buffer, sizeof(buffer), stdin);
    clearBreakLine(buffer);
    sendABuffer(sock, buffer);

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
    printf ("Connected to the RaspberryPi\n");

    for (;;)
    {
        printMenu();
        fflush(stdin);
        fgets(buffer, MAX_BUFFER_LENGTH + 1, stdin);
        clearBreakLine(buffer);
        sendABuffer(sock, buffer);

        choice = (int) strtol(buffer, &errorStr, 10);
        /*tell RaspberryPi which option the user chose, to listen correctly*/

        switch(choice)
        {
            case 1:
                sendCoordinates(sock);
                break;

            case 2:
                sendABuffer(sock, "0");
                break;

            case 3:
                sendStartVoiture();
                break;

            default:
                break;

        }


    }


    return 0;
}
