#include "String.h"
#include <string.h>


struct cMap
{
  unsigned int m_value;
  const char  *m_name;
};


static const char *
ValueToString( cMap *map, unsigned int value, const char *def )
{
  while( map->m_name )
     {
       if ( map->m_value == value )
            return map->m_name;

       map++;
     }

  if ( def == 0 )
       return 0;

  static char str[100];
  sprintf( str, def, value );

  return str;
}


static const char *
hpiBitMask2String( cMap *map, unsigned int value )
{
  static char str[1024];

  str[0] = 0;
  int mask = 0;

  while( map->m_name )
     {
       if ( (map->m_value & value) == map->m_value )
          {
            if ( str[0] != 0 )
                 strcat( str, " " );

            strcat( str, map->m_name );
            mask |= map->m_value;
          }

       map++;
     }

  mask &= ~value;
  
  if ( mask )
     {
       if ( str[0] != 0 )
            strcat( str, " " );
       
       char s[20];
       sprintf( s, "0x%x", mask );

       strcat( str, s );
     }

  return str;
}


const char *
hpiError2String( SaErrorT err )
{
  static cMap error_map[] =
  {
    { SA_OK                        , "ok" },
    { SA_ERR_HPI_ERROR             , "error" },
    { SA_ERR_HPI_UNSUPPORTED_API   , "unsupported_api" },
    { SA_ERR_HPI_BUSY              , "busy" },
    { SA_ERR_HPI_INVALID           , "invalid" },
    { SA_ERR_HPI_INVALID_CMD       , "invalid_cmd" },
    { SA_ERR_HPI_TIMEOUT           , "timeout" },
    { SA_ERR_HPI_OUT_OF_SPACE      , "out_of_space" },
    { SA_ERR_HPI_DATA_TRUNCATED    , "data_truncated" },
    { SA_ERR_HPI_DATA_LEN_INVALID  , "data_len_invalid" },
    { SA_ERR_HPI_DATA_EX_LIMITS    , "data_ex_limits" },
    { SA_ERR_HPI_INVALID_PARAMS    , "invalid_params" },
    { SA_ERR_HPI_INVALID_DATA      , "INVALID_DATA" },
    { SA_ERR_HPI_NOT_PRESENT       , "not_present" },
    { SA_ERR_HPI_INVALID_DATA_FIELD, "invalid_data_field" },
    { SA_ERR_HPI_INVALID_SENSOR_CMD, "invalid_sensor_cmd" },
    { SA_ERR_HPI_NO_RESPONSE       , "no_response" },
    { SA_ERR_HPI_DUPLICATE         , "duplicate" },
    { SA_ERR_HPI_UPDATING          , "updating" },
    { SA_ERR_HPI_INITIALIZING      , "initializing" },
    { SA_ERR_HPI_UNKNOWN           , "unknown" },
    { SA_ERR_HPI_INVALID_SESSION   , "invalid_session" },
    { SA_ERR_HPI_INVALID_DOMAIN    , "invalid_domain" },
    { SA_ERR_HPI_INVALID_RESOURCE  , "invalid_resource" },
    { SA_ERR_HPI_INVALID_REQUEST   , "invalid_request" },
    { SA_ERR_HPI_ENTITY_NOT_PRESENT, "entity_not_present" },
    { SA_ERR_HPI_UNINITIALIZED     , "uninitialized" },
    { 0 , 0 }
  };

  return ValueToString( error_map, err, "invalid error: %x" );
}


