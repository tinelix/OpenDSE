#ifndef OPENDSE_OS_WIN32_WASAPI_H
#define OPENDSE_OS_WIN32_WASAPI_H

#include "../../devices/outdev.h"
#include "../../mmio/mmio.h"
#include "dsewin32.h"

#include <windows.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <functiondiscoverykeys_devpkey.h>

#ifdef __cplusplus
extern "C" {
#endif

int         _dse_select_frontend(dse_frontend_t frontend);
dse_result  _dse_wasapi_open(DSE_OUTDEV* outdev, DSE_MMIO* mmio);
dse_result  _dse_wasapi_prepare();
void        _dse_wasapi_free();
void        _dse_wasapi_write(LPSTR data, int size);
dse_result  _dse_wasapi_close();

#ifdef __cplusplus
}
#endif

#endif
