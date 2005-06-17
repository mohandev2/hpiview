#include <time.h>
#include <sys/time.h>
#include <SaHpi.h>

#include "voh_types.h"
#include "voh_convert_type.h"


static const gchar *vt_value_to_string(VtValNameMapT *map,
				       guint value,
				       const gchar *def) {
	while(map->m_name) {
		if (map->m_value == value)
			return map->m_name;
		map++;
	}

	if ( def == 0 )
		return 0;

	static char str[100];
	sprintf( str, def, value );
	
	return str;
}

static const gchar *vt_flags_to_string(VtValNameMapT *map, guint value)
{
	static gchar str[1024];

	str[0] = 0;
	gint mask = 0;

	while(map->m_name) {
		if ((map->m_value & value) == map->m_value ) {
			if ( str[0] != 0 )
				strcat( str, "|" );

			strcat( str, map->m_name );
			mask |= map->m_value;
		}
		map++;
	}

	mask &= ~value;
  
	if ( mask ) {
		if ( str[0] != 0 )
			strcat( str, " " );
       
		gchar s[20];
		sprintf( s, "0x%x", mask );

		strcat( str, s );
	}
	if (strlen(str) < 1)
		strcat(str, "none");

	return str;
}



const gchar *vt_convert_boolean(guint b)
{
      if (b == TRUE)
	    return "TRUE";
      else
	    return "FALSE";
}

const gchar *vt_convert_ctrl_type(guint type)
{
      static VtValNameMapT type_map[] = {
		{SAHPI_CTRL_TYPE_DIGITAL,		"digital"},
		{SAHPI_CTRL_TYPE_DISCRETE,		"discrete"},
		{SAHPI_CTRL_TYPE_ANALOG,		"analog"},
		{SAHPI_CTRL_TYPE_STREAM,		"stream"},
		{SAHPI_CTRL_TYPE_TEXT,			"text"},
		{SAHPI_CTRL_TYPE_OEM,			"oem type"},
		{0, 0}
      };

      return vt_value_to_string(type_map, type, "%d");

}

const gchar *vt_convert_ctrl_output_type(guint type)
{
      static VtValNameMapT type_map[] = {
		{SAHPI_CTRL_GENERIC,			"generic"},
		{SAHPI_CTRL_LED,			"led"},
		{SAHPI_CTRL_FAN_SPEED,			"fan speed"},
		{SAHPI_CTRL_DRY_CONTACT_CLOSURE, "dry contact closure"},
		{SAHPI_CTRL_POWER_SUPPLY_INHIBIT, "power supply inhibit"},
		{SAHPI_CTRL_AUDIBLE,			"audible"},
		{SAHPI_CTRL_FRONT_PANEL_LOCKOUT, "front panel lockout"},
		{SAHPI_CTRL_POWER_INTERLOCK,		"power interlock"},
		{SAHPI_CTRL_POWER_STATE,		"power state"},
		{SAHPI_CTRL_LCD_DISPLAY,		"lcd display"},
		{SAHPI_CTRL_OEM,			"oem type"},
		{0, 0}
      };

      return vt_value_to_string(type_map, type, "%d");

}

const gchar *vt_convert_ctrl_mode(guint mode)
{
      static VtValNameMapT mode_map[] = {
	      {SAHPI_CTRL_MODE_AUTO,		"auto"},
	      {SAHPI_CTRL_MODE_MANUAL,		"manual"},
	      {0, 0}
      };

      return vt_value_to_string(mode_map, mode, "%d");

}

const gchar *vt_convert_ctrl_state_digital(guint state)
{
      static VtValNameMapT state_map[] = {
	      {SAHPI_CTRL_STATE_OFF,		"off"},
	      {SAHPI_CTRL_STATE_ON,		"on"},
	      {SAHPI_CTRL_STATE_PULSE_OFF,	"pulse off"},
	      {SAHPI_CTRL_STATE_PULSE_ON,	"pulse on"},
	      {0, 0}
      };

      return vt_value_to_string(state_map, state, "%d");

}