const char *
hpiEntityType2String( SaHpiEntityTypeT type )
{
  static cMap ent_map[] =
  {
    { SAHPI_ENT_UNSPECIFIED , "UNSPECIFIED" },
    { SAHPI_ENT_OTHER       , "OTHER" },
    { SAHPI_ENT_UNKNOWN, "UNKNOWN" },
    { SAHPI_ENT_PROCESSOR, "PROCESSOR" },
    { SAHPI_ENT_DISK_BAY, "DISK_BAY" },
    { SAHPI_ENT_PERIPHERAL_BAY, "PERIPHERAL_BAY" },
    { SAHPI_ENT_SYS_MGMNT_MODULE, "SYS_MGMNT_MODULE" },
    { SAHPI_ENT_SYSTEM_BOARD, "SYSTEM_BOARD" },
    { SAHPI_ENT_MEMORY_MODULE, "MEMORY_MODULE" },
    { SAHPI_ENT_PROCESSOR_MODULE, "PROCESSOR_MODULE" },
    { SAHPI_ENT_POWER_SUPPLY, "POWER_SUPPLY" },
    { SAHPI_ENT_ADD_IN_CARD, "ADD_IN_CARD" },
    { SAHPI_ENT_FRONT_PANEL_BOARD, "FRONT_PANEL_BOARD" },
    { SAHPI_ENT_BACK_PANEL_BOARD, "BACK_PANEL_BOARD" },
    { SAHPI_ENT_POWER_SYSTEM_BOARD, "POWER_SYSTEM_BOARD" },
    { SAHPI_ENT_DRIVE_BACKPLANE, "DRIVE_BACKPLANE" },
    { SAHPI_ENT_SYS_EXPANSION_BOARD, "SYS_EXPANSION_BOARD" },
    { SAHPI_ENT_OTHER_SYSTEM_BOARD, "OTHER_SYSTEM_BOARD" },
    { SAHPI_ENT_PROCESSOR_BOARD, "PROCESSOR_BOARD" },
    { SAHPI_ENT_POWER_UNIT, "POWER_UNIT" },
    { SAHPI_ENT_POWER_MODULE, "POWER_MODULE" },
    { SAHPI_ENT_POWER_MGMNT, "POWER_MGMNT" },
    { SAHPI_ENT_CHASSIS_BACK_PANEL_BOARD, "CHASSIS_BACK_PANEL_BOARD" },
    { SAHPI_ENT_SYSTEM_CHASSIS, "SYSTEM_CHASSIS" },
    { SAHPI_ENT_SUB_CHASSIS, "SUB_CHASSIS" },
    { SAHPI_ENT_OTHER_CHASSIS_BOARD, "OTHER_CHASSIS_BOARD" },
    { SAHPI_ENT_DISK_DRIVE_BAY, "DISK_DRIVE_BAY" },
    { SAHPI_ENT_PERIPHERAL_BAY_2, "PERIPHERAL_BAY_2" },
    { SAHPI_ENT_DEVICE_BAY, "DEVICE_BAY" },
    { SAHPI_ENT_COOLING_DEVICE, "COOLING_DEVICE" },
    { SAHPI_ENT_COOLING_UNIT, "COOLING_UNIT" },
    { SAHPI_ENT_INTERCONNECT, "INTERCONNECT" },
    { SAHPI_ENT_MEMORY_DEVICE, "MEMORY_DEVICE" },
    { SAHPI_ENT_SYS_MGMNT_SOFTWARE, "SYS_MGMNT_SOFTWARE" },
    { SAHPI_ENT_BIOS, "BIOS" },
    { SAHPI_ENT_OPERATING_SYSTEM, "OPERATING_SYSTEM" },
    { SAHPI_ENT_SYSTEM_BUS, "SYSTEM_BUS" },
    { SAHPI_ENT_GROUP, "GROUP" },
    { SAHPI_ENT_REMOTE, "REMOTE" },
    { SAHPI_ENT_EXTERNAL_ENVIRONMENT, "EXTERNAL_ENVIRONMENT" },
    { SAHPI_ENT_BATTERY, "BATTERY" },
    { SAHPI_ENT_CHASSIS_SPECIFIC , "CHASSIS_SPECIFIC" },
    { SAHPI_ENT_BOARD_SET_SPECIFIC, "BOARD_SET_SPECIFIC" },
    { SAHPI_ENT_OEM_SYSINT_SPECIFIC, "OEM_SYSINT_SPECIFIC" },
    { SAHPI_ENT_ROOT, "ROOT" },
    { SAHPI_ENT_RACK, "RACK" },
    { SAHPI_ENT_SUBRACK, "SUBRACK" },
    { SAHPI_ENT_COMPACTPCI_CHASSIS, "COMPACTPCI_CHASSIS" },
    { SAHPI_ENT_ADVANCEDTCA_CHASSIS, "ADVANCEDTCA_CHASSIS" },
    { SAHPI_ENT_SYSTEM_SLOT, "SYSTEM_SLOT" },
    { SAHPI_ENT_SBC_BLADE, "SBC_BLADE" },
    { SAHPI_ENT_IO_BLADE, "IO_BLADE" },
    { SAHPI_ENT_DISK_BLADE, "DISK_BLADE" },
    { SAHPI_ENT_DISK_DRIVE, "DISK_DRIVE" },
    { SAHPI_ENT_FAN, "FAN" },
    { SAHPI_ENT_POWER_DISTRIBUTION_UNIT, "POWER_DISTRIBUTION_UNIT" },
    { SAHPI_ENT_SPEC_PROC_BLADE, "SPEC_PROC_BLADE" },
    { SAHPI_ENT_IO_SUBBOARD, "IO_SUBBOARD" },
    { SAHPI_ENT_SBC_SUBBOARD, "SBC_SUBBOARD" },
    { SAHPI_ENT_ALARM_MANAGER, "ALARM_MANAGER" },
    { SAHPI_ENT_ALARM_MANAGER_BLADE, "ALARM_MANAGER_BLADE" },
    { SAHPI_ENT_SUBBOARD_CARRIER_BLADE, "SUBBOARD_CARRIER_BLADE" },
    { 0, 0 }
  };

  return ValueToString( ent_map, type, "%d" );
}


const char *
hpiCapabilities2String( SaHpiCapabilitiesT v )
{
  static cMap map[] =
  {
    { SAHPI_CAPABILITY_DOMAIN , "domain" },
    { SAHPI_CAPABILITY_RESOURCE , "resource" },
    { SAHPI_CAPABILITY_SEL , "SEL" },
    { SAHPI_CAPABILITY_EVT_DEASSERTS , "deasserts" },
    { SAHPI_CAPABILITY_AGGREGATE_STATUS , "aggregate_status" },
    { SAHPI_CAPABILITY_CONFIGURATION , "configuration" },
    { SAHPI_CAPABILITY_MANAGED_HOTSWAP , "managed_hotswap" },
    { SAHPI_CAPABILITY_WATCHDOG , "watchdog" },
    { SAHPI_CAPABILITY_CONTROL , "control" },
    { SAHPI_CAPABILITY_FRU , "FRU" },
    { SAHPI_CAPABILITY_INVENTORY_DATA , "inventory_data" },
    { SAHPI_CAPABILITY_RDR , "RDR" },
    { SAHPI_CAPABILITY_SENSOR , "sensor" },
    { 0, 0 }
  };

  return hpiBitMask2String( map, v );
}


