#ifndef OPENDSE_DEVICES_OUTDEV_H  
#define OPENDSE_DEVICES_OUTDEV_H

#include <utils/c_exts.h>

typedef struct _dse_output_dev {
	uint_t   id;
	uint_t   sample_rate;
	uint_t   bit_depth;
	uint_t   channels;
	char*    product_name;
	uchar_t  max_channels : 4;
	bool     volume_control;
	bool     balance_control;
} DSE_OUTDEV;

int dse_open_outdev(DSE_OUTDEV* outdev);
int dse_write_outdev(uchar_t* buffer, int size);
int dse_close_outdev(DSE_OUTDEV* outdev);  

#endif
