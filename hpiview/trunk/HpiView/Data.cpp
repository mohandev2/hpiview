/***************************************************************************
                          Data.cpp  -  description
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

#include "Data.h"
#include <time.h>


void
tableEntry( QTable *table, int &idx, const char *str1, const char *str2 )
{
  table->setNumRows( idx + 1 );
  table->setText( idx, 0, str1 );
  table->setText( idx, 1, str2 );

  idx++;
}


void
tableEntryBool( QTable *table, int &idx, const char *str1, bool v )
{
  tableEntry( table, idx, str1, v ? "true" : "false" );
}


void
tableEntryInt( QTable *table, int &idx, const char *str1, int v )
{
  char s[20];
  sprintf( s, "%d", v );

  tableEntry( table, idx, str1, s );
}


void
tableEntryHex( QTable *table, int &idx, const char *str1, unsigned int v )
{
  char s[20];
  sprintf( s, "0x%x", v );

  tableEntry( table, idx, str1, s ); 
}


void
tableEntryTextBuffer( QTable *table, int &idx, const char *name, 
                      const SaHpiTextBufferT *tb )
{
  if ( tb == 0 || tb->DataLength == 0 )
     {
       tableEntry( table, idx, name, "" );
       return;
     }

  char str[257];
  memcpy( str, tb->Data, 256 );
  str[tb->DataLength] = 0;

  tableEntry( table, idx, name, str );
}


void
tableRptEntry( QTable *table, int &idx, const SaHpiRptEntryT &rpt )
{
  tableEntryInt( table, idx, "EntityId", rpt.EntryId );
  tableEntryInt( table, idx, "ResourceId", rpt.ResourceId );
  tableEntry   ( table, idx, "Capabilities", hpiCapabilities2String( rpt.ResourceCapabilities ) );
  tableEntryTextBuffer( table, idx, "Tag", &rpt.ResourceTag );
  tableEntry   ( table, idx, "Severity", hpiSeverity2String( rpt.ResourceSeverity ) );
  tableEntryHex( table, idx, "ResourceRev", rpt.ResourceInfo.ResourceRev );
  tableEntryHex( table, idx, "SpecificVer", rpt.ResourceInfo.SpecificVer );
  tableEntryHex( table, idx, "DeviceSupport", rpt.ResourceInfo.DeviceSupport );
  tableEntryHex( table, idx, "ManufacturerId", rpt.ResourceInfo.ManufacturerId );
  tableEntryHex( table, idx, "ProductId", rpt.ResourceInfo.ProductId );
  tableEntryHex( table, idx, "FirmwareMajorRev", rpt.ResourceInfo.FirmwareMajorRev );
  tableEntryHex( table, idx, "FirmwareMinorRev",rpt.ResourceInfo.FirmwareMinorRev );
  tableEntryHex( table, idx, "AuxFirmwareRev", rpt.ResourceInfo.AuxFirmwareRev );
}


void
tableSensorFactors( QTable *table, int &idx, const SaHpiSensorFactorsT &factors )
{
  tableEntryInt( table, idx, "M", factors.M_Factor );
  tableEntryInt( table, idx, "B", factors.B_Factor );
  tableEntryInt( table, idx, "Accuracy", factors.AccuracyFactor );
  tableEntryInt( table, idx, "Tolerance", factors.ToleranceFactor );
  tableEntryInt( table, idx, "ExpA", factors.ExpA );
  tableEntryInt( table, idx, "ExpR", factors.ExpR );
  tableEntryInt( table, idx, "ExpB", factors.ExpB );
  tableEntry( table, idx, "Linearization", hpiSensorLinearization2String( factors.Linearization ) );
}


QString
getInterpretedValue( const SaHpiSensorInterpretedT &inter )
{
  switch( inter.Type )
     {
       case SAHPI_SENSOR_INTERPRETED_TYPE_UINT8:
            return QString( "%1" ).arg( inter.Value.SensorUint8 );

       case SAHPI_SENSOR_INTERPRETED_TYPE_UINT16:
            return QString( "%1" ).arg( inter.Value.SensorUint16 );
            
       case SAHPI_SENSOR_INTERPRETED_TYPE_UINT32:
            return QString( "%1" ).arg( inter.Value.SensorUint32 );

       case SAHPI_SENSOR_INTERPRETED_TYPE_INT8:
            return QString( "%1" ).arg( inter.Value.SensorInt8 );

       case SAHPI_SENSOR_INTERPRETED_TYPE_INT16:
            return QString( "%1" ).arg( inter.Value.SensorInt16 );

       case SAHPI_SENSOR_INTERPRETED_TYPE_INT32:
            return QString( "%1" ).arg( inter.Value.SensorInt32 );

       case SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32:
            return QString( "%1" ).arg( inter.Value.SensorFloat32 );

       case SAHPI_SENSOR_INTERPRETED_TYPE_BUFFER:
            return "buffer";
     }
  
  return "---";
}


QString
getSensorStatus( const SaHpiSensorEvtStatusT & /*status*/ )
{
  return "event status";
}


