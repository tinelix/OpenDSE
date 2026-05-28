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
	DSE_OUTDEV_UNSUPPORTED_PARAMS  = -102,

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