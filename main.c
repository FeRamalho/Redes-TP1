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

#define MAX_IN 60000


//cabeçalho
typedef struct header{
	unsigned char syn1[4];
	unsigned char syn2[4];
	uint16_t chksum;
	uint16_t length;
	unsigned char reserved[2];
	unsigned char buffer[MAX_IN];
}header;

//parametros da thread
typedef struct parameters{
	FILE *output;
	char *out; //usar esse se não puder fechar a conexao
	int mysocket;
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

void zeraBuffer(unsigned char *buffer){
	int i;
	for (i = 0; i < MAX_IN; ++i){ //zera o buffer para usar de novo
		buffer[i] = 0;
    }
}

//thread function que faz o papel do receptor 
void *receptor(void *parameter){
	parameters *p1;
	int flag = 0;
	p1 = (parameters*)parameter;
	FILE *output = p1->output;
	int mysocket = p1->mysocket;
	unsigned char buffer[MAX_IN];
	header h1;
	int i = 0;
	uint16_t lengthTotal = 0;
	read(mysocket, &buffer, 1);
	while(1){ 
		//FILE *output = fopen(p1->out,"ab"); //usar esse se não puder fechar a conexao
		if(buffer[i] = 0xdc){
			flag = recv(mysocket, &buffer, 1 , 0);
			if(buffer[i] = 0xc0){
				flag = recv(mysocket, &buffer, 1 , 0);
				if(buffer[i] = 0x23){
					flag = recv(mysocket, &buffer, 1 , 0);
					if(buffer[i] = 0xc2){
						flag = recv(mysocket, &buffer, 1 , 0);
						if(buffer[i] = 0xdc){
							flag = recv(mysocket, &buffer, 1 , 0);
							if(buffer[i] = 0xc0){
								flag = recv(mysocket, &buffer, 1 , 0);
								if(buffer[i] = 0x23){
									flag = recv(mysocket, &buffer, 1 , 0);
									if(buffer[i] = 0xc2){
										uint16_t newcheck;
										uint16_t length = 0;
										unsigned char aux[2] = {0,0};
										h1.syn1[0] = 0xdc; h1.syn1[1] = 0xc0;
										h1.syn1[2] = 0x23; h1.syn1[3] = 0xc2;
										h1.syn2[0] = 0xdc; h1.syn2[1] = 0xc0;
										h1.syn2[2] = 0x23; h1.syn2[3] = 0xc2;
										read(mysocket, &aux, 2);
										uint16_t checksum = (*(uint16_t *)aux);
										h1.chksum = 0;
										read(mysocket, &aux, 2);
										length = ntohs(*(uint16_t *)aux);
										h1.length = length;
										read(mysocket, &aux, 2);
										h1.reserved[0] = aux[0]; 
										h1.reserved[1] = aux[1];
										read(mysocket, &buffer, length);
										for(i = 0; i < length; i++){
											h1.buffer[i] = buffer[i];
										}
										//if(length%2 != 0) h1.buffer[length] = 0;
        								unsigned char *p=(unsigned char *)&h1;
										newcheck = cksum(p, length+112);
										if(newcheck == checksum){
											printf("deu certo\n");
											//h1.buffer[length] ='\0';
											fwrite(&buffer,sizeof(unsigned char), length, output);
											zeraBuffer(h1.buffer); 
										}
										zeraBuffer(buffer);
										//break;
									}
								}
							}
						}	
					}
				}
			}
		}
		if(flag <= 0){
			return NULL;
		}
	}
	return NULL;
}

//funcao que tem o papel do transmissor
void transmissor(FILE *input, int mysocket, header h2){ 
	unsigned char buffer[1];
	uint16_t length = 0;
	int flag = 1;
	while(!feof(input)){ //le ate o fim do arquivo
		flag = fread(&buffer,sizeof(unsigned char), 1, input);
		while(flag != 0 ){
			if(length == MAX_IN ){
				length = 0;
				break;
			} 
			h2.buffer[length] = buffer[0];
			length ++;
			flag = fread(&buffer,sizeof(unsigned char), 1, input);
		} 
		h2.length = htons(length);
        uint16_t newlen = length+112;
        h2.chksum = 0; //zera o checksum inicial
        //if(length%2 != 0) h2.buffer[length] = 0;
        unsigned char *p=(unsigned char *)&h2;
        uint16_t check = cksum(p, newlen);
        h2.chksum = check;
        unsigned char buf[2];
		send(mysocket, h2.syn1, 4, 0); 		  		// SYN
		send(mysocket, h2.syn2, 4, 0); 		  		// SYN
        send(mysocket, &h2.chksum, 2, 0); 	 		// CHECKSUM
		send(mysocket, &h2.length, 2, 0); 	 		// LENGTH
        send(mysocket, h2.reserved, 2, 0); 	  		// RESERVED
        //h2.buffer[length] ='\0';
        send(mysocket, h2.buffer, length+1, 0); 	// PAYLOAD
        zeraBuffer(h2.buffer); //zera o buffer para usar de novo
	}
}


int main(int argc, char *argv[]){

	//VARIAVEIS
	char* numip;
	int numport;
	char mode[10]; 
	FILE *input, *output;
	int mysock, newsock;
	header h1, h2;
    parameters p1;
    pthread_t thread_id1, thread_id2;

	//INICIALIZAÇÕEs
	input = fopen( argv[1], "rb" );
	output = fopen( argv[2], "ab" );
	numip = argv[3];
	numport = atoi( argv[4] );
	strcpy( mode , argv[5] );
	h1.syn1[0] = 0xDC;	h1.syn1[1] = 0xC0;
	h1.syn1[2] = 0x23;	h1.syn1[3] = 0xC2;
	h1.syn2[0] = 0xDC;	h1.syn2[1] = 0xC0;
	h1.syn2[2] = 0x23;	h1.syn2[3] = 0xC2;
	h1.reserved[0] = 0x00;
	h1.reserved[1] = 0x00;
	h2 = h1;
   	p1.output = output;

    //char *out = argv[2]; //usar esse se não puder fechar a conexao
    //p1.out = out; //usar esse se não puder fechar a conexao

	//cria socket
	mysock = socket(AF_INET, SOCK_STREAM, 0); 
	if (mysock == -1) logexit("socket");

    //ADDRESS STRUCTURE
    //struct in_addr addr = { .s_addr = inet_addr(numip) }; //TROCAR ESSE COM O DEBAIXO DEPOIS
    struct in_addr addr = { .s_addr = htonl(INADDR_LOOPBACK) };
    struct sockaddr_in dst = { .sin_family = AF_INET,
                               .sin_port = htons(numport),
                               .sin_addr = addr };
    struct sockaddr *sa_dst = (struct sockaddr *)&dst;

    //////////////////////////////////////////////////////////////////////////////////
	//					MODO ATIVO
	if(strcmp(mode, "ativo") == 0){
		//ABERTURA ATIVA
		newsock = socket(AF_INET, SOCK_STREAM, 0);
		if(connect(newsock, sa_dst, sizeof(dst))) logexit("connect: ");
		p1.mysocket = newsock;
		//RECEPTOR
		pthread_create( &thread_id1 , NULL, receptor , (void*) &p1);
		//pthread_join(thread_id1, NULL);
		fclose(output);
		//TRANSMISSOR
		transmissor(input, p1.mysocket, h2);
		printf("saiu\n");
		fclose(input);
		close(newsock);
	}
	////////////////////////////////////////////////////////////////////////////////////////////
	//					MODO PASSIVO
	else if(strcmp( mode, "passivo" ) == 0 ){
    	if( bind(mysock, sa_dst, sizeof(dst)) < 0){
      		close(mysock);
      		fclose(input);
      		fclose(output);
      		logexit("bind: ");
    	}
    	//while(1){  printf("ESPERANDO CONEXAO\n");
	//ESPERA DE CONEXÃO
	if( listen(mysock, 1) < 0 ){
      		close(mysock);
      		fclose(input);
      		fclose(output);
      		logexit("listen: \n");
    	} 
    	//CLIENT ADDRESS
    	//struct in_addr client_addr = { .s_addr = inet_addr(numip) }; //TROCAR ESSE COM O DEBAIXO DEPOIS
    	struct in_addr client_addr = { .s_addr = htonl(INADDR_ANY) }; 
    	socklen_t client_len = sizeof(client_addr);
    	//COMPLETA A ABERTURA PASSIVA
    	newsock = accept(mysock, (struct sockaddr *)&client_addr, &client_len);
    	p1.mysocket = newsock;
    	//RECEPTOR
       	pthread_create( &thread_id2 , NULL ,  receptor , (void*) &p1);
    	//pthread_join(thread_id2, NULL);
    	fclose(output);
    	//TRANSMISSOR
    	transmissor(input, p1.mysocket, h2);
    	fclose(input);
    	//}
    	close(newsock);
	}
	//close(mysock);
	//close(newsock);
	return 0;
}