void
tableSensorReading( QTable *table, int &idx, const char *str,
                    const SaHpiSensorReadingT &sr )
{
  QString v = "";

  if ( sr.ValuesPresent & SAHPI_SRF_RAW )
       v += QString( "%1 " ).arg( sr.Raw );

  if ( sr.ValuesPresent & SAHPI_SRF_INTERPRETED )
       v += getInterpretedValue( sr.Interpreted );

  if ( sr.ValuesPresent & SAHPI_SRF_EVENT_STATE )
       v += getSensorStatus( sr.EventStatus );

  tableEntry( table, idx, str, v );
}


void
tableSensorRange( QTable *table, int &idx, const SaHpiSensorRangeT &range )
{
  tableEntry( table, idx, "Flags", hpiSensorRangeFlags2String( range.Flags ) );
  tableSensorReading( table, idx, "Max", range.Max );
  tableSensorReading( table, idx, "Min", range.Min );
  tableSensorReading( table, idx, "Nominal", range.Nominal );
  tableSensorReading( table, idx, "NormalMax", range.NormalMax );
  tableSensorReading( table, idx, "NormalMin", range.NormalMin );
}


void
tableSensorDataFormat( QTable *table, int &idx, const SaHpiSensorDataFormatT &df )
{
  tableEntry( table, idx, "ReadingFormats", hpiSensorReadingFormat2String( df.ReadingFormats ) );
  tableEntryBool( table, idx, "IsNumeric", df.IsNumeric );
  tableEntry( table, idx, "SignFormat", hpiSensorSignFormat2String( df.SignFormat ) );
  tableEntry( table, idx, "BaseUnits", hpiSensorUnits2String( df.BaseUnits ) );
  tableEntry( table, idx, "ModifierUnits", hpiSensorUnits2String( df.ModifierUnits ) );
  tableEntry( table, idx, "ModifierUse", hpiSensorModUnitUse2String( df.ModifierUse ) );
  tableEntryBool( table, idx, "FactorsStatic", df.FactorsStatic );

  if ( df.FactorsStatic )
       tableSensorFactors( table, idx, df.Factors );

  tableEntryBool( table, idx, "Percentage", df.Percentage );
  tableSensorRange( table, idx, df.Range );
}


void
tableSensorThresholdDefinition( QTable *table, int &idx, const SaHpiSensorThdDefnT &th )
{
  tableEntryBool( table, idx, "IsThreshold", th.IsThreshold );
  tableEntry( table, idx, "TholdCapabilities", hpiSensorThdCap2String( th.TholdCapabilities ) );
  tableEntry( table, idx, "ReadThold",  hpiSensorThdMask2String( th.ReadThold ) );
  tableEntry( table, idx, "WriteThold", hpiSensorThdMask2String( th.WriteThold ) );
  tableEntry( table, idx, "FixedThold", hpiSensorThdMask2String( th.FixedThold ) );
}


void
tableSensor( QTable *table, int &idx, const SaHpiSensorRecT &sensor )
{
  tableEntryInt( table, idx, "Num", sensor.Num );
  tableEntry( table, idx, "Type", hpiSensorType2String( sensor.Type ) );
  tableEntry( table, idx, "Category", hpiEventCategory2String( sensor.Category ) );
  tableEntry( table, idx, "EventCtrl", hpiSensorEventCtrl2String( sensor.EventCtrl ) );
  tableEntry( table, idx, "Events", hpiEventState2String( sensor.Category, sensor.Events ) );
  tableEntryBool( table, idx, "Ignore", sensor.Ignore );
  tableSensorDataFormat( table, idx, sensor.DataFormat );
  tableSensorThresholdDefinition( table, idx, sensor.ThresholdDefn );
  tableEntryHex( table, idx, "Oem",  sensor.Oem );
}


