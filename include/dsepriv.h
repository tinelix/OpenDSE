#ifndef _OPENDSE_DSEPRIV_H
#define _OPENDSE_DSEPRIV_H

#define MKMAGIC32(uc1, uc2, uc3, uc4) \
	((uc1 | (uc2 << 8) | (uc3 << 16) | ((unsigned)(uc1) << 24)) 

typedef struct _dse_mediafmt_priv {
	const unsigned int magic_nums;
	size_t magic_len;
	size_t magic_offset;
} DSE_IMEDIAFMT;

typedef struct _dse_mmio_priv {
	DSE_IMEDIAFMT   format;
	unsigned char*  inbuf;
} DSE_IMMIO;

#endif
