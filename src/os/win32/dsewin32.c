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

#ifdef WIN32_MME
	HWAVEOUT hWaveOut;
#endif

int WINAPI DllMain(HINSTANCE hInst, DWORD fdReas, PVOID pvRes) {
	return TRUE;
}

int _dse_open_outdev(DSE_OUTDEV* outdev, DSE_MMIO* mmio) {
	#ifdef WIN32_MME
		_dse_waveout_open(outdev, mmio);
	#endif		
}

int _dse_decode_audio(DSE_MMIO* mmio, ulong_t offset, ulong_t count) {
	
	uchar_t* inbuf       = mmio->_i->inbuf;	
	uint_t   sample_size = (mmio->audio.bit_depth / 2) * mmio->audio.channels;

	#ifdef WIN32_MME
		_dse_waveout_allocate(1024, sample_size, 32);
	#endif

	mmio->bytes_read += fread(inbuf, offset, count, mmio->filesrc);

	#ifdef WIN32_MME
		_dse_waveout_write((LPSTR)inbuf, count);
	#endif
}

#endif

