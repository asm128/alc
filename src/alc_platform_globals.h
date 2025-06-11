#include "llc_array_pod.h"
#include "llc_array_static.h"

#ifndef ALC_PLATFORM_GLOBALS_H
#define ALC_PLATFORM_GLOBALS_H

namespace alc
{
	using namespace llc;
    LLC_USING_TYPEINT();
    typedef ::llc::error_t error_t;
    typedef ::llc::error_t err_t;
    LLC_USING_VIEW();
	LLC_USING_APOD();
	LLC_USING_ASTATIC();
} // namespace

#endif // MLC_PLATFORM_GLOBALS_H
