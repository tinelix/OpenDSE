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

#ifdef UNIX

#include <os/unix/dseunix.h>
#include <devices/frontend.h>
#include <math.h>
#include <stdlib.h>

dse_frontend_t _dse_frontend = DSE_FRONTEND_AUTO;

 /*
  *  This file contains a frontend wrappers for UNIX/Linux, including ALSA.
  */

#ifndef UNIX_LEGACY

__attribute__((constructor))
void libopendse_start() {

}

__attribute__((destructor))
void libopendse_finish() {

}

#endif

int _dse_free_frames = 0;
int _dse_frames_count = 0;
int _dse_frame_samples = 0;

dse_result _dse_select_frontend(dse_frontend_t frontend) {
	
	int result = DSE_OUTDEV_UNSUPPORTED_BACKEND;
	
	switch(frontend) {
		case DSE_FRONTEND_AUTO:
			#ifdef UNIX_PULSEAUDIO
				_dse_frontend = DSE_FRONTEND_UNIX_PULSEAUDIO;
				result = DSE_OK;
			#else
				#ifdef UNIX_ALSA
					_dse_frontend = DSE_FRONTEND_LINUX_ALSA;
					result = DSE_OK;
				#endif
			#endif
			break;
		case DSE_FRONTEND_AUTO_LEGACY:
			#ifdef UNIX_ALSA
				_dse_frontend = DSE_FRONTEND_LINUX_ALSA;
				result = DSE_OK;
			#endif
			break;
		case DSE_FRONTEND_LINUX_ALSA:
			#ifdef UNIX_ALSA
				_dse_frontend = DSE_FRONTEND_LINUX_ALSA;
				result = DSE_OK;
			#endif
			break;
		case DSE_FRONTEND_UNIX_PULSEAUDIO:
			#ifdef UNIX_PULSEAUDIO
				_dse_frontend = DSE_FRONTEND_UNIX_PULSEAUDIO;
				result = DSE_OK;
			#endif
			break;
	}
}

dse_result _dse_open_outdev(DSE_OUTDEV* outdev, DSE_MMIO* mmio) {

	int result = DSE_OUTDEV_UNSUPPORTED_BACKEND;
	
	outdev->_i = malloc(sizeof(DSE_IDEVICE));
	
	switch(_dse_frontend) {
		case DSE_FRONTEND_LINUX_ALSA:
			#ifdef UNIX_ALSA
				_dse_alsa_open(outdev, mmio);
				outdev->_i->frontend = DSE_FRONTEND_LINUX_ALSA;
				result = DSE_OK;
			#endif
			break;
		case DSE_FRONTEND_UNIX_PULSEAUDIO:
			#ifdef UNIX_PULSEAUDIO
				_dse_pulseaudio_open(outdev, mmio);
				outdev->_i->frontend = DSE_FRONTEND_UNIX_PULSEAUDIO;
				result = DSE_OK;
			#endif
			break;
		default:
			_dse_select_frontend(DSE_FRONTEND_AUTO);
			break;
	}
	
	_dse_frame_samples = 384;

	return result;
}

dse_result _dse_alloc_audio(DSE_MMIO* mmio) {
	uint_t   sample_size   = (mmio->audio.bit_depth / 8) * mmio->audio.channels;
	uint_t   frame_size    = _dse_frame_samples * sample_size;

	switch(_dse_frontend) {
		case DSE_FRONTEND_LINUX_ALSA:
			#ifdef UNIX_ALSA
				_dse_alsa_prepare(_dse_frame_samples, sample_size, 32);
			#endif
			break;
		case DSE_FRONTEND_UNIX_PULSEAUDIO:
			#ifdef UNIX_PULSEAUDIO
				_dse_pulseaudio_prepare(_dse_frame_samples, sample_size, 32);
			#endif
			break;
	}
	
	_dse_free_frames = 32;
	_dse_frames_count = 32;

	mmio->_i->inbuf   = (uchar_t*)malloc(
                            _dse_frame_samples * sample_size * sizeof(uchar_t)
                        );
                        
    mmio->_i->inbuf_size        = frame_size;
	mmio->audio.frame_samples   = _dse_frame_samples;
	
	return DSE_OK;
}

