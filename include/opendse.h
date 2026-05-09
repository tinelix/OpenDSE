#ifndef _OPENDSE_OPENDSE_H
#define _OPENDSE_OPENDSE_H

#include "utils/c_exts.h"

// Mutlimedia I/O
#include "mmio/mmio.h"

// I/O devies
#include "devices/indev.h"
#include "devices/outdev.h"

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

int dse_open_input(const char* path);
int dse_close_input();
int dse_decode_audio();
int dse_decode_audio2(ulong_t offset);
int dse_decode_audio3(ulong_t  offset, ulong_t count);

#endif
