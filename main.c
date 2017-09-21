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
	char *arqin, *arqout; //nome dos arquivo de entrada
	char *numip, *numport;
	char *mode; //ativo ou passivo

	if(argc != 6)
		return -1;

	arqin = argv[1];
	arqout = argv[2];
	numip = argv[3];
	//numport = atoi(argv[4]);
	numport = argv[4];
	mode = argv[5];
	

	return 0;
}