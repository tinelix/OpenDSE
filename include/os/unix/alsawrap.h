#ifndef OPENDSE_OS_UNIX_ALSAWRAP_H
#define OPENDSE_OS_UNIX_ALSAWRAP_H

#include "../../devices/outdev.h"
#include "../../mmio/mmio.h"
#include "dseunix.h"

#include <alsa/asoundlib.h>

extern snd_pcm_t* hAlsa;

dse_result  _dse_alsa_open(DSE_OUTDEV* outdev, DSE_MMIO* mmio);
dse_result  _dse_alsa_allocate(uint_t size, uint_t sample_size, uint_t count);
dse_result  _dse_alsa_free();
void        _dse_alsa_write(uchar_t* data, int size);
void        _dse_alsa_write2(uchar_t* data);
void        _dse_alsa_wait();
dse_result  _dse_alsa_close();

#endif