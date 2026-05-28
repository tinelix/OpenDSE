#ifndef _OPENDSE_PARSERS_PARSERS_H
#define _OPENDSE_PARSERS_PARSERS_H

#include <mmio/mmio.h>
#include <utils/c_exts.h>
#include <parsers/riffpars.h>
#include <dsepriv.h>
#include <utils/result.h>

dse_result dse_parse_input(DSE_MMIO* mmio, uchar_t* inbuf);

#endif
