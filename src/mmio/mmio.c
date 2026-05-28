#include <mmio/mmio.h>
#include <dsepriv.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32
#include <io.h>
#define access _access
#endif

int	szFramebuf = 4096;

int dse_mmio_open(DSE_MMIO* mmio, const char* path) {
	
	if (access(path, 0) != 0)
		return -1;

	#ifdef MSVC_GE_800
		fopen_s(&mmio->filesrc, path, "rb");
	#else
		mmio->filesrc = fopen(path, "rb");
	#endif

	if (mmio->filesrc == NULL)
		return -1;

	mmio->_i = (DSE_IMMIO*)malloc(sizeof(DSE_IMMIO));

	mmio->_i->inbuf = (uchar_t*) malloc((szFramebuf * sizeof(uchar_t)) + 1);

	if(!mmio->_i->inbuf)
		return -1;
	
	mmio->bytes_read = fread(mmio->_i->inbuf, 1, szFramebuf, mmio->filesrc);
	
	if(mmio->bytes_read <= 0) {
		fclose(mmio->filesrc);
		return -2;
	}

	if(fseek(mmio->filesrc, 0, SEEK_END) != 0) {
	 	fclose(mmio->filesrc);
		return -3;
	}

	mmio->_i->inbuf_size = szFramebuf;

	mmio->bytes_total = ftell(mmio->filesrc);
	
	return mmio->bytes_read;  
}

int dse_mmio_close(DSE_MMIO* mmio) {
	if(!mmio)
		return -1;

	free(mmio->_i->inbuf);
	mmio->_i->inbuf = NULL;
	
	free(mmio->_i);
	mmio->_i = NULL;

	fclose(mmio->filesrc);
}
