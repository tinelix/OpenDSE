#ifndef OPENDSE_DEVICES_OUTDEV_H  
#define OPENDSE_DEVICES_OUTDEV_H

#include "../utils/c_exts.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _dse_output_dev {
	uint_t        id;
	uint_t        sample_rate;
	uint_t        bit_depth;
	uint_t        channels;
	char*         product_name;
	uchar_t       max_channels : 4;
	cbool         volume_control;
	cbool         balance_control;
} DSE_OUTDEV;

int dse_open_outdev(DSE_OUTDEV* outdev);
int dse_write_outdev(uchar_t* buffer, int size);
int dse_close_outdev(DSE_OUTDEV* outdev);  

#ifdef __cplusplus
}
#endif

#endif
