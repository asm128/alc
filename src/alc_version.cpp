#include "alc_version.h"
#include "alc_version_info.h"

cxpr ::llc::u3_c    alc::versionTime          ()  { return ALC_VERSION_TIME          ; }
cxpr ::llc::u1_c    alc::versionYear          ()  { return ALC_VERSION_YEAR          ; }
cxpr ::llc::u0_c    alc::versionMonth         ()  { return ALC_VERSION_MONTH         ; }
cxpr ::llc::u0_c    alc::versionMday          ()  { return ALC_VERSION_MDAY          ; }
cxpr ::llc::u1_c    alc::versionHour          ()  { return ALC_VERSION_HOUR          ; }
cxpr ::llc::u0_c    alc::versionMinute        ()  { return ALC_VERSION_MINUTE        ; }
cxpr ::llc::vcst_c  alc::versionString        ()  { return ALC_VERSION_STRING        ; }
cxpr ::llc::vcst_c  alc::versionBranch        ()  { return ALC_VERSION_BRANCH        ; }
cxpr ::llc::vcst_c  alc::versionCommit        ()  { return ALC_VERSION_COMMIT        ; }
cxpr ::llc::vcst_c  alc::versionFilename      ()  { return ALC_VERSION_FILENAME      ; }
cxpr ::llc::vcst_c  alc::versionFilesystem    ()  { return ALC_VERSION_FILESYSTEM    ; }