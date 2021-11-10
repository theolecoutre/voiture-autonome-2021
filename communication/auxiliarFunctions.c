#include "auxiliarFunctions.h"

void check_error (char *msg, int val1, int val2)
{
    if (val1==val2)
    {
        perror(msg);
        exit(ERROR);
    }
}

void sendABuffer(int sock, char *buffer)
{
    write(sock, buffer, strlen(buffer));
    usleep(1000);
    
}

void clearBreakLine (char *string)
{
    int i;
    int len = strlen(string);
    if (string[len - 1] == '\n')
    {
        string[len - 1] = '\0';
    }

}

void sendMessage (int sock, void *msg, uint32_t msgSize)
{
    write(sock, msg, msgSize);
    usleep(1000);
}
