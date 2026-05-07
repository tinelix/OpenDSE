#ifndef _OPENDSE_MMIO_MMIO_H
#define _OPENDSE_MMIO_MMIO_H

#include <stdio.h>

typedef enum {
	DSE_PROTOCOL_FILE,
	DSE_PROTOCOL_HTTP,
	DSE_PROTOCOL_RTSP
} mmio_proto_t;

typedef struct dse_mmio {
	FILE* 			filesrc;
	const char* 	path;
	bool 			opened;
	bool 			playing;
	bool 			paused;
	mmio_proto_t    protocol;
	void*           _i;
} DSE_MMIO;
	
#endif
