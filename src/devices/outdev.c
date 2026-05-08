#include <outdev.h>
#include <dse.h>
#include <dsepriv.h>

int dse_open_outdev(DSE_OUTDEV* outdev) {
	return _dse_open_outdev(outdev);
}

int dse_write_outdev(unsigned char* buffer, int size) {
	return _dse_pcm_out(buffer, size);
}

int dse_close_outdev(DSE_OUTDEV* outdev) {
	return _dse_close_outdev(outdev);
}
 
