#ifndef FTP_COM_CON_H
#define FTP_COM_CON_H

#include <ftp_misc.h>
#include <dirent.h> 
#include <libgen.h>

#define _GNU_SOURCE

#define PORT 8080

#define MAX_COM 11
#define SYST_COM "SYST\0"
#define USER_COM "USER\0"
#define PASS_COM "PASS\0"
#define FEAT_COM "FEAT\0"
#define PWD_COM  "PWD\0"
#define QUIT_COM "QUIT\0"
#define PASV_COM "PASV\0"
#define LIST_COM "LIST\0"
#define CWD_COM  "CWD\0"
#define RETR_COM "RETR\0"
#define TYPE_COM "TYPE\0"

void ftp_init();
void ftp_accept_conn();
void ftp_check_msg();
void ftp_proceed();
size_t ftp_command(char* command, char* response, client_ctrl_td* event);



#endif