#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <opendse.h>
#include <mmio/mmio.h>
#include <parsers/parser.h>

#ifdef WIN32
	#include <os/win32/dsewin32.h>
#elif UNIX
	#include <os/unix/dseunix.h>
#endif

DSE_MMIO* 		stdmmio;

int dse_get_version(DSEVERSION* version) {
	version->major = 26;
	version->minor = 5;
	version->patch = 20;
	return ctrue;
}

int dse_open_input(const char* path) {
	
	int result = 0;

	stdmmio = (DSE_MMIO*)malloc(sizeof(DSE_MMIO));

	stdmmio->path = path;
	
	result = dse_mmio_open(stdmmio, path);

	if(result > 0) {
		
		uchar_t* inbuf = stdmmio->_i->inbuf;

		stdmmio->bytes_read = result;
		stdmmio->opened = ctrue;
		stdmmio->playing = cfalse;
		stdmmio->paused = cfalse;
		stdmmio->protocol = DSE_PROTOCOL_FILE;

		stdmmio->audio.sample_rate = 0;
		stdmmio->audio.bit_depth = 0;
		stdmmio->audio.bitrate = 0;
		stdmmio->audio.channels = 0;
		
		result = dse_parse_input(stdmmio, inbuf);
	}

	return result;
}

int dse_close_input() {

	int result = 0;

	result = dse_mmio_close(stdmmio);

	free(stdmmio);
	stdmmio = NULL;

	return result;
}

int dse_decode_audio() {
	_dse_decode_audio(stdmmio, stdmmio->bytes_read, stdmmio->bytes_total);
	return 0;
}

int dse_decode_audio2(ulong_t offset) {
	return _dse_decode_audio2(stdmmio, offset);
}

int dse_alloc_audio() {
	int result;

	result = _dse_alloc_audio(stdmmio);
	return result;
}

int dse_is_busy() {
	int result = _dse_is_busy();

	return result == ctrue;
}

int dse_free_audio() {
	int result;

	result = _dse_free_audio(stdmmio);
	return result;
}

int dse_get_frame_rms(double* rms, uint_t size) {

	uchar_t* inbuf = stdmmio->_i->inbuf;
	double   rms_l = 0.0;
	double   rms_r = 0.0;
	uint_t   i = 0;
	double   qsum_samples_l = 0.0;
	double   qsum_samples_r = 0.0;
	uint_t   ch_buffer_size = 0;
	uint_t   smooth_stage;
	ulong_t  max_value      = pow(2, stdmmio->audio.bit_depth);
	
	if(size >= 1024) {
		smooth_stage = 48 * (stdmmio->audio.bit_depth / 8);
	} else if(size >= 512) {
		smooth_stage = 24 * (stdmmio->audio.bit_depth / 8);
	} else {
		smooth_stage = 16 * (stdmmio->audio.bit_depth / 8);
	}

	ch_buffer_size = 
		(stdmmio->_i->inbuf_size / (stdmmio->audio.bit_depth / 8)) / smooth_stage;
	
	if(stdmmio->_i->inbuf_size == 0 && size == 0)
		return 0;
	
	for(i = 0; i < stdmmio->_i->inbuf_size; i += smooth_stage) {
		double sample_l = 0;
		double sample_r = 0;
		short  sample_16bit_l = 0;
        short  sample_16bit_r = 0;

		switch(stdmmio->audio.bit_depth) {
			case 16:
				if(i == stdmmio->_i->inbuf_size - (stdmmio->audio.channels))
					break;
				memcpy(&sample_16bit_l, &inbuf[i], sizeof(uint_t));
				sample_l = (double)abs(sample_16bit_l);

				if(stdmmio->audio.channels == 2) {
					memcpy(&sample_16bit_r, &inbuf[i + 2], sizeof(uint_t));
					sample_r = (double)abs(sample_16bit_r);
				}
				break;
			default:
				if(i == stdmmio->_i->inbuf_size - (stdmmio->audio.channels))
					break;
				sample_l = (double)abs((char)inbuf[i]);
				break;
		}

		switch(stdmmio->audio.channels) {
				case 2:
					qsum_samples_l += (double)sample_l * sample_l;
					qsum_samples_r += (double)sample_r * sample_r;
					break;
				default:
					qsum_samples_l += (double)sample_l * sample_l;
					break;
			}
	}
	
	rms_l = (double)(sqrt(qsum_samples_l / ch_buffer_size)) / max_value;
	rms[0] = rms_l;
	
	if(stdmmio->audio.channels >= 2 && size >= 2) {
		rms_l = (double)(sqrt(qsum_samples_l / (double)ch_buffer_size)) / max_value;
		rms_r = (double)(sqrt(qsum_samples_r / (double)ch_buffer_size)) / max_value;
		rms[1] = rms_r;
		return 2;
	}
	
	return 1;
}

int dse_decode_audio3(ulong_t offset, ulong_t count) {
	_dse_decode_audio(stdmmio, offset, count);
	return 0; 
}
