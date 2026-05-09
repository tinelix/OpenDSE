#ifndef OPENDSE_DEVICES_INDEV_H  
#define OPENDSE_DEVICES_INDEV_H

#include "../utils/c_exts.h"

typedef struct _dse_input_dev {
	uint_t   id;
	uint_t   sample_rate;
	uint_t   bit_depth;
	uint_t   channels;
	char*    product_name;
	bool     volume_control;
} DSE_INDEV;

/*int dse_open_indev(DSE_INDEV* indev);
int dse_read_indev(uchar_t* buffer, int size);
int dse_close_indev(DSE_INDEV* indev);*/  

#endif
