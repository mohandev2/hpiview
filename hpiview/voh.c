#include "voh.h"
#include "hview_service.h"

#define VOH_ERROR(err, str, rv) if (err) { \
				    if (rv != -1) \
				        sprintf(err, "%s (error code: %s)", \
						str, vohError2String(rv)); \
				    else \
				        sprintf(err, "%s", str); \
				}
/*
static VohDomainsT	*domains = NULL;

static VohDomainsT *v_get_new_domain(VohDomainsT *dlist)
{
      VohDomainsT	*domain = NULL;

      if (dlist == NULL) {
	    return NULL;
      }

      while (dlist->next != NULL) {
	    dlist = dlist->next;
      }

      domain = (VohDomainsT *)calloc(sizeof(VohDomainsT));
      domain->domainid = domain->sessionid = 0;
      domain->next = NULL;

      dlist->next = domain;

      return domain;
}

static VohDomainsT *v_find_domain(SaHpiDomainIdT did, VohDomainsT *dlist)
{
      VohDomainsT	*domain = dlist;

      while (domain != NULL) {
	    if (domain->domainid == did)
		  break;
	    domain = domain->next;
      }

      return domain;
}

static void v_remove_domain(SaHpiDomainIdT did, VohDomainsT *dlist)
{
      VohDomainsT	*domain = dlist;
      VohDomainsT	*prevd = NULL;

      while (domain != NULL) {
	    if (domain->domainid == did)
		  break;
	    prevd = domain;
	    domain = domain->next;
      }

      if (domain != NULL) {
	    if (prevd != NULL) {
		  prevd->next = domain->next;
	    }
	    free(domain);
      }
}
*/

static void fixstr(SaHpiTextBufferT *strptr, char *outbuff)
{
      size_t		datalen = strptr->DataLength;
      char		*str = (char *)strptr->Data;

      if (datalen != 0) {
	    strncpy(outbuff, (char *)str, datalen);
      }
      *(outbuff + datalen) = '\0';
}

guint voh_open_session(guint domainid, gchar *err)
{
      SaErrorT		rv;
      SaHpiDomainIdT	did = (SaHpiDomainIdT)domainid;
      SaHpiSessionIdT	sessionid;

      rv = saHpiSessionOpen(did, &sessionid, NULL);
      if (rv != SA_OK) {
	    VOH_ERROR(err, "Session open failed", rv);
	    return 0;
      }

      return sessionid;
}

gboolean voh_discover(guint sessionid, gchar *err)
{
      SaErrorT		rv;
      SaHpiSessionIdT	sid = (SaHpiSessionIdT)sessionid;

      rv = saHpiDiscover(sid);
      if (rv != SA_OK) {
	    VOH_ERROR(err, "Discovering failed", rv);
	    return FALSE;
      }

      return TRUE;
}

gboolean voh_session_close(guint sessionid, gchar *err)
{
      SaErrorT		rv;
      SaHpiSessionIdT	sid = (SaHpiSessionIdT)sessionid;

      rv = saHpiSessionClose(sid);
      if (rv != SA_OK) {
	    VOH_ERROR(err, "Session close failed", rv);
	    return FALSE;
      }

      return TRUE;
}

static void v_get_infra(SaHpiDomainIdT did, GtkTreeStore *store,
			GtkTreeIter *parent)
{
      SaErrorT			rv;
      SaHpiDomainInfoT		info;
      SaHpiEntryIdT		entryid, nextentryid;
      SaHpiDrtEntryT		drtentry;
      SaHpiSessionIdT		session;
      GtkTreeIter		iter;
      gchar			name[1024];

      if (store == NULL)
	    return;

      rv = saHpiSessionOpen(did, &session, NULL);

      if (rv != SA_OK)
	    return;

      gtk_tree_store_append(store, &iter, parent);

      entryid = SAHPI_FIRST_ENTRY;
      while (entryid != SAHPI_LAST_ENTRY) {
	    rv = saHpiDrtEntryGet(session, entryid, &nextentryid, &drtentry);
	    if (rv != SA_OK)
		  break;
	    v_get_infra(drtentry.DomainId, store, &iter);
	    entryid = nextentryid;
      }

      rv = saHpiDomainInfoGet(session, &info);
      if (rv == SA_OK) {
	    fixstr(&(info.DomainTag), name);
      } else {
	    sprintf(name, "Unknown");
      }


      gtk_tree_store_set(store, &iter,
			 VOH_LIST_COLUMN_NAME, name,
			 VOH_LIST_COLUMN_ID, (guint)did,
			 VOH_LIST_COLUMN_TYPE, VOH_ITER_IS_DOMAIN,
			 -1);

      saHpiSessionClose(session);
}

GtkTreeModel *voh_list_domains(gchar *err)
{
      GtkTreeStore	*pstore;

     pstore = gtk_tree_store_new(VOH_LIST_NUM_COL, G_TYPE_STRING,
				  GDK_TYPE_PIXBUF, G_TYPE_UINT, G_TYPE_UINT,
				  G_TYPE_UINT);
 
      v_get_infra(SAHPI_UNSPECIFIED_DOMAIN_ID, pstore, NULL);

      return GTK_TREE_MODEL(pstore);
}

