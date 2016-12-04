// Final Project
// Server Header

#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H

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

typedef struct Client
{
	int is_open; // free slot
	int sockd; // client_socket
	char channel; // client channel
	char uname[UNAME_MAX];
} cl;

void login(int conn_fd, int max_fd, int id);
void readPacket(char packet[MAX]);

#endif
