#include <SaHpi.h>
#include <gtk/gtk.h>
#include "voh_string.h"

enum {
      VOH_LIST_COLUMN_NAME = 0,
      VOH_LIST_COLUMN_ICON,
      VOH_LIST_COLUMN_ID,
      VOH_LIST_COLUMN_TYPE,
      VOH_LIST_COLUMN_STATE,
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
      VOH_ITER_IS_VALUE,
      VOH_ITER_IS_PATH,
};

enum {
      VOH_ITER_SENSOR_STATE_UNSPECIFIED = 0,
      VOH_ITER_SENSOR_STATE_NOMINAL,
      VOH_ITER_SENSOR_STATE_NORMAL,
      VOH_ITER_SENSOR_STATE_CRITICAL,
};

#define VOH_ITER_RPT_STATE_UNSPECIFIED		0x0
#define VOH_ITER_RPT_STATE_POWER_ON		0x1
#define VOH_ITER_RPT_STATE_POWER_OFF		0x2
#define VOH_ITER_RPT_STATE_RESET_ASSERT		0x4
#define VOH_ITER_RPT_STATE_RESET_DEASSERT	0x8

      
static void fixstr(SaHpiTextBufferT *strptr, char *outbuff);
static void v_get_infra(SaHpiDomainIdT did, GtkTreeStore *store,
			GtkTreeIter *parent);
guint voh_open_session(guint domainid, gchar *err);
gboolean voh_discover(guint sessionid, gchar *err);
gboolean voh_session_close(guint domainid, gchar *err);
GtkTreeModel *voh_list_domains(gchar *err);
GtkTreeModel *voh_domain_info(guint domainid, gchar *err);
GtkTreeModel *voh_resource_info(guint domainid, guint resourceid, gchar *err);
GtkTreeModel *voh_rdr_info(guint domainid, guint resourceid,
			   guint rdrentryid, gchar *err);
gboolean voh_read_sensor(GtkTreeStore *store, guint domainid,
			 guint entryid, gchar *err);
GtkTreeModel *voh_list_resources(guint domainid, gchar *err);
gboolean voh_list_rdrs(GtkTreeStore *pstore, guint domainid,
		       guint resourceid, gchar *err);
void voh_add_resource(GtkTreeStore *pstore, guint domainid,
		      SaHpiRptEntryT *rpt);
void voh_add_rdr(GtkTreeStore *pstore, SaHpiRdrT *rdr, guint resourceid);
gboolean find_iter_by_id(GtkTreeModel *model, guint column_num,
			 guint req_id, GtkTreeIter *iter);
gboolean find_iter_by_name(GtkTreeModel *model, guint column_num,
			   const gchar *req_name, GtkTreeIter *iter);
guint voh_get_sensor_state(SaHpiSensorRecT *sensor, SaHpiSensorReadingT *sv);
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

