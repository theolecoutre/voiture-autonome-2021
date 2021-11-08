#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>


#define SERVER_PATH "testdesocketici"
#define BUFFER_LENGTH 250
#define FALSE 0

void main(int argc, char *argv[])
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

    //Boucle de communication


    while (1)
    {
        //Le client est la premier à envoyer un message
        //Le message est écrit dans buffer
        bzero(buffer, sizeof(buffer));
        fgets(buffer, sizeof(buffer), stdin);

        sprintf("MESSAGE À ENVOYER", buffer, sizeof(buffer));

        rc = send(sd, buffer, sizeof(buffer), 0);
        if (rc < 0)
        {
            perror("send() failed");
        }

        bzero(buffer, sizeof(buffer));
        rc = recv(sd, buffer, sizeof(buffer), 0);
        printf("Reçu : %s\n", buffer);
    };

    if (sd != -1)
        close(sd);
}