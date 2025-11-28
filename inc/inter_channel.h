#ifndef INTERCHANNEL_H
#define INTERCHANNEL_H

#include <ftp_misc.h>

typedef enum{
	NO_TYPE,
	OPEN_DATA_CHANNEL,
	SEND_DATA_BUFFER,
	SEND_DATA_FILE,
	SEND_CTRL_BUFFER
} type_enum;

#define QUEUE_SIZE 10
#define ICH_BUFFER_SIZE 1024

typedef struct{
	type_enum type[QUEUE_SIZE];
	char payload[ICH_BUFFER_SIZE][QUEUE_SIZE];
	int payload_size[QUEUE_SIZE];
	int ret_descr[QUEUE_SIZE];
	int rdptr;
	int wrptr;
} queue_td;

typedef struct{
	type_enum type;
	char* data;
	int size;
	int retdescr;
} queue_read_td;

void init_ich();
int write_queue(type_enum type, char* data, int size, int retdescr);
queue_read_td read_queue();
type_enum peek_queue();

#endif