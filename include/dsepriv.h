#ifndef _OPENDSE_OPENDSE_H
#define _OPENDSE_OPENDSE_H

#define MKMAGIC32(uc1, uc2, uc3, uc4) \
	((uc1 | (uc2 << 8) | (uc3 << 16) | ((unsigned)(uc1) << 24)) 

typedef struct dse_media_fmt {
	const unsigned int magic_nums;
	size_t magic_len;
	size_t magic_offset;
} DSE_MEDIAFMT;

#endif