/*
GtkTreeModel *voh_domain_info(guint domainid, gchar *err)
{
      SaErrorT		rv;
      GtkTreeStore	*info_store;
      GtkTreeIter	iter;
      SaHpiDomainInfoT	info;
      SaHpiTextBufferT	strptr;
      SaHpiDomainIdT	did = (SaHpiDomainIdT)domainid;
      char		outbuff[1024];

      domain = v_find_domain(did, domains);
      if (domain == NULL) {
	    VOH_ERROR(err, "Domain info getting failed (invalid argument)", -1);
	    return NULL;
      }

      if ((rv = saHpiDomainInfoGet(domain->sessionid, &info)) != SA_OK) {
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
*/

GtkTreeModel *voh_resource_info(guint sessionid, guint resourceid, gchar *err)
{
      SaErrorT		rv;
      SaHpiRptEntryT	entry;
      SaHpiResetActionT	reset;
      SaHpiPowerStateT	power;
      SaHpiSessionIdT	sid = (SaHpiSessionIdT)sessionid;
      SaHpiResourceIdT	rid = (SaHpiResourceIdT)resourceid;
      GtkTreeStore	*info_store;
      GtkTreeIter	iter;
      gchar		name[1024];

      rv = saHpiRptEntryGetByResourceId(sid, rid, &entry);
      if (rv != SA_OK) {
	    VOH_ERROR(err, "Resource info getting failed", rv);
	    return NULL;
      }

      info_store = gtk_tree_store_new(2, G_TYPE_STRING, G_TYPE_STRING);

      gtk_tree_store_append(info_store, &iter, NULL);
      sprintf(name, "%d", rid);
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

      if (entry.ResourceCapabilities & SAHPI_CAPABILITY_RESET) {
	    gtk_tree_store_append(info_store, &iter, NULL);
	    gtk_tree_store_set(info_store, &iter,
			       0, "Resource reset state",
			       -1);
	    
	    rv = saHpiResourceResetStateGet(sid, rid, &reset);
	    if (rv == SA_OK) {
		  gtk_tree_store_set(info_store, &iter,
			       1, vohResetAction2String(reset),
			       -1);
	    } else {
		  gtk_tree_store_set(info_store, &iter,
			       1, "unknown",
			       -1);
	    }
      }

      return GTK_TREE_MODEL(info_store);
}

GtkTreeModel *voh_rdr_info(guint sessionid, guint resourceid,
			   guint rdrentryid, gchar *err)
{
      SaErrorT			rv;
      SaHpiRdrT			rdr;
      SaHpiEntryIdT		nextentryid;
      SaHpiSensorRecT		*sensor;
      SaHpiSensorThresholdsT	thresholds;
      SaHpiSessionIdT		sid = (SaHpiSessionIdT)sessionid;
      SaHpiResourceIdT		rid = (SaHpiResourceIdT)resourceid;
      SaHpiEntryIdT		rdrid = (SaHpiEntryIdT)rdrentryid;
      GtkTreeStore		*info_store;
      GtkTreeIter		iter,		child;
      gchar			ids[100];
      gchar			name[1024];

      rv = saHpiRdrGet(sid, rid, rdrid, &nextentryid, &rdr);
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
		rv = saHpiSensorThresholdsGet(sid, rid, sensor->Num,
					      &thresholds);
		if (rv != SA_OK) {
		      VOH_ERROR(err, "Thresholds getting failed", rv);
		} else {

		      gtk_tree_store_append(info_store, &iter, NULL);
		      gtk_tree_store_set(info_store, &iter,
					 0, "Thresholds",
					 -1);

		      sprintf(name, "Low Minor %s",
			      vohReadWriteThds2String(
					sensor->ThresholdDefn.ReadThold,
					sensor->ThresholdDefn.WriteThold,
					SAHPI_STM_LOW_MINOR));
		      gtk_tree_store_append(info_store, &child, &iter);
		      gtk_tree_store_set(info_store, &child,
					 0, name,
					 1, vohSensorValue2FullString(
					     sensor, &thresholds.LowMinor),
					 -1);

		      sprintf(name, "Up Minor %s",
			      vohReadWriteThds2String(
					sensor->ThresholdDefn.ReadThold,
					sensor->ThresholdDefn.WriteThold,
					SAHPI_STM_UP_MINOR));
		      gtk_tree_store_append(info_store, &child, &iter);
		      gtk_tree_store_set(info_store, &child,
					 0, name,
					 1, vohSensorValue2FullString(sensor,
					     &thresholds.UpMinor),
					 -1);

		      sprintf(name, "Low Major %s",
			      vohReadWriteThds2String(
					sensor->ThresholdDefn.ReadThold,
					sensor->ThresholdDefn.WriteThold,
					SAHPI_STM_LOW_MAJOR));
		      gtk_tree_store_append(info_store, &child, &iter);
		      gtk_tree_store_set(info_store, &child,
					 0, name,
					 1, vohSensorValue2FullString(sensor,
					     &thresholds.LowMajor),
					 -1);

		      sprintf(name, "Up Major %s",
			      vohReadWriteThds2String(
					sensor->ThresholdDefn.ReadThold,
					sensor->ThresholdDefn.WriteThold,
					SAHPI_STM_UP_MAJOR));
		      gtk_tree_store_append(info_store, &child, &iter);
		      gtk_tree_store_set(info_store, &child,
					 0, name,
					 1, vohSensorValue2FullString(sensor,
					     &thresholds.UpMajor),
					 -1);

		      sprintf(name, "Low Critical %s",
			      vohReadWriteThds2String(
					sensor->ThresholdDefn.ReadThold,
					sensor->ThresholdDefn.WriteThold,
					SAHPI_STM_LOW_CRIT));
		      gtk_tree_store_append(info_store, &child, &iter);
		      gtk_tree_store_set(info_store, &child,
					 0, name,
					 1, vohSensorValue2FullString(sensor,
					     &thresholds.LowCritical),
					 -1);

		      sprintf(name, "Up Critical %s",
			      vohReadWriteThds2String(
					sensor->ThresholdDefn.ReadThold,
					sensor->ThresholdDefn.WriteThold,
					SAHPI_STM_UP_CRIT));
		      gtk_tree_store_append(info_store, &child, &iter);
		      gtk_tree_store_set(info_store, &child,
					 0, name,
					 1, vohSensorValue2FullString(sensor,
					     &thresholds.UpCritical),
					 -1);

		      sprintf(name, "Low Hysteresis %s",
			      vohReadWriteThds2String(
					sensor->ThresholdDefn.ReadThold,
					sensor->ThresholdDefn.WriteThold,
					SAHPI_STM_LOW_HYSTERESIS));
		      gtk_tree_store_append(info_store, &child, &iter);
		      gtk_tree_store_set(info_store, &child,
					 0, name,
					 1, vohSensorValue2FullString(sensor,
					     &thresholds.NegThdHysteresis),
					 -1);

		      sprintf(name, "Up Hysteresis %s",
			      vohReadWriteThds2String(
					sensor->ThresholdDefn.ReadThold,
					sensor->ThresholdDefn.WriteThold,
					SAHPI_STM_UP_HYSTERESIS));
		      gtk_tree_store_append(info_store, &child, &iter);
		      gtk_tree_store_set(info_store, &child,
					 0, name,
					 1, vohSensorValue2FullString(sensor,
					     &thresholds.PosThdHysteresis),
					 -1);
		}

/*
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
				     -1);*/
	    }

	    break;
	default:
	    break;
      }

      gtk_tree_store_append(info_store, &iter, NULL);

      return GTK_TREE_MODEL(info_store);
}

