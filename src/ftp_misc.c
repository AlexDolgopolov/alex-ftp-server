#include "ftp_misc.h"

void init_serv_socket(serv_socket_td* serv_socket, int port){
    // Creating socket file descriptor
    serv_socket->addrlen = sizeof(serv_socket-> address);
    if ((serv_socket->server_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) == 0){
        printf("%s error message: ", serv_socket->id_string);
        perror("In socket");
        exit(EXIT_FAILURE);
    }

    serv_socket->address.sin_family = AF_INET;
    serv_socket->address.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_socket->address.sin_port = htons(port);
    memset(serv_socket->address.sin_zero, '\0', sizeof(serv_socket->address.sin_zero));
    
    if (bind(serv_socket->server_fd, (struct sockaddr *)&serv_socket->address, sizeof(serv_socket->address))<0){
        printf("%s error message: ", serv_socket->id_string);
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(serv_socket->server_fd, LISTEN_BACKLOG) < 0){
        printf("%s error message: ", serv_socket->id_string);
        perror("In listen");
        exit(EXIT_FAILURE);
    }
}

void serv_socket_accept(serv_socket_td* serv_socket, int gratitude, char* gratitude_msg, client_ctrl_td* clients){
    int sock_descr = accept(serv_socket->server_fd, (struct sockaddr *)&serv_socket->address, (socklen_t*)&serv_socket->addrlen);
    if (sock_descr<0){
        if((errno == EAGAIN) || (errno == EWOULDBLOCK)){
        } else{
            printf("%s error message: ", serv_socket->id_string);
            perror("In accept");
            exit(EXIT_FAILURE);                
        }
    } else {
        printf("New connection to %s\n", serv_socket->id_string);
        int connection_stat = 0;
        for(int i=0;i<MAX_CONNECTIONS;i++){
            if(clients[i].conn_fd == -1){
                clients[i].conn_fd = sock_descr;
                if(gratitude){
                    send(clients[i].conn_fd, gratitude_msg, strlen(gratitude_msg), 0);
                }
                connection_stat = 1;
                break;
            }
        }
        if(connection_stat){
            printf("connection approve to %s\n", serv_socket->id_string);
        } else {
            printf("connection rejected to %s\n", serv_socket->id_string);
            close(sock_descr);
        }
    }
}
