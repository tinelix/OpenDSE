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

#ifdef WIN32_MME

#include <os/win32/waveout.h>
#include <mmio/mmio.h>
#include <dsepriv.h>
#include <stdlib.h>

 /*
  *  This file contains a frontend implementation for the WaveOut API for
  *  Windows NT and 9x without DirectX installed.
  *
  *  NOTE:
  *  The code is for backward compatibility only and is not recommended for 
  *  use on Windows 8 or later.
  *
  *  The implementation by David Overton is used with technical modifications:
  *  https://github.com/Planet-Source-Code/david-overton-playing-audio-in-windows-using-waveout-interface__3-4422
  *
  */

static CRITICAL_SECTION wavSection;
static WAVEHDR*     	wavFrames;
static HANDLE           wavLargeHeap;
static volatile ulong_t wavFreeFrameCount;
       int              wavCurrentFrame;
       uint_t           wavFrameSize        = 1024;	
       uint_t           wavFramesCount      = 32;
	   uint_t           wavReadFramesCount  = 0;
       cbool            wavFrameLock        = cfalse;
	   DSE_OUTDEV*      wavOutdev;

HWAVEOUT hWaveOut;

int _dse_waveout_open(DSE_OUTDEV* outdev, DSE_MMIO* mmio) {

	int           result;
	char          errorText[161];
	WAVEOUTCAPS   devCaps;
	WAVEFORMATEX  wavFormat;
	
	result = waveOutGetDevCaps(outdev->id, &devCaps, sizeof(WAVEOUTCAPS));
	
	if(result != MMSYSERR_NOERROR)
		return -1;
	
	outdev->volume_control  = devCaps.dwSupport & WAVECAPS_VOLUME;
	outdev->balance_control = devCaps.dwSupport & WAVECAPS_LRVOLUME;
	outdev->max_channels    = (uint_t)devCaps.wChannels;
	outdev->product_name    = (char*)malloc(161 * sizeof(char*));

	outdev->sample_rate     = mmio->audio.sample_rate;
	outdev->bit_depth       = mmio->audio.bit_depth;
	outdev->channels        = mmio->audio.channels;

	/*  NOTE: WAVEFORMATEX is undocumented structure in Win32 Programmer's Reference 
	 *	      from Microsoft Visual C++ 2.0 and is located in the 'mmsystem.h' file.
	 *  
	 *        Available with Windows NT 3.1 and above.
	 */

	ZeroMemory(&wavFormat, sizeof(WAVEFORMATEX));

	wavFormat.wFormatTag      = WAVE_FORMAT_PCM;
	wavFormat.nSamplesPerSec  = mmio->audio.sample_rate;
	wavFormat.wBitsPerSample  = mmio->audio.bit_depth;
	wavFormat.nChannels       = mmio->audio.channels;
	wavFormat.nBlockAlign     = (mmio->audio.bit_depth * wavFormat.nChannels) / 8;
	wavFormat.nAvgBytesPerSec = mmio->audio.sample_rate * wavFormat.nBlockAlign;
	
	result = waveOutOpen(
		&hWaveOut, outdev->id, &wavFormat,
		(ulong_t*)_dse_waveout_process,
		(ulong_t*)&wavFreeFrameCount,
		CALLBACK_FUNCTION
	);

	waveOutGetErrorText(result, errorText, 160);

	outdev->product_name[0] = '\0';

	#ifdef MSVC_GE_800
			strcat_s(outdev->product_name, 12, "[waveOut] ");
	        strcat_s(outdev->product_name, 140, devCaps.szPname);
	#else
			strcat(outdev->product_name, "[waveOut] ");
	        strcat(outdev->product_name, devCaps.szPname);
	#endif

	wavOutdev = outdev;

	if(result == WAVERR_BADFORMAT)
		return -3;

	if(result == MMSYSERR_BADDEVICEID)
		return -4;

	if(result == MMSYSERR_NODRIVER)
		return -5;

	if(result == MMSYSERR_NOMEM)
		return -6;

	if(result == MMSYSERR_NOTSUPPORTED)
		return -7;

	if(result != MMSYSERR_NOERROR)
		return -2;

	return 0; 
}

