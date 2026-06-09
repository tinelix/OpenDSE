#ifndef OPENDSE_OS_UNIX_OSSWRAP_H
#define OPENDSE_OS_UNIX_OSSWRAP_H

#include <stdio.h>
#include <fcntl.h>

#include "../../mmio/mmio.h"
#include "../../devices/outdev.h"
#include "../../utils/result.h"

#include <sys/ioctl.h>
#include <linux/soundcard.h>
#include <unistd.h>

#ifdef UNIX_OSS

dse_result  _dse_oss_open(DSE_OUTDEV* outdev, DSE_MMIO* mmio);
dse_result  _dse_oss_prepare(uint_t size, uint_t sample_size, uint_t count);
void        _dse_oss_write(uchar_t* data, int size);
void        _dse_oss_write2(uchar_t* data);

#endif

#endif