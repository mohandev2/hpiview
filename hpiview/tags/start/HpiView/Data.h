/***************************************************************************
                          Data.h  -  description
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


#ifndef DATA_H
#define DATA_H


#include "String.h"
#include <qtable.h>


QString getInterpretedValue( const SaHpiSensorInterpretedT &inter );

void tableEntry( QTable *table, int &idx, const char *str1, const char *str2 );
void tableEntryBool( QTable *table, int &idx, const char *str1, bool v );

void tableRptEntry( QTable *table, int &idx, const SaHpiRptEntryT &rpt_entry );
void tableSensorFactors( QTable *table, int &idx, const SaHpiSensorFactorsT &factors );
void tableSensorReading( QTable *table, int &idx, const char *str,
                         const SaHpiSensorReadingT &sr );
void tableSensorRange( QTable *table, int &idx, const SaHpiSensorRangeT &range );
void tableSensorDataFormat( QTable *table, int &idx, const SaHpiSensorDataFormatT &df );
void tableSensorThresholdDefinition( QTable *table, int &idx, const SaHpiSensorThdDefnT &th );
void tableSensor( QTable *table, int &idx, const SaHpiSensorRecT &sensor );
void tableRdr( QTable *table, int &idx, const SaHpiRdrT &rdr );
void tableInventoryGeneral( QTable *table, int &idx, const SaHpiInventGeneralDataT &gd );
void tableInventoryChassis( QTable *table, int &idx, const SaHpiInventChassisDataT &cd );
void tableEvent( QTable *table, int &idx, const SaHpiEventT &event );


#endif
