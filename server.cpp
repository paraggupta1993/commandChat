/*
<Command Chat : server.cpp>
Copyright (C) <2012>  <Parag Gupta>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/



#include<iostream>
#include<stdio.h>
#include<sys/types.h>
#include<cstdlib>
#include<string.h>
#include<string>
#include<unistd.h>
#include<netinet/in.h> // for struct sockaddr_in 
#include<sys/socket.h> //for socket() and Defined constants
#include<netdb.h>
#include<arpa/inet.h>
#include<pthread.h> //threading
#include<map>

#include<ctype.h>
#define MX 100
#define FLUSH  {fflush(stdout);}
#include<vector>
#define repla 1
using namespace std;

int PROXY_PORT = 5000;

void error( string temp){
	perror(temp.c_str());
	exit(1);
}
void *process_client( void* args );

int main(int argc , char** argv){
	 if ( argc > 1 ){
        PROXY_PORT = atoi( argv[1] );
     }
    
	int sockfd;
	int newsockfd;

	struct sockaddr_in server_address;
	struct sockaddr_in client_address;

	int server_port;

	socklen_t clilen = sizeof( client_address);
	//create a socket
	sockfd = socket( AF_INET , SOCK_STREAM , 0 );
	if(sockfd < 0){
		error("can't establish socket");
	}//IFNET is the protocol family and arg2 => type of socket , arg3 
	//The protocol specifies a particular protocol to be used with the socket.  Normally only a single protocol exists to support a particular socket type within a  given  protocol family, in which case protocol can be specified as 0.  


	//fill the sockaddr_in to specify an internet address (where the socket should be bind)
	bzero( &(server_address) ,sizeof(server_address ));
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PROXY_PORT);
	server_address.sin_addr.s_addr = INADDR_ANY;



	//bind the created socket to the sockaddr_in
	if(bind( sockfd , (struct sockaddr *)&server_address , sizeof(struct sockaddr)) < 0) error("can't bind");

	//listen for the clients
	if(listen(sockfd ,5) < 0) error("cannot listen");
	//create a new socket for each client 
	int n;
	pthread_t threads[MX];
	int num=0;
	while(1){
		newsockfd = accept( sockfd , (sockaddr *)&client_address , &clilen);
		cout << "newsockfd: " << newsockfd << endl;
		pthread_create( &threads[num++], NULL , &process_client , (void*)newsockfd);
	}
	for(int i=0;i<num;i++){
		pthread_join(threads[i] , NULL);
	}
	close(sockfd);
	return 0;

}
void* reader( void *param){
    int sockfd = (int) param;
    char buffer[1024];
    int n;
    while(1){
        bzero( buffer, 1023);
        n = read(sockfd , buffer , 1023);
        if( n > 0){ 
            buffer[n]='\0';
            printf("Other: %s", buffer);
        }
        else break;
    }
    pthread_exit(0);
}
void* writer( void *param){
    int sockfd = (int) param;
    char buffer[1024];
    int n;
    while(1){
        bzero( buffer, 1023);
        scanf("%[^\n]",buffer);
        getchar();
        int len = strlen(buffer);
        if( len > 0){
            buffer[len] = '\n';
            buffer[len+1] = '\0';
            n = write(sockfd , buffer , strlen(buffer));
            if(n<0)break;
        }
    }
    pthread_exit(0);
}


void *process_client( void* args ){
    FLUSH;
    int n;
    int newsockfd = (int) args;
    char send[256];
    char rec[256];
    bzero( send , 256);
    bzero( rec, 256);


    sprintf(send,"Welcome to Command Chat !! \n");
    n = write(newsockfd,send,strlen(send));

    pthread_t threads[2];
    pthread_create( &threads[0] , NULL , reader , (void*) newsockfd);
    pthread_create( &threads[1] , NULL , writer , (void*) newsockfd);
    for(int i=0;i<2;i++){
        pthread_join( threads[i], NULL);
    }
    FLUSH;
    close(newsockfd);
    pthread_exit(0);
}



