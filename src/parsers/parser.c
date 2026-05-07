#include <parsers/parser.h>

DSE_IMEDIAFMT flacFmt = {MKMAGIC32('f', 'L', 'a', 'c'), 4, 0};
DSE_IMEDIAFMT riffFmt = {MKMAGIC32('R', 'I', 'F', 'F'), 4, 0};

int dse_parse_input(uchar_t* inbuf) {
	return 0;			
}
