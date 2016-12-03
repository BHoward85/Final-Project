// Final Project
// Server Code

#include "finalProject.h"

cl clientTable[SERVER_SIZE];

void pack(int type, char chan, int sorce, int dese, char* data, char* packet);
void unpackMess(char* packet, char* mess);
int unpackSorce(char* packet);
int unpackDese(char* packet);
int unpackType(char* packet);
char unpackChan(char* packet);
void intToChar(int fromNum, char* toStr);
void charToInt(char* fromStr, int* toNum);

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
	char packet[MAX];
	struct sockaddr_in serv_addr; 
	struct sockaddr_in cli_addr;
	socklen_t cli_len;
	fd_set rfd, c_rfd;
	FD_ZERO(&rfd);
	FD_ZERO(&c_rfd);
	
	// create socket
    if((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "ERROR: Socket Failed!\n");
        exit(1);
    }

    bzero(&serv_addr, sizeof(serv_addr));    
    serv_addr.sin_family = AF_INET;     
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);     
    serv_addr.sin_port = htons(port);
	
    // bind socket to address
    if(bind(listen_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
    {
        fprintf(stderr, "ERROR: bind() failed!\n");
        exit(1);
    }
    else
    {
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
		
		if(select(max_fd + 1, &c_rfd, NULL, NULL, NULL) == -1)
		{
            fprintf(stderr, "ERROR: select() failed!\n");
            exit(1);
		}
		
		for(i = 0; i <= max_fd; i++)
		{
			if(FD_ISSET(i, &c_rfd))
			{
				if(i == listen_fd)
				{
					cli_len = sizeof(cli_addr);
					// below will be in login()
					if((conn_fd = accept(listen_fd, (struct sockaddr *)&cli_addr, &cli_len)) == -1)
					{
						fprintf(stderr, "ERROR: accept() failed!\n");
						exit(1);
					}
					//
					
					fprintf(stdout, "[ALERT] - %s connected!\n", inet_ntoa(cli_addr.sin_addr));
					FD_SET(conn_fd, &rfd);
					
					// find empty spot on clientTable and add
					for(j = 0; j <= max_fd; j++)
					{
						if(clientTable[j].is_open == 0)
						{
							clientTable[j].is_open = 1;
							clientTable[j].sockd = conn_fd;
							// set channel
							// set uname
							fprintf(stdout, "added %d to clientTable[%d]\n", clientTable[j].sockd, j);
							break;
						}
					}
					
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
						rec = read(conn_fd, packet, sizeof(packet));
						if(rec == 0)
						{	
							// find and remove from clientTable
							for(j = 0; j <= max_fd; j++)
							{
								if(clientTable[j].sockd == conn_fd)
								{
									fprintf(stdout, "[ALERT] - %s disconnected!\n", inet_ntoa(cli_addr.sin_addr));
									close(conn_fd);
									FD_CLR(conn_fd, &rfd);
									clientTable[j].is_open = 0;
									clientTable[j].sockd = 0;
									// clear channel
									// clear uname
								}
							}
						}
						else
						{
							// sendPacket() will decide what to do with packet
						}
					}
				}
			}
		}
	}
	
	return 0;
}