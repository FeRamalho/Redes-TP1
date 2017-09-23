#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <math.h>

#define MAX_BUF 4096
#define MAX_IN 65535

//cabeçalho
typedef struct header{
	unsigned char syn[4];
	unsigned char chksum[2];
	unsigned char length[2];
	unsigned char reserved[2];
}header;

//parametros da thread
typedef struct parameters{
	FILE *output;
	int mysocket;
}parameters; 

//erro
void logexit(const char *str){
    if(errno) perror(str);
    else puts(str);
    exit(EXIT_FAILURE);
}

//calcula o checksum
unsigned checksum(void *buffer, size_t len, unsigned int count){
	unsigned char *buf = (unsigned char *)buffer;
	size_t i;
	for (i=0; i<len; i++)
		count += (unsigned int)(*buf++);
	return count;
}

//retorna o valor do checksum
unsigned checkresult(FILE *f){
	char buf[MAX_BUF];
	size_t len;
	len = fread(buf, sizeof(char), sizeof(buf), f);
	unsigned long check = checksum(buf, len, 0);
	return check;
}

//char to int
unsigned int hexa(unsigned char * entrada, int bytes){
	int i, aux;
	aux = bytes-1;
	unsigned int saida;
	for(i = 0; i < bytes; i++){
		int b = 2*(aux);
		int c = pow(16, b);
		saida += c * entrada[i];
		aux--;
		printf("%d = %x\n", b, saida); 	
	}
	return saida;
}

//thread function que faz o papel do receptor 
void *receptor(void *parameter){ //falta fazer esse trem de thread funcionar, ve na monitoria
	parameters *p1;
	p1 = (parameters*)parameter;
	FILE *output = p1->output;
	int mysock = p1->mysocket;
	return NULL;
}

//receptor sem thread 
void receptor1(FILE *output, int mysocket){
	unsigned char buffer[MAX_IN];
	int i;
	while(1){
		read(mysocket, &buffer, 1);
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
										unsigned int checksum;
										unsigned int length;
										unsigned char aux[2];
										read(mysocket, &aux, 2);
										checksum =  hexa(aux, 2);
										read(mysocket, &aux, 2);
										length = hexa(aux, 2);
										unsigned int ordem = ntohs(length);
										read(mysocket, &aux, 2);
										read(mysocket, &buffer, length);
										printf("recebi: ");
										int i;
										//if(checksum = checksum){}	 // tem que fazer o checksum pra esse quadro 
										for(i = 0; i < length; i++){
											fprintf(output, "%x", buffer[i]);
										}
										//tem que fazer isso aqui funcionar indefinidamente até acabar a conexao
										//porque se nao ele nao acaba e nunca da fclose(); ve na monitoria
										break;
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
	unsigned char buffer[MAX_IN];
	unsigned char len_net;
	unsigned int length;
	while(!EOF){ // faz ler até o fim do arquivo
		fgets(buffer, sizeof(buffer), input);
		send(mysocket, &h2.syn , 4 , 0);
		send(mysocket, &h2.syn , 4 , 0);
		//calcula o checksum
		send(mysocket, &h2.chksum, 2, 0);
		length = strlen(buffer); //ve como faz o length do buffer
		len_net = htons(length);
		send(mysocket, &len_net, 2, 0);
		send(mysocket, &h2.reserved, 2, 0);
		send(mysocket, &buffer, length, 0 ); // ve se isso ta certo
	}
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
	h1.syn[0] = 0xDC;
	h1.syn[1] = 0xC0;
	h1.syn[2] = 0x23;
	h1.syn[3] = 0xC2;
	h1.reserved[0] = 0x00;
	h1.reserved[1] = 0x00;
	h2 = h1;
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
		//abertura ativa
		newsock = socket(AF_INET, SOCK_STREAM, 0);
		if(connect(newsock, sa_dst, sizeof(dst))){
			logexit("connect");
		}
		p1.mysocket = newsock;
		//pthread_create( &thread_id , NULL, receptor , (void*) &p1);
		//transmissor
	}
	//modo passivo
	else if(strcmp( mode, "passivo" ) == 0 ){
		//abertura passiva
    	if( bind(mysock, sa_dst, sizeof(dst)) < 0){
      		close(mysock);
      		logexit("EEROR: binding");
    	}
    	//espera de conexão
    	if( listen(mysock, 1) < 0 ){
      		close(mysock);
      		logexit("ERROR listening\n");
    	}
    	while(1){
    		//client adress structure
    		//struct in_addr client_addr = { .s_addr = htonl(numip) };
    		struct in_addr client_addr = { .s_addr = htonl(INADDR_ANY) }; 
    		socklen_t client_len = sizeof(client_addr);

    		//completa a abertura passiva
    		newsock = accept(mysock, (struct sockaddr *)&client_addr, &client_len);
    		p1.mysocket = newsock;
    		//pthread_create( &thread_id , NULL ,  receptor , (void*) &p1);
    		//transmissor
    	}
	}
	close(mysock);
	close(newsock);
	fclose(input);
	fclose(output);
	return 0;
}