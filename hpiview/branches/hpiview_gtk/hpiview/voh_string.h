#ifndef __VOH_STRING_H__
#define __VOH_STRING_H__


#include <stdio.h>
#include <SaHpi.h>

const char *vohError2String( SaErrorT err );
const char *vohEntityType2String(SaHpiEntityTypeT type);
void vohEntityPath2String(SaHpiEntityT *entity, char *str);
void vohFullEntityPath2String(SaHpiEntityPathT *path, char *str);
const char *vohRdrType2String(SaHpiRdrTypeT type);
const char *vohCapabilities2String(SaHpiCapabilitiesT cf);
const char *vohHsCapabilities2String(SaHpiHsCapabilitiesT cf);

#endif