const char *
hpiSeverity2String( SaHpiSeverityT sev )
{
  static cMap severity_map[] = 
  {
    { SAHPI_CRITICAL     , "critical" },
    { SAHPI_MAJOR        , "major" },
    { SAHPI_MINOR        , "minor" },
    { SAHPI_INFORMATIONAL, "informational" },
    { SAHPI_OK           , "ok" },
    { SAHPI_DEBUG        , "debug" },
    { 0, 0 }
  };

  return ValueToString( severity_map, sev, "invalid severity: %x" );  
}


const char *
hpiSensorLinearization2String( SaHpiSensorLinearizationT l )
{
  static cMap lin_map[] = 
  {
    { SAHPI_SL_LINEAR     , "linear" },
    { SAHPI_SL_LN         , "ln" },
    { SAHPI_SL_LOG10      , "log10" },
    { SAHPI_SL_LOG2       , "log2" },
    { SAHPI_SL_E          , "e" },
    { SAHPI_SL_EXP10      , "exp10" },
    { SAHPI_SL_EXP2       , "exp2" },
    { SAHPI_SL_1OVERX     , "1Overx" },
    { SAHPI_SL_SQRX       , "sqrx" },
    { SAHPI_SL_CUBEX      , "cubex" },
    { SAHPI_SL_SQRTX      , "sqrtx" },
    { SAHPI_SL_CUBERTX    , "cubertx" },
    { SAHPI_SL_NONLINEAR  , "nonlinear" },
    { SAHPI_SL_OEM        , "oem" },
    { SAHPI_SL_UNSPECIFIED, "unspecified" },
    { 0, 0 }
  };

  return ValueToString( lin_map, l, "invalid linearization: %x" );  
}


const char *
hpiSensorRangeFlags2String( SaHpiSensorRangeFlagsT v )
{
  static cMap map[] = 
  {
    { SAHPI_SRF_MIN       , "min" },
    { SAHPI_SRF_MAX       , "max" },
    { SAHPI_SRF_NORMAL_MIN, "normal_min" },
    { SAHPI_SRF_NORMAL_MAX, "normal_max" },
    { SAHPI_SRF_NOMINAL   , "nominal" },
    { 0, 0 }
  };

  return hpiBitMask2String( map, v );
}


const char *
hpiSensorReadingFormat2String( SaHpiSensorReadingFormatsT v )
{
  static cMap map[] =
  {
    { SAHPI_SRF_RAW        , "raw" },
    { SAHPI_SRF_INTERPRETED, "interpreted" },
    { SAHPI_SRF_EVENT_STATE, "event_state" },
    { 0, 0 }
  };

  return hpiBitMask2String( map, v );
}


const char *
hpiSensorSignFormat2String( SaHpiSensorSignFormatT v )
{
  static cMap map[] =
  {
    { SAHPI_SDF_UNSIGNED     , "unsigned" },
    { SAHPI_SDF_1S_COMPLEMENT, "1s_complement" },
    { SAHPI_SDF_2S_COMPLEMENT, "2s_complement" },
    { 0, 0 }
  };

  return ValueToString( map, v, "invalid format: %x" );
}


