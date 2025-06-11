#include "alc_platform_globals.h"

#ifndef ALC_VERSION_H
#define ALC_VERSION_H

namespace alc
{
    cxpr ::llc::u3_c    versionTime       ();
    cxpr ::llc::u1_c    versionYear       ();
    cxpr ::llc::u0_c    versionMonth      ();
    cxpr ::llc::u0_c    versionMday       ();
    cxpr ::llc::u1_c    versionHour       ();
    cxpr ::llc::u0_c    versionMinute     ();
    cxpr ::llc::vcst_c  versionString     ();
    cxpr ::llc::vcst_c  versionBranch     ();
    cxpr ::llc::vcst_c  versionCommit     ();
    cxpr ::llc::vcst_c  versionFilename   ();
    cxpr ::llc::vcst_c  versionFilesystem ();
} // namespace

#endif // ALC_VERSION_H