gboolean voh_read_sensor(GtkTreeStore *store, guint sessionid,
			 guint entryid, gchar *err)
{
      SaErrorT			rv;
      SaHpiRdrT			rdr;
      SaHpiEntryIdT		nextentryid;
      SaHpiSensorRecT		*sensor;
      SaHpiSensorReadingT	reading;
      SaHpiSessionIdT		sid = (SaHpiSessionIdT)sessionid;
      SaHpiEntryIdT		enid = (SaHpiEntryIdT)entryid;
      GtkTreeIter		iter,	parent,	child;
      guint			resourceid,	state;
      gboolean			res;

      res = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(store), &iter);
      if (res == FALSE) {
	    VOH_ERROR(err, "Reading sensor failed (invalid argument)", -1);
	    return FALSE;
      }

      res = find_iter_by_id(GTK_TREE_MODEL(store), VOH_LIST_COLUMN_ID,
			    entryid, &iter);
      if (res == FALSE) {
	    VOH_ERROR(err, "Reading sensor failed (invalid argument)", -1);
	    return FALSE;
      }

      gtk_tree_model_iter_parent(GTK_TREE_MODEL(store), &parent, &iter);
      gtk_tree_model_get(GTK_TREE_MODEL(store), &parent,
			 VOH_LIST_COLUMN_ID, &resourceid, -1);

      rv = saHpiRdrGet(sid, (SaHpiResourceIdT)resourceid,
		       enid, &nextentryid, &rdr);
      if (rv != SA_OK) {
	    VOH_ERROR(err, "Reading sensor failed", rv);
	    return FALSE;
      }

      if (rdr.RdrType != SAHPI_SENSOR_RDR) {
	    VOH_ERROR(err, "Reading sensor failed (invalid argument)", -1);
	    return FALSE;
      }

      sensor = &(rdr.RdrTypeUnion.SensorRec);

      rv = saHpiSensorReadingGet(sid, resourceid, sensor->Num, &reading, NULL);
      if (rv != SA_OK) {
	    VOH_ERROR(err, "Reading sensor failed", rv);
	    return FALSE;
      }

      state = voh_get_sensor_state(sensor, &reading);

      gtk_tree_store_append(store, &child, &iter);
      gtk_tree_store_set(store, &child,
			 VOH_LIST_COLUMN_NAME,
			 	vohSensorValue2FullString(sensor, &reading),
			 VOH_LIST_COLUMN_ID, entryid,
			 VOH_LIST_COLUMN_TYPE, VOH_ITER_IS_VALUE,
			 VOH_LIST_COLUMN_STATE, state,
			 -1);
      gtk_tree_store_set(store, &iter,
			 VOH_LIST_COLUMN_STATE, state,
			 -1);

      return TRUE;
}

GtkTreeModel *voh_list_resources(guint sessionid, gchar *err)
{
      GtkTreeStore	*pstore;
      SaHpiEntryIdT	rptentryid, nextrptentryid;
      SaErrorT		ret;
      SaHpiRptEntryT	rptentry;
      SaHpiSessionIdT	sid = (SaHpiSessionIdT)sessionid;

      pstore = gtk_tree_store_new(VOH_LIST_NUM_COL, G_TYPE_STRING,
				  GDK_TYPE_PIXBUF, G_TYPE_UINT, G_TYPE_UINT,
				  G_TYPE_UINT);

      rptentryid = SAHPI_FIRST_ENTRY;
      while (rptentryid != SAHPI_LAST_ENTRY) {
	    ret = saHpiRptEntryGet(sid, rptentryid, &nextrptentryid, &rptentry);
	    if (ret != SA_OK) {
		  break;
	    }
	    voh_add_resource(pstore, sessionid, &rptentry);
	    voh_list_rdrs(pstore, sessionid, (guint)rptentry.ResourceId, err);
	    rptentryid = nextrptentryid;
      }

      return (GTK_TREE_MODEL(pstore));
}