const char *
hpiSensorUnits2String( SaHpiSensorUnitsT v )
{
  static cMap map[] =
  {
    { SAHPI_SU_UNSPECIFIED         , "UNSPECIFIED" },
    { SAHPI_SU_DEGREES_C           , "DEGREES_C" },
    { SAHPI_SU_DEGREES_F           , "DEGREES_F " },
    { SAHPI_SU_DEGREES_K           , "DEGREES_K " },
    { SAHPI_SU_VOLTS               , "VOLTS" },
    { SAHPI_SU_AMPS                , "AMPS" },
    { SAHPI_SU_WATTS               , "WATTS" },
    { SAHPI_SU_JOULES              , "JOULES" },
    { SAHPI_SU_COULOMBS            , "COULOMBS" },
    { SAHPI_SU_VA                  , "VA" },
    { SAHPI_SU_NITS                , "NITS" },
    { SAHPI_SU_LUMEN               , "LUMEN" },
    { SAHPI_SU_LUX                 , "LUX" },
    { SAHPI_SU_CANDELA             , "CANDELA" },
    { SAHPI_SU_KPA                 , "KPA" },
    { SAHPI_SU_PSI                 , "PSI" },
    { SAHPI_SU_NEWTON              , "NEWTON" },
    { SAHPI_SU_CFM                 , "CFM" },
    { SAHPI_SU_RPM                 , "RPM" },
    { SAHPI_SU_HZ                  , "HZ" },
    { SAHPI_SU_MICROSECOND         , "MICROSECOND" },
    { SAHPI_SU_MILLISECOND         , "MILLISECOND" },
    { SAHPI_SU_SECOND              , "SECOND" },
    { SAHPI_SU_MINUTE              , "MINUTE" },
    { SAHPI_SU_HOUR                , "HOUR" },
    { SAHPI_SU_DAY                 , "DAY" },
    { SAHPI_SU_WEEK                , "WEEK" },
    { SAHPI_SU_MIL                 , "MIL" },
    { SAHPI_SU_INCHES              , "INCHES" },
    { SAHPI_SU_FEET                , "FEET" },
    { SAHPI_SU_CU_IN               , "CU_IN" },
    { SAHPI_SU_CU_FEET             , "CU_FEET" },
    { SAHPI_SU_MM                  , "MM" },
    { SAHPI_SU_CM                  , "CM" },
    { SAHPI_SU_M                   , "M" },
    { SAHPI_SU_CU_CM               , "CU_CM" },
    { SAHPI_SU_CU_M                , "CU_M" },
    { SAHPI_SU_LITERS              , "LITERS" },
    { SAHPI_SU_FLUID_OUNCE         , "FLUID_OUNCE" },
    { SAHPI_SU_RADIANS             , "RADIANS" },
    { SAHPI_SU_STERADIANS          , "STERADIANS" },
    { SAHPI_SU_REVOLUTIONS         , "REVOLUTIONS" },
    { SAHPI_SU_CYCLES              , "CYCLES" },
    { SAHPI_SU_GRAVITIES           , "GRAVITIES" },
    { SAHPI_SU_OUNCE               , "OUNCE" },
    { SAHPI_SU_POUND               , "POUND" },
    { SAHPI_SU_FT_LB               , "FT_LB" },
    { SAHPI_SU_OZ_IN               , "OZ_IN" },
    { SAHPI_SU_GAUSS               , "GAUSS" },
    { SAHPI_SU_GILBERTS            , "GILBERTS" },
    { SAHPI_SU_HENRY               , "HENRY" },
    { SAHPI_SU_MILLIHENRY          , "MILLIHENRY" },
    { SAHPI_SU_FARAD               , "FARAD" },
    { SAHPI_SU_MICROFARAD          , "MICROFARAD" },
    { SAHPI_SU_OHMS                , "OHMS" },
    { SAHPI_SU_SIEMENS             , "SIEMENS" },
    { SAHPI_SU_MOLE                , "MOLE" },
    { SAHPI_SU_BECQUEREL           , "BECQUEREL" },
    { SAHPI_SU_PPM                 , "PPM" },
    { SAHPI_SU_RESERVED            , "RESERVED" },
    { SAHPI_SU_DECIBELS            , "DECIBELS" },
    { SAHPI_SU_DBA                 , "DBA" },
    { SAHPI_SU_DBC                 , "DBC " },
    { SAHPI_SU_GRAY                , "GRAY" },
    { SAHPI_SU_SIEVERT             , "SIEVERT" },
    { SAHPI_SU_COLOR_TEMP_DEG_K    , "COLOR_TEMP_DEG_K" },
    { SAHPI_SU_BIT                 , "BIT" },
    { SAHPI_SU_KILOBIT             , "KILOBIT" },
    { SAHPI_SU_MEGABIT             , "MEGABIT" },
    { SAHPI_SU_GIGABIT             , "GIGABIT" },
    { SAHPI_SU_BYTE                , "BYTE" },
    { SAHPI_SU_KILOBYTE            , "KILOBYTE" },
    { SAHPI_SU_MEGABYTE            , "MEGABYTE" },
    { SAHPI_SU_GIGABYTE            , "GIGABYTE" },
    { SAHPI_SU_WORD                , "WORD" },
    { SAHPI_SU_DWORD               , "DWORD" },
    { SAHPI_SU_QWORD               , "QWORD" },
    { SAHPI_SU_LINE                , "LINE" },
    { SAHPI_SU_HIT                 , "HIT " },
    { SAHPI_SU_MISS                , "MISS" },
    { SAHPI_SU_RETRY               , "RETRY" },
    { SAHPI_SU_RESET               , "RESET" },
    { SAHPI_SU_OVERRUN             , "OVERRUN" },
    { SAHPI_SU_UNDERRUN            , "UNDERRUN" },
    { SAHPI_SU_COLLISION           , "COLLISION" },
    { SAHPI_SU_PACKETS             , "PACKETS" },
    { SAHPI_SU_MESSAGES            , "MESSAGES" },
    { SAHPI_SU_CHARACTERS          , "CHARACTERS" },
    { SAHPI_SU_ERRORS              , "ERRORS" },
    { SAHPI_SU_CORRECTABLE_ERRORS  , "CORRECTABLE_ERRORS" },
    { SAHPI_SU_UNCORRECTABLE_ERRORS, "UNCORRECTABLE_ERRORS" },
    { 0, 0 }
  };

  return ValueToString( map, v, "invalid unit: %x" );  
}


const char *
hpiEventCategory2String( SaHpiEventCategoryT cat )
{
  static cMap event_category_map[] =
  {
    { SAHPI_EC_UNSPECIFIED,   "unspecified" },
    { SAHPI_EC_THRESHOLD,     "threshold" },
    { SAHPI_EC_USAGE,         "usage" },
    { SAHPI_EC_STATE,         "state" },
    { SAHPI_EC_PRED_FAIL,     "pred_fail" },
    { SAHPI_EC_LIMIT,         "limit" },
    { SAHPI_EC_PERFORMANCE,   "performance" },
    { SAHPI_EC_SEVERITY,      "severity" },
    { SAHPI_EC_PRESENCE,      "presence" },
    { SAHPI_EC_ENABLE,        "enable" },
    { SAHPI_EC_AVAILABILITY,  "availability" },
    { SAHPI_EC_REDUNDANCY,    "redundancy" },
    { SAHPI_EC_USER,          "user" },
    { SAHPI_EC_GENERIC,       "generic" },
    { 0, 0 }
  };

  return ValueToString( event_category_map, cat, "event category: %x" );  
}


const char *
hpiSensorModUnitUse2String( SaHpiSensorModUnitUseT v )
{
  static cMap map[] =
  {
    { SAHPI_SMUU_NONE                , "none" },
    { SAHPI_SMUU_BASIC_OVER_MODIFIER , "basic_over_modifier" },
    { SAHPI_SMUU_BASIC_TIMES_MODIFIER, "basic_times_modifier" },
    { 0, 0 }
  };

  return ValueToString( map, v, "event category: %x" );  
}


