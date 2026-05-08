#ifndef _OPENDSE_MMIO_MMIO_H
#define _OPENDSE_MMIO_MMIO_H

#include <stdio.h>
#include <utils/c_exts.h>

typedef enum {
	DSE_PROTOCOL_FILE,
	DSE_PROTOCOL_HTTP,
	DSE_PROTOCOL_RTSP
} mmio_proto_t;

typedef struct _dse_audiofmt {
	char*    codec_id;
	uchar_t  fourcc[4];
	uint_t   sample_rate;
	uchar_t  bit_depth;
	ulong_t  bitrate;
	uchar_t  channels;
	bool     vbr;
	bool     joint_stereo;
} DSE_AUDIOCODEC;

typedef struct dse_mmio {
	FILE*           filesrc;
	const char*     path;
	size_t          bytes_read;
	bool            opened;
	bool            playing;
	bool            paused;
	mmio_proto_t    protocol;
	DSE_AUDIOCODEC  audio;
	void*           _i;
} DSE_MMIO;

int dse_mmio_open(DSE_MMIO* mmio, const char* path);

#endif
