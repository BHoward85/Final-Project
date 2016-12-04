// Main Project
// packetManager sorce

/**
 **		This is a data packer and unpacker for chatcli and chatserver
 **
 **		this a simple tester for prof of concept
 **
 **		this can be used in conjunction with compar
 **
 **		protocol uses 8 bytes for a packet header
 **
 **		1 byte for type, 1 byte for chan, 3 bytes for dese and sorce
 **		and 44 bytes for message
 **
 **/

#include "packetManager.h"

/*int main(int argc, char** argv)
{
	char packet[MAX] = " "; 	
	int n = 0;
	int type = 0;
	char chan = ' ';
	int sorce = 0;
	int dese = 0;
	char data[MAX_MESS] = " ";
	
	pack(2, 's', 1, 255, "Hello to all\n", packet);
	
	printf("%s\n", packet);
	
	charToInt("005", &n);
	
	printf("%d\n", n);
	
	unpackMess(packet, data);
	type = unpackType(packet);
	sorce = unpackSorce(packet);
	dese = unpackDese(packet);
	chan = unpackChan(packet);
	
	printf("type : %d | chan : %c | sorce : %d | dese : %d | mess : %s\n", type, chan, sorce, dese, data);
}*/

void pack(int type, char chan, int sorce, int dese, char* data, char* packet)
{
	char sorCh[3] = "000";
	char desCh[3] = "000";
	int n = 0;
	int m = 0;
	int sdex = 0;
	int ddex = 0;
	
	packet[n++] = (type + '0');
	packet[n++] = chan;
	
	intToChar(sorce, sorCh);
	
	intToChar(dese, desCh);
	
	for(sdex = 0; sdex < 3; sdex++)
	{
		packet[n++] = sorCh[sdex];
	}
	
	for(ddex = 0; ddex < 3; ddex++)
	{
		packet[n++] = desCh[ddex];
	}
	while(n < MAX)
	{
		packet[n++] = data[m++];
	}
}

void unpackMess(char* packet, char* mess)
{
	int offset = 8;
	int index;
	
	for(index = 0; index < MAX_MESS; index++)
		mess[index] = packet[offset + index];
}

int unpackType(char* packet)
{
	return packet[0] - '0';
}

char unpackChan(char* packet)
{
	return packet[1];
}

int unpackSorce(char* packet)
{
	int n = 0;
	int ndex = 0;
	int offset = 2;
	char subSet[3];
	
	for(ndex = 0; ndex < 3; ndex++)
		subSet[ndex] = packet[offset + ndex];
		
	charToInt(subSet, &n);
	
	return n;
}

int unpackDese(char* packet)
{
	int n = 0;
	int ndex =0;
	int offset = 5;
	char subSet[3];
	
	for(ndex = 0; ndex < 3; ndex++)
		subSet[ndex] = packet[offset + ndex];
	
	charToInt(subSet, &n);
	
	return n;
}

void charToInt(char* fromStr, int* toNum)
{
	int index;
	
	for(index = 0; fromStr[index] != '\0'; index++)
	{
		if(fromStr[index] >= 48 && fromStr[index] <= 57)
		{
			*toNum = *toNum * 10 + (fromStr[index] - 48);
		}
	}
}

void intToChar(int fromNum, char* toStr)
{
	int hunth = 0;
	int tenth = 0;
	int oneth = 0;
	
	oneth = fromNum % 10;
	fromNum /= 10;
	tenth = fromNum % 10;
	fromNum /= 10;
	hunth = fromNum % 10;
	
	toStr[0] = (hunth + '0');
	toStr[1] = (tenth + '0');
	toStr[2] = (oneth + '0');
}
