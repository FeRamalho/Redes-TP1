#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

void logexit(const char *str){
    if(errno) perror(str);
    else puts(str);
    exit(EXIT_FAILURE);
}

//cabeçalho
typedef struct header{
	char syn[4];
	char chksum[2];
	char length[2];
	char reserved[2];
}header;

int main(int argc, char *argv[]){
	if(argc != 6) return -1;
	//variaveis
	int numip;
	int numport;
	char *mode; 
	FILE *input, *output;
	int mysock, newsock;
	header h1, h2;

	//inicializações
	input = fopen( argv[1], "r" );
	output = fopen( argv[2], "w" );
	numip = atoi( argv[3] );
	numport = atoi( argv[4] );
	mode = argv[5];
	strcpy( h1.syn, "DCC023C2" );
	strcpy( h2.syn, "DCC023C2" );
	strcpy( h1.reserved, "0000" );
	strcpy( h2.reserved, "0000" );

	mysock = socket(AF_INET, SOCK_STREAM, 0); //cria o socket
	if (mysock == -1) logexit("socket");

    //address structure
    struct in_addr addr = { .s_addr = htonl(numip) };
    struct sockaddr_in dst = { .sin_family = AF_INET,
                               .sin_port = htons(numport),
                               .sin_addr = addr };
    struct sockaddr *sa_dst = (struct sockaddr *)&dst;

	//modo ativo
	if(strcmp(mode, "ativo") == 0){
		//abertura ativa
		if(connect(mysock, sa_dst, sizeof(dst))){
			logexit("connect");
		} 

		//transmissor
		//receptor

	}

	//modo passivo
	else if(strcmp( mode, "passivo" ) == 0 ){
		//abertura passiva
    	if( bind(mysock, sa_dst, sizeof(dst)) < 0){
      		close(mysock);
    	}

    	//espera de conexão
    	if( listen(mysock, 1) < 0 ){
      		close(mysock);
      		logexit("ERROR listening\n");
    	}

    	while(1){
    		//client adress structure
    		struct in_addr client_addr = { .s_addr = htonl(numip) }; 
    		socklen_t client_len = sizeof(client_addr);

    		//completa a abertura passiva
    		newsock = accept(mysock, (struct sockaddr *)&client_addr, &client_len);
    		}

    		//transmissor
    		//receptor
	}
	close(mysock);
	close(newsock);
	fclose(input);
	fclose(output);
	return 0;
}