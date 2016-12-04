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
char toUser[15] = " ";
char  username[15] = " ";
int idFromServer;
int src = 000;
int dest = 255;

char * login(int sockfd);

int main(int argc, char ** argv)
{
	int sockfd,len,ret;
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

	printf("trying to connect\n");
	ret = connect(sockfd,(struct sockaddr *)&saddr,len);
	if(ret == -1)
	{
		perror("connect ");
		exit(1);
	}//end of if statement for connection
	bzero(toUser, 15);
	bzero(username, 15);
	printf("Trying to login\n");
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
	char packet[44] = " ";
	fgets(username, 15, stdin);
	printf("username: %s\n", username);
	pack(1, room, src, dest, username, packet);
	write(sockfd, packet, 50);
	char buffer[50];
	read(sockfd, buffer, 50);
	src = unpackDese(buffer);
	return username;
	//end getting username
}//end o login

/*
 * This method just reads the messages from the server and prints it with out parsing it.
 */
void clientReader(int * socketfd)
{
	//TODO:
	char  buffer[1024];
	char packet[50] = " ";
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
		switch(unpackType(buffer))
		{
			case 4:
				room = unpackChan(buffer);
				printf("-->%s\n", buffer);
				break;
			case 5:
			case 6:
			case 7:
			case 8:
				unpackMess(packet, buffer);
				printf("->%s\n", packet);
				bzero(packet, 50);
				break;
			case 9:
				break;
			default:
				printf("-->%s", buffer);
				break;
		}//end of switch
		bzero(buffer, 1024);
		//buffer[0] = '\0';
	}//end of outer while
}//end of client Reader

/*
 * This method writes the input from the client and parses it so the server can read it
 */
void clientWriter(int *socketfd)
{
	//TODO:
	char buffer[40];
	char packet[] = " ";
	int writer = 0;
	while(1)
	{
		fgets(buffer, 40, stdin);
		if(buffer[0] == '$')
		{
			//TODO: create a call to figure out what the escape character wants to do.
			pack(3, room, src, dest, buffer, packet);
			printf("%s\n", packet);
			write(*socketfd, packet, 50);
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
			pack(0, room, src, dest, buffer, packet);
			printf("%s: is now going write\n", packet);
			writer = write(*socketfd, packet, 40);
			//free(packet);
		}//end of else
		bzero(buffer, 40);


	}//end of outer while
}//end of clientwriter