const char *
hpiSensorThdCap2String( SaHpiSensorThdCapT v )
{
  static cMap map[] =
  {
    { SAHPI_STC_RAW        , "raw" },
    { SAHPI_STC_INTERPRETED, "interpreted" },
    { 0, 0 }
  };

  return hpiBitMask2String( map, v );
}


const char *
hpiSensorThdMask2String( SaHpiSensorThdMaskT v )
{
  static cMap map[] =
  {
    { SAHPI_STM_LOW_MINOR     , "low_minor" },
    { SAHPI_STM_LOW_MAJOR     , "low_major" },
    { SAHPI_STM_LOW_CRIT      , "low_crit" },
    { SAHPI_STM_UP_MINOR      , "up_minor" },
    { SAHPI_STM_UP_MAJOR      , "up_major" },
    { SAHPI_STM_UP_CRIT       , "up_crit" },
    { SAHPI_STM_UP_HYSTERESIS , "up_hysteresis" },
    { SAHPI_STM_LOW_HYSTERESIS, "low_hysteresis" },
    { 0, 0 }
  };

  return hpiBitMask2String( map, v );
}


const char *
hpiSensorType2String( SaHpiSensorTypeT v )
{
  static cMap map[] =
  {
    { SAHPI_TEMPERATURE, "TEMPERATURE SENSOR" },
    { SAHPI_VOLTAGE, "VOLTAGE SENSOR" },
    { SAHPI_CURRENT, "CURRENT SENSOR" },
    { SAHPI_FAN, "FAN SENSOR" },
    { SAHPI_PHYSICAL_SECURITY, "PHYSICAL SECURITY" },
    { SAHPI_PLATFORM_VIOLATION, "PLATFORM_VIOLATION" },
    { SAHPI_PROCESSOR, "PROCESSOR" },
    { SAHPI_POWER_SUPPLY, "POWER SUPPLY" },
    { SAHPI_POWER_UNIT, "POWER UNIT" },
    { SAHPI_COOLING_DEVICE, "COOLING DEVICE" },
    { SAHPI_OTHER_UNITS_BASED_SENSOR, "OTHER UNITS BASED SENSOR" },
    { SAHPI_MEMORY, "MEMORY" },
    { SAHPI_DRIVE_SLOT, "DRIVE_SLOT" },
    { SAHPI_POST_MEMORY_RESIZE, "POST_MEMORY_RESIZE" },
    { SAHPI_SYSTEM_FW_PROGRESS, "SYSTEM_FW_PROGRESS" },
    { SAHPI_EVENT_LOGGING_DISABLED, "EVENT_LOGGING_DISABLED" },
    { SAHPI_RESERVED1, "RESERVED1" },
    { SAHPI_SYSTEM_EVENT, "SYSTEM_EVENT" },
    { SAHPI_CRITICAL_INTERRUPT, "CRITICAL_INTERRUPT" },
    { SAHPI_BUTTON, "BUTTON" },
    { SAHPI_MODULE_BOARD, "MODULE_BOARD" },
    { SAHPI_MICROCONTROLLER_COPROCESSOR, "MICROCONTROLLER_COPROCESSOR" },
    { SAHPI_ADDIN_CARD, "ADDIN_CARD" },
    { SAHPI_CHASSIS, "CHASSIS" },
    { SAHPI_CHIP_SET, "CHIP_SET" },
    { SAHPI_OTHER_FRU, "OTHER_FRU" },
    { SAHPI_CABLE_INTERCONNECT, "CABLE_INTERCONNECT" },
    { SAHPI_TERMINATOR, "TERMINATOR" },
    { SAHPI_SYSTEM_BOOT_INITIATED,"SYSTEM_BOOT_INITIATED" },
    { SAHPI_BOOT_ERROR, "_BOOT_ERROR" },
    { SAHPI_OS_BOOT, "OS_BOOT" },
    { SAHPI_OS_CRITICAL_STOP, "OS_CRITICAL_STOP" },
    { SAHPI_SLOT_CONNECTOR, "SLOT_CONNECTOR" },
    { SAHPI_SYSTEM_ACPI_POWER_STATE, "SYSTEM_ACPI_POWER_STATE" },
    { SAHPI_RESERVED2, "RESERVED2" },
    { SAHPI_PLATFORM_ALERT, "PLATFORM_ALERT" },
    { SAHPI_ENTITY_PRESENCE,"ENTITY_PRESENCE" },
    { SAHPI_MONITOR_ASIC_IC, "MONITOR_ASIC_IC" },
    { SAHPI_LAN, "LAN" },
    { SAHPI_MANAGEMENT_SUBSYSTEM_HEALTH, "MANAGEMENT_SUBSYSTEM_HEALTH" },
    { SAHPI_BATTERY, "BATTERY" },
    { SAHPI_OPERATIONAL, "OPERATIONAL" },
    { SAHPI_OEM_SENSOR, "OEM_SENSOR" },
    { 0xf0, "ATCA_HOTSWAP" },
    { 0, 0 }
  };

  return ValueToString( map, v, "invalid sensor type: %x" );  
}


const char *
hpiRdrType2String( SaHpiRdrTypeT v )
{
  static cMap map[] =
  {
    { SAHPI_NO_RECORD    , "no record" },
    { SAHPI_CTRL_RDR     , "control" },
    { SAHPI_SENSOR_RDR   , "sensor" },
    { SAHPI_INVENTORY_RDR, "inventory" },
    { SAHPI_WATCHDOG_RDR , "watchdog" },
    { 0, 0 }
  };

  return ValueToString( map, v, "invalid rdr type: %x" );  
}


