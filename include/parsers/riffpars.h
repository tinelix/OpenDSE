#ifndef _OPENDSE_PARSERS_RIFFPARS
#define _OPENDSE_PARSERS_RIFFPARS

#include <string.h>

#include <utils/c_exts.h>
#include <mmio/mmio.h>

typedef struct dse_riff_header {
	uchar_t  magic_nums[4];
	uint_t   overrall_size;
	uchar_t  subformat_name[4];
	uchar_t  fmt_chunk_marker[4];
	uint_t   format_length;
	ushort_t coding_type;
	ushort_t channels;
	uint_t   sample_rate;
	uint_t   byte_rate;
	ushort_t bytes_per_block;
	ushort_t bit_depth;
	uchar_t  data_chunk_marker[4];
	uint_t   data_size;
} RIFF_HEADER;

int dse_riff_parse(DSE_MMIO* mmio, uchar_t* buffer);

#endif