void
tableInventory( QTable *table, int &idx, const SaHpiInventoryRecT &inv )
{
  tableEntryInt( table, idx, "EirId", inv.EirId );
  tableEntryHex( table, idx, "Oem", inv.Oem );
}


void
tableCtrlTypeDigital( QTable *table, int &idx, const SaHpiCtrlRecDigitalT dig )
{
  tableEntry( table, idx, "Default", hpiCtrlStateDigital2String( dig.Default ) );
}


void
tableCtrlTypeDiscrete( QTable *table, int &idx, const SaHpiCtrlRecDiscreteT &dis )
{
  tableEntryInt( table, idx, "Default", dis.Default );
}


void
tableCtrlTypeAnalog( QTable *table, int &idx, const SaHpiCtrlRecAnalogT &analog )
{
  tableEntryInt( table, idx, "Min", analog.Min );
  tableEntryInt( table, idx, "Max", analog.Max );
  tableEntryInt( table, idx, "Default", analog.Default );
}


void
tableCtrlTypeStream( QTable * /*table*/, int & /*idx*/, const SaHpiCtrlRecStreamT & /*stream*/ )
{
}


void
tableCtrlTypeText( QTable * /*table*/, int & /*idx*/, const SaHpiCtrlRecTextT & /*text*/ )
{
}


void
tableCtrlTypeOem( QTable *table, int &idx, const SaHpiCtrlRecOemT &oem )
{
  tableEntryHex( table, idx, "MId", oem.MId );
}


void
tableControl( QTable *table, int &idx, const SaHpiCtrlRecT &ctrl )
{
  tableEntryInt( table, idx, "Num", ctrl.Num );
  tableEntryBool( table, idx, "Ignore", ctrl.Ignore );
  tableEntry( table, idx, "OutputType", hpiCtrlOutputType2String( ctrl.OutputType ) );
  tableEntry( table, idx, "Type", hpiCtrlType2String( ctrl.Type ) );

  switch( ctrl.Type )
     {
       case SAHPI_CTRL_TYPE_DIGITAL:
	    tableCtrlTypeDigital( table, idx, ctrl.TypeUnion.Digital );
	    break;
 
       case SAHPI_CTRL_TYPE_DISCRETE:
	    tableCtrlTypeDiscrete( table, idx, ctrl.TypeUnion.Discrete );
	    break;

       case SAHPI_CTRL_TYPE_ANALOG:
	    tableCtrlTypeAnalog( table, idx, ctrl.TypeUnion.Analog );	    
	    break;

       case SAHPI_CTRL_TYPE_STREAM:
	    tableCtrlTypeStream( table, idx, ctrl.TypeUnion.Stream );	    
	    break;

       case SAHPI_CTRL_TYPE_TEXT:
	    tableCtrlTypeText( table, idx, ctrl.TypeUnion.Text );	    
	    break;

       case SAHPI_CTRL_TYPE_OEM:
	    tableCtrlTypeOem( table, idx, ctrl.TypeUnion.Oem );	    
	    break;
     }

  tableEntryHex( table, idx, "Oem", ctrl.Oem );
}

void
tableRdr( QTable *table, int &idx, const SaHpiRdrT &rdr )
{
  tableEntryInt( table, idx, "RecordId", rdr.RecordId );
  tableEntry( table, idx, "RdrType", hpiRdrType2String( rdr.RdrType ) );
  tableEntryTextBuffer( table, idx, "IdString", &rdr.IdString );

  switch( rdr.RdrType )
     {
       case SAHPI_SENSOR_RDR:
            tableSensor( table, idx, rdr.RdrTypeUnion.SensorRec );
            break;

       case SAHPI_INVENTORY_RDR:
            tableInventory( table, idx, rdr.RdrTypeUnion.InventoryRec );
            break;

       case SAHPI_CTRL_RDR:
	    tableControl( table, idx, rdr.RdrTypeUnion.CtrlRec );
	    break;

       default:
            break;
     }
}


