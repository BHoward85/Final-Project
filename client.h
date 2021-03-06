// Main Project
// Client Header

#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <pthread.h>
#include "packetManager.h"

#define TRUE 1
#define BUFF_SIZE 80

void login(int sockfd, char *name);
void *reader(void *sockfd);
void *writer(void *sockfd);

#endif
