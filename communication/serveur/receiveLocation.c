#include "receiveLocation.h"

int main ()
{
    int sock;
    struct sockaddr_in adrRBPI;

    char buffer [MAX_BUFFER_LENGTH + 1];

    sock = socket(AF_INET, SOCK_STREAM, 0);

    check_error("Error opening socket\n", sock, ERROR);

    adrRBPI.sin_family      = AF_INET;
    adrRBPI.sin_port        = htons(REMOTE_PORT);
    adrRBPI.sin_addr.s_addr = inet_addr(REMOTE_IP);

    check_error("Connect Error\n", connect(sock, (const struct sockaddr *) &adrRBPI, sizeof(adrRBPI)), ERROR);

    for (;;)
    {
        bzero(buffer, sizeof(buffer));
        read(sock, buffer, sizeof(buffer));
        printf ("Loc: %s\n", buffer);
    }


    return 0;
}
