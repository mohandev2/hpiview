
#ifndef __VOH_H__
#define __VOH_H__

#include <SaHpi.h>
#include "voh_types.h"

enum {
      VOH_LIST_COLUMN_NAME = 0,
      VOH_LIST_COLUMN_ICON,
      VOH_LIST_COLUMN_VALUE,
      VOH_LIST_COLUMN_ID,
      VOH_LIST_COLUMN_TYPE,
      VOH_LIST_COLUMN_STATE,
      VOH_LIST_COLUMN_CAPABILITY,
      VOH_LIST_NUM_COL,
};

enum {
      VOH_EVENT_LIST_COLUMN_TIME = 0,
      VOH_EVENT_LIST_COLUMN_NAME,
      VOH_EVENT_LIST_COLUMN_SOURCE,
      VOH_EVENT_LIST_COLUMN_SEVER,
      VOH_EVENT_LIST_COLUMN_TYPE,
      VOH_EVENT_LIST_COLUMN_DOMAIN,
      VOH_EVENT_LIST_NUM_COL,
};

enum {
      VOH_ITER_IS_DOMAIN,
      VOH_ITER_IS_RPT,
      VOH_ITER_IS_CONTROL,
      VOH_ITER_IS_SENSOR,
      VOH_ITER_IS_INVENTORY,
      VOH_ITER_IS_WATCHDOG,
      VOH_ITER_IS_ANNUNCIATOR,
      VOH_ITER_IS_NO_RECORD,
      VOH_ITER_IS_PATH,
};

enum {
      VOH_ITER_SENSOR_STATE_UNSPECIFIED = 0,
      VOH_ITER_SENSOR_STATE_NOMINAL,
      VOH_ITER_SENSOR_STATE_NORMAL,
      VOH_ITER_SENSOR_STATE_CRITICAL,
};

#define VOH_DOMAIN_STATE_UNSPECIFIED		0x0
#define VOH_DOMAIN_STATE_OPENED			0x1

#define VOH_ITER_RPT_STATE_UNSPECIFIED		0x0
#define VOH_ITER_RPT_STATE_POWER_ON		0x1
#define VOH_ITER_RPT_STATE_POWER_OFF		0x2
#define VOH_ITER_RPT_STATE_RESET_ASSERT		0x4
#define VOH_ITER_RPT_STATE_RESET_DEASSERT	0x8

#define VOH_ITER_CAPABILITY_UNSPECIFIED		0x0
#define VOH_ITER_CAPABILITY_POWER		0x1
#define VOH_ITER_CAPABILITY_RESET		0x2
#define VOH_ITER_CAPABILITY_CONTROL		0x4
#define VOH_ITER_CAPABILITY_EVENT_LOG		0x8

#define VOH_ITER_SENSOR_CAPABILITY_UNSPECIFIED	0x0
#define VOH_ITER_SENSOR_CAPABILITY_THRESHOLD	0x1

#define VOH_CONTROL_DIGITAL		SAHPI_CTRL_TYPE_DIGITAL
#define VOH_CONTROL_DISCRETE		SAHPI_CTRL_TYPE_DISCRETE
#define VOH_CONTROL_ANALOG		SAHPI_CTRL_TYPE_ANALOG
#define VOH_CONTROL_STREAM		SAHPI_CTRL_TYPE_STREAM
#define VOH_CONTROL_TEXT		SAHPI_CTRL_TYPE_TEXT
#define VOH_CONTROL_OEM			SAHPI_CTRL_TYPE_OEM

guint voh_open_session(guint domainid, gchar *err);
gboolean voh_discover(guint sessionid, gchar *err);
gboolean voh_session_close(guint domainid, gchar *err);
GtkTreeModel *voh_list_domains(gchar *err);
GtkTreeModel *voh_domain_info(guint domainid, gchar *err);
GtkTreeModel *voh_get_resource_details(guint sessionid,
				       guint resourceid,
				       gchar *err);
GtkTreeModel *voh_rdr_info(guint domainid, guint resourceid,
			   guint rdrentryid, gchar *err);
gboolean voh_read_sensor(GtkTreeStore *store, guint domainid, guint resourceid,
			 guint entryid, gchar *err);
GtkTreeModel *voh_list_resources(guint domainid, gchar *err);
gboolean voh_list_rdrs(GtkTreeStore *pstore, guint domainid,
		       guint resourceid, gchar *err);
