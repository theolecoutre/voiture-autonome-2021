#include "receiveLocation.h"

int main ()
{
    int sock, sock2;
    int length;
    struct sockaddr_in adrServer, adrRBPI;

    char buffer [MAX_BUFFER_LENGTH + 1];

    sock = socket(AF_INET, SOCK_STREAM, 0);

    check_error("Error opening socket\n", sock, ERROR);

    adrServer.sin_family      = AF_INET;
    adrServer.sin_port        = htons(LOCAL_PORT);
    adrServer.sin_addr.s_addr = inet_addr(LOCAL_IP);

    check_error("Bind Error\n", bind(sock, (const struct sockaddr *) &adrServer, sizeof(adrServer)), ERROR);

    check_error("Listen Error\n", listen(sock, MAX_CONN), ERROR);

    length = sizeof(adrRBPI);
    check_error("Accept Error\n", sock2 = accept(sock, (struct sockaddr *) &adrRBPI,  &length), ERROR);

    for (;;)
    {
        bzero(buffer, sizeof(buffer));
        read(sock2, buffer, sizeof(buffer));
        printf ("Loc: %s\n", buffer);
        usleep(10000);
    }


    return 0;
}
