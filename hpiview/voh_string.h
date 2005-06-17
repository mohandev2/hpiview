#ifndef __VOH_STRING_H__
#define __VOH_STRING_H__


#include <SaHpi.h>
#include <glib.h>
#include "voh_types.h"

const char *vohBoolean2String(gboolean b);
const char *vohTime2String(SaHpiTimeT time);
const char *vohError2String( SaErrorT err );
const char *vohEntityType2String(SaHpiEntityTypeT type);
void vohEntityPath2String(SaHpiEntityT *entity, char *str);
void vohFullEntityPath2String(SaHpiEntityPathT *path, char *str);
const char *vohRdrType2String(SaHpiRdrTypeT type);
const char *vohSensorType2String(SaHpiSensorTypeT type);
const char *vohCapabilities2String(SaHpiCapabilitiesT cf);
const char *vohSeverity2String(SaHpiSeverityT severity);
const char *vohHsCapabilities2String(SaHpiHsCapabilitiesT cf);
const char *vohEventCategory2String(SaHpiEventCategoryT category);
const char *vohSensorEventCtrl2String(SaHpiSensorEventCtrlT ctrl);
const char *vohEventState2String(SaHpiEventStateT es,
				 SaHpiEventCategoryT category);
const char *vohSensorUnits2String(SaHpiSensorUnitsT unit, gboolean p);
const char *vohSensorUnits2Short(SaHpiSensorUnitsT unit, gboolean p);
const char *vohSensorValue2FullString(SaHpiSensorRecT *sensor,
				     SaHpiSensorReadingT *sv);
const char *vohSensorThdMask2String(SaHpiSensorThdMaskT mask);
const char *vohReadWriteThds2String(SaHpiSensorThdMaskT readm,
				    SaHpiSensorThdMaskT writem,
				    SaHpiSensorThdMaskT mask);
const char *vohSensorValue2String(SaHpiSensorReadingT *sv);
const char *vohResetAction2String(SaHpiResetActionT reset);
const char *vohPowerState2String(SaHpiResetActionT state);
const char *vohEventType2String(SaHpiEventTypeT evtype);
const char *vohResourceEventType2String(SaHpiResourceEventTypeT evtype);
const char *vohDomainEventType2String(SaHpiDomainEventTypeT evtype);
const char *vohSwEventType2String(SaHpiSwEventTypeT evtype);

const char *vohIdrAreaType2String(SaHpiIdrAreaTypeT type);
const char *vohIdrFieldType2String(SaHpiIdrFieldTypeT type);

GList *vohEventState2List(SaHpiEventCategoryT category);
GList *vohIdrAreaType2List(void);
GList *vohIdrFieldType2List(void);
GList *vohSensorThdMask2List(void);
void *vohFillSensorReadingValue(VohObjectT *obj, SaHpiSensorReadingT *sr);

const char *vohCtrlType2String(SaHpiCtrlTypeT type);
const char *vohCtrlOutputType2String(SaHpiCtrlOutputTypeT type);
const char *vohCtrlMode2String(SaHpiCtrlModeT mode);
const char *vohCtrlStateDigital2String(SaHpiCtrlStateDigitalT state);
GList *vohCtrlStateDigital2List(void);

#endif

