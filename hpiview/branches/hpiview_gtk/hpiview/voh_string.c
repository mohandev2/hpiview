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
