/***************************************************************************
                          String.h  -  description
                             -------------------
    begin                : Mon Dec  8 15:36:21 CET 2003
    copyright            : (C) 2003 by Thomas Kanngieser
    email                : Thomas.Kanngieser@fci.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef STRING_H


#include <stdio.h>


__BEGIN_DECLS
#include <SaHpi.h>
__END_DECLS


const char *hpiError2String( SaErrorT err );
const char *hpiEntityType2String( SaHpiEntityTypeT type );
const char *hpiCapabilities2String( SaHpiCapabilitiesT cap );
const char *hpiSeverity2String( SaHpiSeverityT sev );
const char *hpiSensorLinearization2String( SaHpiSensorLinearizationT l );
const char *hpiSensorRangeFlags2String( SaHpiSensorRangeFlagsT f );
const char *hpiSensorReadingFormat2String( SaHpiSensorReadingFormatsT f );
const char *hpiSensorSignFormat2String( SaHpiSensorSignFormatT format );
const char *hpiSensorUnits2String( SaHpiSensorUnitsT unit );
const char *hpiEventCategory2String( SaHpiEventCategoryT cat );
const char *hpiSensorModUnitUse2String( SaHpiSensorModUnitUseT m );
const char *hpiSensorThdCap2String( SaHpiSensorThdCapT v );
const char *hpiSensorThdMask2String( SaHpiSensorThdMaskT v );
const char *hpiSensorType2String( SaHpiSensorTypeT type );
const char *hpiRdrType2String( SaHpiRdrTypeT type );
const char *hpiSensorEventCtrl2String( SaHpiSensorEventCtrlT c );
const char *hpiEventState2String( SaHpiEventCategoryT cat, SaHpiEventStateT es );
const char *hpiEventType2String( SaHpiEventTypeT v );
const char *hpiSensorOptionalData2String( SaHpiSensorOptionalDataT v );
const char *hpiHsState2String( SaHpiHsStateT v );
const char *hpiInventChassisType2String( SaHpiInventChassisTypeT v );


#endif
