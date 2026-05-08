#include <stdlib.h>
#include <stdio.h>

#include <opendse.h>
#include <mmio/mmio.h>
#include <parsers/parser.h>

DSE_MMIO* 		stdmmio;

int dse_open_input(const char* path) {
	
	int result = 0;
	
	result = dse_mmio_open(stdmmio, path);

	if(result > 0) {
		
		uchar_t* inbuf = ((DSE_IMMIO*)stdmmio->_i)->inbuf;
		
		result = dse_parse_input(stdmmio, inbuf);
	}

	return result;	
}
