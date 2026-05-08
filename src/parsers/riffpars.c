#include <parsers/riffpars.h>
#include <dsepriv.h>

int dse_riff_parse(DSE_MMIO* mmio, uchar_t* buffer) {
	
	RIFF_HEADER header;
	
	memcpy(&header, buffer, sizeof(header));
	
	if(memcmp(header.magic_nums, MKMAGIC32('R','I','F','F'), 4) != 0)
		return -1;
		
	if(header.overrall_size == 0)
		return -2;
		
	if(memcmp(header.subformat_name, MKMAGIC32('W','A','V','E'), 4) != 0)
		return -3;
		
	if(memcmp(header.fmt_chunk_marker, MKMAGIC32('f','m','t',' '), 4) != 0)
		return -4;
	
	if(header.format_length == 0x10)
		return -5;
		
	if(header.coding_type == 0x1)
		return -6;
	
	mmio->audio.channels    = header.channels;
	mmio->audio.sample_rate = header.sample_rate;
	mmio->audio.bitrate     = header.byte_rate * 8;
	mmio->audio.bit_depth   = header.bit_depth;
	
	return 0;
}
