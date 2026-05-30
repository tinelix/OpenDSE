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

#ifndef _OPENDSE_OPENDSE_H
#define _OPENDSE_OPENDSE_H

// C/C++ extensions support and result enumerations

#include "utils/c_exts.h"
#include "utils/result.h"

// Mutlimedia I/O
#include "mmio/mmio.h"

// I/O devies
#include "devices/indev.h"
#include "devices/outdev.h"

#ifdef __cplusplus
extern "C" {
#endif

// Global symbols linking support for Windows and Linux

#ifdef OPENDSE_LIB
	extern DSE_MMIO* stdmmio;
#else
	#ifdef UNIX
		DSE_MMIO* stdmmio;
	#elif WIN32
		__declspec(dllimport) DSE_MMIO* stdmmio;
	#endif
#endif

typedef struct _dse_version {
	int major;
	int minor;
	int patch;
} DSEVERSION;

// API functions

dse_result dse_get_version(DSEVERSION* version);
dse_result dse_open_input(const char* path);
dse_result dse_close_input();
dse_result dse_alloc_audio();
dse_result dse_decode_audio();
dse_result dse_decode_audio2(ulong_t offset);
dse_result dse_decode_audio3(ulong_t offset, ulong_t count);
int        dse_get_frame_rms(double* rms, uint_t size);
dse_result dse_free_audio();
dse_result dse_is_busy();

#ifdef __cplusplus
}
#endif

#endif
