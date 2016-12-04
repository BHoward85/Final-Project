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
									bzero(&clientTable[i].uname, sizeof(clientTable[i].uname));
								}
							}
							else
							{
								fprintf(stderr, "ERROR: recv() failed!\n");
							}
						}
						else
						{
							readPacket(&rfd, packet, max_fd, j);
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
	int len = 0;
	
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
			
			len = strlen(clientTable[j].uname);
			if(clientTable[j].uname[len - 1] == '\n')
			{
				clientTable[j].uname[len - 1] = 0;
			}
			
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

void readPacket(fd_set *rfd, char packet[MAX], int max_fd, int j)
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
		sendPacket(chan, source, data, max_fd, j);
		break;
		
		case 1:
		printf("login error\n");
		break;
		
		case 2:
		printf("logout packet\n");
		logout((fd_set *)rfd, chan, source, max_fd, j);
		break;
		
		case 3:
		printf("command packet\n");
		commandPacket(packet, source, max_fd, j);
		break;
	}
}

void logout(fd_set *rfd, char chan, int source, int max_fd, int j)
{	
	char packet[MAX] = " ";
	char on_exit[MAX_MESS] = " has left the room!";
	char *tmp = strdup(on_exit);
	
	for(j = 0; j <= max_fd; j++)
	{
		if(clientTable[j].id == source)
		{	
			strcpy(on_exit, clientTable[j].uname);
			strcat(on_exit, tmp);
			
			printf("%s\n", on_exit);
			
			close(clientTable[j].id);
			FD_CLR(clientTable[j].id, &*rfd);
			clientTable[j].is_open = 0;
			clientTable[j].id = 0;
			clientTable[j].channel = 0;
			bzero(&clientTable[j].uname, sizeof(clientTable[j].uname));
			break;
		}
	}
	// broadcast message to other 
	for(j = 0; j <= max_fd; j++)
	{
		if(clientTable[j].channel == chan)
		{
			printf("sending exit packet to %d\n", clientTable[j].id);
			pack(0, chan, 255, clientTable[j].id, on_exit, packet);
			write(clientTable[j].id, packet, 50);
			bzero(packet, 50);	
		}
	}
}

void sendPacket(char chan, int source, char data[MAX_MESS], int max_fd,  int j)
{
	char packet[MAX] = " ";
	
	switch(chan)
	{
		// IOS
		case 'i':
		for(j = 0; j <= max_fd; j++)
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
		for(j = 0; j <= max_fd; j++)
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
		for(j = 0; j <= max_fd; j++)
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

void commandPacket(char packet[MAX], int source, int max_fd, int j)
{
	char retPacket[MAX] = " ";
	char tag[MAX_MESS];
	char data[MAX_MESS];
	char tmp;
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
		for(j = 0; j <= max_fd; j++)
		{
			if(tmp = tolower(tag[0]) == 'i')
			{
				if(clientTable[j].id == source && clientTable[j].channel != 'i')
				{
					printf("%d changed room to IOS\n", clientTable[j].id);
					clientTable[j].channel = 'i';
					pack(3, clientTable[j].channel, 255, clientTable[j].id, "Change room to IOS!", retPacket);
					break;
				}
			}
			else if(tmp = tolower(tag[0]) == 'a' && clientTable[j].channel != 'a')
			{
				if(clientTable[j].id == source)
				{
					printf("%d changed room to Android\n", clientTable[j].id);
					clientTable[j].channel = 'a';
					pack(3, clientTable[j].channel, 255, clientTable[j].id, "Change room to Android!", retPacket);
					break;
				}
			}
			else if(tmp = tolower(tag[0]) == 'g' && clientTable[j].channel != 'g')
			{
				if(clientTable[j].id == source)
				{
					printf("%d changed room to General\n", clientTable[j].id);
					clientTable[j].channel = 'g';
					pack(3, clientTable[j].channel, 255, clientTable[j].id, "Change room to General!", retPacket);
					write(clientTable[j].id, retPacket, 50);
					bzero(&retPacket, 50);
					break;
				}
			}
		}
		printf("sending packet to %d\n", clientTable[j].id);
		write(clientTable[j].id, retPacket, 50);
		bzero(&retPacket, 50);
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
