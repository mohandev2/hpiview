#include <SaHpi.h>
#include <gtk/gtk.h>
#include "voh_string.h"

enum {
      VOH_LIST_COLUMN_NAME = 0,
      VOH_LIST_COLUMN_ID,
      VOH_LIST_COLUMN_TYPE,
      VOH_LIST_NUM_COL,
};

enum {
      VOH_ITER_IS_DOMAIN = 1,
      VOH_ITER_IS_RPT,
      VOH_ITER_IS_RDR,
      VOH_ITER_IS_PATH,
};
      
int voh_init(gchar *err);
int voh_fini(gchar *err);
GtkTreeModel *voh_list_domains(gchar *err);
GtkTreeModel *voh_list_resources(gchar *err);
int voh_list_rdrs(GtkTreeStore *pstore, SaHpiResourceIdT rid, gchar *err);
GtkTreeModel *voh_domain_info(gchar *err);
GtkTreeModel *voh_resource_info(guint id, gchar *err);
GtkTreeModel *voh_rdr_info(guint id, gchar *err);

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
