#ifndef OPENDSE_DEVICES_FRONTEND_H
#define OPENDSE_DEVICES_FRONTEND_H

typedef enum {
    
    DSE_FRONTEND_AUTO                = 0, 
    DSE_FRONTEND_AUTO_LEGACY         = 1,
    
    // Supported backends for Windows
    DSE_FRONTEND_WINDOWS_WAVEIO      = 1000, 
    DSE_FRONTEND_WINDOWS_DIRECTSOUND = 1001,
    DSE_FRONTEND_WINDOWS_WASAPI      = 1002,
    DSE_FRONTEND_WINDOWS_WASAPI_V2   = 1003,
    
    // Supported backends for Linux and UNIX-like operating systems
    DSE_FRONTEND_LINUX_ALSA          = 1100, 
    DSE_FRONTEND_UNIX_PULSEAUDIO     = 1101, 
    DSE_FRONTEND_UNIX_OSS            = 1102
    
} dse_frontend_t;

extern dse_frontend_t _dse_frontend;

#endif