const char *
hpiSensorEventCtrl2String( SaHpiSensorEventCtrlT v )
{
  static cMap map[] =
  {
    { SAHPI_SEC_PER_EVENT      , "per_event" },
    { SAHPI_SEC_ENTIRE_SENSOR  , "entire_sensor" },
    { SAHPI_SEC_GLOBAL_DISABLE , "global_disable" },
    { SAHPI_SEC_NO_EVENTS      , "no_events" },
    { 0, 0 }
  };

  return ValueToString( map, v, "invalid sensor event control: %x" );  
}


const char *
hpiEventStateThreshold2String( SaHpiEventStateT v )
{  
  static cMap map[] = 
  {
    { SAHPI_ES_LOWER_MINOR , "lower_minor" },
    { SAHPI_ES_LOWER_MAJOR , "lower_major" },
    { SAHPI_ES_LOWER_CRIT  , "lower_crit" },
    { SAHPI_ES_UPPER_MINOR , "upper_minor" },
    { SAHPI_ES_UPPER_MAJOR , "upper_major" },
    { SAHPI_ES_UPPER_CRIT  , "upper_crit" },
    { 0, 0 }
  };

  return hpiBitMask2String( map, v );
}


const char *
hpiEventStateUsage2String( SaHpiEventStateT v )
{
  static cMap map[] =
  {
    { SAHPI_ES_IDLE  , "idle" },
    { SAHPI_ES_ACTIVE, "active" }, 
    { SAHPI_ES_BUSY  , "busy" },
    { 0, 0 }
  };

  return hpiBitMask2String( map, v );
}


const char *
hpiEventStateState2String( SaHpiEventStateT v )
{
  static cMap map[] =
  {
    { SAHPI_ES_STATE_DEASSERTED , "state_deasserted" },
    { SAHPI_ES_STATE_ASSERTED   , "state_asserted" },
    { 0, 0 }
  };
  
  return hpiBitMask2String( map, v );
}


const char *
hpiEventStatePredFail2String( SaHpiEventStateT v )
{
  static cMap map[] =
  {
    { SAHPI_ES_PRED_FAILURE_DEASSERT , "pred_failure_deassert" },
    { SAHPI_ES_PRED_FAILURE_ASSERT   , "pred_failure_assert" },
    { 0, 0 }
  };

  return hpiBitMask2String( map, v );
}


const char *
hpiEventStateLimit2String( SaHpiEventStateT v )
{
  static cMap map[] =
  {
    { SAHPI_ES_LIMIT_NOT_EXCEEDED , "limit_not_exceeded" },
    { SAHPI_ES_LIMIT_EXCEEDED     , "limit_exceeded" },
    { 0, 0 }
  };
  
  return hpiBitMask2String( map, v );
}


const char *
hpiEventStatePerformace2String( SaHpiEventStateT v )
{
  static cMap map[] =
  {
    { SAHPI_ES_PERFORMANCE_MET  , "performance_met" },
    { SAHPI_ES_PERFORMANCE_LAGS , "performance_lags" },
    { 0, 0 }
  };

  return hpiBitMask2String( map, v );
}


const char *
hpiEventStateSeverity2String( SaHpiEventStateT v )
{
  static cMap map[] =
  {
    { SAHPI_ES_OK                 , "OK" },
    { SAHPI_ES_MINOR_FROM_OK      , "MINOR_FROM_OK" },
    { SAHPI_ES_MAJOR_FROM_LESS    , "MAJOR_FROM_LESS" },
    { SAHPI_ES_CRITICAL_FROM_LESS , "CRITICAL_FROM_LESS" },
    { SAHPI_ES_MINOR_FROM_MORE    , "MINOR_FROM_MORE" },
    { SAHPI_ES_MAJOR_FROM_CRITICAL, "MAJOR_FROM_CRITICAL" },
    { SAHPI_ES_CRITICAL           , "CRITICAL" },
    { SAHPI_ES_MONITOR            , "MONITOR" },
    { SAHPI_ES_INFORMATIONAL      , "INFORMATIONAL" },
    { 0, 0 }
  };

  return hpiBitMask2String( map, v );
}


const char *
hpiEventStatePresence2String( SaHpiEventStateT v )
{
  static cMap map[] =
  {
    { SAHPI_ES_ABSENT  , "ABSENT" },
    { SAHPI_ES_PRESENT , "PRESENT" },
    { 0, 0 }
  };

  return hpiBitMask2String( map, v );
}


const char *
hpiEventStateEnable2String( SaHpiEventStateT v )
{
  static cMap map[] =
  {
    { SAHPI_ES_DISABLED , "DISABLED" },
    { SAHPI_ES_ENABLED  , "ENABLED" },
    { 0, 0 }
  };

  return hpiBitMask2String( map, v );
}


const char *
hpiEventStateAvailabilit2String( SaHpiEventStateT v )
{
  static cMap map[] =
  {
    { SAHPI_ES_RUNNING      , "RUNNING" },
    { SAHPI_ES_TEST         , "TEST" },
    { SAHPI_ES_POWER_OFF    , "POWER_OFF" },
    { SAHPI_ES_ON_LINE      , "ON_LINE" },
    { SAHPI_ES_OFF_LINE     , "OFF_LINE" },
    { SAHPI_ES_OFF_DUTY     , "OFF_DUTY" },
    { SAHPI_ES_DEGRADED     , "DEGRADED" },
    { SAHPI_ES_POWER_SAVE   , "POWER_SAVE" },
    { SAHPI_ES_INSTALL_ERROR, "INSTALL_ERROR" },
    { 0, 0 }
  };

  return hpiBitMask2String( map, v );
}


