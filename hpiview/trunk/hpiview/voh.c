#include "voh.h"

#define VOH_ERROR(err, str, rv) if (err) { \
				    if (rv != -1) \
				        sprintf(err, "%s (error code: %s)", \
						str, vohError2String(rv)); \
				    else \
				        sprintf(err, "%s", str); \
				}

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
				  G_TYPE_UINT, G_TYPE_UINT, G_TYPE_UINT);
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

GtkTreeModel *voh_rdr_info(guint rid, guint id, gchar *err)
{
      SaErrorT			rv;
      SaHpiRdrT			rdr;
      SaHpiEntryIdT		nextentryid;
      SaHpiSensorRecT		*sensor;
      GtkTreeStore		*info_store;
      GtkTreeIter		iter,		child;
      gchar			ids[100];
      gchar			name[1024];

      rv = saHpiRdrGet(sessionid, rid, id, &nextentryid, &rdr);
      if (rv != SA_OK) {
	    VOH_ERROR(err, "Rdr info getting failed", rv);
	    return NULL;
      }

      info_store = gtk_tree_store_new(2, G_TYPE_STRING, G_TYPE_STRING);

      gtk_tree_store_append(info_store, &iter, NULL);
      gtk_tree_store_set(info_store, &iter,
			 0, "Type",
			 1, vohRdrType2String(rdr.RdrType),
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

	    if (sensor->DataFormat.IsSupported == TRUE) {
		  gtk_tree_store_append(info_store, &iter, NULL);
		  gtk_tree_store_set(info_store, &iter,
				     0, "Sensor base unit",
				     1, vohSensorUnits2String(
					    sensor->DataFormat.BaseUnits),
				     -1);

		  gtk_tree_store_append(info_store, &iter, NULL);
		  gtk_tree_store_set(info_store, &iter,
				     0, "Modifier base unit",
				     1, vohSensorUnits2String(
					    sensor->DataFormat.ModifierUnits),
				     -1);

		  if (sensor->DataFormat.ModifierUse ==
		      			SAHPI_SMUU_BASIC_TIMES_MODIFIER) {
		  	sprintf(name, "%s*%s",
				vohSensorUnits2String(
				    sensor->DataFormat.BaseUnits),
				vohSensorUnits2String(
					    sensor->DataFormat.ModifierUnits));
		  } else if (sensor->DataFormat.ModifierUse ==
		      			SAHPI_SMUU_BASIC_OVER_MODIFIER) {
			sprintf(name, "%s/%s",
				vohSensorUnits2String(
				    sensor->DataFormat.BaseUnits),
				vohSensorUnits2String(
					    sensor->DataFormat.ModifierUnits));
		  } else {
			sprintf(name, "none");
		  }

		  gtk_tree_store_append(info_store, &iter, NULL);
		  gtk_tree_store_set(info_store, &iter,
					0, "Modifier use unit",
					1, name,
					-1);

		  if (sensor->DataFormat.Range.Flags & SAHPI_SRF_MIN) {
		       gtk_tree_store_append(info_store, &iter, NULL);
		       gtk_tree_store_set(info_store, &iter,
					  0, "Min value",
					  1, vohSensorValue2FullString(sensor,
						&sensor->DataFormat.Range.Min),
					  -1);
		  }

		  if (sensor->DataFormat.Range.Flags & SAHPI_SRF_MAX) {
		       gtk_tree_store_append(info_store, &iter, NULL);
		       gtk_tree_store_set(info_store, &iter,
					  0, "Max value",
					  1, vohSensorValue2FullString(sensor,
						&sensor->DataFormat.Range.Max),
					  -1);
		  }

		  if (sensor->DataFormat.Range.Flags & SAHPI_SRF_NORMAL_MIN) {
		       gtk_tree_store_append(info_store, &iter, NULL);
		       gtk_tree_store_set(info_store, &iter,
					  0, "Normal Min value",
					  1, vohSensorValue2FullString(sensor,
					     	&sensor->DataFormat.Range. \
								NormalMin),
					  -1);
		  }

		  if (sensor->DataFormat.Range.Flags & SAHPI_SRF_NORMAL_MAX) {
		       gtk_tree_store_append(info_store, &iter, NULL);
		       gtk_tree_store_set(info_store, &iter,
					  0, "Normal Max value",
					  1, vohSensorValue2FullString(sensor,
					     	&sensor->DataFormat.Range. \
								NormalMax),
					  -1);
		  }

		  if (sensor->DataFormat.Range.Flags & SAHPI_SRF_NOMINAL) {
		       gtk_tree_store_append(info_store, &iter, NULL);
		       gtk_tree_store_set(info_store, &iter,
					  0, "Nominal value",
					  1, vohSensorValue2FullString(sensor,
					     	&sensor->DataFormat.Range. \
								Nominal),
					  -1);
		  }
	    }

	    gtk_tree_store_append(info_store, &iter, NULL);
	    gtk_tree_store_set(info_store, &iter,
			       0, "Event states supported",
			       -1);
	    gtk_tree_store_append(info_store, &child, &iter);
	    gtk_tree_store_set(info_store, &child,
			       1, vohEventState2String(sensor->Events,
						       sensor->Category),
			       -1);

	    if (sensor->ThresholdDefn.IsAccessible == TRUE) {
		  gtk_tree_store_append(info_store, &iter, NULL);
		  gtk_tree_store_set(info_store, &iter,
				     0, "Thresholds",
				     -1);

		  gtk_tree_store_append(info_store, &child, &iter);
		  gtk_tree_store_set(info_store, &child,
				     0, "Readable thresholds",
				     1, vohSensorThdMask2String(
				            sensor->ThresholdDefn.ReadThold),
				     -1);

		  gtk_tree_store_append(info_store, &child, &iter);
		  gtk_tree_store_set(info_store, &child,
				     0, "Writable thresholds",
				     1, vohSensorThdMask2String(
				            sensor->ThresholdDefn.WriteThold),
				     -1);
	    }

	    break;
	default:
	    break;
      }

      return GTK_TREE_MODEL(info_store);
}