void
tableEventSensor( QTable *table, int &idx, const SaHpiSensorEventT &e )
{
  tableEntryInt( table, idx, "SensorNum", e.SensorNum );
  tableEntry( table, idx, "SensorType", hpiSensorType2String( e.SensorType ) );
  tableEntry( table, idx, "EventCategory", hpiEventCategory2String( e.EventCategory ) );
  tableEntryBool( table, idx, "Assertion", e.Assertion );
  tableEntry( table, idx, "EventState", hpiEventState2String( e.EventCategory, e.EventState ) );
  tableEntry( table, idx, "OptionalDataPresent", hpiSensorOptionalData2String( e.OptionalDataPresent ) );

  if ( e.OptionalDataPresent & SAHPI_SOD_TRIGGER_READING )
       tableSensorReading( table, idx, "TriggerReading", e.TriggerReading );

  if ( e.OptionalDataPresent & SAHPI_SOD_TRIGGER_THRESHOLD )
       tableSensorReading( table, idx, "TriggerThreshold", e.TriggerThreshold );

  if ( e.OptionalDataPresent & SAHPI_SOD_PREVIOUS_STATE )
       tableEntry( table, idx, "PreviousState", hpiEventState2String( e.EventCategory, e.PreviousState ) );

  if ( e.OptionalDataPresent & SAHPI_SOD_OEM )
       tableEntryHex( table, idx, "Oem", e.Oem );

  if ( e.OptionalDataPresent & SAHPI_SOD_SENSOR_SPECIFIC )
       tableEntryHex( table, idx, "SensorSpecific", e.SensorSpecific );
}


void
tableEventHotswap( QTable *table, int &idx, const SaHpiHotSwapEventT &e )
{
  tableEntry( table, idx, "HotSwapState", hpiHsState2String( e.HotSwapState ) );
  tableEntry( table, idx, "PreviousHotSwapState", hpiHsState2String( e.PreviousHotSwapState ) );
}


void
tableEvent( QTable *table, int &idx, const SaHpiEventT &event )
{
  tableEntryInt( table, idx, "Source", event.Source );
  tableEntry( table, idx, "EventType", hpiEventType2String( event.EventType ) );
  tableEntry( table, idx, "Severity", hpiSeverity2String( event.Severity ) );
  
  switch( event.EventType )
     {
       case SAHPI_ET_SENSOR:
            tableEventSensor( table, idx, event.EventDataUnion.SensorEvent );
            break;

       case SAHPI_ET_HOTSWAP:
            tableEventHotswap( table, idx, event.EventDataUnion.HotSwapEvent );
            break;

       case SAHPI_ET_WATCHDOG:
       case SAHPI_ET_OEM:
       case SAHPI_ET_USER:
            break;
     }
}


void
tableInventoryGeneral( QTable *table, int &idx, const SaHpiInventGeneralDataT &gd )
{
  char str[80] = "";

  if ( gd.MfgDateTime != SAHPI_TIME_UNSPECIFIED )
     {
       SaHpiTimeT st = gd.MfgDateTime / 1000000000;
       time_t t = st;

       struct tm tm;
       localtime_r( &t, &tm );

       // 2003.10.30 11:11:11
       strftime( str, 30, "%Y.%m.%d %H:%M:%S", &tm );
     }

  tableEntry( table, idx, "MfgDateTime", str );
  tableEntryTextBuffer( table, idx, "Manufacturer", gd.Manufacturer );
  tableEntryTextBuffer( table, idx, "ProductName", gd.ProductName );
  tableEntryTextBuffer( table, idx, "ProductVersion", gd.ProductVersion );
  tableEntryTextBuffer( table, idx, "ModelNumber", gd.ModelNumber );
  tableEntryTextBuffer( table, idx, "SerialNumber", gd.SerialNumber );
  tableEntryTextBuffer( table, idx, "PartNumber", gd.PartNumber );
  tableEntryTextBuffer( table, idx, "FileId", gd.FileId );
  tableEntryTextBuffer( table, idx, "AssetTag", gd.AssetTag );

  for( int i = 0; i < 10 && gd.CustomField[i]; i++ )
     {
       sprintf( str, "Custom%d", i + 1 );
       tableEntryTextBuffer( table, idx, str, gd.CustomField[i] );
     }
}


void
tableInventoryChassis( QTable *table, int &idx,
                       const SaHpiInventChassisDataT &cd )
{
  tableEntry( table, idx, "Type", hpiInventChassisType2String( cd.Type ) );
  tableInventoryGeneral( table, idx, cd.GeneralData );
}
