#include "voh.h"

SaHpiSessionIdT	sessionid;

int voh_init(void)
{
      SaErrorT	rv;

      rv = saHpiSessionOpen(SAHPI_UNSPECIFIED_DOMAIN_ID, &sessionid, NULL);
	if (rv != SA_OK) {
		return -1;
	}

	/*
	 * Resource discovery
	 */
	rv = saHpiDiscover(sessionid);
	if (rv != SA_OK) {
	      return -1;
	}

	return 0;
}

GtkTreeModel *voh_list_domains(void)
{
      GtkTreeStore	*pstore;
      GtkTreeIter	iter,	child;
      
      pstore = gtk_tree_store_new(1, G_TYPE_STRING);
      gtk_tree_store_append(pstore, &iter, NULL);
      gtk_tree_store_set(pstore, &iter,
			 0, "Domain #(single)",
			 -1);

      return GTK_TREE_MODEL(pstore);
}

GtkTreeModel *voh_list_resources(void)
{
      SaErrorT rv       = SA_OK,
	       rvRdrGet = SA_OK,
 	       rvRptGet = SA_OK;

      SaHpiRptEntryT rptentry;
      SaHpiEntryIdT rptentryid;
      SaHpiEntryIdT nextrptentryid;
      SaHpiEntryIdT entryid;
      SaHpiEntryIdT nextentryid;
      SaHpiRdrT rdr;
      SaHpiEntryIdT rdrid;
      SaHpiResourceIdT l_rid;
      SaHpiResourceIdT rid = 255;

      GtkTreeStore	*pstore;
      GtkTreeIter	iter,	child;

      gchar		name[100];

      pstore = gtk_tree_store_new(1, G_TYPE_STRING);

      rptentryid = SAHPI_FIRST_ENTRY;
      do {
	    rvRptGet = saHpiRptEntryGet(sessionid, rptentryid,
					&nextrptentryid, &rptentry);

	    if (rvRptGet == SA_OK
		&& (rptentry.ResourceCapabilities & SAHPI_CAPABILITY_RDR) 
		&& ((rid == 0xFF) || (rid == rptentry.ResourceId))) {

		  l_rid = rptentry.ResourceId;
		  sprintf(name, "Resource #%d", l_rid);
		  
		  gtk_tree_store_append(pstore, &iter, NULL);
		  gtk_tree_store_set(pstore, &iter,
				     0, name,
				     -1);

		  if (rid != 0xFF)
			nextrptentryid = SAHPI_LAST_ENTRY;

		  entryid = SAHPI_FIRST_ENTRY;

		  do {
			rvRdrGet = saHpiRdrGet(sessionid, l_rid, entryid,
					       &nextentryid, &rdr);

			if (rvRdrGet == SA_OK) {
			      rdrid = rdr.RecordId;
			      sprintf(name, "Rdr #%d", rdrid);
			      gtk_tree_store_append(pstore, &child, &iter);
			      gtk_tree_store_set(pstore, &child,
						 0, name,
						 -1);
			}
			entryid = nextentryid;
		  } while ((rvRdrGet == SA_OK)
			   && (entryid != SAHPI_LAST_ENTRY));
	    }
	    rptentryid = nextrptentryid;
      } while ((rvRptGet == SA_OK) && (rptentryid != SAHPI_LAST_ENTRY));

      return (GTK_TREE_MODEL(pstore));
}
