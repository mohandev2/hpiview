#include "voh.h"

#define VOH_ERROR(err, str, rv) if (err) \
				sprintf(err, "%s (error code: %s)", \
				str, vohError2String(rv));

static SaHpiSessionIdT	sessionid;

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
	      VOH_ERROR(err, "Session opening failed", rv);
	      return -1;
	}

	/*
	 * Resource discovery
	 */
	rv = saHpiDiscover(sessionid);
	if (rv != SA_OK) {
	      VOH_ERROR(err, "Discovering failed", rv);
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
      SaErrorT		rv;
      SaHpiRptEntryT	entry;
      GtkTreeStore	*info_store;
      GtkTreeIter	iter;
      gchar		name[1024];

      rv = saHpiRptEntryGetByResourceId(sessionid, (SaHpiResourceIdT)id,
				      &entry);
      if (rv != SA_OK) {
	    sprintf(err, "Resource info getting failed (%s)",
			  vohError2String(rv));
	    return NULL;
      }

      info_store = gtk_tree_store_new(2, G_TYPE_STRING, G_TYPE_STRING);

      gtk_tree_store_append(info_store, &iter, NULL);
      sprintf(name, "%d", id);
      gtk_tree_store_set(info_store, &iter,
			 0, "ResourceID",
			 1, name,
			 -1);

      gtk_tree_store_append(info_store, &iter, NULL);
      vohFullEntityPath2String(&(entry.ResourceEntity), name);
      gtk_tree_store_set(info_store, &iter,
			 0, "Enity path",
			 1, name,
			 -1);

      gtk_tree_store_append(info_store, &iter, NULL);
      gtk_tree_store_set(info_store, &iter,
			 0, "Capabilities",
			 1, vohCapabilities2String(entry.ResourceCapabilities),
			 -1);

      gtk_tree_store_append(info_store, &iter, NULL);
      gtk_tree_store_set(info_store, &iter,
			 0, "HotSwapCapabilities",
			 1, vohHsCapabilities2String(entry.HotSwapCapabilities),
			 -1);

      gtk_tree_store_append(info_store, &iter, NULL);
      fixstr(&(entry.ResourceTag), name);
      gtk_tree_store_set(info_store, &iter,
			 0, "ResourceTag",
			 1, name,
			 -1);

      return GTK_TREE_MODEL(info_store);
}

