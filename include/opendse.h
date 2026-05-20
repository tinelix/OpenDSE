#ifndef _OPENDSE_OPENDSE_H
#define _OPENDSE_OPENDSE_H

#include "utils/c_exts.h"

// Mutlimedia I/O
#include "mmio/mmio.h"

// I/O devies
#include "devices/indev.h"
#include "devices/outdev.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef OPENDSE_LIB
	extern DSE_MMIO* stdmmio;
#else
	#ifdef UNIX
		DSE_MMIO* stdmmio;
	#elif WIN32
		__declspec(dllimport) DSE_MMIO* stdmmio;
	#endif
#endif

typedef struct _dse_version {
	int major;
	int minor;
	int patch;
} DSEVERSION;

int dse_get_version(DSEVERSION* version);
int dse_open_input(const char* path);
int dse_close_input();
int dse_alloc_audio();
int dse_decode_audio();
int dse_decode_audio2(ulong_t offset);
int dse_decode_audio3(ulong_t  offset, ulong_t count);
int dse_get_frame_rms(double* rms, uint_t size);
int dse_free_audio();
int dse_is_busy();

#ifdef __cplusplus
}
#endif

#endif
