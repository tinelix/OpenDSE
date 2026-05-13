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

#ifdef WIN32

#include <windows.h>
#include <os/win32/dsewin32.h>

int _dse_free_frames = 0;
int _dse_frames_count = 0;
int _dse_frame_samples = 0;

int WINAPI DllMain(HINSTANCE hInst, DWORD fdReas, PVOID pvRes) {
	return TRUE;
}

int _dse_open_outdev(DSE_OUTDEV* outdev, DSE_MMIO* mmio) {

	int result = 0;

	#ifdef WIN32_MME
		result = _dse_waveout_open(outdev, mmio);
	#endif		

	_dse_free_frames = 32;
	_dse_frames_count = 32;
	_dse_frame_samples = 384;

	return result;
}

int _dse_alloc_audio(DSE_MMIO* mmio) {
	uint_t   sample_size   = (mmio->audio.bit_depth / 2) * mmio->audio.channels;
	uchar_t* inbuf         = mmio->_i->inbuf;


	_dse_waveout_allocate(_dse_frame_samples, sample_size, _dse_frames_count);

	inbuf                  = (uchar_t*)malloc(
								_dse_frame_samples * sample_size * sizeof(uchar_t)
							);
	return 0;
}

int _dse_decode_audio(DSE_MMIO* mmio, ulong_t offset, ulong_t count) {
	
	uchar_t* inbuf         = mmio->_i->inbuf;
	uint_t   sample_size   = (mmio->audio.bit_depth / 2) * mmio->audio.channels;
	uint_t   frame_size    = _dse_frame_samples * sample_size;
	uint_t   buffer_size   = frame_size * _dse_frames_count;

	if(_dse_frames_count == 0)
		return -1;	
	
	if(count < buffer_size)
		buffer_size = count;

	_dse_waveout_allocate(_dse_frame_samples, sample_size, _dse_frames_count);

	inbuf                       = (uchar_t*)malloc(buffer_size * sizeof(uchar_t));
	mmio->_i->inbuf_size        = buffer_size;
	mmio->audio.frame_samples   = _dse_frame_samples;

	while(count <= mmio->bytes_total) {
		
		fseek(mmio->filesrc, offset, SEEK_SET);

		while(_dse_free_frames < _dse_frames_count - 1) {
			Sleep(50);
			_dse_free_frames = _dse_waveout_get_free_frames();

			if(_dse_free_frames == _dse_frames_count) {
			 	break;
			}
		}

		mmio->bytes_read += fread(inbuf, 1, buffer_size, mmio->filesrc);

		#ifdef WIN32_MME
			_dse_waveout_write((LPSTR)((char*)inbuf), buffer_size);
		#endif

		_dse_free_frames = _dse_waveout_get_free_frames();
		
		if(mmio->bytes_read < sizeof(inbuf)) {
			memset(inbuf + mmio->bytes_read, 0, sizeof(inbuf) - mmio->bytes_read);
		}

		offset += buffer_size;
		count  -= buffer_size;
	}

	free(inbuf);

	_dse_waveout_free();
}

int _dse_decode_audio2(DSE_MMIO* mmio, ulong_t offset) {

	uchar_t* inbuf         = mmio->_i->inbuf;
	uint_t   sample_size   = (mmio->audio.bit_depth / 2) * mmio->audio.channels;
	uint_t   frame_size    = _dse_frame_samples * sample_size;

	if(_dse_frames_count == 0)
		return -1;	

	if(offset >= mmio->bytes_total)
		return -2;

	if(mmio->bytes_total - offset < frame_size)
		return -3;

	 _dse_free_frames = _dse_waveout_get_free_frames();
	
	fseek(mmio->filesrc, offset, SEEK_SET);

	mmio->_i->inbuf_size        = frame_size;
	mmio->audio.frame_samples   = _dse_frame_samples;

	mmio->bytes_read += fread(inbuf, 1, frame_size, mmio->filesrc);

	#ifdef WIN32_MME
		_dse_waveout_write2((LPSTR)((char*)inbuf));
	#endif				
}

int _dse_free_audio(DSE_MMIO* mmio) {
	uchar_t* inbuf         = mmio->_i->inbuf;
	
	free(inbuf);
	_dse_waveout_free();
	
	return 0;
}

bool _dse_is_busy() {
     _dse_free_frames = _dse_waveout_get_free_frames();
	 return _dse_free_frames != _dse_frames_count ? false : true;
}


int _dse_close_outdev(DSE_OUTDEV* outdev, DSE_MMIO* mmio) {
	#ifdef WIN32_MME
		return _dse_waveout_close(outdev, mmio);
	#endif		
}

#endif

