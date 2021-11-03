#include "auxiliarFunctions.h"

#define ERROR               -1


void check_error (char *msg, int val1, int val2)
{
    if (val1==val2)
    {
        perror(msg);
        exit(ERROR);
    }
}
