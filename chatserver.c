// Final Project
// Server Code

#include "chatserver.h"
#include "packetmanager.h"
#include "compar.h"

cl clientTable[SERVER_SIZE];

int main(int argc, char **argv) 
{
	if(argc < 2)
	{
		fprintf(stderr, "ERROR: Port must be provided!\n");
        exit(1);
	} 
	
	int port = atoi(argv[1]);
	int conn_fd = 0, listen_fd = 0, max_fd = 0;
	int i = 0, j = 0, rec = 0;
	char packet[MAX] = " ";
	struct sockaddr_in serv_addr; 
	struct sockaddr_in cli_addr;
	socklen_t cli_len;
	fd_set rfd, c_rfd;
	FD_ZERO(&rfd);
	FD_ZERO(&c_rfd);
	
	// create socket
	printf("at socket\n");
    if((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "ERROR: Socket Failed!\n");
        exit(1);
    }

    bzero(&serv_addr, sizeof(serv_addr));    
    serv_addr.sin_family = AF_INET;     
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);     
    serv_addr.sin_port = htons(port);
	
	printf("at bind\n");
    // bind socket to address
    if(bind(listen_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
    {
        fprintf(stderr, "ERROR: bind() failed!\n");
        exit(1);
    }
    else
    {
		printf("at listen\n");
        // listen for connections
        if(listen(listen_fd, SERVER_SIZE) == -1)
        {
            fprintf(stderr, "ERROR: listen() failed!\n");
            exit(1);
        }
    }
	
	FD_SET(listen_fd, &rfd);
	// set max_fd for first time
	max_fd = listen_fd;
	
	while(1)
	{
		c_rfd = rfd;
		printf("before select\n");
		if(select(max_fd + 1, &c_rfd, NULL, NULL, NULL) == -1)
		{
            fprintf(stderr, "ERROR: select() failed!\n");
            exit(1);
		}
		
		for(i = 0; i <= max_fd; i++)
		{
			printf("i = %d\n", i);
			if(FD_ISSET(i, &c_rfd))
			{
				if(i == listen_fd)
				{
					cli_len = sizeof(cli_addr);
					printf("at accept\n");
					if((conn_fd = accept(listen_fd, (struct sockaddr *)&cli_addr, &cli_len)) == -1)
					{
						fprintf(stderr, "ERROR: accept() failed!\n");
						exit(1);
					}
					fprintf(stdout, "[ALERT] - %s connected!\n", inet_ntoa(cli_addr.sin_addr));
					FD_SET(conn_fd, &rfd);
					login(conn_fd, max_fd, j);	
					
					// set max_fd if needed
					if(conn_fd > max_fd)
					{
						max_fd = conn_fd;
					}
				}   
				else
				{
					if(FD_ISSET(i, &c_rfd))
					{	
						if((rec = recv(i, packet, sizeof(packet), 0)) <= 0)
						{
							if(rec == 0)
							{	
								if(clientTable[i].id == conn_fd)
								{
									fprintf(stdout, "[ALERT] - %s disconnected!\n", inet_ntoa(cli_addr.sin_addr));
									close(conn_fd);
									FD_CLR(conn_fd, &rfd);
									clientTable[i].is_open = 0;
									clientTable[i].id = 0;
									clientTable[i].channel = 0;
									bzero(&clientTable[i].uname, sizeof(clientTable[j].uname));
								}
							}
							else
							{
								fprintf(stderr, "ERROR: recv() failed!\n");
							}
						}
						else
						{
							readPacket(packet, j);
						}
					}
				}
			}
		}
	}
	return 0;
}

void login(int conn_fd, int max_fd, int j)
{	
	char packet[MAX] = " ";
	char data[MAX_MESS] = " "; 	
	int type = 0;
	char chan = ' ';
	int source = 0;
	int dese = 0;
	
	read(conn_fd, packet, 50);
	
	unpackMess(packet, data);
	type = unpackType(packet);
	source = unpackSorce(packet);
	dese = unpackDese(packet);
	chan = unpackChan(packet);
	
	printf("type : %d | chan : %c | source : %d | dese : %d | mess : %s\n", type, chan, source, dese, data);
	
	// find empty spot on clientTable and add
	for(j = 0; j <= max_fd; j++)
	{
		if(clientTable[j].is_open == 0)
		{
			clientTable[j].is_open = 1;
			clientTable[j].id = conn_fd;
			clientTable[j].channel = chan;
			strcat(clientTable[j].uname, data);
			
			// send client id back to client
			pack(1, 's', 255, conn_fd, " ", packet);
			write(conn_fd, packet, 50);
			bzero(packet, 50);
			
			fprintf(stdout, "added %d to clientTable[%d]\n", clientTable[j].id, j);
			break;
		} 
		else
		{
			printf("clientTable[%d] not available!\n", j);	
		}
	}
}

void readPacket(char packet[MAX], int j)
{
	char data[MAX_MESS] = " "; 	
	int type = 0;
	char chan = ' ';
	int source = 0;
	int dese = 0;	
	
	unpackMess(packet, data);
	type = unpackType(packet);
	source = unpackSorce(packet);
	dese = unpackDese(packet);
	chan = unpackChan(packet);
	
	printf("type : %d | chan : %c | source : %d | dese : %d | mess : %s", type, chan, source, dese, data);
	
	switch(type)
	{
		case 0:
		printf("send packet\n");
		sendPacket(chan, source, data, j);
		break;
		
		case 1:
		printf("login error\n");
		break;
		
		case 2:
		printf("logout packet\n");
		logout(chan, source, j);
		break;
		
		case 3:
		printf("command packet\n");
		commandPacket(packet, source, j);
		break;
	}
}

void logout(char chan, int source, int j)
{
	size_t size = sizeof(clientTable) / sizeof(clientTable[0]);
	
	for(j = 0; size; j++)
	{
		if(clientTable[j].id == source)
		{
			/* SEGFAULT CAUSE HERE
			close(clientTable[j].id);
			*/
		}
	}
}

void sendPacket(char chan, int source, char data[MAX_MESS], int j)
{
	char packet[MAX] = " ";
	size_t size = sizeof(clientTable) / sizeof(clientTable[0]);
	
	switch(chan)
	{
		// IOS
		case 'i':
		for(j = 0; j < size; j++)
		{
			if(clientTable[j].id != source && clientTable[j].channel == 'i')
			{
				pack(0, chan, source, clientTable[j].id, data, packet);
				write(clientTable[j].id, packet, 50);
				bzero(&packet, 50);
			}
		}
		break;
		
		// Android
		case 'a':
		for(j = 0; j < size; j++)
		{
			if(clientTable[j].id != source && clientTable[j].channel == 'a')
			{
				pack(0, chan, source, clientTable[j].id, data, packet);
				write(clientTable[j].id, packet, 50);
				bzero(&packet, 50);
			}
		}
		break;
		
		// Global
		case 'g':
		for(j = 0; j < size; j++)
		{
			if(clientTable[j].id != source && clientTable[j].channel == 'g')
			{
				pack(0, chan, source, clientTable[j].id, data, packet);
				write(clientTable[j].id, packet, 50);
				bzero(&packet, 50);
			}
		}
		break;
		
		default:
		break;
	}
}

void commandPacket(char packet[MAX], int source, int j)
{
	size_t size = sizeof(clientTable) / sizeof(clientTable[0]);
	char tag[MAX_MESS];
	char data[MAX_MESS];
	int comType;
	int offset = 0;
	
	if(packet[offset] == '3')
	{
		offset += 8;
		comType = comPar(packet, &offset, tag, data);
		offset = 0;
	}
	bzero(&packet, 50);
	printf("comType = %d, tag = %s, data = %s\n", comType, tag, data);
	
	switch(comType)
	{
		case 0:
		// change room
		for(j = 0; j < size; j++)
		{

		}
		break;
		
		case 1:
		// user list
		break;
		
		case 2:
		// private message
		break;
		
		default:
		break;
	}
}
