#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>

void logexit(const char *str)
{
    if(errno) perror(str);
    else puts(str);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]){
	//FILE* arqin, arqout;
	char* numip, numport;
	char* mode;

	if(argc != 6){
		printf("Errinho\n");
		return -1;
		
	}
	//arqin = argv[1];
	//arqout = argv[2];
	numip = argv[3];
	numport = atoi(argv[4]);
	mode = argv[5];
	printf("%s\n", numip);
	printf("%d\n", numport);
	printf("%s\n", mode);
	//printf("%s\n", arqin);
	//printf("%s\n", arqout);

	return 0;
}