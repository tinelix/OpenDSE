#include <os/win32/waveout.h>
#include <mmio/mmio.h>
#include <dsepriv.h>

int _dse_open_waveout(DSE_OUTDEV* outdev, DSE_MMIO* mmio) {
	
	int result;
	WAVEOUTCAPS devCaps;
	
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
	
	return 0; 
}