const gchar *vt_convert_watchdog_timer_use(guint t_use)
{
      static VtValNameMapT tmuse_map[] = {
	      {SAHPI_WTU_NONE,		"none"},
	      {SAHPI_WTU_BIOS_FRB2,	"BIOS FRB2"},
	      {SAHPI_WTU_BIOS_POST,	"BIOS post"},
	      {SAHPI_WTU_OS_LOAD,	"OS load"},
	      {SAHPI_WTU_SMS_OS,	"SMS OS"},
	      {SAHPI_WTU_OEM,		"OEM"},
	      {SAHPI_WTU_UNSPECIFIED,	"unspecified"},
	      {0, 0}
      };

      return vt_value_to_string(tmuse_map, t_use, "%d");

}

const gchar *vt_convert_watchdog_action(guint action)
{
      static VtValNameMapT action_map[] = {
	      {SAHPI_WA_NO_ACTION,	"no action"},
	      {SAHPI_WA_RESET,		"reset"},
	      {SAHPI_WA_POWER_DOWN,	"power down"},
	      {SAHPI_WA_POWER_CYCLE,	"power cycle"},
	      {0, 0}
      };

      return vt_value_to_string(action_map, action, "%d");

}

const gchar *vt_convert_watchdog_action_event(guint action)
{
      static VtValNameMapT action_map[] = {
	      {SAHPI_WA_NO_ACTION,	"no action"},
	      {SAHPI_WA_RESET,		"reset"},
	      {SAHPI_WA_POWER_DOWN,	"power down"},
	      {SAHPI_WA_POWER_CYCLE,	"power cycle"},
	      {SAHPI_WAE_TIMER_INT,	"timer interrupt"},
	      {0, 0}
      };

      return vt_value_to_string(action_map, action, "%d");

}

const gchar *vt_convert_watchdog_pretimer_interrupt(guint inter)
{
      static VtValNameMapT inter_map[] = {
	      {SAHPI_WPI_NONE,			"none"},
	      {SAHPI_WPI_SMI,			"SMI"},
	      {SAHPI_WPI_NMI,			"NMI"},
	      {SAHPI_WPI_MESSAGE_INTERRUPT,	"message interrupt"},
	      {SAHPI_WPI_OEM,			"OEM interrupt"},
	      {0, 0}
      };

      return vt_value_to_string(inter_map, inter, "%d");

}

const gchar *vt_convert_watchdog_exp_flags(guint flags)
{
	static VtValNameMapT tmuse_map[] = {
		{SAHPI_WATCHDOG_EXP_BIOS_FRB2,	"BIOS FRB2"},
		{SAHPI_WATCHDOG_EXP_BIOS_POST,	"BIOS post"},
		{SAHPI_WATCHDOG_EXP_OS_LOAD,	"OS load"},
		{SAHPI_WATCHDOG_EXP_SMS_OS,	"SMS OS"},
		{SAHPI_WATCHDOG_EXP_OEM,	"OEM"},
		{0, 0}
	};

	return vt_flags_to_string(tmuse_map, flags);

}

const gchar *vt_convert_domain_capabilities(guint cap)
{
	static VtValNameMapT cap_map[] = {
		{SAHPI_DOMAIN_CAP_AUTOINSERT_READ_ONLY,	"Autoinsert read only"},
		{0, 0}
	};

	return vt_flags_to_string(cap_map, cap);

}

const gchar *vt_convert_event_state(guint evs)
{
	static VtValNameMapT evs_map[] = {
		{SAHPI_ES_LOWER_MINOR,		"Lower Minor"},
		{SAHPI_ES_LOWER_MAJOR,		"Lower Major"},
		{SAHPI_ES_LOWER_CRIT,		"Lower Critical"},
		{SAHPI_ES_UPPER_MINOR,		"Upper Minor"},
		{SAHPI_ES_UPPER_MAJOR,		"Upper Major"},
		{SAHPI_ES_UPPER_CRIT,		"Upper Critical"},
		{0, 0}
	};

	return vt_flags_to_string(evs_map, evs);

}

