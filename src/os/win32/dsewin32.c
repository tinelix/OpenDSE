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

#ifdef WIN32

#include <windows.h>
#include <os/win32/dsewin32.h>
#include <devices/frontend.h>
#include <math.h>

int _dse_free_frames = 0;
int _dse_frames_count = 0;
int _dse_frame_samples = 0;

dse_frontend_t _dse_frontend;

 /*
  *  This file contains a frontend wrappers for Windows NT and 9x, including WaveOut
  *  and WASAPI.
  */

int WINAPI DllMain(HINSTANCE hInst, DWORD fdReas, PVOID pvRes) {
	return TRUE;
}

int _dse_select_frontend(dse_frontend_t frontend) {
	int result = -1;
	
	switch(frontend) {
		case DSE_FRONTEND_AUTO:
			#ifdef WIN32_WASAPI
				_dse_frontend = DSE_FRONTEND_WINDOWS_WASAPI;
				result = 0;
			#else
				#ifdef WIN32_MME
					_dse_frontend = DSE_FRONTEND_WINDOWS_WAVEIO;
					result = 0;
				#endif
			#endif
			break;
		case DSE_FRONTEND_AUTO_LEGACY:
			#ifdef WIN32_MME
				_dse_frontend = DSE_FRONTEND_WINDOWS_WAVEIO;
				result = 0;
			#endif
			break;
		case DSE_FRONTEND_WINDOWS_WASAPI:
			#ifdef WIN32_WASAPI
				_dse_frontend = DSE_FRONTEND_WINDOWS_WASAPI;
				result = 0;
			#endif
			break;
		case DSE_FRONTEND_WINDOWS_WAVEIO:
			#ifdef WIN32_MME
				_dse_frontend = DSE_FRONTEND_WINDOWS_WAVEIO;
				result = 0;
			#endif
			break;
	}
	
	return result;
}

dse_result _dse_open_outdev(DSE_OUTDEV* outdev, DSE_MMIO* mmio) {

	double force_fpi_load = sqrt(4);

	int result = -1;

	#ifdef WIN32_WASAPI
			_dse_free_frames = 8;
			_dse_frames_count = 8;
			_dse_frame_samples = 124 * ((double)mmio->audio.sample_rate / 8000);
	#else
		#ifdef WIN32_MME
			_dse_free_frames = 32;
			_dse_frames_count = 32;
			_dse_frame_samples = 384;
		#endif
	#endif
	
	
	switch(_dse_frontend) {
		case DSE_FRONTEND_WINDOWS_WASAPI:
			#ifdef WIN32_WASAPI
				_dse_wasapi_open(outdev, mmio);
			#endif
			break;
		case DSE_FRONTEND_WINDOWS_WAVEIO:
			#ifdef WIN32_MME
				_dse_waveout_open(outdev, mmio);
			#endif
			break;
	}

	return result;
}

dse_result _dse_alloc_audio(DSE_MMIO* mmio) {
	uint_t   sample_size   = (mmio->audio.bit_depth / 2) * mmio->audio.channels;
	uchar_t* inbuf         = mmio->_i->inbuf;

	#ifdef WIN32_MME
		_dse_waveout_prepare(_dse_frame_samples, sample_size, _dse_frames_count);
	#else
		#ifdef WIN32_WASAPI
			_dse_wasapi_prepare();
		#endif
	#endif

	inbuf = (uchar_t*)malloc(
				_dse_frame_samples * sample_size * sizeof(uchar_t)
			);
	return DSE_OK;
}

