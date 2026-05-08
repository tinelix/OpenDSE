#include <parsers/parser.h>

DSE_IMEDIAFMT flacFmt = {MKMAGIC32('f', 'L', 'a', 'c'), 4, 0};
DSE_IMEDIAFMT riffFmt = {MKMAGIC32('R', 'I', 'F', 'F'), 4, 0};

int dse_parse_input(DSE_MMIO* mmio, uchar_t* inbuf) {

	if(inbuf[0] == 'R' && inbuf[1] == 'I' && inbuf[2] == 'F' && inbuf[3] == 'F') {
		dse_riff_parse(mmio, inbuf);
	}

	return 0;
}
