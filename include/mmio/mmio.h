#ifndef _OPENDSE_MMIO_MMIO_H
#define _OPENDSE_MMIO_MMIO_H

#include <stdio.h>

#ifdef OPENDSE_LIB
	#include <dsepriv.h>
#endif

#include "../utils/c_exts.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	DSE_PROTOCOL_FILE,
	DSE_PROTOCOL_HTTP,
	DSE_PROTOCOL_RTSP
} mmio_proto_t;

typedef struct _dse_audiofmt {
	uint_t    	codec_id;
	const char* str_id;
	uint_t   	sample_rate;
	uchar_t  	bit_depth;
	ulong_t  	bitrate;
	uchar_t  	channels;
	double      frame_rate;
	uint_t      frame_samples;
	cbool     	vbr;
	cbool     	joint_stereo;
	cbool       sbr;
	cbool       param_stereo;
} DSE_AUDIOCODEC;

typedef struct dse_mmio {
	FILE*           filesrc;
	const char*     path;
	ulong_t         bytes_read;
	ulong_t			bytes_total;
	cbool            opened;
	cbool            playing;
	cbool            paused;
	mmio_proto_t    protocol;
	DSE_AUDIOCODEC  audio;
	#ifdef OPENDSE_LIB
		DSE_IMMIO*  _i;
	#else
		void*       _i;
	#endif
} DSE_MMIO;

int dse_mmio_open(DSE_MMIO* mmio, const char* path);
int dse_mmio_close(DSE_MMIO* mmio);

#ifdef __cplusplus
}
#endif


#endif