gboolean voh_list_rdrs(GtkTreeStore *pstore, guint sessionid,
		       guint resourceid, gchar *err)
{
      SaHpiRdrT			rdr;
      SaErrorT			ret;
      SaHpiEntryIdT		entryid;
      SaHpiEntryIdT		nextentryid;
      SaHpiSessionIdT		sid = (SaHpiSessionIdT)sessionid;
      SaHpiResourceIdT		rid = (SaHpiResourceIdT)resourceid;

      entryid = SAHPI_FIRST_ENTRY;
      while (entryid != SAHPI_LAST_ENTRY) {
	    ret = saHpiRdrGet(sid, rid, entryid, &nextentryid, &rdr);
	    if (ret != SA_OK)
		  return FALSE;
	    voh_add_rdr(pstore, &rdr, resourceid);
	    entryid = nextentryid;
      }
      return TRUE;
}

void voh_add_resource(GtkTreeStore *pstore, guint sessionid,
		      SaHpiRptEntryT *rpt)
{
      SaErrorT		rv;
      SaHpiPowerStateT	power;
      SaHpiResetActionT	reset;
      SaHpiSessionIdT	sid = (SaHpiSessionIdT)sessionid;
      GtkTreeIter	iter,	parent,	*found_iter;
      gint		i;
      guint		id,	type;
      gboolean		res,	first = TRUE;
      GdkPixbuf		*image = NULL;
      GError		*error = NULL;
      SaHpiEntityPathT	entity_path = rpt->ResourceEntity;
      guint		state = 0;
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

	    if (rpt->ResourceCapabilities & SAHPI_CAPABILITY_POWER) {
		  rv = saHpiResourcePowerStateGet(sid, (SaHpiResourceIdT)id,
						  &power);
		  if (rv == SA_OK) {
			switch (power) {
			  case SAHPI_POWER_ON:
			      state |= VOH_ITER_RPT_STATE_POWER_ON;
			      image = gdk_pixbuf_new_from_file(
					find_pixmap_file("on.png"), &error);
			      break;
			  case SAHPI_POWER_OFF:
			      state |= VOH_ITER_RPT_STATE_POWER_OFF;
			      image = gdk_pixbuf_new_from_file(
					find_pixmap_file("off.png"), &error);
			      break;
			}
			if (error) {
			      g_critical ("Could not load pixbuf: %s\n",
					  error->message);
			      g_error_free(error);
			}
			if (image) {
			      gtk_tree_store_set(GTK_TREE_STORE(pstore), &iter,
						 VOH_LIST_COLUMN_ICON, image,
						 -1);
			      g_object_unref(image);
			}
		  }
	    }

	    if (rpt->ResourceCapabilities & SAHPI_CAPABILITY_RESET) {
		  rv = saHpiResourceResetStateGet(sid, (SaHpiResourceIdT)id,
						  &reset);
		  if (rv == SA_OK) { 
			switch (reset) {
			  case SAHPI_RESET_ASSERT:
			      state |= VOH_ITER_RPT_STATE_RESET_ASSERT;
			      break;
			  case SAHPI_RESET_DEASSERT:
			      state |= VOH_ITER_RPT_STATE_RESET_DEASSERT;
			      break;
			}
		  } else
			state |= VOH_ITER_RPT_STATE_RESET_DEASSERT;
	    }
	    gtk_tree_store_set(pstore, &iter,
			       VOH_LIST_COLUMN_NAME, path,
			       VOH_LIST_COLUMN_ID, id,
			       VOH_LIST_COLUMN_TYPE, type,
			       VOH_LIST_COLUMN_STATE, state,
			       -1);
	   parent = iter;
	   first = FALSE;
      }
}

void voh_add_rdr(GtkTreeStore *pstore, SaHpiRdrT *rdr, guint resourceid)
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
			    resourceid, &parent);
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
			       VOH_LIST_COLUMN_ID, (guint)rdr->RecordId,
			       VOH_LIST_COLUMN_TYPE, type,
			       VOH_LIST_COLUMN_STATE, state,
			       -1);
      }
}

gboolean find_iter_by_id(GtkTreeModel *model, guint column_num,
			 guint req_id, GtkTreeIter *iter)
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


gboolean find_iter_by_name(GtkTreeModel *model, guint column_num,
			   const gchar *req_name, GtkTreeIter *iter)
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

