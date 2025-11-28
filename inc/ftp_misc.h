#ifndef FTP_MISC_H
#define FTP_MISC_H

#define LISTEN_BACKLOG 10
#define MAX_CONNECTIONS 10

#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include "errno.h"
#include <signal.h>
#include <arpa/inet.h> // For inet_ntoa
#include <inter_channel.h>


typedef struct{
	char id_string[100];
	int server_fd;
	struct sockaddr_in address;
	int addrlen;
} serv_socket_td;

typedef struct{
	int conn_fd;
	char name[250];
	char pass[250];
	char dir[250];
} client_ctrl_td;

void init_serv_socket(serv_socket_td* serv_socket, int port);
void serv_socket_accept(serv_socket_td* serv_socket, int gratitude, char* gratitude_msg, client_ctrl_td* clients);

#endif