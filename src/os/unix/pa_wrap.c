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

#ifdef UNIX_PULSEAUDIO

#include <os/unix/pa_wrap.h>

pa_simple*           _dse_pa_simple_api;
uint_t               _dse_pa_frame_size;

 /*
  *  This file contains a frontend implementation for the PulseAudio Simple API, 
  *  licensed under the GNU Lesser General Public License 2.1 or later.
  */

int _dse_pulseaudio_open(DSE_OUTDEV* outdev, DSE_MMIO* mmio) {
    pa_sample_spec  pa_sample_spec;
 
    pa_sample_spec.format   = PA_SAMPLE_S16LE;
    pa_sample_spec.rate     = mmio->audio.sample_rate;
    pa_sample_spec.channels = mmio->audio.channels;
    
    outdev->sample_rate   = mmio->audio.sample_rate;
    outdev->bit_depth     = mmio->audio.bit_depth;
    outdev->channels      = mmio->audio.channels;
    outdev->product_name  = (char*)malloc(161 * sizeof(char));
    strcpy(outdev->product_name, "PulseAudio Default Playback Device");
    
    switch(mmio->audio.bit_depth) {
        case 8:
            format = PA_SAMPLE_U8;
            break;
        case 16:
            format = PA_SAMPLE_S16LE;
            break;
        case 24:
            format = PA_SAMPLE_S24LE;
            break;
        case 32:
            format = PA_SAMPLE_S32LE;
            break;
    }
    
    _dse_pa_simple_api = pa_simple_new(
        NULL, "Open Digital Sound Engine",
        PA_STREAM_PLAYBACK, NULL,
        "PulseAudio Playback",
        &pa_sample_spec, NULL, NULL, NULL,
    );
    
    return 0;
}

int  _dse_pulseaudio_prepare(uint_t size, uint_t sample_size, uint_t count) {
    _dse_pa_frame_size = size * sample_size;
    return 0;
}

int _dse_pulseaudio_free() {
    int err = 0;
    
    pa_simple_drain(_dse_pa_simple_api, &err);
    pa_simple_free(_dse_pa_simple_api);
    return 0;
}

void _dse_pulseaudio_write(uchar_t* data, int size) {
    if(size > _dse_alsa_frame_size)
        return;
    
    pa_simple_write(data, size, &err);
}

void _dse_pulseaudio_write2(uchar_t* data) {
    int err = 0;
    pa_simple_write(data, _dse_pa_frame_size, &err);
}

#endif
