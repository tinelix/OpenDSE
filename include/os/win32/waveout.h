#ifndef OPENDSE_OS_WIN32_WAVEOUT_H
#define OPENDSE_OS_WIN32_WAVEOUT_H

#include "../../devices/outdev.h"
#include "../../mmio/mmio.h"
#include "dsewin32.h"

extern HWAVEOUT hWaveOut;

dse_result      _dse_waveout_open(DSE_OUTDEV* outdev, DSE_MMIO* mmio);
WAVEHDR* 		_dse_waveout_prepare(uint_t size, uint_t sample_size, uint_t count);
void     		_dse_waveout_free();
void     		_dse_waveout_write(LPSTR data, int size);
void            _dse_waveout_write2(LPSTR data);
ulong_t         _dse_waveout_get_free_frames();

void CALLBACK   _dse_waveout_process(
	         		HWAVEOUT hWaveOut,
	         		uint_t msg_code, ulong_t instance, ulong_t param1, ulong_t param2
         		);
dse_result		_dse_waveout_close();

#endif
