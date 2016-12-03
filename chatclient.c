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

	login(sockfd);

	//This part to set the default user to send incase of private message
	for(index = 0; index < 4; index++)
	{
		toUser[index] = 'z';
	}//end of for loop to create a default

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
	return username;
	//end getting username
}//end o login

char* packetbuilder(int command, char * message)
{//TODO: take the command and message to build a packet and send it back so it can be written
	char packet[50];
	int index =0;
	packet[0]=(char)command;
	packet[1]=room;
	for(index =2; index < 6; index++)
	{
		packet[index] = username[index-2];
	}//end of for loop to add the user name
	for(index = 6; index < 10; index ++)
	{
		packet[index] = toUser[index-6];
	}//end of for loop to add a toUser to the packet
	for(index = 10; index < 50 && message[index -10] != '\0'; index++)
	{
		packet[index] = message[index -10];
	}//end of forloop to build the package
	printf("Finish  building the packet\n");
	return &packet;
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
	char packet[44];
	int writer = 0;
	while(1)
	{
		fgets(buffer, 40, stdin);
		if(buffer[0] == '$')
		{
			//TODO: create a call to figure out what the escape character wants to do.
		}//end of if statement

		if(strcmp(buffer, "quit\n") == 0 || writer == -1 || stillConnected == 0)
		{
			pthread_mutex_lock(&clilock);
			write(*socketfd, "Bye!", strlen("Bye!") * sizeof(char));
			stillConnected = 0;
			pthread_mutex_unlock(&clilock);
			return;
		}//end of if

		writer = write(*socketfd, buffer, 40);
		bzero(buffer, 40);


	}//end of outer while
}//end of clientwriter

