#include <time.h>
#include <sys/time.h>
#include "voh_string.h"


typedef struct cMapS
{
  unsigned int m_value;
  const char  *m_name;
} cMap;

static const char *ValueToString(cMap *map, unsigned int value, const char *def)
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


static const char *hpiBitMask2String( cMap *map, unsigned int value )
{
  static char str[1024];

  str[0] = 0;
  int mask = 0;

  while( map->m_name )
     {
       if ( (map->m_value & value) == map->m_value )
          {
            if ( str[0] != 0 )
                 strcat( str, "\n" );

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

  if (strlen(str) < 1)
	strcat(str, "none");

  return str;
}

const char *vohBoolean2String(gboolean b)
{
      if (b == TRUE)
	    return "yes";
      else
	    return "no";
}

const char *vohTime2String(SaHpiTimeT time)
{
      static char	tstr[1024];
      int count;
      struct tm t;
      time_t tt;

      if (time > SAHPI_TIME_MAX_RELATIVE) { /*absolute time*/
	    tt = time / 1000000000;
	    count = strftime(tstr, 1024, "%F %T", localtime(&tt));
      } else if (time ==  SAHPI_TIME_UNSPECIFIED) {
	    sprintf(tstr, "unknown");
	    count = sizeof("unknown");
      } else if (time > SAHPI_TIME_UNSPECIFIED) { /*invalid time*/
	    sprintf(tstr,"invalid time");
	    count = sizeof("Invalid time");
      } else {   /*relative time*/
	    tt = time / 1000000000;
	    localtime_r(&tt, &t);
	    /* count = strftime(str, size, "%b %d, %Y - %H:%M:%S", &t); */
	    count = strftime(tstr, 1024, "%c", &t);
      }

      if (count == 0)
	    return "invalid time";

	return tstr;
}

const char *vohError2String(SaErrorT err)
{
      static cMap error_map[] = {
		{SA_OK,				"ok"},
		{SA_HPI_ERR_BASE,		"base"},
		{SA_ERR_HPI_ERROR,		"error"},
		{SA_ERR_HPI_UNSUPPORTED_API,	"unsupported_api"},
		{SA_ERR_HPI_BUSY,		"busy"},
		{SA_ERR_HPI_INTERNAL_ERROR,	"internal_error"},
		{SA_ERR_HPI_INVALID_CMD,	"invalid_cmd"},
		{SA_ERR_HPI_TIMEOUT,		"timeout"},
		{SA_ERR_HPI_OUT_OF_SPACE,	"out_of_space"},
		{SA_ERR_HPI_OUT_OF_MEMORY,	"out_of_memory"},
		{SA_ERR_HPI_INVALID_PARAMS,	"invalid_params"},
		{SA_ERR_HPI_INVALID_DATA,	"invalid_data"},
		{SA_ERR_HPI_NOT_PRESENT,	"not_present"},
		{SA_ERR_HPI_NO_RESPONSE,	"no_response"},
		{SA_ERR_HPI_DUPLICATE,		"duplicate"},
		{SA_ERR_HPI_INVALID_SESSION,	"invalid_session"},
		{SA_ERR_HPI_INVALID_DOMAIN,	"invalid_domain"},
		{SA_ERR_HPI_INVALID_RESOURCE,	"invalid_resource"},
		{SA_ERR_HPI_INVALID_REQUEST,	"invalid_request"},
		{SA_ERR_HPI_ENTITY_NOT_PRESENT,	"entity_not_present"},
		{SA_ERR_HPI_READ_ONLY,		"read_only"},
		{SA_ERR_HPI_CAPABILITY,		"capability"},
		{SA_ERR_HPI_UNKNOWN,		"unknown"},
		{ 0 , 0 }
      };

      return ValueToString( error_map, err, "invalid error: %x" );
}

const char *
vohEntityType2String(SaHpiEntityTypeT type)
{
      static cMap ent_map[] = {
		{SAHPI_ENT_UNSPECIFIED,		"UNSPECIFIED"},
		{SAHPI_ENT_OTHER,		"OTHER"},
		{SAHPI_ENT_UNKNOWN,		"UNKNOWN"},
		{SAHPI_ENT_PROCESSOR,		"PROCESSOR"},
		{SAHPI_ENT_DISK_BAY,		"DISC_BAY"},
		{SAHPI_ENT_PERIPHERAL_BAY,	"PERIPHERAL_BAY"},
		{SAHPI_ENT_SYS_MGMNT_MODULE,	"MGMNT_MODULE"},
		{SAHPI_ENT_SYSTEM_BOARD,	"SYSTEM_BOARD"},
		{SAHPI_ENT_MEMORY_MODULE,	"MEMORY_MODULE"},
		{SAHPI_ENT_PROCESSOR_MODULE,	"PROCESSOR_MODULE"},
		{SAHPI_ENT_POWER_SUPPLY,	"POWER_SUPPLY"},
		{SAHPI_ENT_ADD_IN_CARD,		"ADD_IN_CARD"},
		{SAHPI_ENT_FRONT_PANEL_BOARD,	"FRONT_PANEL_BOARD"},
		{SAHPI_ENT_BACK_PANEL_BOARD,	"BACK_PANEL_BOARD"},
		{SAHPI_ENT_POWER_SYSTEM_BOARD,	"POWER_SYSTEM_BOARD"},
		{SAHPI_ENT_DRIVE_BACKPLANE,	"DRIVE_BACKPLANE"},
		{SAHPI_ENT_SYS_EXPANSION_BOARD,	"SYS_EXPANSION_BOARD"},
		{SAHPI_ENT_OTHER_SYSTEM_BOARD,	"OTHER_SYSTEM_BOARD"},
		{SAHPI_ENT_PROCESSOR_BOARD,	"PROCESSOR_BOARD"},
		{SAHPI_ENT_POWER_UNIT,		"POWER_UNIT"},
		{SAHPI_ENT_POWER_MODULE,	"POWER_MODULE"},
		{SAHPI_ENT_POWER_MGMNT,		"POWER_MGMNT"},
		{SAHPI_ENT_CHASSIS_BACK_PANEL_BOARD,
		    				"CHASSIS_BACK_PANEL_BOARD"},
		{SAHPI_ENT_SYSTEM_CHASSIS,	"SYSTEM_CHASSIS"},
		{SAHPI_ENT_SUB_CHASSIS,		"SUB_CHASSIS"},
		{SAHPI_ENT_OTHER_CHASSIS_BOARD,	"OTHER_CHASSIS_BOARD"},
		{SAHPI_ENT_DISK_DRIVE_BAY,	"DISC_DRIVE_BAY"},
		{SAHPI_ENT_PERIPHERAL_BAY_2,	"PERIPHERAL_BAY_2"},
		{SAHPI_ENT_DEVICE_BAY,		"DEVICE_BAY"},
		{SAHPI_ENT_COOLING_DEVICE,	"COOLING_DEVICE"},
		{SAHPI_ENT_COOLING_UNIT,	"COOLING_UNIT"},
		{SAHPI_ENT_INTERCONNECT,	"INTERCONNECT"},
		{SAHPI_ENT_MEMORY_DEVICE,	"MEMORY_DEVICE"},
		{SAHPI_ENT_SYS_MGMNT_SOFTWARE,	"SYS_MGMNT_SOFTWARE"},
		{SAHPI_ENT_BIOS,		"BIOS"},
		{SAHPI_ENT_OPERATING_SYSTEM,	"OPERATING_SYSTEM"},
		{SAHPI_ENT_SYSTEM_BUS,		"SYSTEM_BUS"},
		{SAHPI_ENT_GROUP,		"GROUP"},
		{SAHPI_ENT_REMOTE,		"REMOTE"},
		{SAHPI_ENT_EXTERNAL_ENVIRONMENT,"EXTERNAL_ENVIRONMENT"},
		{SAHPI_ENT_BATTERY,		"BATTERY"},
		{SAHPI_ENT_CHASSIS_SPECIFIC,	"CHASSIS_SPECIFIC"},
		{SAHPI_ENT_BOARD_SET_SPECIFIC,	"BOARD_SET_SPECIFIC"},
		{SAHPI_ENT_OEM_SYSINT_SPECIFIC,	"OEM_SYSINT_SPECIFIC"},
		{SAHPI_ENT_ROOT,		"ROOT"},
		{SAHPI_ENT_RACK,		"RACK"},
		{SAHPI_ENT_SUBRACK,		"SUBRACK"},
		{SAHPI_ENT_COMPACTPCI_CHASSIS,	"COMPACTPCI_CHASSIS"},
		{SAHPI_ENT_ADVANCEDTCA_CHASSIS,	"ADVANCEDTCA_CHASSIS"},
		{SAHPI_ENT_RACK_MOUNTED_SERVER,	"RACK_MOUNTED_SERVER"},
		{SAHPI_ENT_SYSTEM_BLADE,	"SYSTEM_BLADE"},
		{SAHPI_ENT_SWITCH,		"SWITCH"},
		{SAHPI_ENT_SWITCH_BLADE,	"SWITCH_BLADE"},
		{SAHPI_ENT_SBC_BLADE,		"SBC_BLADE"},
		{SAHPI_ENT_IO_BLADE,		"IO_BLADE"},
		{SAHPI_ENT_DISK_BLADE,		"DISC_BLADE"},
		{SAHPI_ENT_DISK_DRIVE,		"DISC_DRIVE"},
		{SAHPI_ENT_FAN,			"FAN"},
		{SAHPI_ENT_POWER_DISTRIBUTION_UNIT,
		    				"POWER_DISTRIBUTION_UNIT"},
		{SAHPI_ENT_SPEC_PROC_BLADE,	"SPEC_PROC_BLADE"},
		{SAHPI_ENT_IO_SUBBOARD,		"IO_SUBBOARD"},
		{SAHPI_ENT_SBC_SUBBOARD,	"SBC_SUBBOARD"},
		{SAHPI_ENT_ALARM_MANAGER,	"ALARM_MANAGER"},
		{SAHPI_ENT_SHELF_MANAGER,	"SHELF_MANAGER"},
		{SAHPI_ENT_DISPLAY_PANEL,	"DISPLAY_PANEL"},
		{SAHPI_ENT_SUBBOARD_CARRIER_BLADE,
		    				"SUBBOARD_CARRIER_BLADE"},
		{SAHPI_ENT_PHYSICAL_SLOT,	"PHYSICAL_SLOT"},
		{ 0, 0 }
      };

      return ValueToString(ent_map, type, "%d");
}

void vohEntityPath2String(SaHpiEntityT *entity, char *str)
{

      sprintf(str, "%s #%d", vohEntityType2String(entity->EntityType),
	      		      entity->EntityLocation);
}

void vohFullEntityPath2String(SaHpiEntityPathT *path, char *str)
{
      int		i;
      gchar		*name, type[100], loc[10];

       for (i = 0; i < SAHPI_MAX_ENTITY_PATH; i++) {
	    if (path->Entry[i].EntityType == SAHPI_ENT_ROOT) {
		  break;
	    }
      }
      for (i--; i >= 0; i--) {
	    sprintf(type, "%s",
		    vohEntityType2String(path->Entry[i].EntityType));
	    sprintf(loc, "%d", path->Entry[i].EntityLocation);
	    name = g_strconcat("{", type," ", loc, "}", NULL);
	    memcpy(str, name, strlen(name));
	    str = str + strlen(name);
	    *str = 0;
      }
}

const char *vohRdrType2String(SaHpiRdrTypeT type)
{
      static cMap rdr_type_map[] = {
		{SAHPI_NO_RECORD,		"no_record"},
		{SAHPI_CTRL_RDR,		"control"},
		{SAHPI_SENSOR_RDR,		"sensor"},
		{SAHPI_INVENTORY_RDR,		"inventory"},
		{SAHPI_WATCHDOG_RDR,		"watchdog"},
		{SAHPI_ANNUNCIATOR_RDR,		"annunciator"},
		{0, 0}
      };
      return ValueToString(rdr_type_map, type, "%d");
}

const char *vohSensorType2String(SaHpiSensorTypeT type)
{
      static cMap s_type_map[] = {
		{SAHPI_TEMPERATURE,		"temperature"},
		{SAHPI_VOLTAGE,			"voltage"},
		{SAHPI_CURRENT,			"current"},
		{SAHPI_FAN,			"fan"},
		{SAHPI_PHYSICAL_SECURITY,	"physical_security"},
		{SAHPI_PLATFORM_VIOLATION,	"platform_violation"},
		{SAHPI_PROCESSOR,		"processor"},
		{SAHPI_POWER_SUPPLY,		"power_supply"},
		{SAHPI_POWER_UNIT,		"power_unit"},
		{SAHPI_COOLING_DEVICE,		"cooling_device"},
		{SAHPI_OTHER_UNITS_BASED_SENSOR,"other_unit_based_sensor"},
		{SAHPI_MEMORY,			"memory"},
		{SAHPI_DRIVE_SLOT,		"drive_slot"},
		{SAHPI_POST_MEMORY_RESIZE,	"post_memory_resize"},
		{SAHPI_SYSTEM_FW_PROGRESS,	"system_fw_progress"},
		{SAHPI_EVENT_LOGGING_DISABLED,	"event_logging_desabled"},
		{SAHPI_RESERVED1,		"reserved1"},
		{SAHPI_SYSTEM_EVENT,		"system_event"},
		{SAHPI_CRITICAL_INTERRUPT,	"critical_interrupt"},
		{SAHPI_BUTTON,			"button"},
		{SAHPI_MODULE_BOARD,		"module_board"},
		{SAHPI_MICROCONTROLLER_COPROCESSOR,
		    				"microcontroller_coprocessor"},
		{SAHPI_ADDIN_CARD,		"addin_card"},
		{SAHPI_CHASSIS,			"chassis"},
		{SAHPI_CHIP_SET,		"chip_set"},
		{SAHPI_OTHER_FRU,		"FRU"},
		{SAHPI_CABLE_INTERCONNECT,	"cable_interconnect"},
		{SAHPI_TERMINATOR,		"terminator"},
		{SAHPI_SYSTEM_BOOT_INITIATED,	"boot_initiated"},
		{SAHPI_BOOT_ERROR,		"boot_error"},
		{SAHPI_OS_BOOT,			"os_boot"},
		{SAHPI_OS_CRITICAL_STOP,	"os_critical_stop"},
		{SAHPI_SLOT_CONNECTOR,		"slot_connector"},
		{SAHPI_SYSTEM_ACPI_POWER_STATE,	"system_acpi_power_state"},
		{SAHPI_RESERVED2,		"reserved2"},
		{SAHPI_PLATFORM_ALERT,		"platform_alert"},
		{SAHPI_ENTITY_PRESENCE,		"entity_presence"},
		{SAHPI_MONITOR_ASIC_IC,		"monitor_asic_ic"},
		{SAHPI_LAN,			"LAN"},
		{SAHPI_MANAGEMENT_SUBSYSTEM_HEALTH,
		    				"management_subsystem_health"},
		{SAHPI_BATTERY,			"battery"},
		{SAHPI_OPERATIONAL,		"operational"},
		{SAHPI_OEM_SENSOR,		"oem_sensor"},
		{0, 0}
      };
      return ValueToString(s_type_map, type, "%d");
}

const char *vohCapabilities2String(SaHpiCapabilitiesT cf)
{
      static cMap cap_map[] = {
	{SAHPI_CAPABILITY_RESOURCE,		"resource"},
	{SAHPI_CAPABILITY_EVT_DEASSERTS,	"deasserts"},
	{SAHPI_CAPABILITY_AGGREGATE_STATUS,	"aggregate_status"},
	{SAHPI_CAPABILITY_CONFIGURATION,	"configuration"},
	{SAHPI_CAPABILITY_MANAGED_HOTSWAP,	"managed_hotswap"},
	{SAHPI_CAPABILITY_WATCHDOG,		"watchdog"},
	{SAHPI_CAPABILITY_CONTROL,		"control"},
	{SAHPI_CAPABILITY_FRU,			"FRU"},
	{SAHPI_CAPABILITY_ANNUNCIATOR,		"annunciator"},
	{SAHPI_CAPABILITY_POWER,		"power"},
	{SAHPI_CAPABILITY_RESET,		"reset"},
	{SAHPI_CAPABILITY_INVENTORY_DATA,	"inventory_data"},
	{SAHPI_CAPABILITY_EVENT_LOG,		"event_log"},
	{SAHPI_CAPABILITY_RDR,			"RDR"},
	{SAHPI_CAPABILITY_SENSOR,		"sensor"},
	{ 0, 0 }
      };

      return hpiBitMask2String(cap_map, cf);
}

const char *vohHsCapabilities2String(SaHpiHsCapabilitiesT cf)
{
      static cMap cap_map[] = {
	{SAHPI_HS_CAPABILITY_AUTOEXTRACT_READ_ONLY,
	    					"autoextract_read_only"},
	{SAHPI_HS_CAPABILITY_INDICATOR_SUPPORTED,
	    					"indicator_supported"},
	{ 0, 0 }
      };

      return hpiBitMask2String(cap_map, cf);
}

const char *vohSeverity2String(SaHpiSeverityT severity)
{
      static cMap severity_map[] = {
		{SAHPI_CRITICAL,		"critical"},
		{SAHPI_MAJOR,			"major"},
		{SAHPI_MINOR,			"minor"},
		{SAHPI_INFORMATIONAL,		"informational"},
		{SAHPI_OK,			"ok"},
		{SAHPI_DEBUG,			"debug"},
		{SAHPI_ALL_SEVERITIES,		"all severities"},
		{0, 0}
      };

      return ValueToString(severity_map, severity, "%d");

}

const char *vohEventCategory2String(SaHpiEventCategoryT category)
{
      static cMap category_map[] = {
		{SAHPI_EC_UNSPECIFIED,		"unspecified"},
		{SAHPI_EC_THRESHOLD,		"threshold"},
		{SAHPI_EC_USAGE,		"usage_state"},
		{SAHPI_EC_STATE,		"generic_state"},
		{SAHPI_EC_PRED_FAIL,		"predictive_fail"},
		{SAHPI_EC_LIMIT,		"limit"},
		{SAHPI_EC_PERFORMANCE,		"performance "},
		{SAHPI_EC_SEVERITY,		"severity_indicating"},
		{SAHPI_EC_PRESENCE,		"device_presence"},
		{SAHPI_EC_ENABLE,		"device enabled"},
		{SAHPI_EC_AVAILABILITY,		"availability_state"},
		{SAHPI_EC_REDUNDANCY,		"redundancy_state"},
		{SAHPI_EC_SENSOR_SPECIFIC,	"sensor-specific"},
		{SAHPI_EC_GENERIC,		"OEM_defined"},
		{0, 0}
      };
      return ValueToString(category_map, category, "%d");
}

const char *vohSensorEventCtrl2String(SaHpiSensorEventCtrlT ctrl)
{
      static cMap ctrl_map[] = {
		{SAHPI_SEC_PER_EVENT,
		    "event message control per event"},
		{SAHPI_SEC_READ_ONLY_MASKS,
		    "event control for entire sensor only"},
		{SAHPI_SEC_READ_ONLY,
		    "event control not supported"},
		{0, 0}
      };
      return ValueToString(ctrl_map, ctrl, "%d");
}

const char *vohEventState2String(SaHpiEventStateT es,
				 SaHpiEventCategoryT category)
{
      static cMap threshold_map[] = {
		{SAHPI_ES_LOWER_MINOR,		"lower_minor"},
		{SAHPI_ES_LOWER_MAJOR,		"lower_major"},
		{SAHPI_ES_LOWER_CRIT,		"lower_crit"},
		{SAHPI_ES_UPPER_MINOR,		"upper_minor"},
		{SAHPI_ES_UPPER_MAJOR,		"upper_major"},
		{SAHPI_ES_UPPER_CRIT,		"upper_crit"},
		{0, 0}
      };
      static cMap usage_map[] = {
		{SAHPI_ES_IDLE,			"idle"},
		{SAHPI_ES_ACTIVE,		"active"},
		{SAHPI_ES_BUSY,			"busy"},
		{0, 0}
      };
      static cMap state_map[] = {
		{SAHPI_ES_STATE_DEASSERTED,	"state_deasserted"},
		{SAHPI_ES_STATE_ASSERTED,	"state_asserted"},
		{0, 0}
      };
      static cMap pred_fail_map[] = {
		{SAHPI_ES_PRED_FAILURE_DEASSERT,"pred_failure_deassert"},
		{SAHPI_ES_PRED_FAILURE_ASSERT,	"pred_failure_assert"},
		{0, 0}
      };
      static cMap limit_map[] = {
		{SAHPI_ES_LIMIT_NOT_EXCEEDED,	"limit_not_exceeded"},
		{SAHPI_ES_LIMIT_EXCEEDED,	"limit_exceeded"},
		{0, 0}
      };
      static cMap performance_map[] = {
		{SAHPI_ES_PERFORMANCE_MET,	"performance_met"},
		{SAHPI_ES_PERFORMANCE_LAGS,	"performance_lags"},
		{0, 0}
      };
      static cMap severity_map[] = {
		{SAHPI_ES_OK,			"OK"},
		{SAHPI_ES_MINOR_FROM_OK,	"minor_from_OK"},
		{SAHPI_ES_MAJOR_FROM_LESS,	"minor_from_less"},
		{SAHPI_ES_CRITICAL_FROM_LESS,	"critical_from_less"},
		{SAHPI_ES_MINOR_FROM_MORE,	"minor_from_more"},
		{SAHPI_ES_MAJOR_FROM_CRITICAL,	"major_from_critical"},
		{SAHPI_ES_CRITICAL,		"critical"},
		{SAHPI_ES_MONITOR,		"monitor"},
		{SAHPI_ES_INFORMATIONAL,	"informational"},
		{0, 0}
      };
      static cMap presence_map[] = {
		{SAHPI_ES_DISABLED,		"disabled"},
		{SAHPI_ES_ENABLED,		"enabled"},
		{0, 0}
      };
      static cMap availability_map[] = {
		{SAHPI_ES_RUNNING,		"running"},
		{SAHPI_ES_TEST,			"test"},
		{SAHPI_ES_POWER_OFF,		"power_off"},
		{SAHPI_ES_ON_LINE,		"on_line"},
		{SAHPI_ES_OFF_LINE,		"off_line"},
		{SAHPI_ES_OFF_DUTY,		"off_duty"},
		{SAHPI_ES_DEGRADED,		"degraded"},
		{SAHPI_ES_POWER_SAVE,		"power_save"},
		{SAHPI_ES_INSTALL_ERROR,	"install_error"},
		{0, 0}
      };
      static cMap redundancy_map[] = {
		{SAHPI_ES_FULLY_REDUNDANT,	"fully_redundant"},
		{SAHPI_ES_REDUNDANCY_LOST,	"redundancy_lost"},
		{SAHPI_ES_REDUNDANCY_DEGRADED,	"redundancy_degraded"},
		{SAHPI_ES_REDUNDANCY_LOST_SUFFICIENT_RESOURCES,
		    "redundancy_lost_sufficient_resources"},
		{SAHPI_ES_NON_REDUNDANT_SUFFICIENT_RESOURCES,
		    "non_redundant_sufficient_resources"},
		{SAHPI_ES_NON_REDUNDANT_INSUFFICIENT_RESOURCES,
		    "non_redundant_insufficient_resources"},
		{SAHPI_ES_REDUNDANCY_DEGRADED_FROM_FULL,
		    "redundancy_degraded_from_full"},
		{SAHPI_ES_REDUNDANCY_DEGRADED_FROM_NON,
		    "redundancy_degraded_from_non"},
		{0, 0}
      };
      static cMap generic_sensor_spec_map[] = {
		{SAHPI_ES_STATE_00,		"state_00"},
		{SAHPI_ES_STATE_01,		"state_01"},
		{SAHPI_ES_STATE_02,		"state_02"},
		{SAHPI_ES_STATE_03,		"state_03"},
		{SAHPI_ES_STATE_04,		"state_04"},
		{SAHPI_ES_STATE_05,		"state_05"},
		{SAHPI_ES_STATE_06,		"state_06"},
		{SAHPI_ES_STATE_07,		"state_07"},
		{SAHPI_ES_STATE_08,		"state_08"},
		{SAHPI_ES_STATE_09,		"state_09"},
		{SAHPI_ES_STATE_10,		"state_10"},
		{SAHPI_ES_STATE_11,		"state_11"},
		{SAHPI_ES_STATE_12,		"state_12"},
		{SAHPI_ES_STATE_13,		"state_13"},
		{SAHPI_ES_STATE_14,		"state_14"},
		{0, 0}
      };

      switch (category) {
	case SAHPI_EC_THRESHOLD:
	    return hpiBitMask2String(threshold_map, es);

	case SAHPI_EC_USAGE:
	    return hpiBitMask2String(usage_map, es);

	case SAHPI_EC_STATE:
	    return hpiBitMask2String(state_map, es);

	case SAHPI_EC_PRED_FAIL:
	    return hpiBitMask2String(pred_fail_map, es);

	case SAHPI_EC_LIMIT:
	    return hpiBitMask2String(limit_map, es);

	case SAHPI_EC_PERFORMANCE: 
	    return hpiBitMask2String(performance_map, es);

	case SAHPI_EC_SEVERITY:
	    return hpiBitMask2String(severity_map, es);

	case SAHPI_EC_PRESENCE:
	    return hpiBitMask2String(presence_map, es);

	case SAHPI_EC_AVAILABILITY:
	    return hpiBitMask2String(availability_map, es);

	case SAHPI_EC_REDUNDANCY:
	    return hpiBitMask2String(redundancy_map, es);

	case SAHPI_EC_GENERIC:
	case SAHPI_EC_SENSOR_SPECIFIC:
	    return hpiBitMask2String(generic_sensor_spec_map, es);

	default:
	    return "unspecified";
      }
}

const char *vohSensorUnits2String(SaHpiSensorUnitsT unit)
{
      static cMap units_map[] = {
		{SAHPI_SU_UNSPECIFIED,	"unspecified unit"},
		{SAHPI_SU_DEGREES_C,	"C (degree Celsius)"},
		{SAHPI_SU_DEGREES_F,	"F (degree Fahrenheit)"},
		{SAHPI_SU_DEGREES_K,	"K (Kelvin degree)"},
		{SAHPI_SU_VOLTS,	"V (volt)"},
		{SAHPI_SU_AMPS,		"A (ampere)"},
		{SAHPI_SU_WATTS,	"W (watt)"},
		{SAHPI_SU_JOULES,	"J (joule)"},
		{SAHPI_SU_COULOMBS,	"C (coulomb)"},
		{SAHPI_SU_VA,		"VA (volt ampere)"},
		{SAHPI_SU_NITS,		"nt (nit)"},
		{SAHPI_SU_LUMEN,	"lm (lumen)"},
		{SAHPI_SU_LUX,		"lx (lux)"},
		{SAHPI_SU_CANDELA,	"cd (candela)"},
		{SAHPI_SU_KPA,		"kPa (kilopaskal)"},
		{SAHPI_SU_PSI,		"psi (pound per square inch)"},
		{SAHPI_SU_NEWTON,	"N (newton)"},
		{SAHPI_SU_CFM,		"cfm (cubic feet per minute)"},
		{SAHPI_SU_RPM,		"r/min (revolution per minute)"},
		{SAHPI_SU_HZ,		"Hz (hertz)"},
		{SAHPI_SU_MICROSECOND,	"microsecond"},
		{SAHPI_SU_MILLISECOND,	"msec (millisecond)"},
		{SAHPI_SU_SECOND,	"sec (second)"},
		{SAHPI_SU_MINUTE,	"min (minute)"},
		{SAHPI_SU_HOUR,		"hr (hour)"},
		{SAHPI_SU_DAY,		"da (day)"},
		{SAHPI_SU_WEEK,		"wk (week)"},
		{SAHPI_SU_MIL,		"mil (milli-inch)"},
		{SAHPI_SU_INCHES,	"in (inch)"},
		{SAHPI_SU_FEET,		"ft (feet)"},
		{SAHPI_SU_CU_IN,	"in^3 (cubic inch)"},
		{SAHPI_SU_CU_FEET,	"ft^3 (cubic feet)"},
		{SAHPI_SU_MM,		"mm (millimeter)"},
		{SAHPI_SU_CM,		"cm (centimeter)"},
		{SAHPI_SU_M,		"m (meter)"},
		{SAHPI_SU_CU_CM,	"cm^3 (cubic centimeter)"},
		{SAHPI_SU_CU_M,		"m^3 (cubic meter)"},
		{SAHPI_SU_LITERS,	"L (liter)"},
		{SAHPI_SU_FLUID_OUNCE,	"fl oz (fluid ounce)"},
		{SAHPI_SU_RADIANS,	"rad (radian)"},
		{SAHPI_SU_STERADIANS,	"sr (steradian)"},
		{SAHPI_SU_REVOLUTIONS,	"rev (revolution)"},
		{SAHPI_SU_CYCLES,	"c (cycle)"},
		{SAHPI_SU_GRAVITIES,	"g (gravity acceleration)"},
		{SAHPI_SU_OUNCE,	"oz (ounce)"},
		{SAHPI_SU_POUND,	"lb (pound)"},
		{SAHPI_SU_FT_LB,	"ft-lb (foot pound)"},
		{SAHPI_SU_OZ_IN, 	"oz in (ounce per inch)"},
		{SAHPI_SU_GAUSS,	"Gs (gauss)"},
		{SAHPI_SU_GILBERTS,	"Gi (gilbert)"},
		{SAHPI_SU_HENRY,	"H (henry)"},
		{SAHPI_SU_MILLIHENRY,	"mH (millihenry)"},
		{SAHPI_SU_FARAD,	"F (farad)"},
		{SAHPI_SU_MICROFARAD,	"microF (microfarad)"},
		{SAHPI_SU_OHMS,		"ohm"},
		{SAHPI_SU_SIEMENS,	"S (siemens)"},
		{SAHPI_SU_MOLE,		"mol (mole)"},
		{SAHPI_SU_BECQUEREL,	"Bq (becquerel)"},
		{SAHPI_SU_PPM,		"ppm (part per million)"},
		{SAHPI_SU_RESERVED,	"reserved unit"},
		{SAHPI_SU_DECIBELS,	"dB (decibel)"},
		{SAHPI_SU_DBA,		"db A (unit of sound intensity)"},
		{SAHPI_SU_DBC,		"dB c (unit of signal strength)"},
		{SAHPI_SU_GRAY,		"Gy (gray)"},
		{SAHPI_SU_SIEVERT,	"Sv (sievert)"},
		{SAHPI_SU_COLOR_TEMP_DEG_K, "CCT (color temperature)"},
		{SAHPI_SU_BIT,		"b (bit)"},
		{SAHPI_SU_KILOBIT,	"kbit (kilobit)"},
		{SAHPI_SU_MEGABIT,	"megabit"},
		{SAHPI_SU_GIGABIT,	"gigabit"},
		{SAHPI_SU_BYTE,		"B (byte)"},
		{SAHPI_SU_KILOBYTE,	"kB (kilobyte)"},
		{SAHPI_SU_MEGABYTE,	"MB (megabyte)"},
		{SAHPI_SU_GIGABYTE,	"GB (gigabyte)"},
		{SAHPI_SU_WORD,		"word"},
		{SAHPI_SU_DWORD,	"double word"},
		{SAHPI_SU_QWORD,	"quadword"},
		{SAHPI_SU_LINE,		"line"},
		{SAHPI_SU_HIT,		"hit"},
		{SAHPI_SU_MISS,		"miss"},
		{SAHPI_SU_RETRY,	"retry"},
		{SAHPI_SU_RESET,	"reset"},
		{SAHPI_SU_OVERRUN,	"overrun"},
		{SAHPI_SU_UNDERRUN,	"underrun"},
		{SAHPI_SU_COLLISION,	"collision"},
		{SAHPI_SU_PACKETS,	"packet(s)"},
		{SAHPI_SU_MESSAGES,	"message(s)"},
		{SAHPI_SU_CHARACTERS,	"character(s)"},
		{SAHPI_SU_ERRORS,	"error(s)"},
		{SAHPI_SU_CORRECTABLE_ERRORS, "correctable error(s)"},
		{SAHPI_SU_UNCORRECTABLE_ERRORS, "uncorrectable error(S)"},
		{0, 0}
      };
      return ValueToString(units_map, unit, "%d");
}

const char *vohSensorUnits2Short(SaHpiSensorUnitsT unit)
{
      static cMap units_map[] = {
		{SAHPI_SU_UNSPECIFIED,	""},
		{SAHPI_SU_DEGREES_C,	"C"},
		{SAHPI_SU_DEGREES_F,	"F"},
		{SAHPI_SU_DEGREES_K,	"K"},
		{SAHPI_SU_VOLTS,	"V"},
		{SAHPI_SU_AMPS,		"A"},
		{SAHPI_SU_WATTS,	"W"},
		{SAHPI_SU_JOULES,	"J"},
		{SAHPI_SU_COULOMBS,	"C"},
		{SAHPI_SU_VA,		"VA"},
		{SAHPI_SU_NITS,		"nt"},
		{SAHPI_SU_LUMEN,	"lm"},
		{SAHPI_SU_LUX,		"lx"},
		{SAHPI_SU_CANDELA,	"cd"},
		{SAHPI_SU_KPA,		"kPa"},
		{SAHPI_SU_PSI,		"psi"},
		{SAHPI_SU_NEWTON,	"N"},
		{SAHPI_SU_CFM,		"cfm"},
		{SAHPI_SU_RPM,		"r/min"},
		{SAHPI_SU_HZ,		"Hz"},
		{SAHPI_SU_MICROSECOND,	"microsecond"},
		{SAHPI_SU_MILLISECOND,	"msec"},
		{SAHPI_SU_SECOND,	"sec"},
		{SAHPI_SU_MINUTE,	"min"},
		{SAHPI_SU_HOUR,		"hr"},
		{SAHPI_SU_DAY,		"da"},
		{SAHPI_SU_WEEK,		"wk"},
		{SAHPI_SU_MIL,		"mil"},
		{SAHPI_SU_INCHES,	"in"},
		{SAHPI_SU_FEET,		"ft"},
		{SAHPI_SU_CU_IN,	"in^3"},
		{SAHPI_SU_CU_FEET,	"ft^3"},
		{SAHPI_SU_MM,		"mm"},
		{SAHPI_SU_CM,		"cm"},
		{SAHPI_SU_M,		"m"},
		{SAHPI_SU_CU_CM,	"cm^3"},
		{SAHPI_SU_CU_M,		"m^3"},
		{SAHPI_SU_LITERS,	"L"},
		{SAHPI_SU_FLUID_OUNCE,	"fl oz"},
		{SAHPI_SU_RADIANS,	"rad"},
		{SAHPI_SU_STERADIANS,	"sr"},
		{SAHPI_SU_REVOLUTIONS,	"rev"},
		{SAHPI_SU_CYCLES,	"c"},
		{SAHPI_SU_GRAVITIES,	"g"},
		{SAHPI_SU_OUNCE,	"oz"},
		{SAHPI_SU_POUND,	"lb"},
		{SAHPI_SU_FT_LB,	"ft-lb"},
		{SAHPI_SU_OZ_IN, 	"oz in"},
		{SAHPI_SU_GAUSS,	"Gs"},
		{SAHPI_SU_GILBERTS,	"Gi"},
		{SAHPI_SU_HENRY,	"H"},
		{SAHPI_SU_MILLIHENRY,	"mH"},
		{SAHPI_SU_FARAD,	"F"},
		{SAHPI_SU_MICROFARAD,	"microF"},
		{SAHPI_SU_OHMS,		"ohm"},
		{SAHPI_SU_SIEMENS,	"S"},
		{SAHPI_SU_MOLE,		"mol"},
		{SAHPI_SU_BECQUEREL,	"Bq"},
		{SAHPI_SU_PPM,		"ppm"},
		{SAHPI_SU_RESERVED,	"reserved unit"},
		{SAHPI_SU_DECIBELS,	"dB"},
		{SAHPI_SU_DBA,		"db A"},
		{SAHPI_SU_DBC,		"dB c"},
		{SAHPI_SU_GRAY,		"Gy"},
		{SAHPI_SU_SIEVERT,	"Sv"},
		{SAHPI_SU_COLOR_TEMP_DEG_K, "CCT"},
		{SAHPI_SU_BIT,		"bit"},
		{SAHPI_SU_KILOBIT,	"kbit"},
		{SAHPI_SU_MEGABIT,	"megabit"},
		{SAHPI_SU_GIGABIT,	"gigabit"},
		{SAHPI_SU_BYTE,		"B"},
		{SAHPI_SU_KILOBYTE,	"kB"},
		{SAHPI_SU_MEGABYTE,	"MB"},
		{SAHPI_SU_GIGABYTE,	"GB"},
		{SAHPI_SU_WORD,		"word"},
		{SAHPI_SU_DWORD,	"double word"},
		{SAHPI_SU_QWORD,	"quadword"},
		{SAHPI_SU_LINE,		"line"},
		{SAHPI_SU_HIT,		"hit"},
		{SAHPI_SU_MISS,		"miss"},
		{SAHPI_SU_RETRY,	"retry"},
		{SAHPI_SU_RESET,	"reset"},
		{SAHPI_SU_OVERRUN,	"overrun"},
		{SAHPI_SU_UNDERRUN,	"underrun"},
		{SAHPI_SU_COLLISION,	"collision"},
		{SAHPI_SU_PACKETS,	"packet(s)"},
		{SAHPI_SU_MESSAGES,	"message(s)"},
		{SAHPI_SU_CHARACTERS,	"character(s)"},
		{SAHPI_SU_ERRORS,	"error(s)"},
		{SAHPI_SU_CORRECTABLE_ERRORS, "correctable error(s)"},
		{SAHPI_SU_UNCORRECTABLE_ERRORS, "uncorrectable error(s)"},
		{0, 0}
      };
      return ValueToString(units_map, unit, "%d");
}

const char *vohSensorValue2FullString(SaHpiSensorRecT *sensor,
				     SaHpiSensorReadingT *sv)
{
      static char	value[100];
      char		baseu[100];

      if (sv->IsSupported == FALSE)
	    return "";

      if (sv->Type == SAHPI_SENSOR_READING_TYPE_BUFFER) {
	    sprintf(baseu, "");
      } else {
	    sprintf(baseu, "(%s)", vohSensorUnits2Short(
					sensor->DataFormat.BaseUnits));
      }

      sprintf(value, "%s %s", vohSensorValue2String(sv), baseu);

      return value;
}

const char *vohSensorThdMask2String(SaHpiSensorThdMaskT mask)
{
      static cMap mask_map[] = {
		{SAHPI_STM_LOW_MINOR,		"Low Minor"},
		{SAHPI_STM_LOW_MAJOR,		"Low Major"},
		{SAHPI_STM_LOW_CRIT,		"Low Critical"},
		{SAHPI_STM_UP_MINOR,		"Up Minor"},
		{SAHPI_STM_UP_MAJOR,		"Up Major"},
		{SAHPI_STM_UP_CRIT,		"Up Critical"},
		{SAHPI_STM_UP_HYSTERESIS,	"Up Hysteresis"},
		{SAHPI_STM_LOW_HYSTERESIS,	"Low Hysteresis"},
		{0, 0}
      };

      return hpiBitMask2String(mask_map, mask);

}

const char *vohReadWriteThds2String(SaHpiSensorThdMaskT readm,
				    SaHpiSensorThdMaskT writem,
				    SaHpiSensorThdMaskT mask)
{

      if ((readm & mask) && (writem & mask)) {
	    return "(RW)";
      } else if (!(readm & mask) && (writem & mask)) {
	    return "(WO)";
      } else if ((readm & mask) && !(writem & mask)) {
	    return "(RO)";
      } else {
	    return "(NA)";
      }
}

const char *vohSensorValue2String(SaHpiSensorReadingT *sv)
{
      SaHpiSensorReadingTypeT		type = sv->Type;
      SaHpiSensorReadingUnionT		value = sv->Value;
      static char				str[1024];

      if (sv->IsSupported == FALSE)
	    return "";

      switch (type) {
	case SAHPI_SENSOR_READING_TYPE_INT64:
	    sprintf(str, "%lld", value.SensorInt64);
	    break;
	case SAHPI_SENSOR_READING_TYPE_UINT64:
	    sprintf(str, "%llu", value.SensorUint64);
	    break;
	case SAHPI_SENSOR_READING_TYPE_FLOAT64:
	    sprintf(str, "%10.3f", value.SensorFloat64);
	    break;
	case SAHPI_SENSOR_READING_TYPE_BUFFER:
	    snprintf(str, strlen((char *)value.SensorBuffer), "%s",
		     (char *)value.SensorBuffer);
	    break;
	default:
	    return "";
      }

      return str;
}

const char *
vohResetAction2String(SaHpiResetActionT reset)
{
      static cMap ra_map[] = {
		{SAHPI_COLD_RESET,		"cold reset"},
		{SAHPI_WARM_RESET,		"warm reset"},
		{SAHPI_RESET_ASSERT,		"assert"},
		{SAHPI_RESET_DEASSERT,		"deassert"},
		{0, 0}
      };

      return ValueToString(ra_map, reset, "%d");
}

const char *
vohPowerState2String(SaHpiResetActionT state)
{
      static cMap ps_map[] = {
		{SAHPI_POWER_OFF,		"off"},
		{SAHPI_POWER_ON,		"on"},
		{SAHPI_POWER_CYCLE,		"cycle"},
		{0, 0}
      };

      return ValueToString(ps_map, state, "%d");
}

const char *
vohEventType2String(SaHpiEventTypeT evtype)
{
      static cMap evtype_map[] = {
		{SAHPI_ET_RESOURCE,		"Resource type"},
		{SAHPI_ET_DOMAIN,		"Domain type"},
		{SAHPI_ET_SENSOR,		"Sensor type"},
		{SAHPI_ET_SENSOR_ENABLE_CHANGE,	"Sensor enable change type"},
		{SAHPI_ET_HOTSWAP,		"Hotswap type"},
		{SAHPI_ET_WATCHDOG,		"Watchdog type"},
		{SAHPI_ET_HPI_SW,		"HPI SW type"},
		{SAHPI_ET_OEM,			"OEM type"},
		{SAHPI_ET_USER,			"User type"},
		{0, 0}
      };

      return ValueToString(evtype_map, evtype, "%d");
}

const char *
vohResourceEventType2String(SaHpiResourceEventTypeT evtype)
{
      static cMap evtype_map[] = {
		{SAHPI_RESE_RESOURCE_FAILURE,		"failure"},
		{SAHPI_RESE_RESOURCE_RESTORED,		"restored"},
		{SAHPI_RESE_RESOURCE_ADDED,		"added"},
		{0, 0}
      };

      return ValueToString(evtype_map, evtype, "%d");
}

const char *
vohDomainEventType2String(SaHpiDomainEventTypeT evtype)
{
      static cMap evtype_map[] = {
		{SAHPI_DOMAIN_REF_ADDED,		"added"},
		{SAHPI_DOMAIN_REF_REMOVED,		"removed"},
		{0, 0}
      };

      return ValueToString(evtype_map, evtype, "%d");
}

const char *
vohSwEventType2String(SaHpiSwEventTypeT evtype)
{
      static cMap evtype_map[] = {
		{SAHPI_HPIE_AUDIT,			"audit"},
		{SAHPI_HPIE_STARTUP,			"sturtup"},
		{SAHPI_HPIE_OTHER,			"other"},
		{0, 0}
      };

      return ValueToString(evtype_map, evtype, "%d");
}

