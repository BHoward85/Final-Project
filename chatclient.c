/*
 * chatclient.c
 *
 *  Created on: Oct 20, 2016
 *      Author: marco
 */


#include"chatclient.h"

pthread_mutex_t clilock;
int stillConnected = 1;
char room = 'g';
char toUser[4];
char  username[4];
int idFromServer;

char * login(int sockfd);
char * packetbuilder(int command, char * message);

int main(int argc, char ** argv)
{
	int sockfd,len,ret, index =0;
	struct sockaddr_in saddr;
	stillConnected = 1;
	pthread_t clReader, clWriter;
	if(argc != 3)
	{
		perror("argv ");
		exit(0);
	}//end of if


	// creating tcp socket
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(atoi(argv[2]));

	//saddr.sin_addr = *(struct in_addr *) *(hostinfo->h_addr_list);
	saddr.sin_addr.s_addr = inet_addr(argv[1]);
	len = sizeof(saddr);


	ret = connect(sockfd,(struct sockaddr *)&saddr,len);
	if(ret == -1)
	{
		perror("connect ");
		exit(1);
	}//end of if statement for connection

	//This part to set the default user to send incase of private message
	for(index = 0; index < 4; index++)
	{
		toUser[index] = 'z';
	}//end of for loop to create a default

	login(sockfd);

	printf("Socket:%d\n", sockfd);
	pthread_create(&clReader, 0, (void *)clientReader, &sockfd);
	pthread_create(&clWriter, 0, (void *)clientWriter, &sockfd);
	pthread_join(clReader, NULL);
	pthread_join(clWriter, NULL);
	close(sockfd);

	return 0;
}//end of main

char * login(int sockfd)
{
	//This is where the client gets the username and writes it
	//TODO: get username from server and return that one
	printf("please type your user name:\n");
	fgets(username, 4, stdin);
	write(sockfd, packetbuilder(1, "login"), 50);
	char buffer[4];
	read(sockfd, buffer, 4);
	idFromServer = atoi(buffer);
	return username;
	//end getting username
}//end o login

char* packetbuilder(int command, char * message)
{//TODO: take the command and message to build a packet and send it back so it can be written
	printf("starting to build packet\n");
	char * packet = (char *)calloc(50, sizeof(char));
	//bzero(packet, 50);
	char charcommand = command + '0';
	char charidFromServer = idFromServer + '0';
	int index =0;
	strncat(packet, &charcommand, sizeof(char));
	strncat(packet, &room, sizeof(char));
	strncat(packet, &charidFromServer, sizeof(int));
	printf("entering for loops\n");
	for(index = 0; index < 4; index ++)//Note for some reason can't do the math to offset come back later when not stupid
	{
		strncat(packet , &toUser[index], sizeof(char) * 4);
	}//end of for loop to add a toUser to the packet
	for(index = 0; index < 40 && message[index] != '\0'; index++)
	{
		strncat(packet, &message[index], sizeof(char));
	}//end of forloop to build the package
	printf("Finish  building the packet: %s\n", packet);
	return packet;
}//end of packetbuilder

void clientReader(int * socketfd)
{
	//TODO:
	char  buffer[1024];
	int reader = 1;
	while(1)
	{
		reader = read(* socketfd, buffer, 40);
		//pthread_mutex_lock(&clilock);
		if(reader <= 0 || stillConnected == 0)
		{
			pthread_mutex_lock(&clilock);
			stillConnected = 0;
			pthread_mutex_unlock(&clilock);
			return;
		}//end of if
		//pthread_mutex_unlock(&clilock);
		printf("-->%s", buffer);
		bzero(buffer, 1024);
		//buffer[0] = '\0';
	}//end of outer while
}//end of client Reader

void clientWriter(int *socketfd)
{
	//TODO:
	char buffer[40];
	//char packet[44];
	int writer = 0;
	while(1)
	{
		fgets(buffer, 40, stdin);
		if(buffer[0] == '$')
		{
			//TODO: create a call to figure out what the escape character wants to do.
		}//end of if statement
		else if(strcmp(buffer, "quit\n") == 0 || writer == -1 || stillConnected == 0)
		{
			pthread_mutex_lock(&clilock);
			write(*socketfd, "Bye!", strlen("Bye!") * sizeof(char));
			stillConnected = 0;
			pthread_mutex_unlock(&clilock);
			return;
		}//end of if
		else
		{
			writer = write(*socketfd, packetbuilder(0, buffer), 40);
		}//end of else
		bzero(buffer, 40);


	}//end of outer while
}//end of clientwriter

