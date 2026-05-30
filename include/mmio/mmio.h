/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2026, Dmitry Tretyakov
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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

// Multimedia Protocols
typedef enum {
	DSE_PROTOCOL_FILE,
	DSE_PROTOCOL_CDDA,
	DSE_PROTOCOL_HTTP,
	DSE_PROTOCOL_RTSP
} mmio_proto_t;

// Audio codec information

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

// Multimedia I/O structure with private object

typedef struct dse_mmio {
	FILE*           filesrc;
	const char*     path;
	ulong_t         bytes_read;
	ulong_t         bytes_total;
	cbool           opened;
	cbool           playing;
	cbool           paused;
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
