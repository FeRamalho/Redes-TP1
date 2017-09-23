#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_BUF 4096

//cabeçalho
typedef struct header{
	char syn[8];
	char chksum[2];
	char length[2];
	char reserved[2];
}header;

//parametros da thread
typedef struct parameters{
	FILE *output;
	int socket;
}parameters; 

void logexit(const char *str){
    if(errno) perror(str);
    else puts(str);
    exit(EXIT_FAILURE);
}

//calcula o checksum
unsigned cksum(void *buffer, int count){
	register unsigned long sum = 0;
	unsigned char *buf = (unsigned char *)buffer;
	while(count--){
		sum += *buf++;
		if(sum & 0xFFFF0000){
			sum &= 0xFFFF;
			sum++;
		}
	}
	return (sum & 0xFFFF);
}

//retorna o valor do checksum
unsigned checkresult(FILE *f){
	char buf[4065];
	uint16_t len;
	len = fread(buf, sizeof(char), sizeof(buf), f);
	unsigned long check = cksum(buf, len);
	return check;
}

void *receptor(void *parameter){
	parameters *p1;
	p1 = (parameters*)parameter;
	FILE *output = p1->output;
	int mysock = p1->socket;
char buffer[8];
strcpy(buffer, "DCC023C2"); 
printf("%s\n", buffer);
send(mysock, buffer, 8, 0);
	return NULL;
}

int main(int argc, char *argv[]){

	//variaveis
	int numip;
	int numport;
	char mode[10]; 
	FILE *input, *output;
	int mysock, newsock;
	header h1, h2;
    parameters p1;
    pthread_t thread_id;
	//inicializações
	input = fopen( argv[1], "w" );
	output = fopen( argv[2], "r" );
	numip = atoi( argv[3] );
	numport = atoi( argv[4] );
	strcpy( mode , argv[5] );
	strcpy( h1.syn, "DCC023C2" );
	strcpy( h2.syn, "DCC023C2" );
	//strcpy( h1.reserved, "0000" );
	//strcpy( h2.reserved, "0000" );
    p1.output = output;
	//cria socket
	mysock = socket(AF_INET, SOCK_STREAM, 0); 
	if (mysock == -1) logexit("socket");

    //address structure
    //struct in_addr addr = { .s_addr = htonl(numip) };
    struct in_addr addr = { .s_addr = htonl(INADDR_LOOPBACK) };
    struct sockaddr_in dst = { .sin_family = AF_INET,
                               .sin_port = htons(numport),
                               .sin_addr = addr };
    struct sockaddr *sa_dst = (struct sockaddr *)&dst;

	//modo ativo
	if(strcmp(mode, "ativo") == 0){
printf("modo ativo\n");
		//abertura ativa
		if(connect(mysock, sa_dst, sizeof(dst))){
			logexit("connect");
		} 
printf("ABRIU\n");
		p1.socket = mysock;
		pthread_create( &thread_id , NULL, receptor , (void*) &p1);
printf("passou da thread\n");
		//transmissor
		//receptor

	}
	//modo passivo
	else if(strcmp( mode, "passivo" ) == 0 ){
printf("modo passivo\n");
		//abertura passiva
    	if( bind(mysock, sa_dst, sizeof(dst)) < 0){
      		close(mysock);
      		logexit("EEROR: binding");
    	}
printf("bound\n");
    	//espera de conexão
    	if( listen(mysock, 1) < 0 ){
      		close(mysock);
      		logexit("ERROR listening\n");
    	}
printf("ESPERANDO CONEXAO\n");

    	while(1){
    		//client adress structure
    		//struct in_addr client_addr = { .s_addr = htonl(numip) };
    		struct in_addr client_addr = { .s_addr = htonl(INADDR_ANY) }; 
    		socklen_t client_len = sizeof(client_addr);

    		//completa a abertura passiva
    		newsock = accept(mysock, (struct sockaddr *)&client_addr, &client_len);
printf("ACEITOU CONEXAO\n");
int len;
char buf[8];
len = recv(newsock, buf, 8, MSG_WAITALL);
printf("%d\nmensagem: %s\n", len, buf);
    		p1.socket = newsock;
    		//pthread_create( &thread_id , NULL ,  receptor , (void*) &p1);
printf("passou da thread\n");
    		//transmissor
    		//receptor
    	}
	}
	close(mysock);
	close(newsock);
	fclose(input);
	fclose(output);
	return 0;
}