gint voh_read_sensor(GtkTreeStore *store, guint id, gchar *err)
{
      SaErrorT			rv;
      SaHpiRdrT			rdr;
      SaHpiEntryIdT		nextentryid;
      SaHpiSensorRecT		*sensor;
      SaHpiSensorReadingT	reading;
      GtkTreeIter		iter,	parent,	child;
      guint			rid,	state;
      gboolean			res;
 
      res = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(store), &iter);
      if (res == FALSE) {
	    VOH_ERROR(err, "Invalid parameters", -1);
	    return -1;
      }

      res = find_iter_by_id(GTK_TREE_MODEL(store), VOH_LIST_COLUMN_ID,
			    (guint)id, &iter);
      if (res == FALSE) {
	    VOH_ERROR(err, "Invalid parameters", -1);
	    return -1;
      }

      gtk_tree_model_iter_parent(GTK_TREE_MODEL(store), &parent, &iter);
      gtk_tree_model_get(GTK_TREE_MODEL(store), &parent,
			 VOH_LIST_COLUMN_ID, &rid, -1);

      rv = saHpiRdrGet(sessionid, rid, id, &nextentryid, &rdr);
      if (rv != SA_OK) {
	    VOH_ERROR(err, "Rdr info getting failed", rv);
	    return -1;
      }

      if (rdr.RdrType != SAHPI_SENSOR_RDR) {
	    VOH_ERROR(err, "Rdr entry isn't sensor", -1);
	    return -1;
      }

      sensor = &(rdr.RdrTypeUnion.SensorRec);

      rv = saHpiSensorReadingGet(sessionid, (SaHpiResourceIdT)rid,
				 sensor->Num, &reading, NULL);
      if (rv != SA_OK) {
	    VOH_ERROR(err, "reading sensor failed", rv);
	    return -1;
      }

      state = voh_get_sensor_state(sensor, &reading);

      gtk_tree_store_append(store, &child, &iter);
      gtk_tree_store_set(store, &child,
			 VOH_LIST_COLUMN_NAME,
			 	vohSensorValue2FullString(sensor, &reading),
			 VOH_LIST_COLUMN_ID, id,
			 VOH_LIST_COLUMN_TYPE, VOH_ITER_IS_VALUE,
			 VOH_LIST_COLUMN_STATE, state,
			 -1);
      gtk_tree_store_set(store, &iter,
			 VOH_LIST_COLUMN_STATE, state,
			 -1);

      return 0;
}