/*
gboolean voh_get_power_state(guint domainid, guint resourceid,
			     GtkWidget *menu, gchar *err)
{
      SaErrorT		rv;
      SaHpiPowerStateT	power;
      SaHpiDomainIdT	did = (SaHpiDomainIdT)domainid;
      SaHpiResourceIdT	rid = (SaHpiResourceIdT)resourceid;
      GtkWidget		*sitem, *item;
      VohDomainsT	*domain;

      domain = v_find_domain(did, domains);
      if (domain == NULL) {
	    VOH_ERROR(err, "Power state getting getting failed \
		      				(invalid argument)", -1);
	    return FALSE;
      }

      rv = saHpiResourcePowerStateGet(domain->sessionid, rid, &power);
      item = gtk_radio_menu_item_new_with_mnemonic(NULL, "none");
//      gtk_container_add(GTK_CONTAINER(menu), sitem);

      sitem = gtk_radio_menu_item_new_with_mnemonic_from_widget(
				GTK_RADIO_MENU_ITEM(item), "on");

      gtk_container_add(GTK_CONTAINER(menu), sitem);
      if (power == SAHPI_POWER_ON)
	    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(sitem), TRUE);
      sitem = gtk_radio_menu_item_new_with_mnemonic_from_widget(
				GTK_RADIO_MENU_ITEM(item), "off");

      gtk_check_menu_item_set_show_toggle(GTK_CHECK_MENU_ITEM(sitem), TRUE);
      gtk_container_add(GTK_CONTAINER(menu), sitem);

      if (power == SAHPI_POWER_OFF)
	    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(sitem), TRUE);

      if (rv != SA_OK) {
	    VOH_ERROR(err, "Power state getting failed", rv);
	    return FALSE;
      }
      return TRUE;
}
*/

gboolean voh_set_power_off(guint sessionid, guint resourceid,
			   GtkTreeStore *store, gchar *err)
{
      SaErrorT		rv;
      SaHpiPowerStateT	power;
      SaHpiSessionIdT	sid = (SaHpiSessionIdT)sessionid;
      SaHpiResourceIdT	rid = (SaHpiResourceIdT)resourceid;
      GtkTreeIter	iter;
      GdkPixbuf		*image = NULL;
      GError		*error = NULL;
      guint		state;
      gboolean		res;
      guint		i;

      if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(store), &iter)
	  							== FALSE) {
	    VOH_ERROR(err, "", -1);
	    return FALSE;
      }
      res = find_iter_by_id(GTK_TREE_MODEL(store), VOH_LIST_COLUMN_ID,
			    resourceid, &iter);
      if (res == FALSE) {
	    VOH_ERROR(err, "Power state setting failed (invalid resource id)",
		      -1);
	    return FALSE;
      }
      rv = saHpiResourcePowerStateSet(sid, rid, SAHPI_POWER_OFF);
      if (rv != SA_OK) {
	    VOH_ERROR(err, "Power state setting failed", rv);
	    return FALSE;
      }

      gtk_tree_model_get(GTK_TREE_MODEL(store), &iter,
			 VOH_LIST_COLUMN_STATE, &state,
			 -1);

      for (i = 1; i < 4; i++) {
	    rv = saHpiResourcePowerStateGet(sid, rid, &power);
	    if (rv != SA_OK) {
		  VOH_ERROR(err, "Power state setting failed", rv);
		  return FALSE;
	    }
	    if (power != SAHPI_POWER_OFF) {
		  sleep (i * 3);
	    }
      }
      if (power != SAHPI_POWER_OFF) {
	    VOH_ERROR(err, "Power state setting failed (unknown)", -1);
	    return FALSE;
      }
      state &=~VOH_ITER_RPT_STATE_POWER_ON;
      state |= VOH_ITER_RPT_STATE_POWER_OFF;

      gtk_tree_store_set(store, &iter,
			 VOH_LIST_COLUMN_STATE, state,
			 -1);

      image = gdk_pixbuf_new_from_file(find_pixmap_file("off.png"), &error);
      if (error) {
	    g_critical ("Could not load pixbuf: %s\n", error->message);
	    g_error_free(error);
      }
      if (image) {
	    gtk_tree_store_set(store, &iter,
			       VOH_LIST_COLUMN_ICON, image,
			       -1);
	    g_object_unref(image);
      }
      return TRUE;
}

gboolean voh_set_power_on(guint sessionid, guint resourceid,
			  GtkTreeStore *store, gchar *err)
{
      SaErrorT		rv;
      SaHpiPowerStateT	power;
      SaHpiSessionIdT	sid = (SaHpiSessionIdT)sessionid;
      SaHpiResourceIdT	rid = (SaHpiResourceIdT)resourceid;
      GtkTreeIter	iter;
      GdkPixbuf		*image = NULL;
      GError		*error = NULL;
      guint		state;
      gboolean		res;
      guint		i;

      if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(store), &iter)
	  							== FALSE) {
	    VOH_ERROR(err, "", -1);
	    return FALSE;
      }
      res = find_iter_by_id(GTK_TREE_MODEL(store), VOH_LIST_COLUMN_ID,
			    resourceid, &iter);
      if (res == FALSE) {
	    VOH_ERROR(err, "Power state setting failed (invalid argument)",
		      -1);
	    return FALSE;
      }
      rv = saHpiResourcePowerStateSet(sid, rid, SAHPI_POWER_ON);
      if (rv != SA_OK) {
	    VOH_ERROR(err, "Power state setting failed", rv);
	    return FALSE;
      }


      for (i = 1; i < 4; i++) {
	    rv = saHpiResourcePowerStateGet(sid, rid, &power);
	    if (rv != SA_OK) {
		  VOH_ERROR(err, "Power state setting failed", rv);
		  return FALSE;
	    }
	    if (power != SAHPI_POWER_ON) {
		  sleep (i * 3);
	    }
      }
      if (power != SAHPI_POWER_ON) {
	    VOH_ERROR(err, "Power state setting failed (unknown)", -1);
	    return FALSE;
      }
      gtk_tree_model_get(GTK_TREE_MODEL(store), &iter,
			 VOH_LIST_COLUMN_STATE, &state,
			 -1);
      state &=~VOH_ITER_RPT_STATE_POWER_OFF;
      state |= VOH_ITER_RPT_STATE_POWER_ON;
      gtk_tree_store_set(store, &iter,
			 VOH_LIST_COLUMN_STATE, state,
			 -1);
      image = gdk_pixbuf_new_from_file(find_pixmap_file("on.png"), &error);
      if (error) {
	    g_critical ("Could not load pixbuf: %s\n", error->message);
	    g_error_free(error);
      }
      if (image) {
	    gtk_tree_store_set(store, &iter,
			       VOH_LIST_COLUMN_ICON, image,
			       -1);
	    g_object_unref(image);
      }
      return TRUE;
}