dse_result _dse_decode_audio(DSE_MMIO* mmio, ulong_t offset, ulong_t count) {
	
	uchar_t* inbuf         = mmio->_i->inbuf;
	uint_t   sample_size   = (mmio->audio.bit_depth / 8) * mmio->audio.channels;
	uint_t   frame_size    = _dse_frame_samples * sample_size;
	uint_t   buffer_size   = frame_size * _dse_frames_count;

	if(_dse_frames_count == 0)
		return -1;
	
	if(count < buffer_size)
		buffer_size = count;

	// TODO: Implement native APIs

	inbuf                       = (uchar_t*)malloc(buffer_size * sizeof(uchar_t));
	mmio->_i->inbuf_size        = buffer_size;
	mmio->audio.frame_samples   = _dse_frame_samples;

	while(count <= mmio->bytes_total) {
		
		fseek(mmio->filesrc, offset, SEEK_SET);

		while(_dse_free_frames < _dse_frames_count - 1) {
			usleep(50000);
			// TODO: Implement native APIs
			
			if(_dse_free_frames == _dse_frames_count) {
			 	break;
			}
		}

		mmio->bytes_read += fread(inbuf, 1, buffer_size, mmio->filesrc);

		// TODO: Implement native APIs
		
		if(mmio->bytes_read < sizeof(inbuf)) {
			memset(inbuf + mmio->bytes_read, 0, sizeof(inbuf) - mmio->bytes_read);
		}

		offset += buffer_size;
		count  -= buffer_size;
	}

	free(inbuf);
	
	// TODO: Implement native APIs
	
}

dse_result _dse_decode_audio2(DSE_MMIO* mmio, ulong_t offset) {

	uchar_t* inbuf         = mmio->_i->inbuf;
	uint_t   sample_size   = (mmio->audio.bit_depth / 8) * mmio->audio.channels;
	uint_t   frame_size    = _dse_frame_samples * sample_size;

	if(_dse_frames_count == 0)
		return -1;	

	if(offset >= mmio->bytes_total)
		return -2;

	if(mmio->bytes_total - offset < frame_size)
		return -3;
		
	#ifdef UNIX_ALSA
		if(_dse_frontend == DSE_FRONTEND_LINUX_ALSA)
			_dse_alsa_wait();
	#endif
	
	fseek(mmio->filesrc, offset, SEEK_SET);

	mmio->bytes_read += fread(inbuf, 1, frame_size, mmio->filesrc);
	
	mmio->_i->inbuf_size = frame_size;
	
	switch(_dse_frontend) {
		case DSE_FRONTEND_LINUX_ALSA:
			#ifdef UNIX_ALSA
				_dse_alsa_write2(inbuf);
			#endif
			break;
		case DSE_FRONTEND_UNIX_PULSEAUDIO:
			#ifdef UNIX_PULSEAUDIO
				_dse_pulseaudio_write2(inbuf);
			#endif
			break;
	}

	return DSE_OK;
}

dse_result _dse_free_audio(DSE_MMIO* mmio) {
	uchar_t* inbuf         = mmio->_i->inbuf;
	
	if(inbuf)
		free(inbuf);
	
	#ifdef UNIX_ALSA
		if(_dse_frontend == DSE_FRONTEND_LINUX_ALSA)
			_dse_alsa_free();
	#endif
	
	return DSE_OK;
}

cbool _dse_is_busy() {
     
     // TODO: Implement native APIs
     
     return cfalse;
}


int _dse_close_outdev(DSE_OUTDEV* outdev, DSE_MMIO* mmio) {
	#ifdef UNIX_ALSA
		if(_dse_frontend == DSE_FRONTEND_LINUX_ALSA)
			_dse_alsa_close();
	#endif
	
	return DSE_OK;
}

#endif
