#include <opendse.h>
#include <stdlib.h>
#include <stdio.h>

//DSE_MEDIAIO* 	dse_mmio;
FILE* 			dse_lfile;
unsigned char*  dse_inbuf;
int				szFramebuf;

int dse_mmio_open(const char* path) {
	
	int result = 0;
	
	#ifdef MSVC_GE_800
		fopen_s(&dse_lfile, path, "rb");
	#else
		dse_lfile = fopen(path, "rb");
	#endif

	if(!dse_inbuf)
		return -1;

	szFramebuf = 4096;

	dse_inbuf = (unsigned char*) 
			malloc((szFramebuf * sizeof(unsigned char)) + 1);

	//result = dse_parse_input(dse_inbuf);

	return result;
}
