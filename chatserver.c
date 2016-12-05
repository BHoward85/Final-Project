// Final Project - CSCD 330
// Server - Written by Rodney Thomas

#include "chatserver.h"

cl clientTable[SERVER_SIZE];

int main(int argc, char **argv) 
{
   if(argc < 2)
   {
      fprintf(stderr, "ERROR: Port must be provided!\n");
      exit(1);
   } 
	
   int port = atoi(argv[1]);
   int conn_fd = 0;
   int listen_fd = 0;
   int max_fd = 0;
   int i = 0;
   int j = 0; 
   int rec = 0;
   char packet[MAX] = " ";
   struct sockaddr_in serv_addr; 
   struct sockaddr_in cli_addr;
   socklen_t cli_len;
   fd_set rfd, c_rfd;
   FD_ZERO(&rfd);
   FD_ZERO(&c_rfd);
	
   for(i = 0; i < SERVER_SIZE; i++)
   {
      clientTable[i].is_open = 0;
   }
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
               if((conn_fd = accept(listen_fd, (struct sockaddr *)&cli_addr, &cli_len)) == -1)
               {
                  fprintf(stderr, "ERROR: accept() failed!\n");
                  exit(1);
               }
               login(conn_fd, max_fd, j);
               FD_SET(conn_fd, &rfd);			   
            	
               for(j = 0; j <= max_fd; j++)
               {			   
                  // set max_fd if needed
                  if(clientTable[j].id > max_fd && clientTable[j].is_open == 1)
                  {
                     max_fd = clientTable[j].id;
                  }
               }
            }   
            else
            {
               if(FD_ISSET(i, &c_rfd))
               {	
                  if((rec = recv(i, packet, sizeof(packet), 0)) <= 0)
                  {
                     if(rec <= 0)
                     {	
			for(j = 0; j < max_fd; j++)
			{
			   if(i == clientTable[j].id)
			   {
                              close(i);
                              FD_CLR(i, &rfd);
                              clientTable[j].is_open = 0;
                              clientTable[j].id = 0;
                              clientTable[j].channel = 0;
                              bzero(&clientTable[j].uname, sizeof(clientTable[i].uname));
		 	      printf("[ALERT] - %d disconnected!\n", i);
			   }
		 	}
                     }
                  }
                  else
                  {
                     if(FD_ISSET(i, &c_rfd))
                     {
			size_t ln = strlen(packet) -1;
			if(packet[ln] == '\n')
			{
			   packet[ln] == '\0';
			}
			printf("Recieved Packet: %s\n", packet);
                        readPacket(&rfd, packet, max_fd, j);
                     }
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
   char chan = 0;   
   int type = 0;
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
	 printf("Sending packet: %s\n", packet);
         write(conn_fd, packet, 50);
	 printf("Sent to %d\n", clientTable[j].id);
         bzero(packet, 50);
      	
         printf("added %d to clientTable[%d]\n", clientTable[j].id, j);
	 printf("[ALERT] - %d connected!\n", clientTable[j].id);
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
   char chan = 0;
   int type = 0;
   int source = 0;
   int dese = 0;	
	
   unpackMess(packet, data);
   type = unpackType(packet);
   source = unpackSorce(packet);
   dese = unpackDese(packet);
   chan = unpackChan(packet);

   printf("type : %d | chan : %c | source : %d | dese : %d | mess : %s\n", type, chan, source, dese, data);
	
   switch(type)
   {
      case 0:
         sendPacket(chan, source, data, max_fd, j);
         break;
   	
      case 1:
         printf("login error\n");
         break;
   	
      case 2:
         logout((fd_set *)rfd, chan, source, max_fd, j);
         break;
   	
      case 3:
         commandPacket(packet, source, chan, data, max_fd, j);
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
      if(clientTable[j].is_open == 1 && clientTable[j].id == source)
      {	
         strcpy(on_exit, clientTable[j].uname);
         strcat(on_exit, tmp);
      	 
         close(clientTable[j].id);
         FD_CLR(clientTable[j].id, &*rfd);
         clientTable[j].is_open = 0;
	 printf("[ALERT] - %d disconnected!\n", clientTable[j].id);
         clientTable[j].id = 0;
         clientTable[j].channel = 0;
         bzero(&clientTable[j].uname, sizeof(clientTable[j].uname));
         break;
      }
   }
   
   for(j = 0; j <= max_fd; j++)
   {
      if(clientTable[j].is_open == 1 && clientTable[j].channel == chan)
      {
         pack(0, chan, 255, clientTable[j].id, on_exit, packet);
	 printf("Sending packet: %s\n", packet);
         write(clientTable[j].id, packet, 50);
	 printf("Sent to %d\n", clientTable[j].id);
         bzero(packet, 50);	
      }
   }
   free(tmp);
}

void sendPacket(char chan, int source, char data[MAX_MESS], int max_fd,  int j)
{
   char packet[MAX] = " ";
	
   switch(chan)
   {
   	  // IOS channel
      case 'i':
         for(j = 0; j <= max_fd; j++)
         {
			printf("j = %d");
            if(clientTable[j].is_open == 1 && clientTable[j].id != source && clientTable[j].channel == 'i')
            {
               pack(0, chan, source, clientTable[j].id, data, packet);
	       printf("Sending packet: %s\n", packet);
               write(clientTable[j].id, packet, 50);
	       printf("Sent to %d\n", clientTable[j].id);
               bzero(&packet, 50);
            }
         }
         break;
   	
   	  // Android channel
      case 'a':
         for(j = 0; j <= max_fd; j++)
         {
            if(clientTable[j].is_open == 1 && clientTable[j].id != source && clientTable[j].channel == 'a')
            {
               pack(0, chan, source, clientTable[j].id, data, packet);
	       printf("Sending packet: %s\n", packet);
               write(clientTable[j].id, packet, 50);
	       printf("Sent packet to %d\n", clientTable[j].id);
               bzero(&packet, 50);
            }
         }
         break;
   	
   	  // Global channel
      case 'g':
         for(j = 0; j <= max_fd; j++)
         {
            if(clientTable[j].is_open == 1 && clientTable[j].id != source /*&& clientTable[j].channel == 'g'*/)
            {
               pack(0, chan, source, clientTable[j].id, data, packet);
	       printf("Sending packet: %s\n", packet);
               write(clientTable[j].id, packet, 50);
               printf("Sent to %d\n", clientTable[j].id);
               bzero(&packet, 50);
            }
         }
         break;
	  
      	  // Message to server not implemented
      case 's':
	 break;
		 
      default:
         break;
   }
}

void commandPacket(char packet[MAX], int source, char chan, char msg[MAX_MESS], int max_fd, int j)
{
   char retPacket[MAX] = " ";
   char who[MAX_MESS] = " ";
   char tag[MAX_MESS] = " ";
   char data[MAX_MESS] = " ";
   char tmp = 0;
   int comType = 0;
   int offset = 0;
   int didFind = 0;
	
   if(packet[offset] == '3')
   {
      offset += 8;
      comType = comPar(packet, &offset, tag, data);
      offset = 0;
   }
   
   printf("comType = %d, tag = %s, data = %s\n", comType, tag, data);
	
   switch(comType)
   {
	  // change room
      case 0:
         for(j = 0; j <= max_fd; j++)
         {
            if(tmp = tolower(tag[0]) == 'i')
            {
               if(clientTable[j].is_open == 1 && clientTable[j].channel != 'i' && clientTable[j].id == source)
               {
                  clientTable[j].channel = 'i';
                  pack(6, clientTable[j].channel, 255, clientTable[j].id, " ", retPacket);
		  printf("Sending packet: %s\n", retPacket);
                  write(clientTable[j].id, retPacket, 50);
		  printf("Sent packet to %d\n", source);
                  bzero(&retPacket, 50);
               }
            }
            else if(tmp = tolower(tag[0]) == 'a')
            {
               if(clientTable[j].is_open == 1 && clientTable[j].id == source && clientTable[j].channel != 'a' && clientTable[j].id == source)
               {
                  clientTable[j].channel = 'a';
                  pack(6, clientTable[j].channel, 255, clientTable[j].id, " ", retPacket);
		  printf("Sending packet: %s\n", retPacket);
                  write(clientTable[j].id, retPacket, 50);
		  printf("Sent packet to %d\n", source);
                  bzero(&retPacket, 50);
                  break;
               }
            }
            else if(tmp = tolower(tag[0]) == 'g')
            {
               if(clientTable[j].is_open == 1 && clientTable[j].channel != 'g' && clientTable[j].id == source)
               {
                  clientTable[j].channel = 'g';
                  pack(6, clientTable[j].channel, 255, clientTable[j].id, " ", retPacket);
		  printf("Sending packet: %s\n", retPacket);
                  write(clientTable[j].id, retPacket, 50);
		  printf("Sent packet to %d\n", source);
                  bzero(&retPacket, 50);
                  break;
               }
            }
            else
            {
               if(clientTable[j].is_open == 1 && clientTable[j].id == source)
               {
                  pack(7, clientTable[j].channel, 255, clientTable[j].id, " ", retPacket);
		  printf("Sending packet: %s\n", retPacket);
                  write(clientTable[j].id, retPacket, 50);
		  printf("Sent packet to %d\n", source);
                  bzero(&retPacket, 50);
                  break;
               }
            }
         }
         break;
		 
      // user list
      case 1:
	 tmp = tolower(tag[0]);
         for(j = 0; j <= max_fd; j++)
         {
            if(tmp == 'i')
            {
               if(clientTable[j].is_open == 1 && clientTable[j].id != source && clientTable[j].channel == tmp)
               {
                  strcat(who, clientTable[j].uname);
                  pack(8, 's', 255, source, who, retPacket);
		  printf("Sending packet: %s\n", retPacket);
                  bzero(&who, 44);
                  write(source, retPacket, 50);
		  printf("Sent packet to %d\n", source);
                  bzero(&retPacket, 50);
                  //break;
               }
            }
            else if(tmp == 'a')
            {
               if(clientTable[j].is_open == 1 && clientTable[j].id != source && clientTable[j].channel == tmp)
               {
                  strcat(who, clientTable[j].uname);
                  pack(8, 's', 255, source, who, retPacket);
		  printf("Sending packet: %s\n", retPacket);
                  bzero(&who, 44);
                  write(source, retPacket, 50);
		  printf("Sent packet to %d\n", source);
                  bzero(&retPacket, 50);
                  //break;
               }
            }
            else if(tmp == 'g')
            {
               if(clientTable[j].is_open == 1 && clientTable[j].id != source && clientTable[j].channel == tmp)
               {
                  strcat(who, clientTable[j].uname);
                  pack(8, 's', 255, source, who, retPacket);
		  bzero(&who, 44);
		  printf("Sending packet: %s\n", retPacket);
                  write(source, retPacket, 50);
		  printf("Sent packet to %d\n", source);
                  bzero(&retPacket, 50);
                  //break;
               }
            }
            else if(tmp == ' ')
            {
               if(clientTable[j].is_open == 1 && clientTable[j].id != source)
               {
                  strcat(who, clientTable[j].uname);
                  pack(8, 's', 255, source, who, retPacket);
		  printf("Sending packet: %s\n", retPacket);
                  bzero(&who, 44);
                  write(source, retPacket, 50);
		  printf("Sent packet to %d\n", source);
                  bzero(&retPacket, 50);
                  //break;
               }
            }
         }
		 
         pack(9, 's', 255, source, " ", retPacket);
	 printf("Sending packet: %s\n", retPacket);
         write(source, retPacket, 50);
	 printf("Sent packet to %d\n", source);
         bzero(&retPacket, 50);
         break;
      
      // private message	  
      case 2:	
	     for(j = 0; j <= max_fd; j++)
	     {
		    if(strcmp(clientTable[j].uname, tag) == 0 && clientTable[j].is_open == 1)
            	    {
                                pack(4, 's', 255, source, " ", retPacket);
				write(source, retPacket, 50);
				printf("Sending packet: %s\n", retPacket);
				bzero(&retPacket, 50);
				pack(0, 's', clientTable[j].id, source, data, retPacket);
				printf("Sending packet: %s\n", retPacket);
				write(clientTable[j].id, retPacket, 50);
				printf("Sent packet to %d\n", source);
				bzero(&retPacket, 50);
				didFind = 1;
				break;
			}
			else if(strcmp("all", tag) == 0 && clientTable[j].is_open == 1)
			{
				pack(4, 's', 255, source, " ", retPacket);
				write(source, retPacket, 50);
				printf("Sending packet: %s\n", retPacket);
				bzero(&retPacket, 50);
				pack(0, 's', clientTable[j].id, source, data, retPacket);
				printf("Sending packet: %s\n", retPacket);
				write(clientTable[j].id, retPacket, 50);
				printf("Sent packet to %d\n", source);
				bzero(&retPacket, 50);
				didFind = 1;
			} 
	     }
		 
		 if(didFind == 1)
		 {
			 didFind = 0;
		 }
		 else
		 {
			pack(5, 's', 255, source, " ", retPacket);
			printf("Sending packet: %s\n", retPacket);
			write(source, retPacket, 50);
			printf("Sent packet to %d\n", source);
			bzero(&retPacket, 50); 
		 }
         break;
   	
      default:
         break;
   }
   bzero(&packet, 50);
}
