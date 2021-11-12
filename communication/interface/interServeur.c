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


    ////////////////////select/////////
    fd_set allsockets;
    FD_ZERO(&allSockets);
    FD_SET(sock, &allSockets);

    while (1)
    {
        fd_set readSockets = allSockets;
        fd_set writeSockets = allSockets;
        fd_set errorSockets = allSockets;

        //waiting time
        struct timeval st;
        st.tv_sec = 0;
        st.tv_usec = 0;

        //select
        int nRes = select(2,&readSockets,&writeSockets,&errorSockets,NULL);
        if (0 == nRes) //no socket response
        {
            continue;
        }else if (nRes > 0){
            // Error
            for(u_int i = 0; i < errorSockets.fd_count; i++){
                char str[100] = { 0 };
                int len = 99;
                if (SOCKET_ERROR == getsockopt(errorSockets.fd_array[i], SOL_SOCKET, SO_ERROR, str, &len))
                {
                    printf("Can't get error information\n");
                }
                printf("%s\n", str);
            }

            for (u_int i = 0; i < writeSockets.fd_count; i++)
            {
                printf("服务器%d,%d:可写\n", socketServer, writeSockets.fd_array[i]);
                if (SOCKET_ERROR == send(writeSockets.fd_array[i], "ok", 2, 0))
                {
                    int a = WSAGetLastError();
                }
            }

            //Have response

            for (u_int i = 0; i < readSockets.fd_count; i++){
                if (readSockets.fd_array[i] == sock)
                {
                    //accept
                    SOCKET socketClient = accept(sock, NULL, NULL);
                    if (INVALID_SOCKET == socketClient)
                    {
                        //error
                        continue;
                    }
                    
                    FD_SET(socketClient, &allSockets);
                    //send
                }
                else
                {
                    char strBuf[1500] = { 0 };
                    //Client
                    int nRecv = recv(readSockets.fd_array[i], strBuf, 1500, 0);
                    //send
                    if (0 == nRecv)
                    {
                        //客户端下线了
                        //从集合中拿掉
                        SOCKET socketTemp = readSockets.fd_array[i];
                        FD_CLR(readSockets.fd_array[i], &allSockets);
                        //释放
                        closesocket(socketTemp);
                    }
                }


            }
        }


    }
    ///////////