// Final Project
// Server Header

#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_SIZE 16 // max number of users
#define UNAME_MAX 15 // who needs a 15 character uname anyway?
#define MAX 50 // packet size
#define MAX_MESS 42 // max message size

typedef struct Client
{
	int is_open; // free slot
	int id; // client_socket
	char channel; // client channel
	char uname[UNAME_MAX];
} cl;
// login get username -> give id
void login(int conn_fd, int max_fd, int id);
// simple logout
void logout(char chan, int source, int j);
// delegate what to do with packet
void readPacket(char packet[MAX], int j);
// send packet to channel
void sendPacket(char chan, int source, char data[MAX_MESS], int j);
// use comPar to figure out what client wants to do and then do it
void commandPacket(char packet[MAX], int source, int j);

#endif
