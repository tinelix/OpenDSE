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

int WINAPI DllMain(HINSTANCE hInst, DWORD fdReas, PVOID pvRes) {
	return TRUE;
}

int _dse_open_outdev(DSE_OUTDEV* outdev, DSE_MMIO* mmio) {
	#ifdef WIN32_MME
		return _dse_waveout_open(outdev, mmio);
	#endif		
}

int _dse_alloc_audio(DSE_MMIO* mmio) {
	uint_t   sample_size   = (mmio->audio.bit_depth / 2) * mmio->audio.channels;
	uint_t   frame_samples = 1024;
	uchar_t* inbuf         = mmio->_i->inbuf;

	_dse_waveout_allocate(frame_samples, sample_size, 32);

	inbuf                  = (uchar_t*)malloc(sample_size * sizeof(uchar_t));
	return 0;
}

int _dse_decode_audio(DSE_MMIO* mmio, ulong_t offset, ulong_t count) {
	
	uchar_t* inbuf         = mmio->_i->inbuf;
	uint_t   frames_count  = 32;
	uint_t   free_frames   = 32;	
	uint_t   sample_size   = (mmio->audio.bit_depth / 2) * mmio->audio.channels;
	uint_t   frame_samples = 1024;
	uint_t   frame_size    = frame_samples * sample_size;
	uint_t   buffer_size   = frame_size * frames_count;

	if(count < buffer_size)
		buffer_size = count;

	_dse_waveout_allocate(frame_samples, sample_size, 32);

	inbuf                       = (uchar_t*)malloc(buffer_size * sizeof(uchar_t));
	mmio->_i->inbuf_size        = buffer_size;
	mmio->audio.frame_samples   = frame_samples;

	while(count <= mmio->bytes_total) {
		
		fseek(mmio->filesrc, offset, SEEK_SET);

		while(free_frames < 31) {
			Sleep(200);
			free_frames = _dse_waveout_get_free_frames();

			if(free_frames > 31) {
			 	break;
			}
		}

		mmio->bytes_read += fread(inbuf, 1, buffer_size, mmio->filesrc);

		#ifdef WIN32_MME
			_dse_waveout_write((LPSTR)((char*)inbuf), buffer_size);
		#endif

		free_frames = _dse_waveout_get_free_frames();
		
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
	uint_t   frames_count  = 32;
	uint_t   free_frames   = 32;
	uint_t   sample_size   = (mmio->audio.bit_depth / 2) * mmio->audio.channels;
	uint_t   frame_samples = 1024;
	uint_t   frame_size    = frame_samples * sample_size;

	if(offset > mmio->bytes_total)
		return -1;

	free_frames = _dse_waveout_get_free_frames();

	/*while(free_frames < 31) {
		Sleep(100);
		free_frames = _dse_waveout_get_free_frames();

		if(free_frames > 31) {
		 	break;
		}
	}*/
	
	fseek(mmio->filesrc, offset, SEEK_SET);

	inbuf                       = (uchar_t*)malloc(sample_size * sizeof(uchar_t));
	mmio->_i->inbuf_size        = frame_size;
	mmio->audio.frame_samples   = frame_samples;

	mmio->bytes_read += fread(inbuf, 1, frame_size, mmio->filesrc);

	#ifdef WIN32_MME
		_dse_waveout_write2((LPSTR)((char*)inbuf), frame_size);
	#endif				
}

int _dse_free_audio(DSE_MMIO* mmio) {
	uchar_t* inbuf         = mmio->_i->inbuf;
	
	free(inbuf);
	_dse_waveout_free();
	
	return 0;
}


int _dse_close_outdev(DSE_OUTDEV* outdev, DSE_MMIO* mmio) {
	#ifdef WIN32_MME
		return _dse_waveout_close(outdev, mmio);
	#endif		
}

#endif

