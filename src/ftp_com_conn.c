#include <ftp_com_conn.h>
#include <ftp_misc.h>

client_ctrl_td ctrl_clients[MAX_CONNECTIONS];
serv_socket_td seso;

int data_sock_descr;
int data_buffer[1024];
int data_buffer_len;
int data_in_response_fd;

int data_descr_ret;
int data_buffer_ret[1024];
int data_buffer_ret_len;

void ftp_init(){
    for(int i=0;i<MAX_CONNECTIONS;i++) {
        ctrl_clients[i].conn_fd = -1;
        char* init_dir = "/mnt/harddrive0";
        memcpy(ctrl_clients[i].dir, init_dir, strlen(init_dir));
    }
    char* server_id = "server_socket";
    memcpy(seso.id_string, server_id, strlen(server_id));
    init_serv_socket(&seso, PORT);
}

void ftp_accept_conn(){
    char* gratitude_msg = "220 FTP server ready.\n";
    serv_socket_accept(&seso, 1, gratitude_msg, ctrl_clients);
}

void ftp_check_msg(){
    for(int i=0;i<MAX_CONNECTIONS;i++){
        if(ctrl_clients[i].conn_fd != -1){
            char buffer[300] = {0};
            long valread = recv(ctrl_clients[i].conn_fd , buffer, 300, MSG_DONTWAIT);
            if(valread == 0){
                // conn closed
                printf("conn close %i\n", i);
                close(ctrl_clients[i].conn_fd);
                ctrl_clients[i].conn_fd = -1;
                continue;
            }
            if(valread == -1){
                if((errno == EAGAIN) || (errno == EWOULDBLOCK)){
                } else {
                    perror("In read");
                    exit(EXIT_FAILURE);                
                }
                continue;
            }
            printf("message from %i\n", i);
            buffer[valread] = 0;
            printf("%s\n", buffer);
            char response[300];
            int resp_len = ftp_command(buffer, response, &ctrl_clients[i]);
            printf("[%d]RESPONSE: %s\n", resp_len, response);
            send(ctrl_clients[i].conn_fd, response, resp_len-1, 0);
        }
    }
}

void ftp_proceed(){
    type_enum type = peek_queue();
    if(type != SEND_CTRL_BUFFER) return;
    queue_read_td queue_read = read_queue();
    if(queue_read.type == SEND_CTRL_BUFFER){
        if(queue_read.retdescr != -1){
            send(queue_read.retdescr, queue_read.data, queue_read.size, 0);
        }
        return;
    }
}

size_t ftp_command(char* command, char* response, client_ctrl_td* client){
    printf("COMMAND = %s\n", command);
    char commands_list[MAX_COM][5] = {SYST_COM, USER_COM, PASS_COM, FEAT_COM, PWD_COM, QUIT_COM, PASV_COM, LIST_COM, CWD_COM, RETR_COM, TYPE_COM};
    for(int i=0;i<MAX_COM;i++){
        char* command_ptr = strstr(command, commands_list[i]);
        if(command_ptr != NULL){
            int idx = command_ptr - command;
            if(idx == 0){
                if(i == 0){
                    char rp[] = "215 UNIX Type: L8\n";
                    memcpy(response, rp, sizeof(rp));
                    return(sizeof(rp));
                }
                if(i == 1){
                    char rp[] = "331 Login succesfull. Hello (USERNAME NON IMPLEMENTED NOW)\n";
                    memcpy(response, rp, sizeof(rp));
                    return(sizeof(rp));
                }
                if(i == 2){
                    char rp[] = "230 nevermind\n";
                    memcpy(response, rp, sizeof(rp));
                    return(sizeof(rp));
                }
                if(i == 3){
                    char rp[] = "211 nothing lol\n";
                    memcpy(response, rp, sizeof(rp));
                    return(sizeof(rp));
                }
                if(i == 4){
                    int size = snprintf(response, 300, "257 \"%s\"\n", client->dir);
                    response[size] = 0;
                    return(size+1);
                }
                if(i == 5){
                    char rp[] = "221 Bye Bye\n";
                    memcpy(response, rp, sizeof(rp));
                    return(sizeof(rp));
                }
                if(i == 6){
                    char rp[] = "227 Entering Passive Mode (127,0,0,1,48,57)\n";
                    memcpy(response, rp, sizeof(rp));
                    write_queue(OPEN_DATA_CHANNEL, 0x0, 0, i);
                    return(sizeof(rp));
                }
                if(i == 7){
                    char rp[] = "150 Here comes the directory listing.\n";
                    memcpy(response, rp, sizeof(rp));

                    char data_rp[1024] = "";
                    DIR *d;
                    struct dirent *dir;
                    d = opendir(client->dir);
                    if (d) {
                        while ((dir = readdir(d)) != NULL) {
                            printf("%s\r\n", dir->d_name);
                            strcat(data_rp, dir->d_name);
                            strcat(data_rp, "\r\n");
                        }
                    closedir(d);
                    }
                    strcat(data_rp, "\r\n");

                    write_queue(SEND_DATA_BUFFER, data_rp, strlen(data_rp)+1, client->conn_fd);
                    return(sizeof(rp));
                }
                if(i == 8){
                    char rp[300] = {0};
                    char local_command[300];
                    strcpy(local_command, &command[4]);
                    printf("%s\n", local_command);
                    local_command[strlen(local_command)-2] = '\0';

                    char new_dir_path[300] = "";        
                    printf("%s\n", local_command);
                    
                    if(strstr(local_command, "..")){
                        char client_dir_cpy[300];
                        strcpy(client_dir_cpy, client->dir);
                        char* parent_dir = dirname(client_dir_cpy);
                        strcpy(new_dir_path, parent_dir);
                    } else {
                        snprintf(new_dir_path, 1024, "%s/%s", client->dir, local_command);
                    }
                    DIR *d;
                    d = opendir(new_dir_path);
                    printf("%s", new_dir_path);
                    if(d){
                        strcpy(client->dir, new_dir_path);
                        strcpy(rp, "250 Directory successfully changed.\n\0");
                    }else{
                        strcpy(rp, "550 Failed to change directory\n\0");
                    }
                    closedir(d);
                    memcpy(response, rp, strlen(rp) + 1);
                    return(strlen(rp) + 1);
                }
                if(i == 9){
                    char rp[] = "150 Opening BINARY mode data connection\n";
                    memcpy(response, rp, sizeof(rp));

                    char data_rp[1024] = "";
                    strcat(data_rp, client->dir);
                    strcat(data_rp, "/");
                    strcat(data_rp, &command[5]);
                    data_rp[strcspn(data_rp, "\r\n")] = '\0';
                    printf("%s", data_rp);
                    write_queue(SEND_DATA_FILE, data_rp, strlen(data_rp)+1, client->conn_fd);
                
                    return(sizeof(rp));
                }
                if(i == 10){
                    char rp[] = "200\n";
                    memcpy(response, rp, sizeof(rp));
                    return(sizeof(rp));
                }
            }
        }
    }
    char rp[] = "500 Non emplemented\r\n";
    memcpy(response, rp, sizeof(rp));
    return(sizeof(rp));
}