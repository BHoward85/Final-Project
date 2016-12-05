// Main Project
// Chat Client

#include "client.h"

int src = 0;
int des = 255;
char room = 'g';
int trip = 1;

int main(int argc, char **argv)
{
	int sockfd, len, ret; 
	int n = 0;
	char userName[15] = " ";
	struct sockaddr_in saddr;
	char buff[BUFF_SIZE];
	pthread_t t1;
	pthread_t t2;

	if(argc != 3)
		return 1;
	
	// tcp socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(atoi(argv[2]));
	saddr.sin_addr.s_addr = inet_addr(argv[1]);
	len = sizeof(saddr);
	ret = connect(sockfd, (struct sockaddr *) &saddr, len);
	

	if(ret == -1)
	{
		perror("connect ");
		exit(1);
	}
	
	login(sockfd, userName);	
	pthread_create(&t1, 0, &reader, (void *) &sockfd);
	pthread_create(&t2, 0, &writer, (void *) &sockfd);
	pthread_detach(t1);
	pthread_detach(t2);

	while(TRUE)
	{
		if(trip == 0)
			break;
	}

	return 0;
}

void login(int sockfd, char* name)
{
	char buffer[50] = " ";
	char packet[44] = " ";

	printf("please type your user name:\n");

	read(0, name, 15);

	pack(1, room, src, des, name, packet);

	printf("%s\n", packet);

	write(sockfd, packet, 50);

	read(sockfd, buffer, 50);

	printf("%s\n", buffer);

	src = unpackDese(buffer);
}

void * writer(void * sockfd)
{
	char buff[BUFF_SIZE] = " ";
	char packet[50] = " ";
	int n = 0;

	while(TRUE)
	{
		fgets(buff, 40, stdin);
		if(buff[0] == '$')
		{
			if(strcmp("$logout\n", buff) == 0 || strcmp("$logoff\n", buff) == 0)
			{
				pack(2, room, src, des, buff, packet);
				write(*(int *)sockfd, packet, 50);
				trip = 0;
				pthread_exit(NULL);
			}//end of if
			else
			{
				pack(3, room, src, des, buff, packet);
				printf("%s\n", packet);
				write(*(int *)sockfd, packet, 50);
				bzero(packet, 50);
			}//end of else
		}//end of if statement
		else
		{
			pack(0, room, src, des, buff, packet);
			printf("<--%s: is now going write\n", packet);
			n = write(*(int *)sockfd, packet, 40);
			bzero(packet, 50);
		}//end of else

		bzero(buff, BUFF_SIZE);
	}
}

void * reader(void * sockfd)
{
	char buff[BUFF_SIZE] = " ";
	char packet[50] = " ";
	int n = 0;

	while(TRUE)
	{
		n = read(*(int *)sockfd, buff, 50);
		printf("%s\n", buff);
		switch(unpackType(buff))
		{
			case 3:
				room = unpackChan(buff);
				printf("-->channel changed to: %s%c\n", buff, unpackChan(buff));
				break;
			case 4:
				unpackMess(buff, packet);
				printf("-->PM sent: %s\n", buff);
				break;
			case 5:
				unpackMess(buff, packet);
				printf("-->PM fail: %s\n", buff);
				break;
			case 6:
				room = unpackChan(buff);
				printf("-->channel changed to: %s: %c\n", buff, unpackChan(buff));
				break;
			case 7:
				unpackMess(buff, packet);
				printf("-->channel changed fail: %s\n", buff);
				break;
			case 8:
				unpackMess(packet, buff);
				printf("->%s\n", packet);
				bzero(packet, 50);
				break;
			case 9:
				break;
			default:
				printf("-->%s", buff);
				break;
		}//end of switch
		if(n == 0)
		{
			trip = 0;
			pthread_exit(NULL);
		}
		bzero(buff, BUFF_SIZE);
	}
}
