#ifndef __VOH_STRING_H__
#define __VOH_STRING_H__


#include <SaHpi.h>
#include <glib.h>

const char *vohBoolean2String(gboolean b);
const char *vohError2String( SaErrorT err );
const char *vohEntityType2String(SaHpiEntityTypeT type);
void vohEntityPath2String(SaHpiEntityT *entity, char *str);
void vohFullEntityPath2String(SaHpiEntityPathT *path, char *str);
const char *vohRdrType2String(SaHpiRdrTypeT type);
const char *vohSensorType2String(SaHpiSensorTypeT type);
const char *vohCapabilities2String(SaHpiCapabilitiesT cf);
const char *vohHsCapabilities2String(SaHpiHsCapabilitiesT cf);
const char *vohEventCategory2String(SaHpiEventCategoryT category);
const char *vohSensorEventCtrl2String(SaHpiSensorEventCtrlT ctrl);
const char *vohEventState2String(SaHpiEventStateT es,
				 SaHpiEventCategoryT category);
const char *vohSensorUnits2String(SaHpiSensorUnitsT unit);
const char *vohSensorUnits2Short(SaHpiSensorUnitsT unit);
const char *vohSensorValue2FullString(SaHpiSensorRecT *sensor,
				     SaHpiSensorReadingT *sv);
const char *vohSensorThdMask2String(SaHpiSensorThdMaskT mask);
const char *vohSensorValue2String(SaHpiSensorReadingT *sv);
const char *vohResetAction2String(SaHpiResetActionT reset);
const char *vohPowerState2String(SaHpiResetActionT state);

#endif

