#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "receiveCommands.h"

#define SERVER_PATH "socketCommands"
#define BUFFER_LENGTH 250
#define FALSE 0

void receiveCoordinates(int sock, coordinates* c)
{
    char buffer [MAX_BUFFER_LENGTH + 1];

    read(sock, buffer, sizeof(buffer));
    c->x = atoi(buffer);
    bzero(buffer, sizeof(buffer));

    read(sock, buffer, sizeof(buffer));
    c->y = atoi(buffer);
    bzero(buffer, sizeof(buffer));

    read(sock, buffer, sizeof(buffer));
    c->z = atoi(buffer);
    bzero(buffer, sizeof(buffer));
    printf ("Received: (x, y, z) - (%d, %d, %d)\n", c->x, c->y, c->z);
}

void receiveStopVoiture(int sock, char buffer[MAX_BUFFER_LENGTH + 1])
{
    bzero(buffer, sizeof(buffer));
    read(sock, buffer, sizeof(buffer));
    printf ("Received: %s\n", buffer);
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
        printf("Client connecté au serveur PYTHON.\n");
    } 

    return sd; 

}

int main ()
{
    int sock, sockServer, sockPython;
    struct sockaddr_in adrRBPI, adrServer;
    int choice, length, rc;
    char *errorStr;
    coordinates newLoc;
    char buffer [MAX_BUFFER_LENGTH + 1];

    sockPython = establishConnectionPythonServer();

    /*establishing connection with the server*/
    sock = socket(AF_INET, SOCK_STREAM, 0);
    
    check_error("Error opening socket\n", sock, ERROR);

    adrRBPI.sin_family      = AF_INET;
    adrRBPI.sin_port        = htons(LOCAL_PORT);
    adrRBPI.sin_addr.s_addr = INADDR_ANY; 

    
    check_error("Bind Error\n", bind(sock, (const struct sockaddr *) &adrRBPI, sizeof(adrRBPI)), ERROR);

    check_error("Listen Error\n", listen(sock, MAX_CONN), ERROR);

    length = sizeof(adrServer);
    check_error("Accept Error\n", sockServer = accept(sock, (struct sockaddr *) &adrServer,  &length), ERROR);

    printf ("Connected to client\n");


    for(;;)
    {

        /*reading the desired command from the server*/
        bzero(buffer, sizeof(buffer));
        
        read(sockServer, buffer, sizeof(buffer));
        printf ("BUFFER:%s\n", buffer);

        choice = (int) strtol(buffer, &errorStr, 10);
        printf ("CHOICE %d\n", choice);
        switch(choice)
        {
            case 1:
                receiveCoordinates(sockServer, &newLoc);
                rc = send(sockPython, &newLoc, sizeof(newLoc), 0);
                if (rc < 0)
                {
                    perror("send() failed");
                }
                break;

            case 2:
                receiveStopVoiture(sockServer, buffer);
                rc = send(sockPython, buffer, sizeof(buffer), 0);
                if (rc < 0)
                {
                    perror("send() failed");
                }
                break;

            default:
                break;

        }

    }
    
    return 0;
}