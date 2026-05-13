#include <os/win32/waveout.h>
#include <mmio/mmio.h>
#include <dsepriv.h>

static CRITICAL_SECTION wavSection;
static WAVEHDR*     	wavFrames;
static HANDLE           wavLargeHeap;
static volatile ulong_t wavFreeFrameCount;
       int              wavCurrentFrame;
       uint_t           wavFrameSize        = 1024;	
       uint_t           wavFramesCount      = 32;
	   uint_t           wavReadFramesCount  = 0;
       bool             wavFrameLock        = false;
	   DSE_OUTDEV*      wavOutdev;

HWAVEOUT hWaveOut;

int _dse_waveout_open(DSE_OUTDEV* outdev, DSE_MMIO* mmio) {

	int           result;
	char          errorText[160];
	WAVEOUTCAPS   devCaps;
	WAVEFORMATEX  wavFormat;
	
	result = waveOutGetDevCaps(outdev->id, &devCaps, sizeof(WAVEOUTCAPS));
	
	if(result != MMSYSERR_NOERROR)
		return -1;
	
	outdev->volume_control  = devCaps.dwSupport & WAVECAPS_VOLUME;
	outdev->balance_control = devCaps.dwSupport & WAVECAPS_LRVOLUME;
	outdev->max_channels    = devCaps.wChannels;
	outdev->product_name    = (char*)malloc(128 * sizeof(char*));

	outdev->sample_rate       = mmio->audio.sample_rate;
	outdev->bit_depth		  = mmio->audio.bit_depth;
	outdev->channels          = mmio->audio.channels;

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
	wavFormat.nAvgBytesPerSec =	mmio->audio.sample_rate * wavFormat.nBlockAlign;
	
	result = waveOutOpen(
		&hWaveOut, outdev->id, &wavFormat,
		(ulong_t)_dse_waveout_process,
		(ulong_t*)&wavFreeFrameCount,
		CALLBACK_FUNCTION
	);

	waveOutGetErrorText(result, errorText, 160);

	strcat(outdev->product_name, devCaps.szPname);

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

WAVEHDR* _dse_waveout_allocate(uint_t size, uint_t sample_size, uint_t count) {
	
	uchar_t* buffer;
	int      i;
	ulong_t  buffer_size;

	wavFrameSize = size * sample_size;
	buffer_size  = (wavFrameSize + sizeof(WAVEHDR)) * count;

	wavLargeHeap = HeapCreate(0, buffer_size, buffer_size * 4);

	if((buffer = HeapAlloc(
	      wavLargeHeap, HEAP_ZERO_MEMORY, buffer_size
	   )) == NULL) {
		return NULL;   
	}

	wavFrames = (WAVEHDR*)buffer;
	buffer += sizeof(WAVEHDR) * count;

	for(i = 0; i < count; i++) {
		wavFrames[i].dwBufferLength = wavFrameSize;
		wavFrames[i].lpData = buffer;
		buffer += wavFrameSize;
	}
	 

	InitializeCriticalSection(&wavSection);

	wavFreeFrameCount = count;

	return wavFrames;
}

void _dse_waveout_free() {
	HeapFree(wavLargeHeap, 0, wavFrames);
	HeapDestroy(wavLargeHeap);
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
	int remain;
	int result = 0;
	int bytesCount = 0;
	int size = wavFrameSize;

	current_frame = &wavFrames[wavCurrentFrame];
	current_frame->dwUser = 0;

	current_frame->dwBufferLength = wavFrameSize;

	waveOutUnprepareHeader(hWaveOut, current_frame, sizeof(WAVEHDR));
		
	if(size < (int)(wavFrameSize - current_frame->dwUser)) {
		memcpy(current_frame->lpData + current_frame->dwUser, data, sizeof(data));
		current_frame->dwUser += size;
		return;
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
	(*wavFreeFrameCount2)++;
	LeaveCriticalSection(&wavSection);
}