gboolean voh_set_power_cycle(guint sessionid, guint resourceid,
			     GtkTreeStore *store, gchar *err)
{
      SaErrorT		rv;
      SaHpiPowerStateT	power;
      SaHpiSessionIdT	sid = (SaHpiSessionIdT)sessionid;
      SaHpiResourceIdT	rid = (SaHpiResourceIdT)resourceid;
      GtkTreeIter	iter;
      GdkPixbuf		*image = NULL;
      GError		*error = NULL;
      guint		state;
      gboolean		res;
      guint		i;

      if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(store), &iter)
	  							== FALSE) {
	    VOH_ERROR(err, "", -1);
	    return FALSE;
      }
      res = find_iter_by_id(GTK_TREE_MODEL(store), VOH_LIST_COLUMN_ID,
			    resourceid, &iter);
      if (res == FALSE) {
	    VOH_ERROR(err, "Power state setting failed (invalid resource id)",
		      -1);
	    return FALSE;
      }
      rv = saHpiResourcePowerStateSet(sid, rid, SAHPI_POWER_CYCLE);
      if (rv != SA_OK) {
	    VOH_ERROR(err, "Power state setting failed", rv);
	    return FALSE;
      }


      for (i = 1; i < 4; i++) {
	    rv = saHpiResourcePowerStateGet(sid, rid, &power);
	    if (rv != SA_OK) {
		  VOH_ERROR(err, "Power state setting failed", rv);
		  return FALSE;
	    }
	    if (power != SAHPI_POWER_ON) {
		  sleep (i * 5);
	    }
      }
      gtk_tree_model_get(GTK_TREE_MODEL(store), &iter,
			 VOH_LIST_COLUMN_STATE, &state,
			 -1);
      if (power == SAHPI_POWER_ON) {
	    state &=~VOH_ITER_RPT_STATE_POWER_OFF;
	    state |= VOH_ITER_RPT_STATE_POWER_ON;
	    image = gdk_pixbuf_new_from_file(find_pixmap_file("on.png"),
					     &error);
      } else if (power == SAHPI_POWER_OFF) {
	    state &=~VOH_ITER_RPT_STATE_POWER_ON;
	    state |= VOH_ITER_RPT_STATE_POWER_OFF;
	    image = gdk_pixbuf_new_from_file(find_pixmap_file("off.png"),
					     &error);
      }

      if (error) {
	    g_critical ("Could not load pixbuf: %s\n", error->message);
	    g_error_free(error);
      }
 
      gtk_tree_store_set(store, &iter,
			 VOH_LIST_COLUMN_STATE, state,
			 VOH_LIST_COLUMN_ICON, image,
			 -1);
      if (image) {
	    g_object_unref(image);
      }
      if (power != SAHPI_POWER_ON) {
	    VOH_ERROR(err, "Power state setting failed (unknown)", -1);
	    return FALSE;
      }
      return TRUE;
}

gboolean voh_set_reset_cold(guint sessionid, guint resourceid,
			    GtkTreeStore *store, gchar *err)
{
      SaErrorT		rv;
      SaHpiResetActionT	reset;
      SaHpiSessionIdT	sid = (SaHpiSessionIdT)sessionid;
      SaHpiResourceIdT	rid = (SaHpiResourceIdT)resourceid;
      GtkTreeIter	iter;
      guint		state;
      gboolean		res;

      if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(store), &iter)
	  							== FALSE) {
	    VOH_ERROR(err, "", -1);
	    return FALSE;
      }
      res = find_iter_by_id(GTK_TREE_MODEL(store), VOH_LIST_COLUMN_ID,
			    resourceid, &iter);
      if (res == FALSE) {
	    VOH_ERROR(err, "Reset state setting failed (invalid resource id)",
		      -1);
	    return FALSE;
      }
      rv = saHpiResourceResetStateSet(sid, rid, SAHPI_COLD_RESET);
      if (rv != SA_OK) {
	    VOH_ERROR(err, "Reset state setting failed", rv);
	    return FALSE;
      }


      rv = saHpiResourceResetStateGet(sid, rid, &reset);
      if (rv != SA_OK) {
	    return TRUE;
      }
      gtk_tree_model_get(GTK_TREE_MODEL(store), &iter,
			 VOH_LIST_COLUMN_STATE, &state,
			 -1);
      if (reset == SAHPI_RESET_ASSERT) {
	    state &=~VOH_ITER_RPT_STATE_RESET_DEASSERT;
	    state |= VOH_ITER_RPT_STATE_RESET_ASSERT;
      } else if (reset == SAHPI_RESET_DEASSERT) {
	    state &=~VOH_ITER_RPT_STATE_RESET_ASSERT;
	    state |= VOH_ITER_RPT_STATE_RESET_DEASSERT;
      }
      gtk_tree_store_set(store, &iter,
			 VOH_LIST_COLUMN_STATE, state,
			 -1);
      return TRUE;
}

