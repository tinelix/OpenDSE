#ifdef UNIX_ALSA

#include <os/unix/alsawrap.h>

snd_pcm_t* hAlsa;
uint_t               _dse_alsa_sample_rate;
uint_t               _dse_alsa_frame_size;
uint_t               _dse_alsa_bit_depth;
uint_t               _dse_alsa_channels;
uchar_t*             _dse_alsa_buffer;
snd_pcm_hw_params_t* _dse_alsa_hw_params;

int _dse_alsa_open(DSE_OUTDEV* outdev, DSE_MMIO* mmio) {
    int                  result    = 0;
    
    _dse_alsa_sample_rate = mmio->audio.sample_rate;
    _dse_alsa_bit_depth   = mmio->audio.bit_depth;
    _dse_alsa_channels    = mmio->audio.channels;
    
    outdev->sample_rate   = mmio->audio.sample_rate;
    outdev->bit_depth     = mmio->audio.bit_depth;
    outdev->channels      = mmio->audio.channels;
    outdev->product_name  = (char*)malloc(161 * sizeof(char*));
    strcpy(outdev->product_name, "ALSA Default Playback Device");
    
    result = snd_pcm_open(
        &hAlsa, "default", SND_PCM_STREAM_PLAYBACK, 0
    );
    
    snd_pcm_hw_params_malloc(&_dse_alsa_hw_params);
    snd_pcm_hw_params_any(hAlsa, _dse_alsa_hw_params);
    
    // Near sample rate and channels
    snd_pcm_hw_params_set_rate_near(hAlsa, _dse_alsa_hw_params, &_dse_alsa_sample_rate, 0);
    
    if(mmio->audio.bit_depth == 8) {
        snd_pcm_hw_params_set_format(hAlsa, _dse_alsa_hw_params, SND_PCM_FORMAT_U8);
    } else if(mmio->audio.bit_depth == 16) {
        snd_pcm_hw_params_set_format(hAlsa, _dse_alsa_hw_params, SND_PCM_FORMAT_S16_LE);
    } else if(mmio->audio.bit_depth == 24) {
        snd_pcm_hw_params_set_format(hAlsa, _dse_alsa_hw_params, SND_PCM_FORMAT_S24_LE);
    }
    
    snd_pcm_hw_params_set_channels(hAlsa, _dse_alsa_hw_params, mmio->audio.channels);
    
    snd_pcm_hw_params_set_access(
        hAlsa, _dse_alsa_hw_params, SND_PCM_ACCESS_RW_INTERLEAVED
    );
    
    snd_pcm_hw_params(hAlsa, _dse_alsa_hw_params);
}

int  _dse_alsa_allocate(uint_t size, uint_t sample_size, uint_t count) {
    
    int                  minBufferSize = size;
    int                  result = 0;
    snd_pcm_sw_params_t* swParams;
    
    snd_pcm_sw_params_malloc(&swParams);
    snd_pcm_sw_params_current(hAlsa, swParams);
    
    snd_pcm_sw_params_set_avail_min(hAlsa, swParams, minBufferSize);
    snd_pcm_sw_params_set_start_threshold(hAlsa, swParams, 0);
    snd_pcm_sw_params(hAlsa, swParams);
    
    snd_pcm_hw_params_get_rate(_dse_alsa_hw_params, &_dse_alsa_sample_rate, 0);
    result = snd_pcm_prepare(hAlsa);
    
    _dse_alsa_frame_size = size;
    _dse_alsa_buffer     = (uchar_t*)malloc(_dse_alsa_frame_size  * sample_size * count);

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
    snd_pcm_wait(hAlsa, -1);
}

int _dse_alsa_close() {
    snd_pcm_close(hAlsa);
}

#endif