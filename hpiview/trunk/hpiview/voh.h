#include <SaHpi.h>
#include <gtk/gtk.h>
#include "voh_string.h"

enum {
      VOH_LIST_COLUMN_ICON = 0,
      VOH_LIST_COLUMN_NAME,
      VOH_LIST_COLUMN_ID,
      VOH_LIST_COLUMN_TYPE,
      VOH_LIST_COLUMN_STATE,
      VOH_LIST_NUM_COL,
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

#define VOH_ITER_RPT_STATE_POWER_ON 0x1
#define VOH_ITER_RPT_STATE_POWER_OFF 0x2
      
int voh_init(gchar *err);
int voh_fini(gchar *err);
GtkTreeModel *voh_list_domains(gchar *err);
GtkTreeModel *voh_list_resources(gchar *err);
int voh_list_rdrs(GtkTreeStore *pstore, SaHpiResourceIdT rid, gchar *err);
GtkTreeModel *voh_domain_info(gchar *err);
GtkTreeModel *voh_resource_info(guint id, gchar *err);
GtkTreeModel *voh_rdr_info(guint rid, guint id, gchar *err);
gint voh_read_sensor(GtkTreeStore *store, guint id, gchar *err);

void voh_add_resource(GtkTreeStore *pstore,
		      SaHpiRptEntryT *rpt);
void voh_add_rdr(GtkTreeStore *pstore,
		 SaHpiRdrT *rdr,
		 SaHpiResourceIdT rid);
gboolean find_iter_by_name(GtkTreeModel *model,
			   guint column_num,
			   const gchar *req_name,
			   GtkTreeIter *iter);
gboolean find_iter_by_id(GtkTreeModel *model,
			 guint column_num,
			 guint req_id,
			 GtkTreeIter *iter);

guint voh_get_sensor_state(SaHpiSensorRecT *sensor, SaHpiSensorReadingT *sv);
gboolean voh_get_power_state(guint id, GtkWidget *menu, gchar *err);
gboolean voh_set_power_off(guint id, GtkTreeStore *store, gchar *err);
gboolean voh_set_power_on(guint id, GtkTreeStore *store, gchar *err);
gboolean voh_set_power_cycle(guint id, GtkTreeStore *store, gchar *err);

