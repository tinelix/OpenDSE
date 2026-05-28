#include <stdio.h>
#include <stdlib.h>

#include <mmio/mmio.h>
#include <utils/result.h>
#include <dsepriv.h>

#ifdef WIN32
#include <io.h>
#define access _access
#endif

int	szFramebuf = 4096;

int dse_mmio_open(DSE_MMIO* mmio, const char* path) {
	
	if (access(path, 0) != 0)
		return DSE_NOT_FOUND;

	#ifdef MSVC_GE_800
		fopen_s(&mmio->filesrc, path, "rb");
	#else
		mmio->filesrc = fopen(path, "rb");
	#endif

	if (mmio->filesrc == NULL)
		return DSE_NOT_FOUND;

	mmio->_i = (DSE_IMMIO*)malloc(sizeof(DSE_IMMIO));

	mmio->_i->inbuf = (uchar_t*) malloc((szFramebuf * sizeof(uchar_t)) + 1);

	if(!mmio->_i->inbuf)
		return DSE_ALLOCATION_ERROR;
	
	mmio->bytes_read = fread(mmio->_i->inbuf, 1, szFramebuf, mmio->filesrc);
	
	if(mmio->bytes_read <= 0) {
		fclose(mmio->filesrc);
		return DSE_MMIO_EMPTY_FILE;
	}

	if(fseek(mmio->filesrc, 0, SEEK_END) != 0) {
	 	fclose(mmio->filesrc);
		return DSE_INTERNAL_ERROR;
	}

	mmio->_i->inbuf_size = szFramebuf;

	mmio->bytes_total = ftell(mmio->filesrc);
	
	return mmio->bytes_read;  
}

dse_result dse_mmio_close(DSE_MMIO* mmio) {
	if(!mmio)
		return DSE_NOT_FOUND;

	free(mmio->_i->inbuf);
	mmio->_i->inbuf = NULL;
	
	free(mmio->_i);
	mmio->_i = NULL;

	fclose(mmio->filesrc);
}
