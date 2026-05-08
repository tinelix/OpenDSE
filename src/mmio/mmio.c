#include <mmio/mmio.h>
#include <dsepriv.h>
#include <stdio.h>
#include <stdlib.h>

int	szFramebuf = 4096;

int dse_mmio_open(DSE_MMIO* mmio, const char* path) {
	
	uchar_t* buffer;

	mmio = (DSE_MMIO*)malloc(sizeof(DSE_MMIO));
	
	#ifdef MSVC_GE_800
		fopen_s(&mmio->filesrc, path, "rb");
	#else
		mmio->filesrc = fopen(path, "rb");
	#endif

	mmio->_i = (DSE_IMMIO*)malloc(sizeof(DSE_IMMIO));

	buffer = ((DSE_IMMIO*)mmio->_i)->inbuf;

	buffer = (uchar_t*) malloc((szFramebuf * sizeof(uchar_t)) + 1);

	if(!buffer)
		return -1;
	
	mmio->bytes_read = fread(&buffer, 1, szFramebuf, mmio->filesrc);
	
	if(mmio->bytes_read <= 0) {
		fclose(mmio->filesrc);
		return -2;
	}
	
	return mmio->bytes_read;  
}
