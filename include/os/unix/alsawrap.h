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

#ifdef UNIX_ALSA

#include <os/unix/alsawrap.h>

snd_pcm_t*           hAlsa;
uint_t               _dse_alsa_sample_rate;
uint_t               _dse_alsa_frame_size;
uint_t               _dse_alsa_bit_depth;
uint_t               _dse_alsa_channels;
uchar_t*             _dse_alsa_buffer;
snd_pcm_hw_params_t* _dse_alsa_hw_params;

 /*
  *  This file contains a frontend implementation for the Advanced Linux Sound 
  *  Architecture (ALSA), licensed under the GNU Lesser General Public License 2.1 
  *  or later.
  *
  *  For the implementation to work correctly, make sure you have libasound1 or 
  *  libasound2 version 0.9.1 or higher and a Linux kernel version 2.4 or higher 
  *  with ALSA support installed.
  *
  *  Some libasound versions shipped with Linux distributions may cause a SIGSEGV 
  *  error.
  */

int _dse_alsa_open(DSE_OUTDEV* outdev, DSE_MMIO* mmio) {
    int    result         = 0;
    int    dir            = 0;
    uint_t sample_rate    = mmio->audio.sample_rate;
    uint_t channels       = mmio->audio.channels;
    
    _dse_alsa_sample_rate = mmio->audio.sample_rate;
    _dse_alsa_bit_depth   = mmio->audio.bit_depth;
    _dse_alsa_channels    = mmio->audio.channels;
    
    outdev->sample_rate   = mmio->audio.sample_rate;
    outdev->bit_depth     = mmio->audio.bit_depth;
    outdev->channels      = mmio->audio.channels;
    outdev->product_name  = (char*)malloc(161 * sizeof(char));
    strcpy(outdev->product_name, "ALSA Default Playback Device");
    
    result = snd_pcm_open(
        &hAlsa, "default", SND_PCM_STREAM_PLAYBACK, 0
    );
    
    result = snd_pcm_hw_params_malloc(&_dse_alsa_hw_params);
    result = snd_pcm_hw_params_any(hAlsa, _dse_alsa_hw_params);
    result = snd_pcm_hw_params_set_access(
        hAlsa, _dse_alsa_hw_params, SND_PCM_ACCESS_RW_INTERLEAVED
    );

    if(mmio->audio.bit_depth == 8) {
        result = snd_pcm_hw_params_set_format(hAlsa, _dse_alsa_hw_params, SND_PCM_FORMAT_U8);
    } else if(mmio->audio.bit_depth == 16) {
        result = snd_pcm_hw_params_set_format(hAlsa, _dse_alsa_hw_params, SND_PCM_FORMAT_S16_LE);
    } else if(mmio->audio.bit_depth == 24) {
        result = snd_pcm_hw_params_set_format(hAlsa, _dse_alsa_hw_params, SND_PCM_FORMAT_S24_LE);
    }
    
    result = snd_pcm_hw_params_set_channels(hAlsa, _dse_alsa_hw_params, mmio->audio.channels);
    
    // Near sample rate
    #ifdef UNIX_LEGACY
        result = snd_pcm_hw_params_set_rate_near(hAlsa, _dse_alsa_hw_params, sample_rate, dir);
    #else
        result = snd_pcm_hw_params_set_rate_near(hAlsa, _dse_alsa_hw_params, &sample_rate, 0);
    #endif
    result = snd_pcm_hw_params(hAlsa, _dse_alsa_hw_params);
    
    return result;
}

int  _dse_alsa_allocate(uint_t size, uint_t sample_size, uint_t count) {
    
    int                  minBufferSize = size;
    int                  result = 0;
    snd_pcm_sw_params_t* swParams;
    
    snd_pcm_sw_params_malloc(&swParams);
    
    result = snd_pcm_prepare(hAlsa);
    
    snd_pcm_sw_params_current(hAlsa, swParams);
    
    result = snd_pcm_sw_params_set_avail_min(hAlsa, swParams, minBufferSize);
    
    result = snd_pcm_sw_params_set_start_threshold(hAlsa, swParams, 0);
    
    result = snd_pcm_sw_params(hAlsa, swParams);
    
    #ifdef UNIX_LEGACY
        snd_pcm_hw_params_get_rate(_dse_alsa_hw_params, &_dse_alsa_sample_rate);
    #else
        snd_pcm_hw_params_get_rate(_dse_alsa_hw_params, &_dse_alsa_sample_rate, 0);
    #endif
    
    _dse_alsa_frame_size = size;
    //_dse_alsa_buffer     = (uchar_t*)malloc(_dse_alsa_frame_size  * sample_size * count);

    return result;
}

int _dse_alsa_free() {
    //free(_dse_alsa_buffer);
    return 0;
}

void _dse_alsa_write(uchar_t* data, int size) {
    if(size > _dse_alsa_frame_size)
        return;
    
    memcpy(_dse_alsa_buffer, data, size);
}

void _dse_alsa_write2(uchar_t* data) {
    int result = 0;
    snd_pcm_uframes_t frames_to_write = _dse_alsa_frame_size;
    
    result = snd_pcm_writei(hAlsa, (short*)data, frames_to_write);

    if(result == -EPIPE) {
        snd_pcm_prepare(hAlsa);
    }
}

void _dse_alsa_wait() {
    #ifndef UNIX_LEGACY
        snd_pcm_wait(hAlsa, -1);
    #endif
}

int _dse_alsa_close() {
    snd_pcm_drain(hAlsa);
    snd_pcm_close(hAlsa);
}

#endif