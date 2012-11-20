/*
<Command Chat : Client.cpp>
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
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h> //read and write
#include<netinet/in.h>
#include<sys/socket.h>
#include<netdb.h>

#include<arpa/inet.h> //inet_ntop

#include<iostream>
#define PROXY "10.1.34.29" //your ip address
int PROXY_PORT = 5000;

using namespace std;

void error(string msg){
	perror(msg.c_str());
	exit(1);
}

void* reader(void *param){
    int sockfd = (int) param;
    char buffer[1024];
    int n;
    while(1){
        bzero(buffer , 1023);
        n = read(sockfd , buffer , 1023);
        if( n > 0){ 
            buffer[n]='\0';
            printf("Other: %s",buffer);
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
        bzero(buffer , 1023);
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
int main(int argc, char** argv){

    if( argc > 1 ){
        PROXY_PORT = atoi(argv[1]);
    }
	int sockfd;
	struct sockaddr_in  serv_addr;
	struct hostent* server;


	int n;
	//address of server
	//create a socket
	sockfd = socket ( AF_INET,SOCK_STREAM,0);

	server = gethostbyname(PROXY);
	int iplen = 16;
	char* ip = (char*) malloc(iplen);
	inet_ntop( AF_INET , (void*) server->h_addr_list[0], ip ,iplen);

	bzero( (sockaddr_in *)&serv_addr , sizeof(sockaddr_in));
	serv_addr.sin_family = AF_INET;

	bcopy( (char*)server->h_addr, (char*)& serv_addr.sin_addr.s_addr,server->h_length );
	serv_addr.sin_port = htons(PROXY_PORT);

	if(connect(sockfd,(struct sockaddr*)& serv_addr, sizeof(serv_addr)) < 0){
		error("Cannot connect to server");
	}
    
    pthread_t threads[2];
    pthread_create( &threads[0] , NULL , reader , (void*) sockfd);
    pthread_create( &threads[1] , NULL , writer , (void*) sockfd);
    for(int i=0;i<2;i++){
        pthread_join( threads[i], NULL);
    }
	close(sockfd);
	return 0;
}
