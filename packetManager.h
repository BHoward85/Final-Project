// Main Project
// packetManager header

#ifndef PACKETMANAGER_H
#define PACKETMANAGER_H

#include "ComPar.h"

#define MAX 50
#define MAX_MESS 42

//     tCsordesdata(44bytes worth) total of 50by
// ie "0A001255Hi to all\0" with size of 50 bytes

void pack(int type, char chan, int sorce, int dese, char* data, char* packet);
void unpackMess(char* packet, char* mess);
int unpackSorce(char* packet);
int unpackDese(char* packet);
int unpackType(char* packet);
char unpackChan(char* packet);
void intToChar(int fromNum, char* toStr);
void charToInt(char* fromStr, int* toNum);

#endif