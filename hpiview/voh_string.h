#ifndef __VOH_STRING_H__
#define __VOH_STRING_H__


#include <stdio.h>
#include <SaHpi.h>

const char *vohError2String( SaErrorT err );
const char *vohEntityType2String(SaHpiEntityTypeT type);
void vohEntityPath2String(SaHpiEntityT *entity, char *path);
const char *vohRdrType2String(SaHpiRdrTypeT type);

#endif
