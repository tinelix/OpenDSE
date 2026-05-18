#ifdef UNIX

#include <os/unix/dseunix.h>
#include <math.h>

#ifndef UNIX_LEGACY

__attribute__((constructor))
void libopendse_start() {

}

__attribute__((destructor))
void libopendse_finish() {

}

#endif

int _dse_free_frames = 0;
int _dse_frames_count = 0;
int _dse_frame_samples = 0;

int _dse_open_outdev(DSE_OUTDEV* outdev, DSE_MMIO* mmio) {

	int result = 0;
	
	#ifdef UNIX_ALSA
	_dse_alsa_open(outdev, mmio);
	#endif
	
	_dse_frame_samples = 384;

	return result;
}

int _dse_alloc_audio(DSE_MMIO* mmio) {
	uint_t   sample_size   = (mmio->audio.bit_depth / 8) * mmio->audio.channels;
	uchar_t* inbuf         = mmio->_i->inbuf;

	#ifdef UNIX_ALSA
		_dse_alsa_allocate(_dse_frame_samples, sample_size, 32);
		_dse_free_frames = 32;
		_dse_frames_count = 32;
	#endif

	inbuf   = (uchar_t*)malloc(
                            _dse_frame_samples * sample_size * sizeof(uchar_t)
                        );
	return 0;
}

int _dse_decode_audio(DSE_MMIO* mmio, ulong_t offset, ulong_t count) {
	
	uchar_t* inbuf         = mmio->_i->inbuf;
	uint_t   sample_size   = (mmio->audio.bit_depth / 8) * mmio->audio.channels;
	uint_t   frame_size    = _dse_frame_samples * sample_size;
	uint_t   buffer_size   = frame_size * _dse_frames_count;

	if(_dse_frames_count == 0)
		return -1;
	
	if(count < buffer_size)
		buffer_size = count;

	// TODO: Implement native APIs

	inbuf                       = (uchar_t*)malloc(buffer_size * sizeof(uchar_t));
	mmio->_i->inbuf_size        = buffer_size;
	mmio->audio.frame_samples   = _dse_frame_samples;

	while(count <= mmio->bytes_total) {
		
		fseek(mmio->filesrc, offset, SEEK_SET);

		while(_dse_free_frames < _dse_frames_count - 1) {
			usleep(50);
			// TODO: Implement native APIs
			
			if(_dse_free_frames == _dse_frames_count) {
			 	break;
			}
		}

		mmio->bytes_read += fread(inbuf, 1, buffer_size, mmio->filesrc);

		// TODO: Implement native APIs
		
		if(mmio->bytes_read < sizeof(inbuf)) {
			memset(inbuf + mmio->bytes_read, 0, sizeof(inbuf) - mmio->bytes_read);
		}

		offset += buffer_size;
		count  -= buffer_size;
	}

	free(inbuf);
	
	// TODO: Implement native APIs
	
}

int _dse_decode_audio2(DSE_MMIO* mmio, ulong_t offset) {

	uchar_t* inbuf         = mmio->_i->inbuf;
	uint_t   sample_size   = (mmio->audio.bit_depth / 8) * mmio->audio.channels;
	uint_t   frame_size    = _dse_frame_samples * sample_size;

	if(_dse_frames_count == 0)
		return -1;	

	if(offset >= mmio->bytes_total)
		return -2;

	if(mmio->bytes_total - offset < frame_size)
		return -3;
	
	#ifdef UNIX_ALSA
		_dse_alsa_wait();
	#endif
	
	fseek(mmio->filesrc, offset, SEEK_SET);

	mmio->_i->inbuf_size        = frame_size;
	mmio->audio.frame_samples   = _dse_frame_samples;

	mmio->bytes_read += fread(inbuf, 1, frame_size, mmio->filesrc);

	#ifdef UNIX_ALSA
		_dse_alsa_write2(inbuf);
	#endif

	return 0;
}

int _dse_free_audio(DSE_MMIO* mmio) {
	uchar_t* inbuf         = mmio->_i->inbuf;
	
	if(inbuf)
		free(inbuf);
	
	#ifdef UNIX_ALSA
		_dse_alsa_free();
	#endif
	
	return 0;
}

cbool _dse_is_busy() {
     
     // TODO: Implement native APIs
     
     return cfalse;
}


int _dse_close_outdev(DSE_OUTDEV* outdev, DSE_MMIO* mmio) {
	#ifdef UNIX_ALSA
		_dse_alsa_close();
	#endif
	
	return 0;
}

#endif
