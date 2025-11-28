#ifndef FTP_DATA_CONN_H
#define FTP_DATA_CONN_H

#include <ftp_misc.h>

#define DATA_PORT 12345

typedef struct{
	int data_channel_fd;
	int open;
} data_channel_td;

void data_ftp_init();
void data_ftp_proceed();
void data_ftp_accept_conn();
void data_ftp_check_channel_valid();

#endif