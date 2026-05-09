#ifndef OPENDSE_OS_WIN32_DSEWIN32_H
#define OPENDSE_OS_WIN32_DSEWIN32_H

#include "../../mmio/mmio.h"
#include "../../devices/outdev.h"

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <mmsystem.h>

int _dse_outdev_open(DSE_OUTDEV* outdev, DSE_MMIO* mmio);

#endif
