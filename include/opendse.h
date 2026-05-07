#ifndef _OPENDSE_OPENDSE_H
#define _OPENDSE_OPENDSE_H

#include <utils/c_exts.h>
#include <mmio/mmio.h>

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

#endif