WAVEHDR* _dse_waveout_prepare(uint_t size, uint_t sample_size, uint_t count) {
	
	uchar_t* buffer;
	int      i;
	ulong_t  buffer_size;

	wavFrameSize = size * sample_size;
	buffer_size  = (wavFrameSize + sizeof(WAVEHDR)) * count;

	//wavLargeHeap = HeapCreate(0, buffer_size, buffer_size * 4);

	if((buffer = HeapAlloc(
	      GetProcessHeap(), HEAP_ZERO_MEMORY, buffer_size
	   )) == NULL) {
		return NULL;   
	}

	wavFrames = (WAVEHDR*)buffer;
	buffer += sizeof(WAVEHDR) * count;

	for(i = 0; i < count; i++) {
		wavFrames[i].dwBufferLength = wavFrameSize;
		wavFrames[i].lpData = (LPSTR)buffer;
		buffer += wavFrameSize;
	}
	 

	InitializeCriticalSection(&wavSection);

	wavFreeFrameCount = count;

	return wavFrames;
}

void _dse_waveout_free() {
	HeapFree(GetProcessHeap(), 0, wavFrames);
	//HeapDestroy(wavLargeHeap);
}

void _dse_waveout_write(LPSTR data, int size) {
	
	WAVEHDR* current_frame;
	int remain;
	int result = 0;
	int bytesCount = 0;

	current_frame = &wavFrames[wavCurrentFrame];
	wavFreeFrameCount = wavFramesCount;
	
	while(size > 0) {

		if(current_frame->dwFlags & WHDR_PREPARED)
			waveOutUnprepareHeader(hWaveOut, current_frame, sizeof(WAVEHDR));

		if(size < (int)(wavFrameSize - current_frame->dwUser)) {
			memcpy(current_frame->lpData + current_frame->dwUser, data, size);
			current_frame->dwUser += size;
			break;
		}

		remain = wavFrameSize - current_frame->dwUser;
		
		memcpy(current_frame->lpData + current_frame->dwUser, data, remain);

		size   -= remain;
		data   += remain;

		current_frame->dwBufferLength = wavFrameSize;

		waveOutPrepareHeader(hWaveOut, current_frame, sizeof(WAVEHDR));
		waveOutWrite(hWaveOut, current_frame, sizeof(WAVEHDR));

		EnterCriticalSection(&wavSection);
		wavFreeFrameCount--;
		LeaveCriticalSection(&wavSection);

		while(wavFreeFrameCount < (int)(wavFramesCount / 1.5)) {
			Sleep(20);
		}
		
		wavCurrentFrame++;
		wavCurrentFrame %= wavFramesCount;
		bytesCount      += wavFramesCount;

		current_frame = &wavFrames[wavCurrentFrame];
		current_frame->dwUser = 0;
	
	}
}

void _dse_waveout_write2(LPSTR data) {
	WAVEHDR* current_frame;
	int result = 0;

	current_frame = &wavFrames[wavCurrentFrame];
	current_frame->dwUser = 0;

	if (current_frame->dwFlags & WHDR_PREPARED)
		waveOutUnprepareHeader(hWaveOut, current_frame, sizeof(WAVEHDR));
	
	memcpy(current_frame->lpData, data, wavFrameSize);

	data   += wavFrameSize;

	current_frame->dwBufferLength = wavFrameSize;
	
	waveOutPrepareHeader(hWaveOut, current_frame, sizeof(*current_frame));
	waveOutWrite(hWaveOut, current_frame, sizeof(*current_frame));

	EnterCriticalSection(&wavSection);
	wavFreeFrameCount--;
	LeaveCriticalSection(&wavSection);

	while(wavFreeFrameCount < (int)(wavFramesCount / 1.5)) {
		Sleep(20);
	}
	
	wavCurrentFrame++;
	wavCurrentFrame %= wavFramesCount;
}

ulong_t _dse_waveout_get_free_frames() {
	return wavFreeFrameCount;
}

int _dse_waveout_close() {
	waveOutClose(hWaveOut);
	if(wavOutdev)
		free(wavOutdev->product_name);
	return 0;
}

void CALLBACK _dse_waveout_process(
	HWAVEOUT hWaveOut,
	uint_t msg_code, ulong_t instance, ulong_t param1, ulong_t param2
) {

	ulong_t* wavFreeFrameCount2 = (ulong_t*)instance;
	
	if(msg_code != WOM_DONE)
		return;

	EnterCriticalSection(&wavSection);
	
	wavFreeFrameCount++;

	LeaveCriticalSection(&wavSection);
}

#endif
