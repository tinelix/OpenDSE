/*
 * The Open Digital Sound Engine (OpenDSE) is free software and is licensed under 
 * the BSD 3-Clause license.
 *
 * Copyright (c) 2026, Dmitry Tretyakov <tinelix@mail.ru>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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
