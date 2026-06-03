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

#ifndef _OPENDSE_UTILS_RESULT_H
#define _OPENDSE_UTILS_RESULT_H

// OpenDSE API result codes
typedef enum {
	DSE_OK                         =    0,
	DSE_NOT_FOUND                  =   -1,
	DSE_ACCESS_DENIED              =   -2,
	DSE_BUSY                       =   -3,
	DSE_ALLOCATION_ERROR           =   -4,
	DSE_NOT_IMPLEMENTED            =   -5,
	DSE_INTERNAL_ERROR             =   -6,

	// Output Devices

	DSE_OUTDEV_NOT_INITIALIZED     = -101,
	DSE_OUTDEV_UNSUPPORTED_BACKEND = -102, 
	DSE_OUTDEV_UNSUPPORTED_PARAMS  = -103,

	// Mutlimedia I/O
	DSE_MMIO_EMPTY_FILE            = -301,
	DSE_MMIO_EOF                   = -302,  // <-- if the EOF is reached
	DSE_MMIO_UNKNOWN_FORMAT        = -303,  // <-- if file header not found in the 4 kB first block
	DSE_MMIO_UNSUPPORTED_FORMAT    = -304,  // <-- if the decoder does not support the required 
	                                        //     encoding parameters
	DSE_MMIO_BAD_FRAME             = -305   // <-- if the decoder has encountered a header and 
	                                        //     cannot deсode or convert the data

} dse_result;

#endif
