/*
 * The Open Digital Sound Engine (OpenDSE) is free software and is licensed under 
 * the BSD 3-Clause license.
 *
 * Copyright (c) 2026, Dmitry Tretyakov <tinelix@mail.ru>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
#ifdef UNIX_OSS

#include <os/unix/osswrap.h>

int _dse_oss_devid = -1;
int _dse_oss_frame_size = 0;

 /*
  *  This file contains an implementation of the Open Sound System (OSS) frontend
  *  for Linux, BSD and Solaris, distributed under four licenses:
  *  
  *  1. BSD 2-Clause                    (header files with FreeBSD version)
  *  2. GNU General Public License 2.0  (Linux version)
  *  3. CDDL-1.0                        (Solaris version)
  *  4. Commercial license
  *  
  *  The OSS API uses standard POSIX functions for working with character devices.
  *  Definitions related to this API are found in the soundcard.h header.
  */
  
int _dse_oss_open(DSE_OUTDEV* outdev, DSE_MMIO* mmio) {
    int result = 0;
    int format = 0;
    int stereo = 0;
    
    if(mmio->audio.channels >= 2)
        stereo = 1;
    
    _dse_oss_devid = open("/dev/dsp", O_WRONLY, 0);
    
    outdev->sample_rate   = mmio->audio.sample_rate;
    outdev->bit_depth     = mmio->audio.bit_depth;
    outdev->channels      = mmio->audio.channels;
    outdev->product_name  = (char*)malloc(161 * sizeof(char));
    strcpy(outdev->product_name, "OSS Default Playback Device");
    
    switch(mmio->audio.bit_depth) {
        case 8:
            format = AFMT_U8;
            break;
        case 16:
            format = AFMT_S16_LE;
            break;
    }
    
    if(_dse_oss_devid < 0)
        return DSE_OUTDEV_NOT_INITIALIZED;
    
    result = ioctl(_dse_oss_devid, SNDCTL_DSP_SAMPLESIZE, &mmio->audio.bit_depth);
    
    // SNDCTL_DSP_SETFMT - bit depth
    result = ioctl(_dse_oss_devid, SNDCTL_DSP_SETFMT, &format);
    if(result < 0)
        return DSE_OUTDEV_UNSUPPORTED_PARAMS;
    
    #ifndef UNIX_AOUT
    // SNDCTL_DSP_CHANNELS - channels amount 
    result = ioctl(_dse_oss_devid, SNDCTL_DSP_CHANNELS, &mmio->audio.channels);
    if(result < 0)
        return DSE_OUTDEV_UNSUPPORTED_PARAMS;
    #endif
        
    result = ioctl(_dse_oss_devid, SNDCTL_DSP_STEREO, &stereo);
    if(result < 0)
        return DSE_OUTDEV_UNSUPPORTED_PARAMS;
    
    // SNDCTL_DSP_SPEED - sample rate
    result = ioctl(_dse_oss_devid, SNDCTL_DSP_SPEED, &mmio->audio.sample_rate);
    if(result < 0)
        return DSE_OUTDEV_UNSUPPORTED_PARAMS;
    
    return DSE_OK;
}

int _dse_oss_prepare(uint_t size, uint_t sample_size, uint_t count) {
    _dse_oss_frame_size = size * sample_size;
    return DSE_OK;
}

void _dse_oss_write(uchar_t* data, int size) {
    int result = 0;
    
    if(size > _dse_oss_frame_size)
        return;
        
    result = write(_dse_oss_devid, data, _dse_oss_frame_size);
    
    if(result < 0)
        return;
}

void _dse_oss_write2(uchar_t* data) {
    
    int result = 0;
    result = write(_dse_oss_devid, data, _dse_oss_frame_size);
    
    if(result < 0)
        return;
}

#endif