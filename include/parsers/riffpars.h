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

#ifndef _OPENDSE_PARSERS_RIFFPARS
#define _OPENDSE_PARSERS_RIFFPARS

#include <string.h>

#include <utils/c_exts.h>
#include <mmio/mmio.h>

/* 
 * RIFF/WAV structure
 * From: https://truelogic.org/wordpress/2015/09/04/parsing-a-wav-file-in-c/
 */

typedef struct dse_riff_header {
	uchar_t  magic_nums[4];
	uint_t   overrall_size;
	uchar_t  subformat_name[4];
	uchar_t  fmt_chunk_marker[4];
	uint_t   format_length;
	ushort_t coding_type;
	ushort_t channels;
	uint_t   sample_rate;
	uint_t   byte_rate;
	ushort_t bytes_per_block;
	ushort_t bit_depth;
	uchar_t  data_chunk_marker[4];
	uint_t   data_size;
} RIFF_HEADER;

int dse_riff_parse(DSE_MMIO* mmio, uchar_t* buffer);

#endif