gboolean voh_parm_control(guint sessionid, guint resourceid,
			  guint action, gchar *err);
gboolean voh_get_power_state(guint domainid, guint resourceid,
			     GtkWidget *menu, gchar *err);
gboolean voh_set_power_off(guint domainid, guint resourceid,
			   GtkTreeStore *store, gchar *err);
gboolean voh_set_power_on(guint domainid, guint resourceid,
			  GtkTreeStore *store, gchar *err);
gboolean voh_set_power_cycle(guint domainid, guint resourceid,
			     GtkTreeStore *store, gchar *err);
gboolean voh_set_reset_cold(guint domainid, guint resourceid,
			    GtkTreeStore *store, gchar *err);
gboolean voh_set_reset_warm(guint domainid, guint resourceid,
			    GtkTreeStore *store, gchar *err);
gboolean voh_set_reset_assert(guint domainid, guint resourceid,
			      GtkTreeStore *store, gchar *err);
gboolean voh_set_reset_deassert(guint domainid, guint resourceid,
				GtkTreeStore *store, gchar *err);
gboolean voh_subscribe_events(guint domainid, gchar *err);
gboolean voh_unsubscribe_events(guint domainid, gchar *err);
gboolean voh_get_events(GtkTreeStore *event_list, guint domainid, gchar *err);

gboolean voh_set_resource_severity(guint sessionid, guint resourceid,
				   guint severity, gchar *err);
gchar *voh_get_resource_severity(guint sessionid, guint resourceid, gchar *err);

gboolean voh_check_resource_presence(guint sessionid,
				     guint resourceid,
				     gchar *err);
gboolean voh_check_rdr_presence(guint sessionid,
				guint resourceid,
				guint rdrentryid,
				gchar *err);
GtkListStore *voh_get_severity_list(void);

GList *voh_get_resource_info(guint sessionid,
			     guint resourceid,
			     gchar *err);
gchar *voh_get_resource_tag(guint sessionid,
			    guint resourceid,
			    gchar *err);
gboolean voh_set_resource_tag(guint sessionid,
			      guint resourceid,
			      const gchar *resource_tag,
			      gchar *err);
GList *voh_get_sensor_info(guint sessionid,
			   guint resourceid,
			   guint rdrentryid,
			   gchar *err);

gboolean voh_get_sensor_enable(guint sessionid,
			       guint resourceid,
			       guint rdrentryid,
			       gboolean *status,
			       gchar *err);

gboolean voh_set_sensor_enable(guint sessionid,
			       guint resourceid,
			       guint rdrentryid,
			       gboolean status,
			       gchar *err);

GList *voh_get_sensor_event_info(guint sessionid,
				 guint resourceid,
				 guint rdrentryid,
				 gchar *err);

gboolean voh_get_sensor_assert_event_mask(guint sessionid,
					  guint resourceid,
					  guint rdrentryid,
					  GList **evlist,
					  gchar *err);
gboolean voh_get_sensor_deassert_event_mask(guint sessionid,
					    guint resourceid,
					    guint rdrentryid,
					    GList **evlist,
					    gchar *err);
gboolean voh_set_sensor_event_masks(guint sessionid,
				    guint resourceid,
				    guint rdrentryid,
				    GList *assert_mask,
				    GList *deassert_mask,
				    gchar *err);

gboolean voh_get_sensor_event_states_supported(guint sessionid,
					       guint resourceid,
					       guint rdrentryid,
					       GList **evlist,
					       gchar *err);

GList *voh_get_sensor_threshold_info(guint sessionid,
				     guint resourceid,
				     guint rdrentryid,
				     gchar *err);

gboolean voh_get_sensor_thresholds(guint sessionid,
				   guint resourceid,
				   guint rdrentryid,
				   GList **thrlist,
				   gchar *err);
gboolean voh_set_sensor_thresholds(guint sessionid,
				   guint resourceid,
				   guint rdrentryid,
				   GList *thrlist,
				   gchar *err);

gboolean voh_get_inventory_info(guint sessionid,
				guint resourceid,
				guint rdrentryid,
				GList **info,
				gchar *err);
gboolean voh_get_idr_area_with_field(guint sessionid,
				      guint resourceid,
				      guint rdrentryid,
				      GList **areas,
				      gchar *err);

