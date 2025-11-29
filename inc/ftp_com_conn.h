#ifndef FTP_COM_CON_H
#define FTP_COM_CON_H

#include <ftp_misc.h>
#include <dirent.h> 
#include <libgen.h>

#define _GNU_SOURCE

#define PORT 8080

void ftp_init();
void ftp_accept_conn();
void ftp_check_msg();
void ftp_proceed();
size_t ftp_command(char* command, char* response, client_ctrl_td* event);



#endif