gboolean voh_set_reset_warm(guint sessionid, guint resourceid,
			    GtkTreeStore *store, gchar *err)
{
      SaErrorT		rv;
      SaHpiResetActionT	reset;
      SaHpiSessionIdT	sid = (SaHpiSessionIdT)sessionid;
      SaHpiResourceIdT	rid = (SaHpiResourceIdT)resourceid;
      GtkTreeIter	iter;
      guint		state;
      gboolean		res;

      if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(store), &iter)
	  							== FALSE) {
	    VOH_ERROR(err, "", -1);
	    return FALSE;
      }
      res = find_iter_by_id(GTK_TREE_MODEL(store), VOH_LIST_COLUMN_ID,
			    resourceid, &iter);
      if (res == FALSE) {
	    VOH_ERROR(err, "Reset state setting failed (invalid resource id)",
		      -1);
	    return FALSE;
      }
      rv = saHpiResourceResetStateSet(sid, rid, SAHPI_WARM_RESET);
      if (rv != SA_OK) {
	    VOH_ERROR(err, "Reset state setting failed", rv);
	    return FALSE;
      }


      rv = saHpiResourceResetStateGet(sid, rid, &reset);
      if (rv != SA_OK) {
	    return TRUE;
      }
      gtk_tree_model_get(GTK_TREE_MODEL(store), &iter,
			 VOH_LIST_COLUMN_STATE, &state,
			 -1);
      if (reset == SAHPI_RESET_ASSERT) {
	    state &=~VOH_ITER_RPT_STATE_RESET_DEASSERT;
	    state |= VOH_ITER_RPT_STATE_RESET_ASSERT;
      } else if (reset == SAHPI_RESET_DEASSERT) {
	    state &=~VOH_ITER_RPT_STATE_RESET_ASSERT;
	    state |= VOH_ITER_RPT_STATE_RESET_DEASSERT;
      }
      gtk_tree_store_set(store, &iter,
			 VOH_LIST_COLUMN_STATE, state,
			 -1);
      return TRUE;
}

gboolean voh_set_reset_assert(guint sessionid, guint resourceid,
			      GtkTreeStore *store, gchar *err)
{
      SaErrorT		rv;
      SaHpiResetActionT	reset;
      SaHpiSessionIdT	sid = (SaHpiSessionIdT)sessionid;
      SaHpiResourceIdT	rid = (SaHpiResourceIdT)resourceid;
      GtkTreeIter	iter;
      guint		state;
      gboolean		res;

      if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(store), &iter)
	  							== FALSE) {
	    VOH_ERROR(err, "", -1);
	    return FALSE;
      }
      res = find_iter_by_id(GTK_TREE_MODEL(store), VOH_LIST_COLUMN_ID,
			    resourceid, &iter);
      if (res == FALSE) {
	    VOH_ERROR(err, "Reset state setting failed (invalid resource id)",
		      -1);
	    return FALSE;
      }
      rv = saHpiResourceResetStateSet(sid, rid, SAHPI_RESET_ASSERT);
      if (rv != SA_OK) {
	    VOH_ERROR(err, "Reset state setting failed", rv);
	    return FALSE;
      }


      rv = saHpiResourceResetStateGet(sid, rid, &reset);
      if (rv != SA_OK) {
	    return TRUE;
      }
      gtk_tree_model_get(GTK_TREE_MODEL(store), &iter,
			 VOH_LIST_COLUMN_STATE, &state,
			 -1);
      if (reset == SAHPI_RESET_ASSERT) {
	    state &=~VOH_ITER_RPT_STATE_RESET_DEASSERT;
	    state |= VOH_ITER_RPT_STATE_RESET_ASSERT;
      } else if (reset == SAHPI_RESET_DEASSERT) {
	    state &=~VOH_ITER_RPT_STATE_RESET_ASSERT;
	    state |= VOH_ITER_RPT_STATE_RESET_DEASSERT;
      }
      gtk_tree_store_set(store, &iter,
			 VOH_LIST_COLUMN_STATE, state,
			 -1);
      return TRUE;
}