void voh_get_idr_area_type(GList **area_types);
void voh_get_idr_field_type(GList **field_types);

gboolean voh_idr_area_add(guint sessionid,
			  guint resourceid,
			  guint rdrentryid,
			  guint areatype,
			  guint *areaid,
			  gchar *err);
gboolean voh_idr_area_delete(guint sessionid,
			     guint resourceid,
			     guint rdrentryid,
			     guint areaid,
			     gchar *err);
gboolean voh_idr_field_add(guint sessionid,
			   guint resourceid,
			   guint rdrentryid,
			   VtDataT *field,
			   gchar *err);
gboolean voh_idr_field_delete(guint sessionid,
			     guint resourceid,
			     guint rdrentryid,
			     guint areaid,
			     guint fieldid,
			     gchar *err);
gboolean voh_idr_field_get(guint sessionid,
			   guint resourceid,
			   guint rdrentryid,
			   guint areaid,
			   guint fieldtype,
			   guint fieldid,
			   guint *nextfieldid,
			   VtDataT *field,
			   gchar *err);
gboolean voh_idr_field_set(guint sessionid,
			   guint resourceid,
			   guint rdrentryid,
			   VtDataT *field,
			   gchar *err);

gboolean voh_get_control_info(guint sessionid,
			      guint resourceid,
			      guint rdrentryid,
			      VtData1T **info,
			      gchar *err);

gboolean voh_control_get(guint sessionid,
			 guint resourceid,
			 guint rdrentryid,
			 VtData1T **mode,
			 VtData1T **state,
			 gchar *err);

gboolean voh_control_type_get(guint sessionid,
			      guint resourceid,
			      guint rdrentryid,
			      VtData1T **ctrltype,
			      gchar *err);
gboolean voh_control_set(guint sessionid,
			 guint resourceid,
			 guint rdrentryid,
			 VtData1T *mode_data,
			 VtData1T *state_data,
			 gchar *err);
gboolean voh_watchdog_timer_get(guint sessionid,
				guint resourceid,
				guint rdrentryid,
				VtData1T **watchdog_data,
				gchar *err);
gboolean voh_watchdog_timer_set(guint sessionid,
				guint resourceid,
				guint rdrentryid,
				VtData1T *watchdog_data,
				gchar *err);
gboolean voh_watchdog_timer_reset(guint sessionid,
				  guint resourceid,
				  guint rdrentryid,
				  gchar *err);
gboolean voh_domain_tag_set(guint sessionid,
			    VtData1T *tag_data,
			    gchar *err);
gboolean voh_alarm_get_next(guint sessionid,
			    guint severity,
			    gboolean unack_only,
			    VtData1T *alarm_data,
			    gchar *err);
gboolean voh_alarm_get(guint sessionid,
		       guint entryid,
		       VtData1T **alarm_data,
		       gchar *err);

GList *voh_get_all_alarms(guint sessionid,
			  gchar *err);

gboolean voh_event_log_time_get(guint sessionid,
				gint resourceid,
				gfloat *time,
				gchar *err);
gboolean voh_event_log_time_set(guint sessionid,
				gint resourceid,
				gfloat time,
				gchar *err);

gboolean voh_event_log_info_get(guint sessionid,
				guint resourceid,
				VtData1T **evlog_data,
				gchar *err);
gboolean voh_event_log_state_set(guint sessionid,
				 guint resourceid,
				 gboolean enabled,
				 gchar *err);
gboolean voh_event_log_clear(guint sessionid,
			     guint resourceid,
			     gchar *err);
gboolean voh_event_log_overflow_reset(guint sessionid,
				      guint resourceid,
				      gchar *err);

gboolean voh_get_evlog_entry_info(guint sessionid,
				  guint resourceid,
				  guint entryid,
				  VtData1T **entry_data,
				  gchar *err);

GList *voh_get_evlog_entries(guint sessionid,
			     guint resourceid,
			     gchar *err);

GtkListStore *voh_get_ctrl_digital_state_list(void);
GtkListStore *voh_get_ctrl_mode_list(void);

GtkListStore *voh_get_watchdog_timer_use_list(void);
GtkListStore *voh_get_watchdog_action_list(void);
GtkListStore *voh_get_watchdog_pretimer_interrupt_list(void);
GList *voh_get_watchdog_exp_flags_list();

#endif /* __VOH_H__ */