GtkTreeModel *voh_list_resources(gchar *err)
{
      GtkTreeStore	*pstore;
      SaHpiEntryIdT	rptentryid, nextrptentryid;
      SaErrorT		ret;
      SaHpiRptEntryT	rptentry;

      pstore = gtk_tree_store_new(VOH_LIST_NUM_COL, G_TYPE_STRING,
				  G_TYPE_UINT, G_TYPE_UINT, G_TYPE_UINT);

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
      gboolean		res,	first = TRUE;
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
		  else
			first = FALSE;
	    }

      } else {
	    i--;
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
      guint		type,	state = 0;
      gchar		name[100];

      if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(pstore), &parent)
	  							== FALSE)
	    return;

      res = find_iter_by_id(GTK_TREE_MODEL(pstore), VOH_LIST_COLUMN_ID,
			    (guint)rid, &parent);
      if (res != FALSE) {
	    strptr = rdr->IdString;
	    fixstr(&strptr, name);
	    switch (rdr->RdrType) {
	      case SAHPI_CTRL_RDR:
		  type = VOH_ITER_IS_CONTROL;
		  break;
	      case SAHPI_SENSOR_RDR:
		  type = VOH_ITER_IS_SENSOR;
		  state = VOH_ITER_SENSOR_STATE_UNSPECIFIED;
		  break;
	      case SAHPI_INVENTORY_RDR:
		  type = VOH_ITER_IS_INVENTORY;
		  break;
	      case SAHPI_WATCHDOG_RDR:
		  type = VOH_ITER_IS_WATCHDOG;
		  break;
	      case SAHPI_ANNUNCIATOR_RDR:
		  type = VOH_ITER_IS_ANNUNCIATOR;
		  break;
	      default:
		  type = VOH_ITER_IS_NO_RECORD;
		  break;
	    }

	    gtk_tree_store_append(pstore, &iter, &parent);
	    gtk_tree_store_set(pstore, &iter,
			       VOH_LIST_COLUMN_NAME, name,
			       VOH_LIST_COLUMN_ID, rdr->RecordId,
			       VOH_LIST_COLUMN_TYPE, type,
			       VOH_LIST_COLUMN_STATE, state,
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

guint voh_get_sensor_state(SaHpiSensorRecT *sensor, SaHpiSensorReadingT *sv)
{
      guint		state = VOH_ITER_SENSOR_STATE_NOMINAL;

      if (sv->IsSupported == FALSE) {
	    return VOH_ITER_SENSOR_STATE_UNSPECIFIED;
      }

      switch (sv->Type) {
	case SAHPI_SENSOR_READING_TYPE_INT64:
	    
	    if (sensor->DataFormat.Range.Flags & SAHPI_SRF_MIN) {
		  if (sv->Value.SensorInt64 <= sensor->DataFormat.Range.Min. \
							Value.SensorInt64) {
			return VOH_ITER_SENSOR_STATE_CRITICAL;
		  }
	    }

	    if (sensor->DataFormat.Range.Flags & SAHPI_SRF_MAX) {
		  if (sv->Value.SensorInt64 >= sensor->DataFormat.Range.Max. \
							Value.SensorInt64) {
			return VOH_ITER_SENSOR_STATE_CRITICAL;
		  }
	    }

	    if (sensor->DataFormat.Range.Flags & SAHPI_SRF_NORMAL_MIN) {
		  if (sv->Value.SensorInt64 <= sensor->DataFormat.Range. \
		      			NormalMin.Value.SensorInt64) {
			state = VOH_ITER_SENSOR_STATE_NORMAL;
		  }
	    }

	    if (sensor->DataFormat.Range.Flags & SAHPI_SRF_NORMAL_MAX) {
		  if (sv->Value.SensorInt64 >= sensor->DataFormat.Range. \
		      			NormalMax.Value.SensorInt64) {
			state = VOH_ITER_SENSOR_STATE_NORMAL;
		  }
	    }

	    break;
	case SAHPI_SENSOR_READING_TYPE_UINT64:
	    if (sensor->DataFormat.Range.Flags & SAHPI_SRF_MIN) {
		  if (sv->Value.SensorUint64 <= sensor->DataFormat.Range.Min. \
							Value.SensorUint64) {
			return VOH_ITER_SENSOR_STATE_CRITICAL;
		  }
	    }

	    if (sensor->DataFormat.Range.Flags & SAHPI_SRF_MAX) {
		  if (sv->Value.SensorUint64 >= sensor->DataFormat.Range.Max. \
							Value.SensorUint64) {
			return VOH_ITER_SENSOR_STATE_CRITICAL;
		  }
	    }

	    if (sensor->DataFormat.Range.Flags & SAHPI_SRF_NORMAL_MIN) {
		  if (sv->Value.SensorUint64 <= sensor->DataFormat.Range. \
		      			NormalMin.Value.SensorUint64) {
			state = VOH_ITER_SENSOR_STATE_NORMAL;
		  }
	    }

	    if (sensor->DataFormat.Range.Flags & SAHPI_SRF_NORMAL_MAX) {
		  if (sv->Value.SensorUint64 >= sensor->DataFormat.Range. \
		      			NormalMax.Value.SensorUint64) {
			state = VOH_ITER_SENSOR_STATE_NORMAL;
		  }
	    }

	    break;
	case SAHPI_SENSOR_READING_TYPE_FLOAT64:
	    if (sensor->DataFormat.Range.Flags & SAHPI_SRF_MIN) {
		  if (sv->Value.SensorFloat64 <= sensor->DataFormat.Range.Min. \
							Value.SensorFloat64) {
			return VOH_ITER_SENSOR_STATE_CRITICAL;
		  }
	    }

	    if (sensor->DataFormat.Range.Flags & SAHPI_SRF_MAX) {
		  if (sv->Value.SensorFloat64 >= sensor->DataFormat.Range.Max. \
							Value.SensorFloat64) {
			return VOH_ITER_SENSOR_STATE_CRITICAL;
		  }
	    }

	    if (sensor->DataFormat.Range.Flags & SAHPI_SRF_NORMAL_MIN) {
		  if (sv->Value.SensorFloat64 <= sensor->DataFormat.Range. \
		      			NormalMin.Value.SensorFloat64) {
			state = VOH_ITER_SENSOR_STATE_NORMAL;
		  }
	    }

	    if (sensor->DataFormat.Range.Flags & SAHPI_SRF_NORMAL_MAX) {
		  if (sv->Value.SensorFloat64 >= sensor->DataFormat.Range. \
		      			NormalMax.Value.SensorFloat64) {
			state = VOH_ITER_SENSOR_STATE_NORMAL;
		  }
	    }

	    break;
	default:
	    return VOH_ITER_SENSOR_STATE_UNSPECIFIED;
      }

      return state;
}


