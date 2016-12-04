// Main Project
// ComPars Header

#ifndef COMPAR_H
#define COMPAR_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0

int comPar(char *dataPack, int *offset, char* comTag, char* data);
int findCom(char *dataSeg);

#endif
