#include <ftp_com_conn.h>
#include <ftp_data_conn.h>
#include <ftp_misc.h>

extern serv_socket_td seso;
extern serv_socket_td daso;
extern client_ctrl_td ctrl_clients[MAX_CONNECTIONS];
void sigint_handler(int signum);
int main(int argc, char const *argv[])
{
	if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("Error registering SIGINT handler");
        return 1;
    }
	ftp_init();
	data_ftp_init();
	while(1){
        ftp_accept_conn();
        ftp_check_msg();
        ftp_proceed();
        data_ftp_accept_conn();
        data_ftp_proceed();
        data_ftp_check_channel_valid();
    }
    return 0;
}

void sigint_handler(int signum) {
    printf("\nCtrl+C detected. Closing socket and exiting...\n");

    if(seso.server_fd != -1){
        shutdown(seso.server_fd, SHUT_RD);
        if(close(seso.server_fd) != 0){
            printf("WTF SERV\n");
        }
    }

    if(daso.server_fd != -1){
        shutdown(daso.server_fd, SHUT_RD);
        if(close(daso.server_fd) != 0){
            printf("WTF DATA\n");
        }
    }

    for(int i=0;i<MAX_CONNECTIONS;i++){
        if(ctrl_clients[i].conn_fd != -1){
            if(close(ctrl_clients[i].conn_fd) != 0){
            printf("WTF CTRL CLIENT\n");
            }   
        }
    }

    printf("All sockets closed\n");
    exit(0); // Terminate the program
}