#include <stdlib.h>
#include <stdio.h>

#include <opendse.h>
#include <mmio/mmio.h>
#include <parsers/parser.h>

DSE_MMIO* 		stdmmio;

int dse_open_input(const char* path) {
	
	int result = 0;

	stdmmio = (DSE_MMIO*)malloc(sizeof(DSE_MMIO));

	stdmmio->path = path;
	
	result = dse_mmio_open(stdmmio, path);

	if(result > 0) {
		
		uchar_t* inbuf = stdmmio->_i->inbuf;
		
		result = dse_parse_input(stdmmio, inbuf);

		stdmmio->bytes_read = result;
	}

	return result;
}

int dse_close_input() {

	int result = 0;

	result = dse_mmio_close(stdmmio);

	free(stdmmio);
	stdmmio = NULL;

	return result;
}

int dse_decode_audio() {
	_dse_decode_audio(stdmmio, stdmmio->bytes_read, stdmmio->bytes_total);
	return 0;
}

int dse_decode_audio2(ulong_t offset) {
	_dse_decode_audio2(stdmmio, offset);
	return 0; 
}

int dse_alloc_audio() {
	int result;

	result = _dse_alloc_audio(stdmmio);
	return 0;
}

int dse_free_audio() {
	int result;

	result = _dse_free_audio(stdmmio);
	return 0;
}

float dse_get_frame_rms() {
	if(stdmmio->_i->inbuf_size == 0)
		return 0.0;

	return 0.0;
}

int dse_decode_audio3(ulong_t offset, ulong_t count) {
	_dse_decode_audio(stdmmio, offset, count);
	return 0; 
}
