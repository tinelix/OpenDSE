#ifndef OPENDSE_OS_WIN32_WAVEOUT_H
#define OPENDSE_OS_WIN32_WAVEOUT_H

#include "../../devices/outdev.h"
#include "../../mmio/mmio.h"
#include "dsewin32.h"

int _dse_open_waveout(DSE_OUTDEV* outdev, DSE_MMIO* mmio);

#endif
