#ifndef _OPENDSE_MMIO_MMIO_H
#define _OPENDSE_MMIO_MMIO_H

#include <stdio.h>
#include <utils/c_exts.h>

typedef enum {
	DSE_PROTOCOL_FILE,
	DSE_PROTOCOL_HTTP,
	DSE_PROTOCOL_RTSP
} mmio_proto_t;

typedef struct dse_mmio {
	FILE* 			filesrc;
	const char* 	path;
	size_t			bytes_read;
	bool 			opened;
	bool 			playing;
	bool 			paused;
	mmio_proto_t    protocol;
	void*           _i;
} DSE_MMIO;

int dse_mmio_open(DSE_MMIO* mmio, const char* path);
	
#endif