dse_result _dse_decode_audio(DSE_MMIO* mmio, ulong_t offset, ulong_t count) {
	
	uchar_t* inbuf         = mmio->_i->inbuf;
	uint_t   sample_size   = (mmio->audio.bit_depth / 2) * mmio->audio.channels;
	uint_t   frame_size    = _dse_frame_samples * sample_size;
	uint_t   buffer_size   = frame_size * _dse_frames_count;

	if(_dse_frames_count == 0)
		return -1;	
	
	if(count < buffer_size)
		buffer_size = count;

	switch(_dse_frontend) {
		case DSE_FRONTEND_WINDOWS_WASAPI:
			#ifdef WIN32_WASAPI
				_dse_wasapi_prepare(_dse_frame_samples, sample_size, _dse_frames_count);
				_dse_free_frames = 32;
				_dse_frames_count = 32;
			#endif
			break;
		case DSE_FRONTEND_WINDOWS_WAVEIO:
			#ifdef WIN32_MME
			_dse_waveout_prepare(_dse_frame_samples, sample_size, _dse_frames_count);
			#endif
			break;
	}

	inbuf                       = (uchar_t*)malloc(buffer_size * sizeof(uchar_t));
	mmio->_i->inbuf_size        = buffer_size;
	mmio->audio.frame_samples   = _dse_frame_samples;

	while(count <= mmio->bytes_total) {
		
		fseek(mmio->filesrc, offset, SEEK_SET);

		while(_dse_free_frames < _dse_frames_count - 1) {
			Sleep(50);
			
			#ifdef WIN32_MME
				if(_dse_frontend == DSE_FRONTEND_WINDOWS_WAVEIO)
					_dse_free_frames = _dse_waveout_get_free_frames();
			#endif
			
			if(_dse_free_frames == _dse_frames_count) {
			 	break;
			}
		}

		mmio->bytes_read += fread(inbuf, 1, buffer_size, mmio->filesrc);

		switch(_dse_frontend) {
			case DSE_FRONTEND_WINDOWS_WASAPI:
				#ifdef WIN32_WASAPI
					_dse_wasapi_write((LPSTR)((char*)inbuf), buffer_size);
				#endif
				break;
			case DSE_FRONTEND_WINDOWS_WAVEIO:
				#ifdef WIN32_MME
					_dse_waveout_write((LPSTR)((char*)inbuf), buffer_size);
					_dse_free_frames = _dse_waveout_get_free_frames();
				#endif
				break;
		}
		
		if(mmio->bytes_read < sizeof(inbuf))
			memset(inbuf + mmio->bytes_read, 0, sizeof(inbuf) - mmio->bytes_read);

		offset += buffer_size;
		count  -= buffer_size;
	}

	free(inbuf);
	
	switch(_dse_frontend) {
		case DSE_FRONTEND_WINDOWS_WASAPI:
			#ifdef WIN32_WASAPI
				_dse_wasapi_free();
			#endif
			break;
		case DSE_FRONTEND_WINDOWS_WAVEIO:
			#ifdef WIN32_MME
				_dse_waveout_free();
			#endif
			break;
	}
	
	return 0;
}

dse_result _dse_decode_audio2(DSE_MMIO* mmio, ulong_t offset) {

	uchar_t* inbuf         = mmio->_i->inbuf;
	uint_t   sample_size   = (mmio->audio.bit_depth / 2) * mmio->audio.channels;
	uint_t   frame_size    = _dse_frame_samples * sample_size;

	if(_dse_frames_count == 0)
		return -1;

	if(offset >= mmio->bytes_total)
		return -2;

	if(mmio->bytes_total - offset < frame_size)
		return -3;
	
	#ifdef WIN32_MME
		if(_dse_frontend == DSE_FRONTEND_WINDOWS_WAVEIO)
			_dse_free_frames = _dse_waveout_get_free_frames();
	#endif
	
	fseek(mmio->filesrc, offset, SEEK_SET);

	mmio->_i->inbuf_size        = frame_size;
	mmio->audio.frame_samples   = _dse_frame_samples;

	mmio->bytes_read += fread(inbuf, 1, frame_size, mmio->filesrc);
	
	switch(_dse_frontend) {
		case DSE_FRONTEND_WINDOWS_WASAPI:
			#ifdef WIN32_WASAPI
				_dse_wasapi_write((LPSTR)((char*)inbuf), frame_size);
			#endif
			break;
		case DSE_FRONTEND_WINDOWS_WAVEIO:
			#ifdef WIN32_MME
				_dse_waveout_write2((LPSTR)((char*)inbuf));
			#endif
			break;
	}
	return 0;
}

dse_result _dse_free_audio(DSE_MMIO* mmio) {
	uchar_t* inbuf         = mmio->_i->inbuf;
	
	switch(_dse_frontend) {
		case DSE_FRONTEND_WINDOWS_WASAPI:
			#ifdef WIN32_WASAPI
				_dse_wasapi_free();
			#endif
			break;
		case DSE_FRONTEND_WINDOWS_WAVEIO:
			#ifdef WIN32_MME
				_dse_waveout_free();
			#endif
			break;
	}

	if (mmio->filesrc) {
		fclose(mmio->filesrc);
		mmio->filesrc = NULL;
	}

	if (mmio && mmio->_i && mmio->_i->inbuf) {
		free(mmio->_i->inbuf);
		mmio->_i->inbuf = NULL;
	}
	
	return 0;
}

dse_result _dse_is_busy() {
	#ifdef WIN32_MME
		if(_dse_frontend == DSE_FRONTEND_WINDOWS_WAVEIO)
			_dse_free_frames = _dse_waveout_get_free_frames();
	#endif
	 return _dse_free_frames != _dse_frames_count ? DSE_OK : DSE_BUSY;
}


dse_result _dse_close_outdev(DSE_OUTDEV* outdev, DSE_MMIO* mmio) {
	int result = 0;
	
	switch(_dse_frontend) {
		case DSE_FRONTEND_WINDOWS_WASAPI:
			#ifdef WIN32_WASAPI
				result = _dse_wasapi_close(outdev, mmio);
			#endif
			break;
		case DSE_FRONTEND_WINDOWS_WAVEIO:
			#ifdef WIN32_MME
				result = _dse_waveout_close();
			#endif
			break;
	}
	
	return result;
}

#endif

