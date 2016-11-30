// Main Project
// Server Header

#ifndef SERVER_H
#define SERVER_H

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>
#include <fcntl.h>
#include <string.h>

#define TRUE 1
#define FALSE 0
#define OPEN 0
#define CLOSE 1

typedef struct
{
	int isFree;
	int CID; // client_id
	int CSN; // client_socket_number
	char *name;
}Client;

typedef struct
{
	char *nameOfChan;
	char *chanMess;
	int *userID;
}Channel;

void login(char *packet, int *offset);

void logout(char *packet, int *offset);

void chanSwitch(char *packet, int *offset);

void recPack(char *packet, int *offset);

void sendPack(char *packet, int *offset);

void findUser(char *packet, int *offset);

void userList(char *packet, int *offset);

void pmUser(char *packet, int *offset);
#endif