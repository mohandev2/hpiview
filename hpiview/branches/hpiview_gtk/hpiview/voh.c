#include "voh.h"

#define V_ERROR(str, err, rv) sprintf(err, "%s (error code: %s)", \
			              vohError2String(rv));
SaHpiSessionIdT	sessionid;

extern gchar voh_msg_err[100];

static void fixstr(SaHpiTextBufferT *strptr, char *outbuff)
{
      size_t		datalen,	len;
      char		*str = (char *)strptr->Data;

      if ((datalen = strptr->DataLength) != 0) {
	    len = strlen(str);
	    strncpy(outbuff, (char *)strptr->Data, datalen);
      }
      outbuff[datalen] = 0;
}

int voh_init(gchar *err)
{
      SaErrorT	rv;

      rv = saHpiSessionOpen(SAHPI_UNSPECIFIED_DOMAIN_ID, &sessionid, NULL);
	if (rv != SA_OK) {
	      if (err)
		    sprintf(err, "Session opening failed (%s)",
			    vohError2String(rv));
	      return -1;
	}

	/*
	 * Resource discovery
	 */
	rv = saHpiDiscover(sessionid);
	if (rv != SA_OK) {
	      if (err)
		    sprintf(err, "Discovering failed (%s)",
			    vohError2String(rv));
	      return -1;
	}

	return 0;
}

int voh_fini(gchar *err)
{
      SaErrorT		rv;

      rv = saHpiSessionClose(sessionid);
      if (rv != SA_OK) {
	    if (err)
		  sprintf(err, "Session close failed (%s)",
			  vohError2String(rv));
	    return -1;
      }

      return 0;
}

GtkTreeModel *voh_list_domains(gchar *err)
{
      GtkTreeStore	*pstore;
      GtkTreeIter	iter,	child;

      pstore = gtk_tree_store_new(VOH_LIST_NUM_COL, G_TYPE_STRING,
				  G_TYPE_UINT, G_TYPE_UINT);
      gtk_tree_store_append(pstore, &iter, NULL);
      gtk_tree_store_set(pstore, &iter,
			 VOH_LIST_COLUMN_NAME, "Domain #(single)",
			 VOH_LIST_COLUMN_ID, SAHPI_UNSPECIFIED_DOMAIN_ID,
			 VOH_LIST_COLUMN_TYPE, VOH_ITER_IS_DOMAIN,
			 -1);

      return GTK_TREE_MODEL(pstore);
}

GtkTreeModel *voh_domain_info(gchar *err)
{
      SaErrorT		rv;
      GtkTreeStore	*info_store;
      GtkTreeIter	iter;
      SaHpiDomainInfoT	info;
      SaHpiTextBufferT	strptr;
      char		outbuff[1024];

      if ((rv = saHpiDomainInfoGet(sessionid, &info)) != SA_OK) {
	    if (err)
		  sprintf(err, "Domain info getting failed (%s)",
			  vohError2String(rv));
	    return NULL;
      }

      info_store = gtk_tree_store_new(2, G_TYPE_STRING, G_TYPE_STRING);

      
      gtk_tree_store_append(info_store, &iter, NULL);
      gtk_tree_store_set(info_store, &iter,
			 0, "DomainId",
			 1, "Default",
			 -1);

      strptr = info.DomainTag;
      fixstr(&strptr, outbuff);
      gtk_tree_store_append(info_store, &iter, NULL);
      gtk_tree_store_set(info_store, &iter,
			 0, "DomainTag",
			 1, outbuff,
			 -1);

      return GTK_TREE_MODEL(info_store);
}

GtkTreeModel *voh_resource_info(guint id, gchar *err)
{
      GtkTreeStore	*info_store;
      GtkTreeIter	iter;
      gchar		ids[10];

      info_store = gtk_tree_store_new(2, G_TYPE_STRING, G_TYPE_STRING);

      gtk_tree_store_append(info_store, &iter, NULL);
      sprintf(ids, "%d", id);
      gtk_tree_store_set(info_store, &iter,
			 0, "ResourceID",
			 1, ids,
			 -1);

      return GTK_TREE_MODEL(info_store);
}

GtkTreeModel *voh_rdr_info(guint id, gchar *err)
{
      GtkTreeStore	*info_store;
      GtkTreeIter	iter;
      gchar		ids[10];

      info_store = gtk_tree_store_new(2, G_TYPE_STRING, G_TYPE_STRING);

      gtk_tree_store_append(info_store, &iter, NULL);
      sprintf(ids, "%d", id);
      gtk_tree_store_set(info_store, &iter,
			 0, "RdrID",
			 1, ids,
			 -1);

      return GTK_TREE_MODEL(info_store);
}

GtkTreeModel *voh_list_resources(gchar *err)
{
      GtkTreeStore	*pstore;
      SaHpiEntryIdT	rptentryid, nextrptentryid;
      SaErrorT		ret;
      SaHpiRptEntryT	rptentry;

      pstore = gtk_tree_store_new(VOH_LIST_NUM_COL, G_TYPE_STRING,
				  G_TYPE_UINT, G_TYPE_UINT);

      rptentryid = SAHPI_FIRST_ENTRY;
      while (rptentryid != SAHPI_LAST_ENTRY) {
	    ret = saHpiRptEntryGet(sessionid, rptentryid, &nextrptentryid,
				   &rptentry);
	    if (ret != SA_OK)
		  return(NULL);
	    voh_add_resource(pstore, &rptentry);
	    voh_list_rdrs(pstore, rptentry.ResourceId, err);
	    rptentryid = nextrptentryid;
      }

      return (GTK_TREE_MODEL(pstore));
}