const char *
hpiEventStateRedundancy2String( SaHpiEventStateT v )
{
  static cMap map[] =
  {
    { SAHPI_ES_FULLY_REDUNDANT                      , "FULLY_REDUNDANT" },
    { SAHPI_ES_REDUNDANCY_LOST                      , "REDUNDANCY_LOST" },
    { SAHPI_ES_REDUNDANCY_DEGRADED                  , "REDUNDANCY_DEGRADED" },
    { SAHPI_ES_REDUNDANCY_LOST_SUFFICIENT_RESOURCES , "REDUNDANCY_LOST_SUFFICIENT_RESOURCES" },
    { SAHPI_ES_NON_REDUNDANT_SUFFICIENT_RESOURCES   , "NON_REDUNDANT_SUFFICIENT_RESOURCES" },
    { SAHPI_ES_NON_REDUNDANT_INSUFFICIENT_RESOURCES , "NON_REDUNDANT_INSUFFICIENT_RESOURCES" },
    { SAHPI_ES_REDUNDANCY_DEGRADED_FROM_FULL        , "REDUNDANCY_DEGRADED_FROM_FULL" },
    { SAHPI_ES_REDUNDANCY_DEGRADED_FROM_NON         , "REDUNDANCY_DEGRADED_FROM_NON" },
    { 0, 0 }
  };

  return hpiBitMask2String( map, v );
}


const char *
hpiEventStateGenericOrUser2String( SaHpiEventStateT v )
{
  static cMap map[] =
  {
    { SAHPI_ES_STATE_00 , "STATE_00" },
    { SAHPI_ES_STATE_01 , "STATE_01" },
    { SAHPI_ES_STATE_02 , "STATE_02" },
    { SAHPI_ES_STATE_03 , "STATE_03" },
    { SAHPI_ES_STATE_04 , "STATE_04" },
    { SAHPI_ES_STATE_05 , "STATE_05" },
    { SAHPI_ES_STATE_06 , "STATE_06" },
    { SAHPI_ES_STATE_07 , "STATE_07" },
    { SAHPI_ES_STATE_08 , "STATE_08" },
    { SAHPI_ES_STATE_09 , "STATE_09" },
    { SAHPI_ES_STATE_10 , "STATE_10" },
    { SAHPI_ES_STATE_11 , "STATE_11" },
    { SAHPI_ES_STATE_12 , "STATE_12" },
    { SAHPI_ES_STATE_13 , "STATE_13" },
    { SAHPI_ES_STATE_14 , "STATE_14" },
    { 0, 0 }
  };

  return hpiBitMask2String( map, v );
}


const char *
hpiEventState2String( SaHpiEventCategoryT cat, SaHpiEventStateT es )
{
  switch( cat )
     {
       case SAHPI_EC_THRESHOLD:
            return hpiEventStateThreshold2String( es );

       case SAHPI_EC_USAGE:
            return hpiEventStateUsage2String( es );

       case SAHPI_EC_STATE:
            return hpiEventStateState2String( es );

       case SAHPI_EC_PRED_FAIL:
            return hpiEventStatePredFail2String( es );

       case SAHPI_EC_LIMIT:
            return hpiEventStateLimit2String( es );

       case SAHPI_EC_PERFORMANCE:
            return hpiEventStatePerformace2String( es );

       case SAHPI_EC_SEVERITY:
            return hpiEventStateSeverity2String( es );

       case SAHPI_EC_PRESENCE:
            return hpiEventStatePresence2String( es );

       case SAHPI_EC_ENABLE:
            return hpiEventStateEnable2String( es );

       case SAHPI_EC_AVAILABILITY:
            return hpiEventStateAvailabilit2String( es );

       case SAHPI_EC_REDUNDANCY:
            return hpiEventStateRedundancy2String( es );

       case SAHPI_EC_GENERIC:
            return hpiEventStateGenericOrUser2String( es );

       case SAHPI_EC_USER:
            return hpiEventStateGenericOrUser2String( es );
     }
  
  static char str[80];
  sprintf( str, "0x%x", es );
  
  return str;
}


const char *
hpiEventType2String( SaHpiEventTypeT v )
{
  static cMap map[] =
  {
    { SAHPI_ET_SENSOR  , "sensor" },
    { SAHPI_ET_HOTSWAP , "hotswap" },
    { SAHPI_ET_WATCHDOG, "watchdog" },
    { SAHPI_ET_OEM     , "oem" },
    { SAHPI_ET_USER    , "user" },
    { 0, 0 }
  };

  return ValueToString( map, v, "invalid event type: %x" );  
}


const char *
hpiSensorOptionalData2String( SaHpiSensorOptionalDataT v )
{
  static cMap map[] =
  {
    { SAHPI_SOD_TRIGGER_READING  , "trigger_reading" },
    { SAHPI_SOD_TRIGGER_THRESHOLD, "trigger_threshold" },
    { SAHPI_SOD_OEM              , "oem" },
    { SAHPI_SOD_PREVIOUS_STATE   , "previous_state" },
    { SAHPI_SOD_SENSOR_SPECIFIC  , "sensor_specific" },
    { 0, 0 }
  };

  return hpiBitMask2String( map, v );
}