gboolean voh_set_reset_deassert(guint sessionid, guint resourceid,
				GtkTreeStore *store, gchar *err)
{
      SaErrorT		rv;
      SaHpiResetActionT	reset;
      SaHpiSessionIdT	sid = (SaHpiSessionIdT)sessionid;
      SaHpiResourceIdT	rid = (SaHpiResourceIdT)resourceid;
      GtkTreeIter	iter;
      guint		state;
      gboolean		res;

      if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(store), &iter)
	  							== FALSE) {
	    VOH_ERROR(err, "", -1);
	    return FALSE;
      }
      res = find_iter_by_id(GTK_TREE_MODEL(store), VOH_LIST_COLUMN_ID,
			    resourceid, &iter);
      if (res == FALSE) {
	    VOH_ERROR(err, "Reset state setting failed (invalid resource id)",
		      -1);
	    return FALSE;
      }
      rv = saHpiResourceResetStateSet(sid, rid, SAHPI_RESET_DEASSERT);
      if (rv != SA_OK) {
	    VOH_ERROR(err, "Reset state setting failed", rv);
	    return FALSE;
      }


      rv = saHpiResourceResetStateGet(sid, rid, &reset);
      if (rv != SA_OK) {
	    return TRUE;
      }
      gtk_tree_model_get(GTK_TREE_MODEL(store), &iter,
			 VOH_LIST_COLUMN_STATE, &state,
			 -1);
      if (reset == SAHPI_RESET_ASSERT) {
	    state &=~VOH_ITER_RPT_STATE_RESET_DEASSERT;
	    state |= VOH_ITER_RPT_STATE_RESET_ASSERT;
      } else if (reset == SAHPI_RESET_DEASSERT) {
	    state &=~VOH_ITER_RPT_STATE_RESET_ASSERT;
	    state |= VOH_ITER_RPT_STATE_RESET_DEASSERT;
      }
      gtk_tree_store_set(store, &iter,
			 VOH_LIST_COLUMN_STATE, state,
			 -1);
      return TRUE;
}

gboolean voh_subscribe_events(guint sessionid, gchar *err)
{
      SaErrorT		rv;
      SaHpiSessionIdT	sid = (SaHpiSessionIdT)sessionid;

      rv = saHpiSubscribe(sid);
      if (rv != SA_OK) {
	    VOH_ERROR(err, "Event subscribing failed", rv);
	    return FALSE;
      }
      return TRUE;
}

gboolean voh_unsubscribe_events(guint sessionid, gchar *err)
{
      SaErrorT		rv;
      SaHpiSessionIdT	sid = (SaHpiSessionIdT)sessionid;

      rv = saHpiUnsubscribe(sid);
      if (rv != SA_OK) {
	    VOH_ERROR(err, "Event unsubscribing failed", rv);
	    return FALSE;
      }
      return TRUE;
}

gboolean voh_get_events(GtkTreeStore *event_list, guint sessionid, gchar *err)
{
      SaErrorT		rv;
      SaHpiEventT	event;
      GtkTreeIter	iter;
      SaHpiSessionIdT	sid = (SaHpiSessionIdT)sessionid;
      SaHpiDomainInfoT	info;
      gchar		name[1024];
      gchar		time[1024];
      gchar		source[1024];
      gchar		severity[1024];
      gchar		type[1024];
      gchar		domn[1024];

      if (event_list == NULL) {
	    VOH_ERROR(err, "Event getting failed (invalid parameter)", -1);
	    return FALSE;
      }
 
	    rv = saHpiEventGet(sid, SAHPI_TIMEOUT_IMMEDIATE,
			       &event, NULL, NULL, NULL);
	    if (rv == SA_OK) {
		  gtk_tree_store_append(event_list, &iter, NULL);
		  sprintf(name, "%s event",
			  vohEventType2String(event.EventType));
		  sprintf(time, "%s", vohTime2String(event.Timestamp));
		  sprintf(source, "ResourceId: (%ld)", event.Source);
		  sprintf(severity, "%s", vohSeverity2String(event.Severity));

		  rv = saHpiDomainInfoGet(sid, &info);
		  if (rv == SA_OK) {
			info.DomainTag;
			fixstr(&(info.DomainTag), domn);
		  } else {
			sprintf(domn, "unknown");
		  }

		  switch (event.EventType) {
		    case SAHPI_ET_RESOURCE:
			sprintf(type, "%s",
				vohResourceEventType2String(
				  event.EventDataUnion.ResourceEvent. \
				  			ResourceEventType));
			break;
		    case SAHPI_ET_DOMAIN:
			sprintf(type, "%s",
				vohDomainEventType2String(
				  event.EventDataUnion.DomainEvent.Type));
			break;
		    case SAHPI_ET_SENSOR:
			if (event.EventDataUnion.SensorEvent.Assertion == TRUE)
			      sprintf(type, "asserted");
			else
			      sprintf(type, "deasserted");
			break;
		    case SAHPI_ET_SENSOR_ENABLE_CHANGE:
			type[0] = 0;
			break;
		    case SAHPI_ET_HOTSWAP:
			type[0] = 0;
			break;
		    case SAHPI_ET_WATCHDOG:
			type[0] = 0;
			break;
		    case SAHPI_ET_HPI_SW:
			sprintf(type, "%s",
				vohSwEventType2String(
				  event.EventDataUnion.HpiSwEvent.Type));
			break;
		    case SAHPI_ET_OEM:
			type[0] = 0;
			break;
		    case SAHPI_ET_USER:
			type[0] = 0;
			break;
		    default:
			sprintf(type, "unknown");
		  }
		  gtk_tree_store_set(event_list, &iter,
				     VOH_EVENT_LIST_COLUMN_TIME, time,
				     VOH_EVENT_LIST_COLUMN_NAME, name,
				     VOH_EVENT_LIST_COLUMN_SOURCE, source,
				     VOH_EVENT_LIST_COLUMN_SEVER, severity,
				     VOH_EVENT_LIST_COLUMN_TYPE, type,
				     VOH_EVENT_LIST_COLUMN_DOMAIN, domn,
				     -1);
		  return TRUE;
	    }
	    if (rv == SA_ERR_HPI_TIMEOUT) {
		  return TRUE;
	    }
	    else {
		  VOH_ERROR(err, "Event getting failed", rv);
		  return FALSE;
	    }
}

