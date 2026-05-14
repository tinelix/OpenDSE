#include <parsers/riffpars.h>
#include <dsepriv.h>
#include <stdlib.h>

int dse_riff_parse(DSE_MMIO* mmio, uchar_t* buffer) {
	
	RIFF_HEADER header;
	
	memcpy(&header, buffer, sizeof(header));

	free(mmio->_i->inbuf);
		
	if(header.overrall_size == 0)
		return -2;
		
	/*if(memcmp(header.subformat_name, MKMAGIC32('W','A','V','E'), sizeof(uint_t)) != 0)
		return -3;
		
	if(memcmp(header.fmt_chunk_marker, MKMAGIC32('f','m','t',' '), sizeof(uint_t)) != 0)
		return -4;*/
	
	if(header.format_length != 0x10)
		return -5;
		
	if(header.coding_type != 0x1)
		return -6;
	
	if(header.bit_depth == 8)
		mmio->audio.str_id = "LPCM-U8";
	else if(header.bit_depth == 16)
		mmio->audio.str_id = "LPCM-S16LE";
	else if(header.bit_depth == 24)
		mmio->audio.str_id = "LPCM-S24LE";
	else if(header.bit_depth == 32)
		mmio->audio.str_id = "LPCM-S32LE"; 

	mmio->audio.channels    = (uchar_t)header.channels;
	mmio->audio.sample_rate = header.sample_rate;
	mmio->audio.bitrate     = header.byte_rate * 8;
	mmio->audio.bit_depth   = (uchar_t)header.bit_depth;
	mmio->opened            = true;
	
	return sizeof(header);
}
