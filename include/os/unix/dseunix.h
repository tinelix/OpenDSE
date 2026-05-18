#ifndef OPENDSE_OS_UNIX_DSEUNIX_H
#define OPENDSE_OS_UNIX_DSEUNIX_H

#include "../../mmio/mmio.h"
#include "../../devices/outdev.h"

#ifdef UNIX_ALSA
    #include <alsa/asoundlib.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

int _dse_open_outdev(DSE_OUTDEV* outdev, DSE_MMIO* mmio);
int _dse_close_outdev(DSE_OUTDEV* outdev, DSE_MMIO* mmio);
int _dse_decode_audio(DSE_MMIO* mmio, ulong_t offset, ulong_t count);
int _dse_decode_audio2(DSE_MMIO* mmio, ulong_t offset);
int _dse_alloc_audio(DSE_MMIO* mmio);
int _dse_free_audio(DSE_MMIO* mmio);
cbool _dse_is_busy();
//int _dse_outdev_write(DSE_OUTDEV* outdev, DSE_MMIO* mmio);

#ifdef __cplusplus
}
#endif

#endif
