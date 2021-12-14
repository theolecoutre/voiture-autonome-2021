#include "receiveLocation.h"

int main ()
{
    int sock, sock2;
    int length, received;
    struct sockaddr_in adrServer, adrRBPI;

    char buffer [MAX_BUFFER_LENGTH + 1];

    sock = socket(AF_INET, SOCK_STREAM, 0);

    check_error("Error opening socket\n", sock, ERROR);
    printf ("Socket open\n");

    adrServer.sin_family      = AF_INET;
    adrServer.sin_port        = htons(LOCAL_PORT);
    adrServer.sin_addr.s_addr = INADDR_ANY;

    check_error("Bind Error\n", bind(sock, (const struct sockaddr *) &adrServer, sizeof(adrServer)), ERROR);
    printf ("Socket Binded\n");

    printf ("Listening...\n");
    check_error("Listen Error\n", listen(sock, MAX_CONN), ERROR);
    
    length = sizeof(adrRBPI);
    check_error("Accept Error\n", sock2 = accept(sock, (struct sockaddr *) &adrRBPI,  &length), ERROR);
    
    printf ("Accepted Connection\n");

    for (;;)
    {
        bzero(buffer, sizeof(buffer));
        received = read(sock2, buffer, sizeof(buffer));
        if (received != 0)
        {
            printf ("Loc: %s\n", buffer);
        }
    }


    return 0;
}
