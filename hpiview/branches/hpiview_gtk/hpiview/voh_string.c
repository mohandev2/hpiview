#include "voh_string.h"
#include <string.h>


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

void vohEntityPath2String(SaHpiEntityT *entity, char *path)
{

      sprintf(path, "%s #%d", vohEntityType2String(entity->EntityType),
	      		      entity->EntityLocation);
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
