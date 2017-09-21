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
	char *numip;
	int numport;
	char *mode; //ativo ou passivo

	if(argc != 6)
		return -1;

	arqin = argv[1]; arqout = argv[2];
	numip = argv[3]; numport = atoi(argv[4]);
	mode = argv[5];

	int s = socket(AF_INET, SOCK_STREAM, 0); //cria o socket
	if (s == -1)
		logexit("socket");

	struct in_addr addr = { .s_addr = inet_addr(numip) }; 
	struct sockaddr_in address = {	.sin_family = AF_INET,
                                    .sin_port = htons(numport),
	                                .sin_addr = addr };
	struct sockaddr_in client_addr;

	//fazer uma thread para o servidor e outra para o clientes
	printf("%s\n", numip);
	printf("%d\n", numport);
	printf("%s\n", mode);
	printf("%s\n", arqin);
	printf("%s\n", arqout);

	return 0;
}