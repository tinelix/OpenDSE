/*
 * The Open Digital Sound Engine (OpenDSE) is free software and is licensed under 
 * the BSD 3-Clause license.
 *
 * Copyright (c) 2026, Dmitry Tretyakov <tinelix@mail.ru>
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

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <opendse.h>
#include <mmio/mmio.h>
#include <parsers/parser.h>

#ifdef WIN32
	#include <os/win32/dsewin32.h>
#elif UNIX
	#include <os/unix/dseunix.h>
#endif

DSE_MMIO* 		stdmmio;

dse_result dse_get_version(DSEVERSION* version) {
	version->major = 0;
	version->minor = 0;
	version->patch = 2;
	return DSE_OK;
}

dse_result dse_select_frontend(dse_frontend_t frontend) {
	_dse_select_frontend(frontend);
}

dse_result dse_open_input(const char* path) {
	
	int result = 0;

	stdmmio = (DSE_MMIO*)malloc(sizeof(DSE_MMIO));

	stdmmio->path = path;
	
	result = dse_mmio_open(stdmmio, path);

	if(result > 0) {
		
		uchar_t* inbuf = stdmmio->_i->inbuf;

		stdmmio->bytes_read = result;
		stdmmio->opened = ctrue;
		stdmmio->playing = cfalse;
		stdmmio->paused = cfalse;
		stdmmio->protocol = DSE_PROTOCOL_FILE;

		stdmmio->audio.sample_rate = 0;
		stdmmio->audio.bit_depth = 0;
		stdmmio->audio.bitrate = 0;
		stdmmio->audio.channels = 0;
		
		result = dse_parse_input(stdmmio, inbuf);
	}

	return result;
}

dse_result dse_close_input() {

	int result = 0;

	result = dse_mmio_close(stdmmio);

	free(stdmmio);
	stdmmio = NULL;

	return result;
}

dse_result dse_decode_audio() {
	int result = 0;
	result = _dse_decode_audio(stdmmio, stdmmio->bytes_read, stdmmio->bytes_total);
	return DSE_OK;
}

dse_result dse_decode_audio2(ulong_t offset) {
	return _dse_decode_audio2(stdmmio, offset);
}

dse_result dse_alloc_audio() {
	int result;

	result = _dse_alloc_audio(stdmmio);
	return result;
}

dse_result dse_is_busy() {
	int result = _dse_is_busy();

	return result == ctrue ? DSE_BUSY : DSE_OK;
}

dse_result dse_free_audio() {
	int result;

	result = _dse_free_audio(stdmmio);
	return DSE_OK;
}

int dse_get_frame_rms(double* rms, uint_t size) {

	uchar_t* inbuf          = stdmmio->_i->inbuf;
	double   rms_l          = 0.0;
	double   rms_r          = 0.0;
	uint_t   i              = 0;
	double   qsum_samples_l = 0.0;
	double   qsum_samples_r = 0.0;
	uint_t   buffer_size    = stdmmio->_i->inbuf_size;
	uint_t   ch_buffer_size = 0;
	uint_t   smooth_stage   = 4;
	uint_t   sample_size    = (double)(stdmmio->audio.bit_depth / 8) * stdmmio->audio.channels;
	ulong_t  max_value      = (double)pow(2, stdmmio->audio.bit_depth) / stdmmio->audio.channels;
	
	if(buffer_size >= 8192)
		smooth_stage = 64 * sample_size;
	else if(buffer_size >= 1024)
		smooth_stage = 32 * sample_size;
	else
		smooth_stage = 8 * sample_size;

	ch_buffer_size = 
		((double)buffer_size / smooth_stage) / sample_size;
	
	if(buffer_size == 0 && size == 0)
		return 0;
	
	for(i = 0; i < buffer_size; i += smooth_stage) {
		double    sample_l = 0;
		double    sample_r = 0;
		short     sample_16bit_l = 0;
        	short     sample_16bit_r = 0;

		if(i >= buffer_size - stdmmio->audio.channels) {
			break;
		}

		switch(stdmmio->audio.bit_depth) {
			case 16:
				memcpy(&sample_16bit_l, &inbuf[i], sizeof(short));
				sample_l = (double)abs(sample_16bit_l);

				if(stdmmio->audio.channels == 2) {
					memcpy(&sample_16bit_r, &inbuf[i + 2], sizeof(short));
					sample_r = (double)abs(sample_16bit_r);
				}
				break;
			default:
				if(i == buffer_size - (stdmmio->audio.channels))
					break;
				sample_l = (double)abs((char)inbuf[i]);
				break;
		}

		switch(stdmmio->audio.channels) {
			case 2:
				qsum_samples_l += (double)sample_l * sample_l;
				qsum_samples_r += (double)sample_r * sample_r;
				break;
			default:
				qsum_samples_l += (double)sample_l * sample_l;
				break;
		}
	}
	
	rms_l = (double)(sqrt(qsum_samples_l / ch_buffer_size)) / max_value;
	rms[0] = rms_l;
	
	if(stdmmio->audio.channels >= 2 && size >= 2) {
		rms_l = (double)(sqrt(qsum_samples_l / (double)ch_buffer_size)) / max_value;
		rms_r = (double)(sqrt(qsum_samples_r / (double)ch_buffer_size)) / max_value;
		rms[1] = rms_r;
		return 2;
	}
	
	return 1;
}
