// Final Project - CSCD 330
// Server - Written by Rodney Thomas

#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
// Brads files
#include "packetmanager.h"
#include "compar.h"

#define SERVER_SIZE 16 // max number of users
#define UNAME_MAX 15 // max uname allowed
#define MAX 50 // constant packet size
#define MAX_MESS 42 // max message size within packet

typedef struct Client
{
	int is_open; // free slot
	int id; // client's socket
	char channel; // client's  channel
	char uname[UNAME_MAX]; // client's username
} cl;

// login get username -> give id
void login(int conn_fd, int max_fd, int id);

// delegate what to do with packet (logout, sendPacket, commandPacket)
void readPacket(fd_set *rfd, char packet[MAX], int max_fd, int j);

// simple logout
void logout(fd_set *rfd, char chan, int source, int max_fd, int j);

// send packet to current channel users
void sendPacket(char chan, int source, char data[MAX_MESS], int max_fd, int j);

// use comPar to figure out what client wants to do and then do it ($ch, $pm, $ul)
void commandPacket(char packet[MAX], int source, char chan, char msg[MAX_MESS], int max_fd, int j);

#endif
