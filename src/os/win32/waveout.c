#include <os/win32/waveout.h>
#include <mmio/mmio.h>
#include <dsepriv.h>

static CRITICAL_SECTION wavSection;
static WAVEHDR*     	wavFrames;
static volatile ulong_t wavFreeFrameCount;
static int              wavCurrentFrame;
       uint_t           wavFrameSize        = 1024;	
       uint_t           wavFramesCount      = 32;

int _dse_waveout_open(DSE_OUTDEV* outdev, DSE_MMIO* mmio) {
	
	int result;
	WAVEOUTCAPS devCaps;
	WAVEFORMAT  wavFormat;
	
	result = waveOutGetDevCaps(outdev->id, &devCaps, sizeof(WAVEOUTCAPS));
	
	if(result == MMSYSERR_NOERROR)
		return -1;
	
	outdev->volume_control  = devCaps.dwSupport & WAVECAPS_VOLUME;
	outdev->balance_control = devCaps.dwSupport & WAVECAPS_LRVOLUME;
	outdev->product_name    = devCaps.szPname;
	outdev->max_channels    = devCaps.wChannels;

	outdev->sample_rate     = mmio->audio.sample_rate;
	outdev->bit_depth		= mmio->audio.bit_depth;
	outdev->channels        = mmio->audio.channels;
	
	result = waveOutOpen(
		&hWaveOut, WAVE_MAPPER, &wavFormat,
		(ulong_t*)_dse_waveout_process,
		(ulong_t*)&wavFreeFrameCount,
		CALLBACK_FUNCTION
	);

	if(result == MMSYSERR_NOERROR)
		return -2;

	return 0; 
}

WAVEHDR* _dse_waveout_allocate(uint_t size, uint_t sample_size, uint_t count) {
	
	uchar_t* buffer;
	ulong_t  bufferSize;
	WAVEHDR* frames;
	int      i;

	wavFrameSize   = size * sample_size;
	wavFramesCount = count;
	
	bufferSize = (wavFrameSize + sizeof(WAVEHDR)) * count;

	buffer = (uchar_t)HeapAlloc(
		                   GetProcessHeap(), HEAP_ZERO_MEMORY,
		                   bufferSize
		              );

	if(!buffer)
		return NULL;

	frames = (WAVEHDR*)buffer;
	buffer += sizeof(WAVEHDR) * count;
	
	for(i = 0; i < count; i++) {
		frames[i].dwBufferLength = size * sample_size;
		frames[i].lpData = (LPSTR)buffer;
		buffer += size;			
	}

	return frames;
}

void _dse_waveout_free(WAVEHDR* frames) {
	HeapFree(GetProcessHeap(), 0, frames);
}

void _dse_waveout_write(LPSTR data, int size) {
	
	WAVEHDR* current_frame;
	int remain;

	current_frame = &wavFrames[wavCurrentFrame];

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
		
		size -= remain;
		data += remain;
		current_frame->dwBufferLength = wavFrameSize;

		waveOutPrepareHeader(hWaveOut, current_frame, sizeof(WAVEHDR));
		waveOutWrite(hWaveOut, current_frame, sizeof(WAVEHDR));

		EnterCriticalSection(&wavSection);
		wavFreeFrameCount--;
		LeaveCriticalSection(&wavSection);

		while(!wavFreeFrameCount)
			Sleep(10);

		wavCurrentFrame++;
		wavCurrentFrame %= wavFrameSize;
		
		current_frame = &wavFrames[wavCurrentFrame];
		current_frame->dwUser = 0;
	}
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