const char *
hpiHsState2String( SaHpiHsStateT v )
{
  static cMap map[] =
  {
    { SAHPI_HS_STATE_INACTIVE, "inactive" },
    { SAHPI_HS_STATE_INSERTION_PENDING, "insertion_pending" },
    { SAHPI_HS_STATE_ACTIVE_HEALTHY, "active_healthy" },
    { SAHPI_HS_STATE_ACTIVE_UNHEALTHY, "active_unhealthy" },
    { SAHPI_HS_STATE_EXTRACTION_PENDING, "extraction_pending" },
    { SAHPI_HS_STATE_NOT_PRESENT, "not_present" },
    { 0, 0 }
  };

  return ValueToString( map, v, "invalid hotswap state: %x" );
}


const char *
hpiInventChassisType2String( SaHpiInventChassisTypeT v )
{
  static cMap map[] =
  {
    { SAHPI_INVENT_CTYP_OTHER                , "other " },
    { SAHPI_INVENT_CTYP_UNKNOWN              , "unknown" },
    { SAHPI_INVENT_CTYP_DESKTOP              , "desktop" },
    { SAHPI_INVENT_CTYP_LOW_PROFILE_DESKTOP  , "low_profile_desktop" },
    { SAHPI_INVENT_CTYP_PIZZA_BOX            , "pizza_box" },
    { SAHPI_INVENT_CTYP_MINI_TOWER           , "mini_tower" },
    { SAHPI_INVENT_CTYP_TOWER                , "tower" },
    { SAHPI_INVENT_CTYP_PORTABLE             , "portable" },
    { SAHPI_INVENT_CTYP_LAPTOP               , "laptop" },
    { SAHPI_INVENT_CTYP_NOTEBOOK             , "notebook" },
    { SAHPI_INVENT_CTYP_HANDHELD             , "handheld" },
    { SAHPI_INVENT_CTYP_DOCKING_STATION      , "docking_station" },
    { SAHPI_INVENT_CTYP_ALLINONE             , "allinone" },
    { SAHPI_INVENT_CTYP_SUBNOTEBOOK          , "subnotebook" },
    { SAHPI_INVENT_CTYP_SPACE_SAVING         , "space_saving" },
    { SAHPI_INVENT_CTYP_LUNCH_BOX            , "lunch_box" },
    { SAHPI_INVENT_CTYP_MAIN_SERVER          , "main_server" },
    { SAHPI_INVENT_CTYP_EXPANSION            , "expansion" },
    { SAHPI_INVENT_CTYP_SUBCHASSIS           , "subchassis" },
    { SAHPI_INVENT_CTYP_BUS_EXPANSION_CHASSIS, "bus_expansion_chassis" },
    { SAHPI_INVENT_CTYP_PERIPHERAL_CHASSIS   , "peripheral_chassis" },
    { SAHPI_INVENT_CTYP_RAID_CHASSIS         , "raid_chassis" },
    { SAHPI_INVENT_CTYP_RACKMOUNT            , "rackmount" },
    { 0, 0 }
  };

  return ValueToString( map, v, "invalid chassis type: %x" );
}


const char *
hpiCtrlOutputType2String( SaHpiCtrlOutputTypeT type )
{
  static cMap map[] =
  {
    { SAHPI_CTRL_GENERIC             , "generic" },
    { SAHPI_CTRL_LED                 , "led" },
    { SAHPI_CTRL_FAN_SPEED           , "fan speed" },
    { SAHPI_CTRL_DRY_CONTACT_CLOSURE , "dry contact closure" },
    { SAHPI_CTRL_POWER_SUPPLY_INHIBIT, "power supply inhibit" },
    { SAHPI_CTRL_AUDIBLE             , "audible" },
    { SAHPI_CTRL_FRONT_PANEL_LOCKOUT , "front panel lockout" },
    { SAHPI_CTRL_POWER_INTERLOCK     , "power interlock" },
    { SAHPI_CTRL_POWER_STATE         , "power state " },
    { SAHPI_CTRL_LCD_DISPLAY         , "lcd display" },
    { SAHPI_CTRL_OEM                 , "oem" },
    { 0, 0 }
  };

  return ValueToString( map, type, "invalid control output type: %x" );
}


const char *
hpiCtrlType2String( SaHpiCtrlTypeT type )
{
  static cMap map[] =
  {
    { SAHPI_CTRL_TYPE_DIGITAL , "digital" },
    { SAHPI_CTRL_TYPE_DISCRETE, "discrete" },
    { SAHPI_CTRL_TYPE_ANALOG  , "analog" },
    { SAHPI_CTRL_TYPE_STREAM  , "stream" },
    { SAHPI_CTRL_TYPE_TEXT    , "text" },
    { SAHPI_CTRL_TYPE_OEM     , "oem" },
    { 0, 0 }
  };

  return ValueToString( map, type, "invalid control type: %x" );
}


const char *
hpiCtrlStateDigital2String( SaHpiCtrlStateDigitalT dig )
{
  static cMap map[] =
  {
    { SAHPI_CTRL_STATE_OFF      , "off" },
    { SAHPI_CTRL_STATE_ON       , "on" },
    { SAHPI_CTRL_STATE_PULSE_OFF, "pulse off" },
    { SAHPI_CTRL_STATE_PULSE_ON , "pulse on" },
    { SAHPI_CTRL_STATE_AUTO     , "auto" },
    { 0, 0 }
  };

  return ValueToString( map, dig, "invalid control digital: %x" );
}