GtkTreeModel *voh_rdr_info(guint rid, guint type, guint id, gchar *err)
{
      SaErrorT			rv;
      SaHpiRdrT			rdr;
      SaHpiEntryIdT		nextentryid;
      SaHpiSensorRecT		*sensor;
      GtkTreeStore		*info_store;
      GtkTreeIter		iter;
      gchar			ids[100];
      gchar			name[1024];

      rv = saHpiRdrGet(sessionid, rid, id, &nextentryid, &rdr);
      if (rv != SA_OK) {
	    VOH_ERROR(err, "Rdr info getting failed", rv);
	    return NULL;
      }

      info_store = gtk_tree_store_new(2, G_TYPE_STRING, G_TYPE_STRING);

      gtk_tree_store_append(info_store, &iter, NULL);
      sprintf(ids, "%d", rdr.RecordId);
      gtk_tree_store_set(info_store, &iter,
			 0, "Rdr Id",
			 1, ids,
			 -1);

      gtk_tree_store_append(info_store, &iter, NULL);
      gtk_tree_store_set(info_store, &iter,
			 0, "Type",
			 1, vohRdrType2String(type),
			 -1);

      gtk_tree_store_append(info_store, &iter, NULL);
      vohFullEntityPath2String(&(rdr.Entity), name);
      gtk_tree_store_set(info_store, &iter,
			 0, "Entity path",
			 1, name,
			 -1);

      switch (rdr.RdrType) {
	case SAHPI_SENSOR_RDR:
	    sensor = &(rdr.RdrTypeUnion.SensorRec);

	    gtk_tree_store_append(info_store, &iter, NULL);
	    sprintf(ids, "%d", sensor->Num);
	    gtk_tree_store_set(info_store, &iter,
			       0, "Sensor Id",
			       1, ids,
			       -1);

	    gtk_tree_store_append(info_store, &iter, NULL);
	    gtk_tree_store_set(info_store, &iter,
				  0, "Sensor type",
				  1, vohSensorType2String(sensor->Type),
				  -1);

	    gtk_tree_store_append(info_store, &iter, NULL);
	    gtk_tree_store_set(info_store, &iter,
			       0, "Event category",
			       1, vohEventCategory2String(sensor->Category),
			       -1);

	    gtk_tree_store_append(info_store, &iter, NULL);
	    gtk_tree_store_set(info_store, &iter,
			       0, "Sensor control",
			       1, vohBoolean2String(sensor->EnableCtrl),
			       -1);

	    gtk_tree_store_append(info_store, &iter, NULL);
	    gtk_tree_store_set(info_store, &iter,
			       0, "Sensor event control",
			       1, vohSensorEventCtrl2String(sensor->EventCtrl),
			       -1);

	    gtk_tree_store_append(info_store, &iter, NULL);
	    gtk_tree_store_set(info_store, &iter,
			       0, "Event states supported",
			       1, vohEventState2String(sensor->Events,
						       sensor->Category),
			       -1);

	    break;
	default:
	    break;
      }

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
      while (entryid != SAHPI_LAST_ENTRY) {
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
      GtkTreeIter	iter,	parent,	*found_iter;
      gint		i;
      guint		id,	type;
      gboolean		res,	first = FALSE;
      SaHpiEntityPathT	entity_path = rpt->ResourceEntity;
      gchar		path[100];

      for (i = 0; i < SAHPI_MAX_ENTITY_PATH; i++) {
	    if (entity_path.Entry[i].EntityType == SAHPI_ENT_ROOT) {
		  break;
	    }
      }

      res = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(pstore), &parent);

      if (res == TRUE) {
	    for (i--; i > 0; i--) {
		  vohEntityPath2String(&(entity_path.Entry[i]), path);

		  res = find_iter_by_name(GTK_TREE_MODEL(pstore),
						 VOH_LIST_COLUMN_NAME,
						 path, &parent);
		  if (res == FALSE)
			break;
	    }

      } else {
	    i--;
	    first = TRUE;
      }	    

      for (i; i >= 0; i--) {
	    vohEntityPath2String(&(entity_path.Entry[i]), path);
	    if (first == FALSE)
		  gtk_tree_store_append(pstore, &iter, &parent);
	    else
		  gtk_tree_store_append(pstore, &iter, NULL);
	    
	    if (i == 0) {
		  type = VOH_ITER_IS_RPT;
		  id = (guint) rpt->ResourceId;
	    }
	    else {
		  type = VOH_ITER_IS_PATH;
		  id = 0;
	    }
	    
	    gtk_tree_store_set(pstore, &iter,
			       VOH_LIST_COLUMN_NAME, path,
			       VOH_LIST_COLUMN_ID, id,
			       VOH_LIST_COLUMN_TYPE, type,
			       -1);
	   parent = iter;
	   first = FALSE;
      }
}

void voh_add_rdr(GtkTreeStore *pstore,
		 SaHpiRdrT *rdr,
		 SaHpiResourceIdT rid)
{
      gboolean		res;
      GtkTreeIter	iter,	parent;
      SaHpiTextBufferT	strptr;
      gchar		name[100];

      if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(pstore), &parent)
	  							== FALSE)
	    return;

      res = find_iter_by_id(GTK_TREE_MODEL(pstore), VOH_LIST_COLUMN_ID,
			    (guint)rid, &parent);
      if (res != FALSE) {
	    strptr = rdr->IdString;
	    fixstr(&strptr, name);
	    gtk_tree_store_append(pstore, &iter, &parent);
	    gtk_tree_store_set(pstore, &iter,
			       VOH_LIST_COLUMN_NAME, name,
			       VOH_LIST_COLUMN_ID, rdr->RecordId,
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
			*iter = child;
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

      if (iter == NULL)
	    return FALSE;

      while (res == TRUE) {
	    gtk_tree_model_get(model, iter, column_num, &name, -1);
	    if (strcmp(name, req_name) == 0) {
		  g_free(name);
		  return TRUE;
	    }
	    g_free(name);

	    if (gtk_tree_model_iter_children(model, &child, iter) == TRUE) {
		  res = find_iter_by_name(model, column_num,
						 req_name, &child);
		  if (res == TRUE) {
			*iter = child;
			return TRUE;
		  }
	    }
	    res = gtk_tree_model_iter_next(model, iter);
      }
      return FALSE;
}

