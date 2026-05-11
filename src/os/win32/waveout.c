#include <os/win32/waveout.h>
#include <mmio/mmio.h>
#include <dsepriv.h>

static CRITICAL_SECTION wavSection;
static WAVEHDR*     	wavFrames;
static volatile ulong_t wavFreeFrameCount;
       int              wavCurrentFrame;
       uint_t           wavFrameSize        = 1024;	
       uint_t           wavFramesCount      = 32;
	   uint_t           wavReadFramesCount  = 0;
       bool             wavFrameLock        = false;

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
	outdev->product_name    = "";

	strcat(outdev->product_name, devCaps.szPname);

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
		(ulong_t)wavFreeFrameCount,
		CALLBACK_FUNCTION
	);

	waveOutGetErrorText(result, errorText, 160);

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
	
	int      i;
	int      result;

	wavFrameSize      = size * sample_size;
	wavFramesCount    = count;
	wavFreeFrameCount = count;

	wavFrames = (WAVEHDR*)malloc(sizeof(WAVEHDR) * (count + 1));
	
	for(i = 0; i < count; i++) {
		wavFrames[i].dwBufferLength  = wavFrameSize;
		wavFrames[i].dwBytesRecorded = 0;
		wavFrames[i].dwUser          = 0;
		wavFrames[i].dwFlags         = 0;
		wavFrames[i].lpNext          = NULL;
		wavFrames[i].reserved        = 0;
		wavFrames[i].lpData          = (LPSTR)malloc(wavFrameSize);
		
		result = waveOutPrepareHeader(hWaveOut, &wavFrames[i], sizeof(WAVEHDR));		
	}

	return wavFrames;
}

void _dse_waveout_free() {
	int i = 0;
	
	for(i = 0; i < wavFramesCount; i++) {
		waveOutUnprepareHeader(hWaveOut, &wavFrames[i], sizeof(WAVEHDR));
		free(wavFrames[i].lpData);
		free(wavFrames[i]);
	}
}

void _dse_waveout_write(LPSTR data, int size) {
	
	WAVEHDR* current_frame;
	int remain;
	int result = 0;
	int bytesCount = 0;
	if(wavFreeFrameCount == 0) {
	    wavFreeFrameCount = wavFramesCount;
	    wavCurrentFrame = 0;
	}	

	current_frame = &wavFrames[wavCurrentFrame];

	while(wavFrameLock) {
		Sleep(20);		
	}
	

	wavFrameLock = true;

	if(current_frame->dwUser > 0)
		current_frame->dwUser = 0;

	if(size < wavFrameSize && size > wavFrameSize)
		return;
		
	memcpy(current_frame->lpData, data, wavFrameSize);

	waveOutWrite(hWaveOut, current_frame, sizeof(WAVEHDR));

	size -= wavFrameSize;
	current_frame->dwUser += wavFrameSize;
		
	wavCurrentFrame++;
	wavFreeFrameCount--;
	bytesCount += wavFramesCount;
}

int _dse_waveout_close() {
	waveOutClose(hWaveOut);
	return 0;
}

void CALLBACK _dse_waveout_process(
	HWAVEOUT hWaveOut,
	uint_t msg_code, ulong_t instance, ulong_t param1, ulong_t param2
) {

	ulong_t* wavFreeFrameCount2 = (ulong_t*)instance;
	
	if(msg_code != WOM_DONE)
		return;

	if(wavReadFrames == 32)
		wavFrameLock = false;


	wavReadFrames++;
}
