#include "ftp_data_conn.h"
#include "ftp_misc.h"

data_channel_td data_channel;
serv_socket_td daso;

void data_ftp_init(){
    data_channel.open = 0;
    data_channel.data_channel_fd = -1;
    init_serv_socket(&daso, DATA_PORT);
}

void data_ftp_accept_conn(){
    int sock_descr = accept(daso.server_fd, (struct sockaddr *)&daso.address, (socklen_t*)&daso.addrlen);
    if (sock_descr<0){
        if((errno == EAGAIN) || (errno == EWOULDBLOCK)){
        } else{
            perror("In data accept");
            exit(EXIT_FAILURE);                
        }
    } else {
        printf("\n+++++++ new data connection ++++++++\n\n");
        if((data_channel.data_channel_fd != -1) || (data_channel.open == 0)){
            printf("data connection rejected\n");
            close(sock_descr);
        } else {
            printf("data connection approve\n");
            data_channel.data_channel_fd = sock_descr;
        }
    }
}
void data_ftp_proceed(){
    type_enum type = peek_queue();
    if(type != OPEN_DATA_CHANNEL &&
       type != SEND_DATA_BUFFER  &&
       type != SEND_DATA_FILE) 
        return;
    queue_read_td queue_read = read_queue();
    if(queue_read.type == OPEN_DATA_CHANNEL){
        data_channel.open = 1;
        return;
    }
    if(queue_read.type == SEND_DATA_BUFFER){
        if(data_channel.data_channel_fd != -1){
            send(data_channel.data_channel_fd, queue_read.data, queue_read.size, 0);
            shutdown(data_channel.data_channel_fd, SHUT_WR);
            char rp[] = "226 Directory send ok\r\n";
            write_queue(SEND_CTRL_BUFFER, rp, sizeof(rp), queue_read.retdescr);
        }
        return;
    }
    if(queue_read.type == SEND_DATA_FILE){
        if(data_channel.data_channel_fd != -1){
            FILE *fptr;
            fptr = fopen(queue_read.data, "r");
            if(fptr == NULL){
                printf("Cannot open a file %s", queue_read.data);
                shutdown(data_channel.data_channel_fd, SHUT_WR);
                char rp[] = "226 Transfer complete\r\n";
                write_queue(SEND_CTRL_BUFFER, rp, sizeof(rp), queue_read.retdescr);
                return;
            } else {
                printf("opening file %s", queue_read.data);
            }
            char buffer[1024];
            int ctr = 0;
            while(1){
                int byte = fgetc(fptr);
                if(byte == -1){
                    send(data_channel.data_channel_fd, buffer, ctr, 0);
                    break;
                }
                buffer[ctr] = (char)byte;
                ctr++;
                if(ctr == 1024){
                    send(data_channel.data_channel_fd, buffer, ctr, 0);
                    ctr = 0;
                }
            }
            fclose(fptr);

            shutdown(data_channel.data_channel_fd, SHUT_WR);
            char rp[] = "226 Transfer complete\r\n";
            write_queue(SEND_CTRL_BUFFER, rp, sizeof(rp), queue_read.retdescr);
        }
        return;
    }

}

void data_ftp_check_channel_valid(){
    if(data_channel.data_channel_fd != -1){
        char buffer[300] = {0};
        long valread = recv(data_channel.data_channel_fd, buffer, 300, MSG_DONTWAIT);
        if(valread == 0){
            // conn closed
            printf("data conn close %d\n", data_channel.data_channel_fd);
            close(data_channel.data_channel_fd);
            data_channel.data_channel_fd = -1;
        }
    }
}