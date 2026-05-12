#ifndef _OPENDSE_DSEPRIV_H
#define _OPENDSE_DSEPRIV_H

#ifdef OPENDSE_LIB

#include <stdio.h>
#include <utils/c_exts.h>

#define MKMAGIC32(uc1, uc2, uc3, uc4) \
	(uc4 | (uc3 << 8) | (uc2 << 16) | (uc1 << 24)) 

typedef struct _dse_mediafmt_priv {
	const uint_t magic_nums;
	size_t 		 magic_len;
	size_t 		 magic_offset;
} DSE_IMEDIAFMT;

typedef struct _dse_mmio_priv {
	DSE_IMEDIAFMT   format;
	uchar_t*  		inbuf;
	ulong_t         inbuf_size;
} DSE_IMMIO;

#endif

#endif
