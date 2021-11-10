#include "receiveCommands.h"

void receiveCoordinates()
{

}

void receiveStopVoiture()
{
    
}

void receiveStartVoiture(){

}

int main ()
{
    int sock, sockServer;
    struct sockaddr_in adrRBPI, adrServer;
    int choice, length;
    char *errorStr;

    char buffer [MAX_BUFFER_LENGTH + 1];

    /*establishing connection with the server*/
    sock = socket(AF_INET, SOCK_STREAM, 0);

    check_error("Error opening socket\n", sock, ERROR);

    adrRBPI.sin_family      = AF_INET;
    adrRBPI.sin_port        = htons(LOCAL_PORT);
    adrRBPI.sin_addr.s_addr = inet_addr(LOCAL_IP); 

    check_error("Bind Error\n", bind(sock, (const struct sockaddr *) &adrRBPI, sizeof(adrRBPI)), ERROR);

    check_error("Listen Error\n", listen(sock, MAX_CONN), ERROR);

    length = sizeof(adrServer);
    check_error("Accept Error\n", sockServer = accept(sock, (struct sockaddr *) &adrServer,  &length), ERROR);

    /*reading the desired command from the server*/
    bzero(buffer, sizeof(buffer));
    read(sock, buffer, sizeof(buffer));

    choice = (int) strtol(buffer, &errorStr, 10);
    
    switch(choice)
    {
        case 1:
            receiveCoordinates();
            break;

        case 2:
            receiveStopVoiture();
            break;

        case 3:
            receiveStartVoiture();
            break;

        default:
            break;

    }

    return 0;
}