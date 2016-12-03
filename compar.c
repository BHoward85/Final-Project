// Main Project
// Compars sorce

//test sets for command parser

// there is a memory bug if more than 3 strings are in used

/**
 ** 	::syntax::
 ** 	${command} <{command tag}> [:] <- optional but needed for pm command
 **
 ** 	::command list::
 ** 	ch = change channel
 ** 	pm = user to user chat
 ** 	ul = user list
 **
 ** 	::command tags::
 ** 	pm -> <user name>
 ** 	ch -> <channel name>
 ** 	ul -> <channel name or All>
 **
 ** 	~notes~
 ** 	for ul command the tag All is used to print
 ** 	off all the users on the server
 **		
 **		for pm command the message is after a colon
 **		i.e. $pm <tim> : hello
 **/

#include "ComPar.h"

#define MAX 50
#define MAX_MESS 42

int main()
{
	char first[MAX] = "3s001255$pm <tim> : how are you?\n";
	//char second[MAX] = "3s002255$ch <iOS>: \n";
	char third[MAX] = "3g008255$ul<all>:\n";
	//char forth[MAX] = "3g004255$ch<And>\n";
	char fifth[MAX] = "$ch <And>\n";
	int offset = 0;

	if(first[offset] == '3')
	{
		offset += 8;
		comPar(first, &offset);
		offset = 0;
	}/*
	if(second[offset] == '3')
	{
		offset += 8;
		comPar(second, &offset);
		offset = 0;
	}*/
	if(third[offset] == '3')
	{
		offset += 8;
		comPar(third, &offset);
		offset = 0;
	}/*
	if(forth[offset] == '3')
	{
		offset += 8;
		comPar(forth, &offset);
		offset = 0;
	}*/
	offset = 0;
	comPar(fifth, &offset);
	return 0;
}

void comPar(char *dataPack, int *offset)
{
	int n = 0;
	int comType = 0;
	char tempChar = '\0';
	char command[MAX_MESS];
	char comtag[MAX_MESS];
	char mess[MAX_MESS];

	//bzero(command, MAX - (*offset));
	//bzero(comtag, MAX - (*offset));
	//bzero(mess, MAX - (*offset));
	
	printf("dataPack: %s\n%d\n%d\n", dataPack, *offset, (MAX - (*offset)));
	
	while(*offset < MAX)
	{
		tempChar = dataPack[(*offset)++];
		
		if(tempChar == '$')
		{
			while(tempChar != ' ' && tempChar != '<')
			{
				tempChar = dataPack[(*offset)++];
				
				if(tempChar != ' ' && tempChar != '<')
				{
					printf("%c", tempChar);
					command[n++] = tempChar;
				}
			}
			printf("\n");
			printf("tempChar: %c\ncommand: %s\nn = %d\n", tempChar, command, n);
			n = 0;
			while(tempChar != '<')
			{
				tempChar = dataPack[(*offset)++];
			}
			printf("\n");
			if(tempChar == '<')
			{
				while(tempChar != '>')
				{
					tempChar = dataPack[(*offset)++];
					
					if(tempChar != '>')
					{
						printf("%c", tempChar);
						comtag[n++] = tempChar;
					}
				}
			}
			printf("\n");
			printf("tempChar: %c\ncomTag %s\nn = %d\n", tempChar, comtag, n);
			n = 0;
			while(tempChar != ':' && *offset < MAX)
			{
				tempChar = dataPack[(*offset)++];
			}
			printf("\n");
			n = 0;
			if(tempChar == ':')
			{
				while(*offset < MAX)
				{
					mess[n++] = dataPack[(*offset)++];
				}
			}
		}
	}
	
	printf("segs>> command: %s | comtag: %s | mess: %s\n", command, comtag, mess);
	
	comType = findCom(command);
	
	printf("%d\n", comType);
}

int findCom(char *dataSeg)
{
	if( strcmp("ch", dataSeg) == 0 )
		return 0;
	if( strcmp("ul", dataSeg) == 0 )
		return 1;
	if( strcmp("pm", dataSeg) == 0 )
		return 2;
	else
		return -1;
}