const gchar *vt_convert_event_category(guint category)
{
	static VtValNameMapT category_map[] = {
		{SAHPI_EC_THRESHOLD,		"Threshold"},
		{SAHPI_EC_USAGE,		"Usage"},
		{SAHPI_EC_STATE,		"Generic"},
		{SAHPI_EC_PRED_FAIL,		"Predictive fail"},
		{SAHPI_EC_LIMIT,		"Limit"},
		{SAHPI_EC_PERFORMANCE,		"Performance"},
		{SAHPI_EC_SEVERITY,		"Severity"},
		{SAHPI_EC_PRESENCE,		"Presence"},
		{SAHPI_EC_ENABLE,		"Enabled"},
		{SAHPI_EC_AVAILABILITY,		"Availability"},
		{SAHPI_EC_REDUNDANCY,		"Redundancy"},
		{SAHPI_EC_SENSOR_SPECIFIC,	"Sensor-specific"},
		{ SAHPI_EC_GENERIC,		"OEM"},
		{0, 0}
	};

	return vt_value_to_string(category_map, category, "%d");

}
const gchar *vt_convert_severity(guint sev)
{
      static VtValNameMapT sev_map[] = {
	      {SAHPI_CRITICAL,			"Critical"},
	      {SAHPI_MAJOR,			"Major"},
	      {SAHPI_MINOR,			"Minor"},
	      {SAHPI_INFORMATIONAL,		"Informational"},
	      {SAHPI_OK,			"OK"},
	      {SAHPI_DEBUG,			"Debug"},
	      {SAHPI_ALL_SEVERITIES,		"All"},
	      {0, 0}
      };

      return vt_value_to_string(sev_map, sev, "%d");

}

const gchar *vt_convert_status_cond_type(guint type)
{
      static VtValNameMapT type_map[] = {
	      {SAHPI_STATUS_COND_TYPE_SENSOR,	"Sensor"},
	      {SAHPI_STATUS_COND_TYPE_RESOURCE,	"Resource"},
	      {SAHPI_STATUS_COND_TYPE_OEM,	"OEM"},
	      {SAHPI_STATUS_COND_TYPE_USER,	"User"},
	      {0, 0}
      };

      return vt_value_to_string(type_map, type, "%d");

}

const gchar *vt_convert_event_log_overflow_action(guint act)
{
      static VtValNameMapT act_map[] = {
	      {SAHPI_EL_OVERFLOW_DROP,		"Drop"},
	      {SAHPI_EL_OVERFLOW_OVERWRITE,	"Overwrite"},
	      {0, 0}
      };

      return vt_value_to_string(act_map, act, "%d");

}

const gchar *vt_convert_event_type(guint type)
{
      static VtValNameMapT type_map[] = {
	      {SAHPI_ET_RESOURCE,	"Resource"},
	      {SAHPI_ET_DOMAIN,		"Domain"},
	      {SAHPI_ET_SENSOR,		"Sensor"},
	      {SAHPI_ET_SENSOR_ENABLE_CHANGE,
		      			"Sensor enable change"},
	      {SAHPI_ET_HOTSWAP,	"Hotswap"},
	      {SAHPI_ET_WATCHDOG,	"Watchdog"},
	      {SAHPI_ET_HPI_SW,		"HPI software"},
	      {SAHPI_ET_OEM,		"OEM"},
	      {SAHPI_ET_USER,		"User"},
	      {0, 0}
      };

      return vt_value_to_string(type_map, type, "%d");

}

const gchar *vt_convert_resource_event_type(guint type)
{
      static VtValNameMapT type_map[] = {
	      {SAHPI_RESE_RESOURCE_FAILURE,	"Failure"},
	      {SAHPI_RESE_RESOURCE_RESTORED,	"Restored"},
	      {SAHPI_RESE_RESOURCE_ADDED,	"Added"},
	      {0, 0}
      };

      return vt_value_to_string(type_map, type, "%d");

}

