#ifndef AUXILIAR_FUNCTIONS_H
#define AUXILIAR_FUNCTIONS_H

#define ERROR               -1

void check_error (char *msg, int val1, int val2);
void clearBreakLine (char *string);
void sendABuffer(int sock, char *buffer);

#endif