int voh_list_rdrs(GtkTreeStore *pstore, SaHpiResourceIdT rid, gchar *err)
{
      SaHpiRdrT			rdr;
      SaErrorT			ret;
      SaHpiEntryIdT		entryid;
      SaHpiEntryIdT		nextentryid;

      entryid = SAHPI_FIRST_ENTRY;
      while (entryid !=SAHPI_LAST_ENTRY) {
	    ret = saHpiRdrGet(sessionid, rid, entryid, &nextentryid, &rdr);
	    if (ret != SA_OK)
		  return(-1);
	    voh_add_rdr(pstore, &rdr, rid);
	    entryid = nextentryid;
      }
}

void voh_add_resource(GtkTreeStore *pstore,
		      SaHpiRptEntryT *rpt)
{
      GtkTreeIter	parent, iter;
      gint		entity_path_num,	i;
      guint		id,			type;
      gboolean		res;
      SaHpiEntityPathT	entity_path = rpt->ResourceEntity;
      gchar		path[100];

      for (i = 0; i < SAHPI_MAX_ENTITY_PATH; i++) {
	    if (entity_path.Entry[i].EntityType == SAHPI_ENT_ROOT) {
		  break;
	    }
      }
      entity_path_num = --i;

      res = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(pstore), &parent);
      for (i = entity_path_num; i >= 0; i--) {
	    vohEntityPath2String(&(entity_path.Entry[i]), path);
	    if (res == TRUE) {
		  res = find_iter_by_name(GTK_TREE_MODEL(pstore),
					  VOH_LIST_COLUMN_NAME,
					  path, &parent);
	    } else {
		  res = FALSE;
	    }
	    
	    if (res == FALSE) {
		  if (i == entity_path_num)
			gtk_tree_store_append(pstore, &iter, NULL);
		  else
			gtk_tree_store_append(pstore, &iter, &parent);
		  if (i == 0) {
			type = VOH_ITER_IS_RPT;
			id = (guint) rpt->ResourceId;
		  }
		  else {
			type = VOH_ITER_IS_PATH;
			id = 100;
		  }
		  gtk_tree_store_set(pstore, &iter,
				     VOH_LIST_COLUMN_NAME, path,
				     VOH_LIST_COLUMN_ID, id,
				     VOH_LIST_COLUMN_TYPE, type,
				     -1);
		  strncpy((char *)&parent, (char *)&iter, sizeof(GtkTreeIter));
	    }
      }
}

void voh_add_rdr(GtkTreeStore *pstore,
		 SaHpiRdrT *rdr,
		 SaHpiResourceIdT rid)
{
      gboolean		res;
      gchar		name[100];
      GtkTreeIter	iter,	parent;
      if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(pstore), &parent)
	  							== FALSE)
	    return;

      res = find_iter_by_id(GTK_TREE_MODEL(pstore), VOH_LIST_COLUMN_ID,
			    (guint)rid, &parent);
      if (res != FALSE) {
	    sprintf(name, "Rdr #%d", rdr->RecordId);
	    gtk_tree_store_append(pstore, &iter, &parent);
	    gtk_tree_store_set(pstore, &iter,
			       VOH_LIST_COLUMN_NAME,
			       		vohRdrType2String(rdr->RdrType),
			       VOH_LIST_COLUMN_ID, (guint)rdr->RecordId,
			       VOH_LIST_COLUMN_TYPE, VOH_ITER_IS_RDR,
			       -1);
      }
}

gboolean find_iter_by_id(GtkTreeModel *model,
			 guint column_num,
			 guint req_id,
			 GtkTreeIter *iter)
{
      gboolean		res = TRUE;
      guint		id;
      GtkTreeIter	niter,	child;

      while (res != FALSE) {
	    gtk_tree_model_get(model, iter, column_num, &id, -1);
	    if (req_id == id) {
		  return TRUE;
	    }

	    if (gtk_tree_model_iter_children(model, &child, iter) == TRUE) {
		  res = find_iter_by_id(model, column_num, req_id, &child);
		  if (res == TRUE) {
			strncpy((char *)iter, (char *)&child,
						sizeof(GtkTreeIter));
			return TRUE;
		  }
	    }
	    res = gtk_tree_model_iter_next(model, iter);
      }
      return FALSE;
}


gboolean find_iter_by_name(GtkTreeModel *model,
			   guint column_num,
			   const gchar *req_name,
			   GtkTreeIter *iter)
{
      gboolean		res = TRUE;
      gchar		*name;
      GtkTreeIter	child;


      while (res != FALSE) {
	    gtk_tree_model_get(model, iter, column_num, &name, -1);
	    if (strcmp(name, req_name) == 0) {
		  g_free(name);
		  return TRUE;
	    }
	    g_free(name);

	    if (gtk_tree_model_iter_children(model, &child, iter) == TRUE) {
		  res = find_iter_by_name(model, column_num, req_name, &child);
		  if (res == TRUE) {
			strncpy((char *)iter, (char *)&child,
						sizeof(GtkTreeIter));
			return TRUE;
		  }
	    }
	    res = gtk_tree_model_iter_next(model, iter);
      }
      return FALSE;
}