const gchar *vt_convert_domain_event_type(guint type)
{
      static VtValNameMapT type_map[] = {
	      {SAHPI_DOMAIN_REF_ADDED,		"Added"},
	      {SAHPI_DOMAIN_REF_REMOVED,	"Removed"},
	      {0, 0}
      };

      return vt_value_to_string(type_map, type, "%d");

}

const gchar *vt_convert_sensor_type(guint type)
{
      static VtValNameMapT type_map[] = {
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

      return vt_value_to_string(type_map, type, "%d");

}

const gchar *vt_convert_hs_state(guint state)
{
      static VtValNameMapT state_map[] = {
	      {SAHPI_HS_STATE_INACTIVE,		"Inactive"},
	      {SAHPI_HS_STATE_INSERTION_PENDING,"Insertion pending"},
	      {SAHPI_HS_STATE_ACTIVE,		"Active"},
	      {SAHPI_HS_STATE_EXTRACTION_PENDING,"Extraction pending"},
	      {SAHPI_HS_STATE_NOT_PRESENT,	"Not present"},
	      {0, 0}
      };

      return vt_value_to_string(state_map, state, "%d");

}

const gchar *vt_convert_hpi_sw_event_type(guint type)
{
      static VtValNameMapT type_map[] = {
	      {SAHPI_HPIE_AUDIT,		"Audit"},
	      {SAHPI_HPIE_STARTUP,		"Sturtup"},
	      {SAHPI_HPIE_OTHER,		"Other"},
	      {0, 0}
      };

      return vt_value_to_string(type_map, type, "%d");

}


/*--------------------------------------------------------------------------*/
const gchar *vt_convert_watchdog_counter(guint counter)
{
	static gchar	str[1024];

	g_sprintf(str, "%d (ms)", counter);

	return str;
}

const gchar *vt_convert_time(gfloat time)
{
      static gchar	tstr[1024];
      gint 		count;
      struct tm 	t;
      time_t 		tt;

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
	    count = strftime(tstr, 1024, "%F %T", &t);
      }

      if (count == 0)
	    return "invalid time";

	return tstr;
}

gboolean vt_get_dmyhms(gfloat time,
		       guint *year,
		       guint *month,
		       guint *day,
		       guint *hour,
		       guint *min,
		       guint *sec)
{
	struct tm 	t,	*tp;
	time_t 		tt;

	if (time > SAHPI_TIME_MAX_RELATIVE) { /*absolute time*/
		tt = time / 1000000000;
		memcpy(&t, localtime(&tt), sizeof(struct tm));
	} else if (time ==  SAHPI_TIME_UNSPECIFIED) {
		return FALSE;
	} else if (time > SAHPI_TIME_UNSPECIFIED) { /*invalid time*/
		return FALSE;
	} else {   /*relative time*/
		tt = time / 1000000000;
		localtime_r(&tt, &t);
	}

	if (year)
		*year = t.tm_year + 1900;
	if (month)
		*month = t.tm_mon;
	if (day)
		*day = t.tm_mday;
	if (hour)
		*hour = t.tm_hour;
	if (min)
		*min = t.tm_min;
	if (sec)
		*sec = t.tm_sec;
	return TRUE;
}

gdouble vt_dmyhms_to_time(guint year,
			 guint month,
			 guint day,
			 guint hour,
			 guint min,
			 guint sec)
{
	struct tm 	t;
	time_t 		tt;

	time(&tt);
	localtime_r(&tt, &t);
	t.tm_year = year - 1900;
	t.tm_mon = month - 1;
	t.tm_mday = day;
	t.tm_hour = hour;
	t.tm_min = min;
	t.tm_sec = sec;
	tt = mktime(&t);
	return (gdouble) (tt) * 1000000000;
}
