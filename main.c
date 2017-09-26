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
#include <math.h>

//#define MAX_BUF 4096
#define MAX_IN 65535


//cabeçalho
typedef struct header{
	unsigned char syn1[4];
	unsigned char syn2[4];
	unsigned char chksum[2];
	unsigned char length[2];
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

void receiver(FILE *output, int mysocket, header h1){
	unsigned char buffer[MAX_IN];
	int i;
	while(recv(mysocket,buffer,8,0) == 4){ //recebe o sync
		if(buffer[0] == 0xdc && buffer[1] == 0xc0 && buffer[2] == 0x23 && buffer[3] == 0xc2){
			if(buffer[4] == 0xdc && buffer[5] == 0xc0 && buffer[6] == 0x23 && buffer[7] == 0xc2){
				//COLOCAR NA STRUCT DO HEADER PRA CALCULAR O CHECKSUM
				zeraBuffer(buffer); //zera o buffer para usar de novo
				if(recv(mysocket,buffer,2,0) == 2){ //recebe o checksum
					//unsigned int check = buffer;
					//converter o checksum de volta
					zeraBuffer(buffer);
					if(recv(mysocket,buffer,2,0) == 2){ //recebe o length
						//colocar no header
						if(recv(mysocket,buffer,2,0) == 2){ //recebe o reserved
							if(recv(mysocket,buffer,length,0) != -1){ //recebe o payload
								//colocar no header
								//calcular o checksum
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
	unsigned int length;
	while(!feof(input)){ //le ate o fim do arquivo
		int i;
		fread(&buffer,80, 1, input); //////////////MUDAR O TAMANHO DEPOIS
		strcpy(h2.buffer, buffer); //coloca o buffer no header
		length = strlen(h2.buffer) + 1; 
		length = htonl(length); //converte o length
		h2.chksum[0] = 0; //zera o checksum inicial
		h2.chksum[1] = 0;
		h2.length[0] = (length & 0xFF); //coloca o length no header
		h2.length[1] = ((length >> 8) & 0xFF);
      
        uint16_t newlen = length+115;
        unsigned char *p=(unsigned char *)&h2;
        unsigned int check = cksum(p, newlen);
        printf("CHE: %u\n", check);
        check = htonl(check); //converte o checksum
        h2.chksum[0] = (check & 0xFF); //coloca o checksum no header
		h2.chksum[1] = ((check >> 8) & 0xFF);

		unsigned char newbuffer[MAX_IN];
		zeraBuffer(newbuffer); //zera o buffer para usar de novo
		strcpy(newbuffer,h2.syn1);
		strcat(newbuffer,h2.syn2);
		send(mysocket, newbuffer, 8, 0);
		zeraBuffer(newbuffer); //zera o buffer para usar de novo
        strcpy(newbuffer,h2.chksum);
        //strcat(newbuffer,h2.length);
		send(mysocket, newbuffer, 2, 0);
		zeraBuffer(newbuffer); //zera o buffer para usar de novo
        strcpy(newbuffer,h2.length);
		send(mysocket, newbuffer, 2, 0);
		zeraBuffer(newbuffer); //zera o buffer para usar de novo
        strcpy(newbuffer,h2.reserved);
        send(mysocket, newbuffer, 2, 0);
        zeraBuffer(newbuffer); //zera o buffer para usar de novo
        strcpy(newbuffer,h2.buffer);
        send(mysocket, newbuffer, length, 0);
        zeraBuffer(buffer);
	}
		
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
	output = fopen( argv[2], "w" );
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
    struct in_addr addr = { .s_addr = inet_addr(numip) };
    //struct in_addr addr = { .s_addr = htonl(INADDR_LOOPBACK) };
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
		transmissor(input, p1.mysocket, h2);
		//receiver(output, newsock, h1);
		printf("saiu\n");
	}
	else if(strcmp( mode, "passivo" ) == 0 ){ //modo passivo
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
	//fclose(output);
	return 0;
}
