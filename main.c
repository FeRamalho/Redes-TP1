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

//#define MAX_BUF 4096
#define MAX_IN 65535


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
void *receptor(void *parameter){ //falta fazer esse trem de thread funcionar, ve na monitoria
	parameters *p1;
	p1 = (parameters*)parameter;
	FILE *output = p1->output;
	int mysocket = p1->mysocket;
	unsigned char buffer[MAX_IN];
	header h1;
	int i = 0;
	//unsigned char arqbuffer[MAX_IN]; zeraBuffer(arqbuffer);
	uint16_t lengthTotal = 0;
	read(mysocket, &buffer, 1);
	while(1){
		if(buffer[i] = 0xdc){
			read(mysocket, &buffer, 1);
			if(buffer[i] = 0xc0){
				read(mysocket, &buffer, 1);
				if(buffer[i] = 0x23){
					read(mysocket, &buffer, 1);
					if(buffer[i] = 0xc2){
						read(mysocket, &buffer, 1);
						if(buffer[i] = 0xdc){
							read(mysocket, &buffer, 1);
							if(buffer[i] = 0xc0){
								read(mysocket, &buffer, 1);
								if(buffer[i] = 0x23){
									read(mysocket, &buffer, 1);
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
        								unsigned char *p=(unsigned char *)&h1;
										newcheck = cksum(p, length+112);
										if(newcheck == checksum){
											printf("deu certo\n");
											//strcat(arqbuffer, buffer);
											//lengthTotal+= length;
											printf("%s\n", buffer);
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
		int a = recv(mysocket,&buffer,1,0);
		//read(mysocket, &buffer, 1);
		if(a <= 0) {  //fwrite(&arqbuffer,sizeof(unsigned char), lengthTotal, output); 
			break;}
	}
	//pthread_exit(NULL);
	//return NULL;
}

//receptor sem thread 
void receptor1(FILE *output, int mysocket){
	unsigned char buffer[MAX_IN];
	header h1;
	int i = 0;
	unsigned char arqbuffer[MAX_IN];
	uint16_t length = 0;
	while(1){
		int a = recv(mysocket,&buffer,1,0);
		if(a <= 0) {  /*fwrite(&arqbuffer,sizeof(unsigned char), length, output);*/ break;}
		if(buffer[i] = 0xdc){
			read(mysocket, &buffer, 1);
			if(buffer[i] = 0xc0){
				read(mysocket, &buffer, 1);
				if(buffer[i] = 0x23){
					read(mysocket, &buffer, 1);
					if(buffer[i] = 0xc2){
						read(mysocket, &buffer, 1);
						if(buffer[i] = 0xdc){
							read(mysocket, &buffer, 1);
							if(buffer[i] = 0xc0){
								read(mysocket, &buffer, 1);
								if(buffer[i] = 0x23){
									read(mysocket, &buffer, 1);
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
										zeraBuffer(buffer);
        								unsigned char *p=(unsigned char *)&h1;
										newcheck = cksum(p, length+112);
										if(newcheck == checksum){
											printf("deu certo\n");
											strcat(arqbuffer, h1.buffer);
											//uint16_t lengthTotal; lengthTotal+= length;
											fwrite(&buffer,sizeof(unsigned char), length, output);
										}
										zeraBuffer(h1.buffer);
										//break;
									}
								}
							}
						}	
					}
				}
			}
		}

		
	}
}

//funcao que tem o papel do transmissor
void transmissor(FILE *input, int mysocket, header h2){ //tem que terminar a transmissao
	unsigned char buffer[1];
	uint16_t length = 0, flaglength=0;
	while(!feof(input)){ //le ate o fim do arquivo
		int i;
		while( fread(&buffer,sizeof(unsigned char), 1, input) ){
			if(flaglength == 80){ //mudar para MAX_IN depois
				flaglength = 0;
				break;
			} 
			length ++;
			flaglength++;
			strcat(h2.buffer, buffer);
		} 
		//printf("%s\n", h2.buffer);
		h2.length = htons(length);
        uint16_t newlen = length+112;
        h2.chksum = 0; //zera o checksum inicial
        unsigned char *p=(unsigned char *)&h2;
        uint16_t check = cksum(p, newlen);
        h2.chksum = check;
        unsigned char buf[2];
		send(mysocket, h2.syn1, 4, 0); 		  	// syn
		send(mysocket, h2.syn2, 4, 0); 		  	// syn
        send(mysocket, &h2.chksum, 2, 0); 	 	//checksum
		send(mysocket, &h2.length, 2, 0); 	 	//length
        send(mysocket, h2.reserved, 2, 0); 	  	//reserved
        send(mysocket, h2.buffer, length, 0); 	//buffer
        zeraBuffer(h2.buffer); //zera o buffer para usar de novo
        length=1 ;
        flaglength=1;
        h2.buffer[0] = buffer[0];
	}
	close(mysocket);	
}


int main(int argc, char *argv[]){

	//variaveis
	char* numip;
	int numport;
	char mode[10]; 
	FILE *input, *output;
	int mysock, newsock;
	header h1, h2;
    parameters p1;
    pthread_t thread_id;

	//inicializações
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

	//cria socket
	mysock = socket(AF_INET, SOCK_STREAM, 0); 
	if (mysock == -1) logexit("socket");

    //address structure
    //struct in_addr addr = { .s_addr = inet_addr(numip) }; //TROCAR ESSE COM O DEBAIXO DEPOIS
    struct in_addr addr = { .s_addr = htonl(INADDR_LOOPBACK) };
    struct sockaddr_in dst = { .sin_family = AF_INET,
                               .sin_port = htons(numport),
                               .sin_addr = addr };
    struct sockaddr *sa_dst = (struct sockaddr *)&dst;

	//modo ativo
	if(strcmp(mode, "ativo") == 0){
		//while(1){
		//abertura ativa
		newsock = socket(AF_INET, SOCK_STREAM, 0);
		if(connect(newsock, sa_dst, sizeof(dst))) logexit("connect: ");
		p1.mysocket = newsock;
		pthread_create( &thread_id , NULL, receptor , (void*) &p1);
		pthread_join(thread_id, NULL);
		//transmissor
		//transmissor(input, p1.mysocket, h2);
		//pthread_join(thread_id, NULL);	
		////////////////teste/////////////
		//receptor1(output,newsock);
		//pthread_exit(NULL);
		printf("saiu\n"); //}
	}
	else if(strcmp( mode, "passivo" ) == 0 ){ //modo passivo
		//abertura passiva
    	if( bind(mysock, sa_dst, sizeof(dst)) < 0){
      		close(mysock);
      		logexit("bind: ");
    	}
    	//espera de conexão
    	if( listen(mysock, 1) < 0 ){
      		close(mysock);
      		logexit("listen: \n");
    	}
    	while(1){
    		//client adress structure
    		//struct in_addr client_addr = { .s_addr = inet_addr(numip) }; //TROCAR ESSE COM O DEBAIXO DEPOIS
    		struct in_addr client_addr = { .s_addr = htonl(INADDR_ANY) }; 
    		socklen_t client_len = sizeof(client_addr);

    		//completa a abertura passiva
    		newsock = accept(mysock, (struct sockaddr *)&client_addr, &client_len);
    		p1.mysocket = newsock;
    		//pthread_create( &thread_id , NULL ,  receptor , (void*) &p1);
    		//pthread_join(thread_id, NULL);
    		//transmissor

    		//////////////teste/////////////
    		transmissor(input, p1.mysocket, h2);
    		//pthread_join(thread_id, NULL);
    		
    	}
	}
	close(mysock);
	close(newsock);
	fclose(input);
	fclose(output);
	return 0;
}