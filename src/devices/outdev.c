#include <devices/outdev.h>
#include <opendse.h>
#include <dsepriv.h>

#ifdef WIN32
	#include <os/win32/dsewin32.h>
#endif

int dse_open_outdev(DSE_OUTDEV* outdev) {
	return _dse_open_outdev(outdev, stdmmio);
}

int dse_write_outdev(uchar_t* buffer, int size) {
	//return _dse_write_outdev(buffer, size);
}

int dse_close_outdev(DSE_OUTDEV* outdev) {
	return _dse_close_outdev(outdev, stdmmio);
}
 
