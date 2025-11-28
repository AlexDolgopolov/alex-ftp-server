#include "inter_channel.h"

queue_td queue;

void init_ich(){
	queue.rdptr = 0;
	queue.wrptr = 0;
	for(int i=0;i<QUEUE_SIZE;i++) queue.type[i] = NO_TYPE;
}

int write_queue(type_enum type, char* data, int size, int retdescr){
	if((queue.wrptr+1) == queue.rdptr) return -1;
	else{
		if((queue.wrptr+1 == QUEUE_SIZE) && (queue.rdptr == 0)) return -1;
		else{
			queue.type[queue.wrptr] = type;
			queue.ret_descr[queue.wrptr] = retdescr;
			if(size > ICH_BUFFER_SIZE) return -1;
			queue.payload_size[queue.wrptr] = size;
			if(size != 0) memcpy(queue.payload[queue.wrptr], data, size);
			if((queue.wrptr+1) == QUEUE_SIZE) queue.wrptr = 0;
			else queue.wrptr++;
			return 0;
		}
	}
}

queue_read_td read_queue(){
	queue_read_td retval;
	retval.type = NO_TYPE;
	if(queue.rdptr == queue.wrptr) return retval;
	else{
		retval.type = queue.type[queue.rdptr];
		retval.data = queue.payload[queue.rdptr];
		retval.size = queue.payload_size[queue.rdptr];
		retval.retdescr = queue.ret_descr[queue.rdptr];
		if((queue.rdptr+1) == QUEUE_SIZE) queue.rdptr = 0;
		else queue.rdptr++;
		return retval;
	}
}

type_enum peek_queue(){
	if(queue.rdptr == queue.wrptr) return NO_TYPE;
	else{
		return queue.type[queue.rdptr];
	}
}