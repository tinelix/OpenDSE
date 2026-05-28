#ifndef OPENDSE_OS_WIN32_DSEWIN32_H
#define OPENDSE_OS_WIN32_DSEWIN32_H

#include "../../mmio/mmio.h"
#include "../../devices/outdev.h"
#include "../../utils/result.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#ifdef WIN32_MME
	#include <mmsystem.h>
	#include "waveout.h"
#else
	#ifdef WIN32_WASAPI
		#include "wasapi.h"
	#endif
#endif

dse_result _dse_open_outdev(DSE_OUTDEV* outdev, DSE_MMIO* mmio);
dse_result _dse_close_outdev(DSE_OUTDEV* outdev, DSE_MMIO* mmio);
dse_result _dse_decode_audio(DSE_MMIO* mmio, ulong_t offset, ulong_t count);
dse_result _dse_decode_audio2(DSE_MMIO* mmio, ulong_t offset);
dse_result _dse_alloc_audio(DSE_MMIO* mmio);
dse_result _dse_free_audio(DSE_MMIO* mmio);
dse_result _dse_is_busy();
//int _dse_outdev_write(DSE_OUTDEV* outdev, DSE_MMIO* mmio);

#ifdef __cplusplus
}
#endif

#endif
