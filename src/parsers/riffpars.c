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

#include <parsers/riffpars.h>
#include <utils/result.h>
#include <dsepriv.h>
#include <stdlib.h>

int dse_riff_parse(DSE_MMIO* mmio, uchar_t* buffer) {
	
	RIFF_HEADER header;
	
	memcpy(&header, buffer, sizeof(header));

	free(mmio->_i->inbuf);
		
	if(header.overrall_size == 0)
		return DSE_MMIO_UNSUPPORTED_FORMAT;
		
	/*if(memcmp(header.subformat_name, MKMAGIC32('W','A','V','E'), sizeof(uint_t)) != 0)
		return -3;
		
	if(memcmp(header.fmt_chunk_marker, MKMAGIC32('f','m','t',' '), sizeof(uint_t)) != 0)
		return -4;*/
	
	if(header.format_length != 0x10)
		return DSE_MMIO_UNSUPPORTED_FORMAT;
		
	if(header.coding_type != 0x1)
		return DSE_MMIO_UNSUPPORTED_FORMAT;
	
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
	mmio->opened            = ctrue;
	
	return sizeof(header);
}
