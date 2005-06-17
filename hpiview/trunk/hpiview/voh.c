#include <gtk/gtk.h>
#include <SaHpi.h>

#include "voh_string.h"
#include "voh.h"
#include "hview_utils.h"
#include "voh_convert_type.h"

/* #include "hview_service.h" */

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

static gchar	ascii6_codes[64] = {
		' ', '!', '"', '#', '$',  '%', '&', '\'',
		'(', ')', '*', '+', ',',  '-', '.', '/',
		'0', '1', '2', '3', '4',  '5', '6', '7',
		'8', '9', ':', ';', '<',  '=', '>', '?',
		'&', 'A', 'B', 'C', 'D',  'E', 'F', 'G',
		'H', 'I', 'J', 'K', 'L',  'M', 'N', 'O',
		'P', 'Q', 'R', 'S', 'T',  'U', 'V', 'W',
		'X', 'Y', 'Z', '[', '\\', ']', '^', '_' };

static int ascii6tostring(char *ascii, int n_ascii, char *str, int n)
{
	int	ascii_ind = 0, ascii_len, i;
	int	res = 0;
	char	byte = 0;

	ascii_len = n_ascii * 8 / 6;
	memset(str, 0, n);
	if (ascii_len > n) ascii_len = n;
	for (i = 0; i < ascii_len; i++) {
		switch (i % 4) {
			case 0:
				byte = ascii[ascii_ind++];
				res = byte & 0x3F;
				break;
			case 1:
				res = (byte & 0xC0) >> 6;
				byte = ascii[ascii_ind++];
				res += (byte & 0x0F) << 2;
				break;
			case 2:
				res = (byte & 0xF0) >> 4;
				byte = ascii[ascii_ind++];
				res += (byte & 0x03) << 4;
				break;
			case 3:
				res = byte & 0xFC;
				res >>= 2;
				break;
		};
		str[i] = ascii6_codes[res];
	};
	return(ascii_len);
}

static void fixstr(SaHpiTextBufferT *strptr, char *outbuff)
{
	size_t		datalen = strptr->DataLength;
	size_t		len;
	gint		tmp_ind,	i,	c;
	char		*str = (char *)strptr->Data;
	static gchar	*hex_codes = "0123456789ABCDEF";
	static gchar	*bcdplus_codes = "0123456789 -.???";

	if (datalen < 2) {
		*outbuff = '\0';
		return;
	}
	switch (strptr->DataType) {
	case SAHPI_TL_TYPE_UNICODE:
		*outbuff = '\0';
		return;
	case SAHPI_TL_TYPE_BCDPLUS:
		len = datalen * 2;
		outbuff[len] = '\0';
		tmp_ind = 0;
		for (i = 0; i < datalen; i++) {
			c = (str[i] & 0xF0) >> 4;
			outbuff[tmp_ind++] = bcdplus_codes[c];
			c = str[i] & 0x0F;
			outbuff[tmp_ind++] = bcdplus_codes[c];
		};
		return;
	case SAHPI_TL_TYPE_ASCII6:
		len = datalen * 8 / 6;
		*(outbuff + len) = '\0';
		ascii6tostring(str, datalen, outbuff, len);
		return;
	case SAHPI_TL_TYPE_TEXT:
		strncpy(outbuff, (char *)str, datalen);
		*(outbuff + datalen) = '\0';
		return;
	case SAHPI_TL_TYPE_BINARY:
		len = datalen * 2;
		*(outbuff + len) = '\0';
		tmp_ind = 0;
		for (i = 0; i < datalen; i++) {
			c = (str[i] & 0xF0) >> 4;
			outbuff[tmp_ind++] = hex_codes[c];
			c = str[i] & 0x0F;
			outbuff[tmp_ind++] = hex_codes[c];
		};
		return;
	default:
		*(outbuff) = '\0';
		return;
	}
}

static void fillstr(SaHpiTextBufferT *buf, const gchar *str)
{

	buf->DataType = SAHPI_TL_TYPE_TEXT;
	buf->Language = SAHPI_LANG_ENGLISH;
	buf->DataLength = strlen(str);
	memcpy((char *)(buf->Data), str, strlen(str));
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
			 VOH_LIST_COLUMN_ID, (guint)session,
			 VOH_LIST_COLUMN_TYPE, VOH_ITER_IS_DOMAIN,
			 VOH_LIST_COLUMN_STATE, 0,
			 -1);

}

static void voh_add_resource(GtkTreeStore *pstore, guint sessionid,
		      SaHpiRptEntryT *rpt)
{
      SaErrorT		rv;
      SaHpiPowerStateT	power;
      SaHpiResetActionT	reset;
      SaHpiSessionIdT	sid = (SaHpiSessionIdT)sessionid;
      GtkTreeIter	iter,	parent,	*found_iter;
      gint		i;
      guint		id,	type,	capability = 0;
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

		  res = hutil_find_iter_by_name(GTK_TREE_MODEL(pstore),
					        VOH_LIST_COLUMN_NAME,
						path, NULL, &parent, 0);
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
		  capability |= VOH_ITER_CAPABILITY_POWER;
		  rv = saHpiResourcePowerStateGet(sid, (SaHpiResourceIdT)id,
						  &power);
		  if (rv == SA_OK) {
			switch (power) {
			  case SAHPI_POWER_ON:
			      state |= VOH_ITER_RPT_STATE_POWER_ON;
			      image = gdk_pixbuf_new_from_file(
					hutil_find_pixmap_file("on.png"),
								&error);
			      break;
			  case SAHPI_POWER_OFF:
			      state |= VOH_ITER_RPT_STATE_POWER_OFF;
			      image = gdk_pixbuf_new_from_file(
					hutil_find_pixmap_file("off.png"),
								&error);
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
		  capability |= VOH_ITER_CAPABILITY_RESET;
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

	    if (rpt->ResourceCapabilities & SAHPI_CAPABILITY_CONTROL) {
		  capability |= VOH_ITER_CAPABILITY_CONTROL;
	    }
	    if (rpt->ResourceCapabilities & SAHPI_CAPABILITY_EVENT_LOG) {
		  capability |= VOH_ITER_CAPABILITY_EVENT_LOG;
	    }
	    gtk_tree_store_set(pstore, &iter,
			       VOH_LIST_COLUMN_NAME, path,
			       VOH_LIST_COLUMN_ID, id,
			       VOH_LIST_COLUMN_TYPE, type,
			       VOH_LIST_COLUMN_STATE, state,
			       VOH_LIST_COLUMN_CAPABILITY, capability,
			       -1);
	   parent = iter;
	   first = FALSE;
      }
}

static void voh_add_rdr(GtkTreeStore *pstore, SaHpiRdrT *rdr, guint resourceid)
{
      gboolean			res;
      GtkTreeIter		iter,	parent;
      SaHpiRdrTypeUnionT	*un;
      SaHpiTextBufferT		strptr;
      guint			type,	state = 0,	capability = 0;
      gchar			name[100];

      res = hutil_find_iter_by_id(GTK_TREE_MODEL(pstore), VOH_LIST_COLUMN_ID,
			          resourceid, 0, &parent, HUTIL_FIRST_ITER);
      if (res != FALSE) {
	    strptr = rdr->IdString;
	    fixstr(&strptr, name);
	    un = &rdr->RdrTypeUnion;
	    switch (rdr->RdrType) {
	      case SAHPI_CTRL_RDR:
		  type = VOH_ITER_IS_CONTROL;
		  break;
	      case SAHPI_SENSOR_RDR:
		  type = VOH_ITER_IS_SENSOR;
		  state = VOH_ITER_SENSOR_STATE_UNSPECIFIED;
		  capability = VOH_ITER_SENSOR_CAPABILITY_UNSPECIFIED;
		  if(un->SensorRec.ThresholdDefn.IsAccessible == TRUE) {
			  capability |= VOH_ITER_SENSOR_CAPABILITY_THRESHOLD;
		  }
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
			       VOH_LIST_COLUMN_CAPABILITY, capability,
			       -1);
      }
}

static guint voh_get_sensor_state(SaHpiSensorRecT *sensor,
				  SaHpiSensorReadingT *sv)
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

GtkTreeModel *voh_list_domains(gchar *err)
{
      GtkTreeStore	*pstore;

     pstore = gtk_tree_store_new(VOH_LIST_NUM_COL, G_TYPE_STRING,
				 GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_UINT,
				 G_TYPE_UINT, G_TYPE_UINT, G_TYPE_UINT);
 
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

GtkTreeModel *voh_get_resource_details(guint sessionid,
				       guint resourceid,
				       gchar *err)
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
			 0, "Entity path",
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

      gtk_tree_store_append(info_store, &iter, NULL);
      gtk_tree_store_set(info_store, &iter,
		         0, "Severity",
			 1, vohSeverity2String(entry.ResourceSeverity),
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
      SaHpiInventoryRecT	inventory;
      SaHpiCtrlRecT		control;
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

      gtk_tree_store_set(info_store, &iter,
		         0, "FRU entity",
			 1, vohBoolean2String(rdr.IsFru),
			 -1);

      switch (rdr.RdrType) {
	case SAHPI_SENSOR_RDR:
	    sensor = &(rdr.RdrTypeUnion.SensorRec);

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
					    sensor->DataFormat.BaseUnits,
					    sensor->DataFormat.Percentage),
				     -1);

		  gtk_tree_store_append(info_store, &iter, NULL);
		  gtk_tree_store_set(info_store, &iter,
				     0, "Modifier base unit",
				     1, vohSensorUnits2String(
					    sensor->DataFormat.ModifierUnits,
					    sensor->DataFormat.Percentage),
				     -1);

		  if (sensor->DataFormat.ModifierUse ==
		      			SAHPI_SMUU_BASIC_TIMES_MODIFIER) {
		  	sprintf(name, "%s*%s",
				vohSensorUnits2String(
				    sensor->DataFormat.BaseUnits,
				    sensor->DataFormat.Percentage),
				vohSensorUnits2String(
					    sensor->DataFormat.ModifierUnits,
					    sensor->DataFormat.Percentage));
		  } else if (sensor->DataFormat.ModifierUse ==
		      			SAHPI_SMUU_BASIC_OVER_MODIFIER) {
			sprintf(name, "%s/%s",
				vohSensorUnits2String(
				    sensor->DataFormat.BaseUnits,
				    sensor->DataFormat.Percentage),
				vohSensorUnits2String(
					    sensor->DataFormat.ModifierUnits,
					    sensor->DataFormat.Percentage));
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
	    break;
	case SAHPI_INVENTORY_RDR:
	    inventory = rdr.RdrTypeUnion.InventoryRec;

	    gtk_tree_store_append(info_store, &iter, NULL);
	    gtk_tree_store_set(info_store, &iter,
			       0, "Inventory's persistent",
			       1, vohBoolean2String(inventory.Persistent),
			       -1);

	    break;
	case SAHPI_CTRL_RDR:
	    control = rdr.RdrTypeUnion.CtrlRec;

	    gtk_tree_store_append(info_store, &iter, NULL);
	    gtk_tree_store_set(info_store, &iter,
			       0, "Control type",
			       1, vohCtrlType2String(control.Type),
			       -1);

	    gtk_tree_store_append(info_store, &iter, NULL);
	    gtk_tree_store_set(info_store, &iter,
			      0, "Write only control",
			      1, vohBoolean2String(control.WriteOnly),
			      -1);

	    gtk_tree_store_append(info_store, &iter, NULL);
	    gtk_tree_store_set(info_store, &iter,
			       0, "Control output type",
			       1, vohCtrlOutputType2String(control.OutputType),
			       -1);

	    gtk_tree_store_append(info_store, &iter, NULL);
	    gtk_tree_store_set(info_store, &iter,
			       0, "Default mode",
			       1, vohCtrlMode2String(control.DefaultMode.Mode),
			       -1);

	    gtk_tree_store_append(info_store, &iter, NULL);
	    gtk_tree_store_set(info_store, &iter,
			       0, "Read only mode",
			       1, vohBoolean2String(
				       control.DefaultMode.ReadOnly),
			       -1);

	    switch (control.Type) {
	    case SAHPI_CTRL_TYPE_DIGITAL:
		    gtk_tree_store_append(info_store, &iter, NULL);
		    gtk_tree_store_set(info_store, &iter,
				       0, "Control default state",
				       1, vohCtrlStateDigital2String(
					    control.TypeUnion.Digital.Default),
				       -1);
		    break;
	    case SAHPI_CTRL_TYPE_DISCRETE:
		    gtk_tree_store_append(info_store, &iter, NULL);
		    sprintf(name, "%d", control.TypeUnion.Discrete.Default);
		    gtk_tree_store_set(info_store, &iter,
				       0, "Control default state",
				       1, name,
				       -1);
		    break;
	    case SAHPI_CTRL_TYPE_ANALOG:
		    gtk_tree_store_append(info_store, &iter, NULL);
		    sprintf(name, "%d", control.TypeUnion.Analog.Min);
		    gtk_tree_store_set(info_store, &iter,
				       0, "Min control state value",
				       1, name,
				       -1);

		    gtk_tree_store_append(info_store, &iter, NULL);
		    sprintf(name, "%d", control.TypeUnion.Analog.Max);
		    gtk_tree_store_set(info_store, &iter,
				       0, "Max control state value",
				       1, name,
				       -1);

		    gtk_tree_store_append(info_store, &iter, NULL);
		    sprintf(name, "%d", control.TypeUnion.Analog.Default);
		    gtk_tree_store_set(info_store, &iter,
				       0, "Default control state value",
				       1, name,
				       -1);
		    break;
	    case SAHPI_CTRL_TYPE_STREAM:
		    gtk_tree_store_append(info_store, &iter, NULL);
		    memcpy(name, control.TypeUnion.Stream.Default.Stream,
				 control.TypeUnion.Stream.Default.StreamLength);
		    name[control.TypeUnion.Stream.Default.StreamLength] = '\0';
		    gtk_tree_store_set(info_store, &iter,
				       0, "Control default state",
				       1, name,
				       -1);
		    break;
	    case SAHPI_CTRL_TYPE_TEXT:
		    gtk_tree_store_append(info_store, &iter, NULL);
		    sprintf(name, "%d", control.TypeUnion.Text.MaxChars);
		    gtk_tree_store_set(info_store, &iter,
				       0, "Maximum of chars per line",
				       1, name,
				       -1);

		    gtk_tree_store_append(info_store, &iter, NULL);
		    sprintf(name, "%d", control.TypeUnion.Text.MaxLines);
		    gtk_tree_store_set(info_store, &iter,
				       0, "Maximum of lines",
				       1, name,
				       -1);

		    gtk_tree_store_append(info_store, &iter, NULL);
		    sprintf(name, "%d", control.TypeUnion.Text.Default.Line);
		    gtk_tree_store_set(info_store, &iter,
				       0, "Control default line",
				       1, name,
				       -1);

		    gtk_tree_store_append(info_store, &iter, NULL);
		    fixstr(&control.TypeUnion.Text.Default.Text, name);
		    gtk_tree_store_set(info_store, &iter,
				       0, "Control default state",
				       1, name,
				       -1);
		    break;
	    case SAHPI_CTRL_TYPE_OEM:
		    gtk_tree_store_append(info_store, &iter, NULL);
		    sprintf(name, "%d", control.TypeUnion.Oem.MId);
		    gtk_tree_store_set(info_store, &iter,
				       0, "Control manufacturer Id",
				       1, name,
				       -1);

		    gtk_tree_store_append(info_store, &iter, NULL);
		    memcpy(name, control.TypeUnion.Oem.ConfigData,
				    SAHPI_CTRL_OEM_CONFIG_LENGTH);
		    gtk_tree_store_set(info_store, &iter,
				       0, "Oem configuration data",
				       1, name,
				       -1);

		    gtk_tree_store_append(info_store, &iter, NULL);
		    memcpy(name, control.TypeUnion.Oem.Default.Body,
				    control.TypeUnion.Oem.Default.BodyLength);
		    name[control.TypeUnion.Oem.Default.BodyLength] = '\0';
		    gtk_tree_store_set(info_store, &iter,
				       0, "Control default state",
				       1, name,
				       -1);
		    break;
	    default:
		    break;
	    }

	    break;
	default:
	    break;
      }

      gtk_tree_store_append(info_store, &iter, NULL);

      return GTK_TREE_MODEL(info_store);
}

gboolean voh_read_sensor(GtkTreeStore *store, guint sessionid, guint resourceid,
			 guint entryid, gchar *err)
{
      SaErrorT			rv;
      SaHpiRdrT			rdr;
      SaHpiEntryIdT		nextentryid;
      SaHpiSensorRecT		*sensor;
      SaHpiSensorReadingT	reading;
      SaHpiSessionIdT		sid = (SaHpiSessionIdT)sessionid;
      SaHpiEntryIdT		enid = (SaHpiEntryIdT)entryid;
      SaHpiEntryIdT		resid = (SaHpiEntryIdT)resourceid;
      GtkTreeIter		iter;
      guint			state;
      gboolean			res;

      res = hutil_find_iter_by_id(GTK_TREE_MODEL(store), VOH_LIST_COLUMN_ID,
		                  entryid, resid, &iter,
				  HUTIL_FIRST_ITER | HUTIL_WITH_PARENT);
      if (res == FALSE) {
	    VOH_ERROR(err, "Reading sensor failed (invalid argument)", -1);
	    return FALSE;
      }

      rv = saHpiRdrGet(sid, resid, enid, &nextentryid, &rdr);
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

      gtk_tree_store_set(store, &iter,
		         VOH_LIST_COLUMN_VALUE, vohSensorValue2FullString(
				 			    sensor, &reading),
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
				  GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_UINT,
				  G_TYPE_UINT, G_TYPE_UINT, G_TYPE_UINT);

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

gboolean voh_parm_control(guint sessionid, guint resourceid,
			  guint action, gchar *err)
{
      SaErrorT		rv;
      SaHpiPowerStateT	power;
      SaHpiSessionIdT	sid = (SaHpiSessionIdT)sessionid;
      SaHpiResourceIdT	rid = (SaHpiResourceIdT)resourceid;

      rv = saHpiParmControl(sid, rid, (SaHpiParmActionT) action);
      if (rv != SA_OK) {
	    VOH_ERROR(err, "Resource parameters setting failed", rv);
	    return FALSE;
      }

      return TRUE;
}

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

      res = hutil_find_iter_by_id(GTK_TREE_MODEL(store), VOH_LIST_COLUMN_ID,
			          resourceid, 0, &iter, HUTIL_FIRST_ITER);
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

      image = gdk_pixbuf_new_from_file(hutil_find_pixmap_file("off.png"),
		      		       &error);
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

      res = hutil_find_iter_by_id(GTK_TREE_MODEL(store), VOH_LIST_COLUMN_ID,
			          resourceid, 0, &iter, HUTIL_FIRST_ITER);
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
      image = gdk_pixbuf_new_from_file(hutil_find_pixmap_file("on.png"),
		      		       &error);
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

      res = hutil_find_iter_by_id(GTK_TREE_MODEL(store), VOH_LIST_COLUMN_ID,
		                  resourceid, 0, &iter, HUTIL_FIRST_ITER);
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
	    image = gdk_pixbuf_new_from_file(hutil_find_pixmap_file("on.png"),
					     &error);
      } else if (power == SAHPI_POWER_OFF) {
	    state &=~VOH_ITER_RPT_STATE_POWER_ON;
	    state |= VOH_ITER_RPT_STATE_POWER_OFF;
	    image = gdk_pixbuf_new_from_file(hutil_find_pixmap_file("off.png"),
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

      res = hutil_find_iter_by_id(GTK_TREE_MODEL(store), VOH_LIST_COLUMN_ID,
			          resourceid, 0, &iter, HUTIL_FIRST_ITER);
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

      res = hutil_find_iter_by_id(GTK_TREE_MODEL(store), VOH_LIST_COLUMN_ID,
			          resourceid, 0, &iter, HUTIL_FIRST_ITER);
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

      res = hutil_find_iter_by_id(GTK_TREE_MODEL(store), VOH_LIST_COLUMN_ID,
			          resourceid, 0, &iter, HUTIL_FIRST_ITER);
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

      res = hutil_find_iter_by_id(GTK_TREE_MODEL(store), VOH_LIST_COLUMN_ID,
			          resourceid, 0, &iter, HUTIL_FIRST_ITER);
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

GtkListStore *voh_get_severity_list(void)
{
	GtkListStore	*store;
	GtkTreeIter	iter;

	store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_UINT);
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter,
			   0, vohSeverity2String(SAHPI_CRITICAL),
			   1, SAHPI_CRITICAL, 
			   -1);
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter,
			   0, vohSeverity2String(SAHPI_MAJOR),
			   1, SAHPI_MAJOR, 
			   -1);
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter,
			   0, vohSeverity2String(SAHPI_MINOR),
			   1, SAHPI_MINOR,
			   -1);
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter,
			   0, vohSeverity2String(SAHPI_INFORMATIONAL),
			   1, SAHPI_INFORMATIONAL,
			   -1);
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter,
			   0, vohSeverity2String(SAHPI_OK),
			   1, SAHPI_OK,
			   -1);
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter,
			   0, vohSeverity2String(SAHPI_DEBUG),
			   1, SAHPI_DEBUG,
			   -1);

	return store;
}

gboolean voh_check_resource_presence(guint sessionid,
				     guint resourceid,
				     gchar *err)
{
	SaErrorT		rv;
	SaHpiResourceIdT	rid = (SaHpiResourceIdT) resourceid;
	SaHpiSessionIdT		sid = (SaHpiSessionIdT) sessionid;
	SaHpiRptEntryT		entry;

	rv = saHpiRptEntryGetByResourceId(sid, rid, &entry);
	if (rv != SA_OK) {
		VOH_ERROR(err, "Getting resource entry failed", rv);
		return FALSE;
	} else {
		return TRUE;
	}
}

gboolean voh_check_rdr_presence(guint sessionid,
				guint resourceid,
				guint rdrentryid,
				gchar *err)
{
	SaErrorT		rv;
	SaHpiResourceIdT	rid = (SaHpiResourceIdT) resourceid;
	SaHpiSessionIdT		sid = (SaHpiSessionIdT) sessionid;
	SaHpiEntryIdT		rdrid = (SaHpiEntryIdT) rdrentryid;
	SaHpiRdrT		rdr;
	SaHpiEntryIdT		nextentryid;

	rv = saHpiRdrGet(sid, rid, rdrid, &nextentryid, &rdr);
	if (rv != SA_OK) {
		VOH_ERROR(err, "Getting rdr failed", rv);
		return FALSE;
	}

	return TRUE;
}

gboolean voh_set_resource_severity(guint sessionid, guint resourceid,
				   guint severity, gchar *err)
{
	SaErrorT		rv;
	SaHpiResourceIdT	rid = (SaHpiResourceIdT) resourceid;
	SaHpiSessionIdT		sid = (SaHpiSessionIdT) sessionid;
	SaHpiSeverityT		sev = (SaHpiSeverityT) severity;

	rv = saHpiResourceSeveritySet(sid, rid, sev);
	if (rv != SA_OK) {
		VOH_ERROR(err, "Setting resource severity failed", rv);
		return FALSE;
	}

	return TRUE;
}

gchar *voh_get_resource_severity(guint sessionid, guint resourceid, gchar *err)
{
	SaErrorT		rv;
	SaHpiResourceIdT	rid = (SaHpiResourceIdT) resourceid;
	SaHpiSessionIdT		sid = (SaHpiSessionIdT) sessionid;
	SaHpiRptEntryT		entry;
	gchar			*severity;

	err[0] = 0;

	rv = saHpiRptEntryGetByResourceId(sid, rid, &entry);

	if (rv != SA_OK) {
		VOH_ERROR(err, "Getting resource entry failed", rv);
		return NULL;
	}

	severity = g_strdup(vohSeverity2String(entry.ResourceSeverity));

	return severity;
}

GList *voh_get_resource_info(guint sessionid,
			     guint resourceid,
			     gchar *err)
{
	SaErrorT		rv;
	SaHpiResourceIdT	rid = (SaHpiResourceIdT) resourceid;
	SaHpiSessionIdT		sid = (SaHpiSessionIdT) sessionid;
	SaHpiRptEntryT		entry;
	GList			*info = NULL;
	gchar			info_str[1024];

	rv = saHpiRptEntryGetByResourceId(sid, rid, &entry);

	if (rv != SA_OK) {
		VOH_ERROR(err, "Getting resource entry failed", rv);
		return NULL;
	}

	sprintf(info_str, "Resource revision:\t\t%d",
		entry.ResourceInfo.ResourceRev);
	info = g_list_prepend(info, g_strdup(info_str));

	sprintf(info_str, "Specific version:\t\t%d",
		entry.ResourceInfo.SpecificVer);
	info = g_list_prepend(info, g_strdup(info_str));

	sprintf(info_str, "Device support:\t\t\t%d",
		entry.ResourceInfo.DeviceSupport);
	info = g_list_prepend(info, g_strdup(info_str));

	sprintf(info_str, "ManufacturerId:\t\t\t%d",
		entry.ResourceInfo.ManufacturerId);
	info = g_list_prepend(info, g_strdup(info_str));

	sprintf(info_str, "ProductId:\t\t\t\t%d",
		entry.ResourceInfo.ProductId);
	info = g_list_prepend(info, g_strdup(info_str));

	sprintf(info_str, "Firmware major revision:\t%d",
		entry.ResourceInfo.FirmwareMajorRev);
	info = g_list_prepend(info, g_strdup(info_str));

	sprintf(info_str, "Firmware minor revision:\t%d",
		entry.ResourceInfo.FirmwareMinorRev);
	info = g_list_prepend(info, g_strdup(info_str));

	sprintf(info_str, "AuxFirmware revision:\t%d",
		entry.ResourceInfo.AuxFirmwareRev);
	info = g_list_prepend(info, g_strdup(info_str));

	return info;
}

gchar *voh_get_resource_tag(guint sessionid,
			    guint resourceid,
			    gchar *err)
{
	SaErrorT		rv;
	SaHpiResourceIdT	rid = (SaHpiResourceIdT) resourceid;
	SaHpiSessionIdT		sid = (SaHpiSessionIdT) sessionid;
	SaHpiRptEntryT		entry;
	gchar			tag[1024];

	rv = saHpiRptEntryGetByResourceId(sid, rid, &entry);

	if (rv != SA_OK) {
		VOH_ERROR(err, "Getting resource entry failed", rv);
		return NULL;
	}

	fixstr(&(entry.ResourceTag), tag);

	return g_strdup(tag);
}

gboolean voh_set_resource_tag(guint sessionid,
			      guint resourceid,
			      const gchar *resource_tag,
			      gchar *err)
{
	SaErrorT		rv;
	SaHpiResourceIdT	rid = (SaHpiResourceIdT) resourceid;
	SaHpiSessionIdT		sid = (SaHpiSessionIdT) sessionid;
	SaHpiTextBufferT	tag_buf;

	fillstr(&tag_buf, resource_tag);

	rv = saHpiResourceTagSet(sid, rid, &tag_buf);
	if (rv != SA_OK) {
		VOH_ERROR(err, "Setting resource tag failed", rv);
		return FALSE;
	}

	return TRUE;
}

GList *voh_get_sensor_info(guint sessionid,
			   guint resourceid,
			   guint rdrentryid,
			   gchar *err)
{
	SaErrorT		rv;
	SaHpiResourceIdT	rid = (SaHpiResourceIdT) resourceid;
	SaHpiSessionIdT		sid = (SaHpiSessionIdT) sessionid;
	SaHpiRdrT		rdr;
	SaHpiEntryIdT		nextentryid;
	SaHpiSensorRecT		sensor;
	SaHpiEntryIdT		rdrid = (SaHpiEntryIdT) rdrentryid;
	GList			*info = NULL;
	gchar			info_str[1024];
	gchar			name[1024];

	rv = saHpiRdrGet(sid, rid, rdrid, &nextentryid, &rdr);
	if (rv != SA_OK) {
		VOH_ERROR(err, "Getting sensor info failed", rv);
		return NULL;
	}

	if (rdr.RdrType != SAHPI_SENSOR_RDR) {
		VOH_ERROR(err, "Getting sensor info failed (invalid argument)",
									-1);
		return NULL;
	}

	sensor = rdr.RdrTypeUnion.SensorRec;

	if (sensor.DataFormat.Range.Flags & SAHPI_SRF_NOMINAL) {
		sprintf(info_str, "Nominal value:\t\t%s",
			vohSensorValue2FullString(&sensor,
					&sensor.DataFormat.Range.Nominal));
		info = g_list_prepend(info, g_strdup(info_str));
	}

	if (sensor.DataFormat.Range.Flags & SAHPI_SRF_NORMAL_MAX) {
		sprintf(info_str, "Normal Max value:\t%s",
			vohSensorValue2FullString(&sensor,
					&sensor.DataFormat.Range.NormalMax));
		info = g_list_prepend(info, g_strdup(info_str));
	}

	if (sensor.DataFormat.Range.Flags & SAHPI_SRF_NORMAL_MIN) {
		sprintf(info_str, "Normal Min value:\t%s",
			vohSensorValue2FullString(&sensor,
					&sensor.DataFormat.Range.NormalMin));
		info = g_list_prepend(info, g_strdup(info_str));
	}

	if (sensor.DataFormat.Range.Flags & SAHPI_SRF_MAX) {
		sprintf(info_str, "Max value:\t\t%s",
			vohSensorValue2FullString(&sensor,
						&sensor.DataFormat.Range.Max));
		info = g_list_prepend(info, g_strdup(info_str));
	}

	if (sensor.DataFormat.Range.Flags & SAHPI_SRF_MIN) {
		sprintf(info_str, "Min value:\t\t\t%s",
			vohSensorValue2FullString(&sensor,
						&sensor.DataFormat.Range.Min));
		info = g_list_prepend(info, g_strdup(info_str));
	}

	if (sensor.DataFormat.IsSupported == TRUE) {
		sprintf(info_str, "Sensor base unit:\t\%s",
			vohSensorUnits2String(sensor.DataFormat.BaseUnits,
						sensor.DataFormat.Percentage));
		info = g_list_prepend(info, g_strdup(info_str));
	}

	sprintf(info_str, "Sensor control:\t\t%s",
		vohBoolean2String(sensor.EnableCtrl));
	info = g_list_prepend(info, g_strdup(info_str));

	sprintf(info_str, "Sensor type:\t\t%s",
		vohSensorType2String(sensor.Type));
	info = g_list_prepend(info, g_strdup(info_str));


	fixstr(&rdr.IdString, name);
	sprintf(info_str, "Sensor name:\t\t%s", name);
	info = g_list_prepend(info, g_strdup(info_str));

	return info;
}

gboolean voh_get_sensor_enable(guint sessionid,
			       guint resourceid,
			       guint rdrentryid,
			       gboolean *status,
			       gchar *err)
{
	SaErrorT		rv;
	SaHpiResourceIdT	rid = (SaHpiResourceIdT) resourceid;
	SaHpiSessionIdT		sid = (SaHpiSessionIdT) sessionid;
	SaHpiRdrT		rdr;
	SaHpiEntryIdT		nextentryid;
	SaHpiSensorRecT		sensor;
	SaHpiEntryIdT		rdrid = (SaHpiEntryIdT) rdrentryid;
	SaHpiBoolT		lstatus;

	rv = saHpiRdrGet(sid, rid, rdrid, &nextentryid, &rdr);
	if (rv != SA_OK) {
		VOH_ERROR(err, "Getting sensor enable status failed", rv);
		return TRUE;
	}

	if (rdr.RdrType != SAHPI_SENSOR_RDR) {
		VOH_ERROR(err, "Getting sensor enable status failed", -1);
		return TRUE;
	}

	sensor = rdr.RdrTypeUnion.SensorRec;

	rv = saHpiSensorEnableGet(sid, rid, sensor.Num, &lstatus);

	if (rv != SA_OK) {
		VOH_ERROR(err, "Getting sensor enable status failed", rv);
		return FALSE;
	}

	*status = (gboolean) lstatus;
	return TRUE;
}

gboolean voh_set_sensor_enable(guint sessionid,
			       guint resourceid,
			       guint rdrentryid,
			       gboolean status,
			       gchar *err)
{
	SaErrorT		rv;
	SaHpiResourceIdT	rid = (SaHpiResourceIdT) resourceid;
	SaHpiSessionIdT		sid = (SaHpiSessionIdT) sessionid;
	SaHpiRdrT		rdr;
	SaHpiEntryIdT		nextentryid;
	SaHpiSensorRecT		sensor;
	SaHpiEntryIdT		rdrid = (SaHpiEntryIdT) rdrentryid;

	rv = saHpiRdrGet(sid, rid, rdrid, &nextentryid, &rdr);
	if (rv != SA_OK) {
		VOH_ERROR(err, "Setting sensor enable status failed", rv);
		return TRUE;
	}

	if (rdr.RdrType != SAHPI_SENSOR_RDR) {
		VOH_ERROR(err, "Setting sensor enable status failed", -1);
		return TRUE;
	}

	sensor = rdr.RdrTypeUnion.SensorRec;

	rv = saHpiSensorEnableSet(sid, rid, sensor.Num, status);

	if (rv != SA_OK) {
		VOH_ERROR(err, "Setting sensor enable status failed", rv);
		return FALSE;
	}

	return TRUE;
}

GList *voh_get_sensor_event_info(guint sessionid,
				 guint resourceid,
				 guint rdrentryid,
				 gchar *err)
{
	SaErrorT		rv;
	SaHpiResourceIdT	rid = (SaHpiResourceIdT) resourceid;
	SaHpiSessionIdT		sid = (SaHpiSessionIdT) sessionid;
	SaHpiRdrT		rdr;
	SaHpiEntryIdT		nextentryid;
	SaHpiSensorRecT		sensor;
	SaHpiEntryIdT		rdrid = (SaHpiEntryIdT) rdrentryid;
	SaHpiEventStateT	event_state, as_state, de_state;
	SaHpiSensorReadingT	reading;
	GList			*info = NULL;
	gchar			info_str[1024];
	gchar			name[1024];

	rv = saHpiRdrGet(sid, rid, rdrid, &nextentryid, &rdr);
	if (rv != SA_OK) {
		VOH_ERROR(err, "Getting sensor info failed", rv);
		return NULL;
	}

	if (rdr.RdrType != SAHPI_SENSOR_RDR) {
		VOH_ERROR(err, "Getting sensor info failed (invalid argument)",
									-1);
		return NULL;
	}

	sensor = rdr.RdrTypeUnion.SensorRec;

	rv = saHpiSensorReadingGet(sid, rid, sensor.Num, &reading,
				   &event_state);

	if (rv == SA_OK) {
		sprintf(info_str, "Current event state:\t\t%s",
				vohEventState2String(event_state,
						     sensor.Category));
		info = g_list_prepend(info, g_strdup(info_str));
	}
/*
	sprintf(info_str, "Sensor states supported:\n%s",
		vohEventState2String(sensor.Events, sensor.Category));
	info = g_list_prepend(info, g_strdup(info_str));
*/
	sprintf(info_str, "Sensor event control:\t\t%s",
		vohSensorEventCtrl2String(sensor.EventCtrl));
	info = g_list_prepend(info, g_strdup(info_str));

	sprintf(info_str, "Event category:\t\t\t%s",
		vohEventCategory2String(sensor.Category));
	info = g_list_prepend(info, g_strdup(info_str));

	return info;
}

gboolean voh_get_sensor_event_enable(guint sessionid,
				     guint resourceid,
				     guint rdrentryid,
				     gboolean *status,
				     gchar *err)
{
	SaErrorT		rv;
	SaHpiResourceIdT	rid = (SaHpiResourceIdT) resourceid;
	SaHpiSessionIdT		sid = (SaHpiSessionIdT) sessionid;
	SaHpiRdrT		rdr;
	SaHpiEntryIdT		nextentryid;
	SaHpiSensorRecT		sensor;
	SaHpiEntryIdT		rdrid = (SaHpiEntryIdT) rdrentryid;
	SaHpiBoolT		lstatus;

	rv = saHpiRdrGet(sid, rid, rdrid, &nextentryid, &rdr);
	if (rv != SA_OK) {
		VOH_ERROR(err, "Getting sensor event enable status failed", rv);
		return TRUE;
	}

	if (rdr.RdrType != SAHPI_SENSOR_RDR) {
		VOH_ERROR(err, "Getting sensor event enable status failed", -1);
		return TRUE;
	}

	sensor = rdr.RdrTypeUnion.SensorRec;

	rv = saHpiSensorEventEnableGet(sid, rid, sensor.Num, &lstatus);

	if (rv != SA_OK) {
		VOH_ERROR(err, "Getting sensor event enable status failed", rv);
		return FALSE;
	}

	*status = (gboolean) lstatus;
	return TRUE;
}

gboolean voh_set_sensor_event_enable(guint sessionid,
				     guint resourceid,
				     guint rdrentryid,
				     gboolean status,
				     gchar *err)
{
	SaErrorT		rv;
	SaHpiResourceIdT	rid = (SaHpiResourceIdT) resourceid;
	SaHpiSessionIdT		sid = (SaHpiSessionIdT) sessionid;
	SaHpiRdrT		rdr;
	SaHpiEntryIdT		nextentryid;
	SaHpiSensorRecT		sensor;
	SaHpiEntryIdT		rdrid = (SaHpiEntryIdT) rdrentryid;

	rv = saHpiRdrGet(sid, rid, rdrid, &nextentryid, &rdr);
	if (rv != SA_OK) {
		VOH_ERROR(err, "Setting sensor event enable status failed", rv);
		return TRUE;
	}

	if (rdr.RdrType != SAHPI_SENSOR_RDR) {
		VOH_ERROR(err, "Setting sensor event enable status failed", -1);
		return TRUE;
	}

	sensor = rdr.RdrTypeUnion.SensorRec;

	rv = saHpiSensorEventEnableSet(sid, rid, sensor.Num, status);

	if (rv != SA_OK) {
		VOH_ERROR(err, "Setting sensor event enable status failed", rv);
		return FALSE;
	}

	return TRUE;
}

gboolean voh_get_sensor_assert_event_mask(guint sessionid,
					  guint resourceid,
					  guint rdrentryid,
					  GList **evlist,
					  gchar *err)
{
	SaErrorT		rv;
	SaHpiResourceIdT	rid = (SaHpiResourceIdT) resourceid;
	SaHpiSessionIdT		sid = (SaHpiSessionIdT) sessionid;
	SaHpiRdrT		rdr;
	SaHpiEntryIdT		nextentryid;
	SaHpiSensorRecT		sensor;
	SaHpiEntryIdT		rdrid = (SaHpiEntryIdT) rdrentryid;
	SaHpiEventStateT	assertm, deassertm;
	GList			*event_list = NULL;
	VohEventStateT		*evst;

	*evlist = NULL;

	rv = saHpiRdrGet(sid, rid, rdrid, &nextentryid, &rdr);
	if (rv != SA_OK) {
		VOH_ERROR(err, "Getting sensor event masks failed", rv);
		return FALSE;
	}

	if (rdr.RdrType != SAHPI_SENSOR_RDR) {
		VOH_ERROR(err, "Getting sensor event masks failed", -1);
		return FALSE;
	}

	sensor = rdr.RdrTypeUnion.SensorRec;

	rv = saHpiSensorEventMasksGet(sid, rid, sensor.Num,
				      &assertm, &deassertm);

	if (rv != SA_OK) {
		VOH_ERROR(err, "Getting sensor event masks failed", rv);
		return FALSE;
	}

	event_list = vohEventState2List(sensor.Category);

	*evlist = event_list;

	while (event_list != NULL) {
		evst = (VohEventStateT *)(event_list->data);
		if (evst->value & assertm) {
			evst->active = TRUE;
		} else {
			evst->active = FALSE;
		}

		event_list = event_list->next;
	}

	return TRUE;
}

gboolean voh_get_sensor_deassert_event_mask(guint sessionid,
					    guint resourceid,
					    guint rdrentryid,
					    GList **evlist,
					    gchar *err)
{
	SaErrorT		rv;
	SaHpiResourceIdT	rid = (SaHpiResourceIdT) resourceid;
	SaHpiSessionIdT		sid = (SaHpiSessionIdT) sessionid;
	SaHpiRdrT		rdr;
	SaHpiEntryIdT		nextentryid;
	SaHpiSensorRecT		sensor;
	SaHpiEntryIdT		rdrid = (SaHpiEntryIdT) rdrentryid;
	SaHpiEventStateT	assertm, deassertm;
	GList			*event_list = NULL;
	VohEventStateT		*evst;

	*evlist = NULL;

	rv = saHpiRdrGet(sid, rid, rdrid, &nextentryid, &rdr);
	if (rv != SA_OK) {
		VOH_ERROR(err, "Getting sensor event masks failed", rv);
		return FALSE;
	}

	if (rdr.RdrType != SAHPI_SENSOR_RDR) {
		VOH_ERROR(err, "Getting sensor event masks failed", -1);
		return FALSE;
	}

	sensor = rdr.RdrTypeUnion.SensorRec;

	rv = saHpiSensorEventMasksGet(sid, rid, sensor.Num,
				      &assertm, &deassertm);

	if (rv != SA_OK) {
		VOH_ERROR(err, "Getting sensor event masks failed", rv);
		return FALSE;
	}

	event_list = vohEventState2List(sensor.Category);

	*evlist = event_list;

	while (event_list != NULL) {
		evst = (VohEventStateT *)(event_list->data);
		if (evst->value & deassertm) {
			evst->active = TRUE;
		} else {
			evst->active = FALSE;
		}

		event_list = event_list->next;
	}

	return TRUE;
}


gboolean voh_set_sensor_event_masks(guint sessionid,
				    guint resourceid,
				    guint rdrentryid,
				    GList *assert_mask,
				    GList *deassert_mask,
				    gchar *err)
{
	SaErrorT		rv;
	SaHpiResourceIdT	rid = (SaHpiResourceIdT) resourceid;
	SaHpiSessionIdT		sid = (SaHpiSessionIdT) sessionid;
	SaHpiRdrT		rdr;
	SaHpiEntryIdT		nextentryid;
	SaHpiSensorRecT		sensor;
	SaHpiEntryIdT		rdrid = (SaHpiEntryIdT) rdrentryid;
	SaHpiEventStateT	assertm = 0, deassertm = 0;
	SaHpiEventStateT	rm_assertm = 0, rm_deassertm = 0;
	VohEventStateT		*evst;


	rv = saHpiRdrGet(sid, rid, rdrid, &nextentryid, &rdr);
	if (rv != SA_OK) {
		VOH_ERROR(err, "Setting sensor event masks failed", rv);
		return FALSE;
	}

	if (rdr.RdrType != SAHPI_SENSOR_RDR) {
		VOH_ERROR(err, "Setting sensor event masks failed", -1);
		return FALSE;
	}

	sensor = rdr.RdrTypeUnion.SensorRec;

	while (assert_mask != NULL) {
		evst = (VohEventStateT *) assert_mask->data;
		if (evst->active == TRUE) {
			assertm |= evst->value;
		} else {
			rm_assertm |= evst->value;
		}
		assert_mask = assert_mask->next;
	}

	while (deassert_mask != NULL) {
		evst = (VohEventStateT *) deassert_mask->data;
		if (evst->active == TRUE) {
			deassertm |= evst->value;
		} else {
			rm_deassertm |= evst->value;
		}
		deassert_mask = deassert_mask->next;
	}

	rv = saHpiSensorEventMasksSet(sid, rid, sensor.Num,
				      SAHPI_SENS_ADD_EVENTS_TO_MASKS,
				      assertm, deassertm);

	if (rv != SA_OK) {
		VOH_ERROR(err, "Setting sensor event masks failed", rv);
		return FALSE;
	}

	rv = saHpiSensorEventMasksSet(sid, rid, sensor.Num,
				      SAHPI_SENS_REMOVE_EVENTS_FROM_MASKS,
				      rm_assertm, rm_deassertm);

	if (rv != SA_OK) {
		VOH_ERROR(err, "Setting sensor event masks failed", rv);
		return FALSE;
	}

	return TRUE;
}

gboolean voh_get_sensor_event_states_supported(guint sessionid,
					       guint resourceid,
					       guint rdrentryid,
					       GList **evlist,
					       gchar *err)
{
	SaErrorT		rv;
	SaHpiResourceIdT	rid = (SaHpiResourceIdT) resourceid;
	SaHpiSessionIdT		sid = (SaHpiSessionIdT) sessionid;
	SaHpiRdrT		rdr;
	SaHpiEntryIdT		nextentryid;
	SaHpiSensorRecT		sensor;
	SaHpiEntryIdT		rdrid = (SaHpiEntryIdT) rdrentryid;
	GList			*event_list = NULL;
	VohEventStateT		*evst;

	*evlist = NULL;

	rv = saHpiRdrGet(sid, rid, rdrid, &nextentryid, &rdr);
	if (rv != SA_OK) {
		VOH_ERROR(err, "Getting sensor event states failed", rv);
		return FALSE;
	}

	if (rdr.RdrType != SAHPI_SENSOR_RDR) {
		VOH_ERROR(err, "Getting sensor event states failed", -1);
		return FALSE;
	}

	sensor = rdr.RdrTypeUnion.SensorRec;

	event_list = vohEventState2List(sensor.Category);

	*evlist = event_list;

	while (event_list != NULL) {
		evst = (VohEventStateT *)(event_list->data);
		if (evst->value & sensor.Events) {
			evst->active = TRUE;
		} else {
			evst->active = FALSE;
		}

		event_list = event_list->next;
	}

	return TRUE;
}

GList *voh_get_sensor_threshold_info(guint sessionid,
				     guint resourceid,
				     guint rdrentryid,
				     gchar *err)
{
	SaErrorT		rv;
	SaHpiResourceIdT	rid = (SaHpiResourceIdT) resourceid;
	SaHpiSessionIdT		sid = (SaHpiSessionIdT) sessionid;
	SaHpiRdrT		rdr;
	SaHpiEntryIdT		nextentryid;
	SaHpiSensorRecT		sensor;
	SaHpiEntryIdT		rdrid = (SaHpiEntryIdT) rdrentryid;
	SaHpiSensorThresholdsT	thresholds;
	GList			*info = NULL;
	gchar			info_str[1024];

	rv = saHpiRdrGet(sid, rid, rdrid, &nextentryid, &rdr);
	if (rv != SA_OK) {
		VOH_ERROR(err, "Getting sensor threshold info failed", rv);
		return NULL;
	}

	if (rdr.RdrType != SAHPI_SENSOR_RDR) {
		VOH_ERROR(err, "Getting sensor threshold info failed", -1);
		return NULL;
	}

	sensor = rdr.RdrTypeUnion.SensorRec;

	rv = saHpiSensorThresholdsGet(sid, rid, sensor.Num, &thresholds);

	if (rv != SA_OK) {
		VOH_ERROR(err, "Thresholds getting failed", rv);
	}

	sprintf(info_str, "Low Minor:\t\t%s",
		vohReadWriteThds2String(sensor.ThresholdDefn.ReadThold,
					sensor.ThresholdDefn.WriteThold,
					SAHPI_STM_LOW_MINOR));
	info = g_list_prepend(info, g_strdup(info_str));

	sprintf(info_str, "Up Minor:\t\t\t%s",
		vohReadWriteThds2String(sensor.ThresholdDefn.ReadThold,
					sensor.ThresholdDefn.WriteThold,
					SAHPI_STM_UP_MINOR));
	info = g_list_prepend(info, g_strdup(info_str));

	sprintf(info_str, "Low Major:\t\t%s",
		vohReadWriteThds2String(sensor.ThresholdDefn.ReadThold,
					sensor.ThresholdDefn.WriteThold,
					SAHPI_STM_LOW_MAJOR));
	info = g_list_prepend(info, g_strdup(info_str));

	sprintf(info_str, "Up Major:\t\t\t%s",
		vohReadWriteThds2String(sensor.ThresholdDefn.ReadThold,
					sensor.ThresholdDefn.WriteThold,
					SAHPI_STM_UP_MAJOR));
	info = g_list_prepend(info, g_strdup(info_str));

	sprintf(info_str, "Low Critical:\t\t%s",
		vohReadWriteThds2String(sensor.ThresholdDefn.ReadThold,
					sensor.ThresholdDefn.WriteThold,
					SAHPI_STM_LOW_CRIT));
	info = g_list_prepend(info, g_strdup(info_str));

	sprintf(info_str, "Up Critical:\t\t%s",
		vohReadWriteThds2String(sensor.ThresholdDefn.ReadThold,
					sensor.ThresholdDefn.WriteThold,
					SAHPI_STM_UP_CRIT));
	info = g_list_prepend(info, g_strdup(info_str));

	sprintf(info_str, "Low Hysteresis:\t\t%s",
		vohReadWriteThds2String(sensor.ThresholdDefn.ReadThold,
					sensor.ThresholdDefn.WriteThold,
					SAHPI_STM_LOW_HYSTERESIS));
	info = g_list_prepend(info, g_strdup(info_str));

	sprintf(info_str, "Up Hysteresis:\t\t%s",
		vohReadWriteThds2String(sensor.ThresholdDefn.ReadThold,
					sensor.ThresholdDefn.WriteThold,
					SAHPI_STM_UP_HYSTERESIS));
	info = g_list_prepend(info, g_strdup(info_str));

	return info;
}

gboolean voh_get_sensor_thresholds(guint sessionid,
				   guint resourceid,
				   guint rdrentryid,
				   GList **thrlist,
				   gchar *err)
{
	SaErrorT		rv;
	SaHpiResourceIdT	rid = (SaHpiResourceIdT) resourceid;
	SaHpiSessionIdT		sid = (SaHpiSessionIdT) sessionid;
	SaHpiRdrT		rdr;
	SaHpiEntryIdT		nextentryid;
	SaHpiSensorRecT		sensor;
	SaHpiEntryIdT		rdrid = (SaHpiEntryIdT) rdrentryid;
	SaHpiSensorThresholdsT	thresholds;
	VohObjectT		*obj;
	GList			*thresholds_list = NULL;

	*thrlist = NULL;

	rv = saHpiRdrGet(sid, rid, rdrid, &nextentryid, &rdr);
	if (rv != SA_OK) {
		VOH_ERROR(err, "Getting sensor thresholds failed", rv);
		return FALSE;
	}

	if (rdr.RdrType != SAHPI_SENSOR_RDR) {
		VOH_ERROR(err, "Getting sensor thresholds failed", -1);
		return FALSE;
	}

	sensor = rdr.RdrTypeUnion.SensorRec;

	rv = saHpiSensorThresholdsGet(sid, rid, sensor.Num, &thresholds);

	if (rv != SA_OK) {
		VOH_ERROR(err, "Getting sensor thresholds failed", rv);
		return FALSE;
	}

	thresholds_list = vohSensorThdMask2List();

	*thrlist = thresholds_list;

	while (thresholds_list != NULL) {
		obj = (VohObjectT *) (thresholds_list->data);
		obj->state = VOH_OBJECT_NOT_AVAILABLE;
		if (sensor.ThresholdDefn.ReadThold & obj->numerical) {
			obj->state |= VOH_OBJECT_READABLE;
		}
		if (sensor.ThresholdDefn.WriteThold & obj->numerical) {
			obj->state |= VOH_OBJECT_WRITABLE;
		}
		obj->data = g_strdup(vohSensorUnits2Short(
					sensor.DataFormat.BaseUnits,
					sensor.DataFormat.Percentage));
		switch (obj->numerical) {
		case SAHPI_STM_LOW_MINOR:
			vohFillSensorReadingValue(obj, &thresholds.LowMinor);
			break;
		case SAHPI_STM_LOW_MAJOR:
			vohFillSensorReadingValue(obj, &thresholds.LowMajor);
			break;
		case SAHPI_STM_LOW_CRIT:
			vohFillSensorReadingValue(obj, &thresholds.LowCritical);
			break;
		case SAHPI_STM_UP_MINOR:
			vohFillSensorReadingValue(obj, &thresholds.UpMinor);
			break;
		case SAHPI_STM_UP_MAJOR:
			vohFillSensorReadingValue(obj, &thresholds.UpMajor);
			break;
		case SAHPI_STM_UP_CRIT:
			vohFillSensorReadingValue(obj, &thresholds.UpCritical);
			break;
		case SAHPI_STM_LOW_HYSTERESIS:
			vohFillSensorReadingValue(obj,
						  &thresholds.NegThdHysteresis);
			break;
		case SAHPI_STM_UP_HYSTERESIS:
			vohFillSensorReadingValue(obj,
						  &thresholds.PosThdHysteresis);
			break;
		}
		thresholds_list = thresholds_list->next;
	}

	return TRUE;
}

gboolean voh_set_sensor_thresholds(guint sessionid,
				   guint resourceid,
				   guint rdrentryid,
				   GList *thrlist,
				   gchar *err)
{
	SaErrorT		rv;
	SaHpiResourceIdT	rid = (SaHpiResourceIdT) resourceid;
	SaHpiSessionIdT		sid = (SaHpiSessionIdT) sessionid;
	SaHpiRdrT		rdr;
	SaHpiEntryIdT		nextentryid;
	SaHpiSensorRecT		sensor;
	SaHpiEntryIdT		rdrid = (SaHpiEntryIdT) rdrentryid;
	SaHpiSensorThresholdsT	thresholds;
	SaHpiSensorReadingT	*thr;
	VohObjectT		*obj;

	if (thrlist == NULL) {
		VOH_ERROR(err, "Setting sensor threshold failed", -1);
		return FALSE;
	}

	rv = saHpiRdrGet(sid, rid, rdrid, &nextentryid, &rdr);
	if (rv != SA_OK) {
		VOH_ERROR(err, "Setting sensor thresholds failed", rv);
		return FALSE;
	}

	if (rdr.RdrType != SAHPI_SENSOR_RDR) {
		VOH_ERROR(err, "Setting sensor thresholds failed", -1);
		return FALSE;
	}

	sensor = rdr.RdrTypeUnion.SensorRec;

	rv = saHpiSensorThresholdsGet(sid, rid, sensor.Num, &thresholds);

	if (rv != SA_OK) {
		VOH_ERROR(err, "Setting sensor thresholds failed", rv);
		return FALSE;
	}
	
	while (thrlist != NULL) {
		obj = (VohObjectT *) (thrlist->data);
		switch (obj->numerical) {
		case SAHPI_STM_LOW_MINOR:
			thr = &(thresholds.LowMinor);	
			break;
		case SAHPI_STM_LOW_MAJOR:
			thr = &(thresholds.LowMajor);
			break;
		case SAHPI_STM_LOW_CRIT:
			thr = &(thresholds.LowCritical);
			break;
		case SAHPI_STM_UP_MINOR:
			thr = &(thresholds.UpMinor);
			break;
		case SAHPI_STM_UP_MAJOR:
			thr = &(thresholds.UpMajor);
			break;
		case SAHPI_STM_UP_CRIT:
			thr = &(thresholds.UpCritical);
			break;
		case SAHPI_STM_LOW_HYSTERESIS:
			thr = &(thresholds.NegThdHysteresis);
			break;
		case SAHPI_STM_UP_HYSTERESIS:
			thr = &(thresholds.PosThdHysteresis);
			break;
		}

		switch (obj->value.type) {
		case VOH_OBJECT_TYPE_INT:
			thr->Value.SensorInt64 = obj->value.vint;
			thr->Type = SAHPI_SENSOR_READING_TYPE_INT64;
			break;
		case VOH_OBJECT_TYPE_UINT:
			thr->Value.SensorUint64 = obj->value.vuint;
			thr->Type = SAHPI_SENSOR_READING_TYPE_UINT64;
			break;
		case VOH_OBJECT_TYPE_FLOAT:
			thr->Value.SensorFloat64 = obj->value.vfloat;
			thr->Type = SAHPI_SENSOR_READING_TYPE_FLOAT64;
			break;
		}
		thrlist = thrlist->next;
	}

	rv = saHpiSensorThresholdsSet(sid, rid, sensor.Num, &thresholds);

	if (rv != SA_OK) {
		VOH_ERROR(err, "Setting sensor thresholds failed", rv);
		return FALSE;
	}

	return TRUE;
}

gboolean voh_get_inventory_info(guint sessionid,
				guint resourceid,
				guint rdrentryid,
				GList **info,
				gchar *err)
{
	SaErrorT		rv;
	SaHpiResourceIdT	rid = (SaHpiResourceIdT) resourceid;
	SaHpiSessionIdT		sid = (SaHpiSessionIdT) sessionid;
	SaHpiRdrT		rdr;
	SaHpiEntryIdT		nextentryid;
	SaHpiInventoryRecT	inventory;
	SaHpiEntryIdT		rdrid = (SaHpiEntryIdT) rdrentryid;
	SaHpiIdrInfoT		idrinfo;
	VohObjectT		*obj;
	GList			*info_list = NULL;
	gchar			str[1024];

	*info = NULL;

	rv = saHpiRdrGet(sid, rid, rdrid, &nextentryid, &rdr);
	if (rv != SA_OK) {
		VOH_ERROR(err, "Getting inventory info failed", rv);
		return FALSE;
	}

	if (rdr.RdrType != SAHPI_INVENTORY_RDR) {
		VOH_ERROR(err, "Getting inventory info", -1);
		return FALSE;
	}

	inventory = rdr.RdrTypeUnion.InventoryRec;

	rv = saHpiIdrInfoGet(sid, rid, inventory.IdrId, &idrinfo);

	if (rv != SA_OK) {
		VOH_ERROR(err, "Getting inventory info failed", rv);
		return FALSE;
	}

	obj = (VohObjectT *) g_malloc(sizeof(VohObjectT));
	info_list = g_list_prepend(info_list, obj);
	obj->name = g_strdup("Read only");
	obj->state = VOH_OBJECT_READABLE;
	obj->value.type = VOH_OBJECT_TYPE_UINT;
	obj->value.vuint = idrinfo.ReadOnly;
	obj->value.vbuffer = g_strdup(vohBoolean2String(idrinfo.ReadOnly));
	obj->data = NULL;

	obj = (VohObjectT *) g_malloc(sizeof(VohObjectT));
	info_list = g_list_prepend(info_list, obj);
	obj->name = g_strdup("Number of areas");
	obj->state = VOH_OBJECT_READABLE;
	obj->value.type = VOH_OBJECT_TYPE_UINT;
	obj->value.vuint = idrinfo.NumAreas;
	sprintf(str, "%d", idrinfo.NumAreas);
	obj->value.vbuffer = g_strdup(str);
	obj->data = NULL;

	obj = (VohObjectT *) g_malloc(sizeof(VohObjectT));
	info_list = g_list_prepend(info_list, obj);
	obj->name = g_strdup("Inventory update counter");
	obj->state = VOH_OBJECT_READABLE;
	obj->value.type = VOH_OBJECT_TYPE_UINT;
	obj->value.vuint = idrinfo.UpdateCount;
	sprintf(str, "%d", idrinfo.UpdateCount);
	obj->value.vbuffer = g_strdup(str);
	obj->data = NULL;

	obj = (VohObjectT *) g_malloc(sizeof(VohObjectT));
	info_list = g_list_prepend(info_list, obj);
	obj->name = g_strdup("Inventory persistant");
	obj->state = VOH_OBJECT_READABLE;
	obj->value.type = VOH_OBJECT_TYPE_UINT;
	obj->value.vuint = inventory.Persistent;
	obj->value.vbuffer = g_strdup(vohBoolean2String(inventory.Persistent));
	obj->data = NULL;

	obj = (VohObjectT *) g_malloc(sizeof(VohObjectT));
	info_list = g_list_prepend(info_list, obj);
	obj->name = g_strdup("FRU inventory entity");
	obj->state = VOH_OBJECT_READABLE;
	obj->value.type = VOH_OBJECT_TYPE_UINT;
	obj->value.vuint = rdr.IsFru;
	obj->value.vbuffer = g_strdup(vohBoolean2String(rdr.IsFru));
	obj->data = NULL;

	obj = (VohObjectT *) g_malloc(sizeof(VohObjectT));
	info_list = g_list_prepend(info_list, obj);
	obj->name = g_strdup("Inventory name");
	obj->state = VOH_OBJECT_READABLE;
	obj->value.type = VOH_OBJECT_TYPE_BUFFER;
	fixstr(&rdr.IdString, str);
	obj->value.vbuffer = g_strdup(str);
	obj->data = NULL;

	*info = info_list;

	return TRUE;
}

gboolean voh_get_idr_area_with_field(guint sessionid,
				      guint resourceid,
				      guint rdrentryid,
				      GList **areas,
				      gchar *err)
{
	SaErrorT		rv;
	SaHpiResourceIdT	rid = (SaHpiResourceIdT) resourceid;
	SaHpiSessionIdT		sid = (SaHpiSessionIdT) sessionid;
	SaHpiRdrT		rdr;
	SaHpiEntryIdT		nextentryid;
	SaHpiInventoryRecT	inventory;
	SaHpiEntryIdT		rdrid = (SaHpiEntryIdT) rdrentryid;
	SaHpiEntryIdT		areaid,	nextareaid;
	SaHpiEntryIdT		fieldid, nextfieldid;
	SaHpiIdrAreaHeaderT	area_header;
	SaHpiIdrFieldT		field;
	VohObjectT		*obj;
	VohObjectT		*fobj;
	GList			*area_list = NULL;
	GList			*field_list = NULL;
	gchar			str[1024];

	*areas = NULL;

	rv = saHpiRdrGet(sid, rid, rdrid, &nextentryid, &rdr);
	if (rv != SA_OK) {
		VOH_ERROR(err, "Getting inventory info failed", rv);
		return FALSE;
	}

	if (rdr.RdrType != SAHPI_INVENTORY_RDR) {
		VOH_ERROR(err, "Getting inventory info", -1);
		return FALSE;
	}

	inventory = rdr.RdrTypeUnion.InventoryRec;

	areaid = SAHPI_FIRST_ENTRY;
	while (areaid != SAHPI_LAST_ENTRY) {
		rv = saHpiIdrAreaHeaderGet(sid, rid, inventory.IdrId,
					   SAHPI_IDR_AREATYPE_UNSPECIFIED,
					   areaid, &nextareaid, &area_header);
		if (rv != SA_OK) {
			break;
		}

		obj = (VohObjectT *) g_malloc(sizeof(VohObjectT));
		area_list = g_list_append(area_list, obj);
		obj->name = g_strdup(vohIdrAreaType2String(area_header.Type));
		obj->numerical = area_header.Type;
		obj->state = VOH_OBJECT_READABLE;
		if (area_header.ReadOnly == FALSE) {
			obj->state |= VOH_OBJECT_WRITABLE;
		}
		obj->id = area_header.AreaId;
		obj->value.type = VOH_OBJECT_TYPE_UINT;
		obj->data = NULL;

		field_list = NULL;

		fieldid = SAHPI_FIRST_ENTRY;
		while (fieldid != SAHPI_LAST_ENTRY) {
			rv = saHpiIdrFieldGet(sid, rid, inventory.IdrId,
					      area_header.AreaId,
					      SAHPI_IDR_FIELDTYPE_UNSPECIFIED,
					      fieldid, &nextfieldid, &field);

			if (rv != SA_OK) {
				break;
			}

			fobj = (VohObjectT *) g_malloc(sizeof(VohObjectT));
			field_list = g_list_append(field_list, fobj);
			fobj->name = g_strdup(vohIdrFieldType2String(
								field.Type));
			fobj->numerical = field.Type;
			fobj->state = VOH_OBJECT_READABLE;
			if (field.ReadOnly == FALSE) {
				fobj->state |= VOH_OBJECT_WRITABLE;
			} else {
				obj->state &=~ VOH_OBJECT_WRITABLE;
			}
			fobj->id = field.FieldId;
			fobj->value.type = VOH_OBJECT_TYPE_UINT;

			fixstr(&(field.Field), str);
			fobj->data = g_strdup(str);

			fieldid = nextfieldid;
		}

		obj->data = field_list;

		areaid = nextareaid;
	}

	*areas = area_list;

	return TRUE;
}

void voh_get_idr_area_type(GList **area_types)
{
	GList		*types;

	*area_types = NULL;

	types = vohIdrAreaType2List();

	*area_types = types;
}

void voh_get_idr_field_type(GList **field_types)
{
	GList		*types;

	*field_types = NULL;

	types = vohIdrFieldType2List();

	*field_types = types;
}

gboolean voh_idr_area_add(guint sessionid,
			  guint resourceid,
			  guint rdrentryid,
			  guint areatype,
			  guint *areaid,
			  gchar *err)
{
	SaErrorT		rv;
	SaHpiResourceIdT	rid = (SaHpiResourceIdT) resourceid;
	SaHpiSessionIdT		sid = (SaHpiSessionIdT) sessionid;
	SaHpiRdrT		rdr;
	SaHpiEntryIdT		nextentryid;
	SaHpiInventoryRecT	inventory;
	SaHpiEntryIdT		rdrid = (SaHpiEntryIdT) rdrentryid;
	SaHpiEntryIdT		aid;

	rv = saHpiRdrGet(sid, rid, rdrid, &nextentryid, &rdr);
	if (rv != SA_OK) {
		VOH_ERROR(err, "Adding inventory area failed", rv);
		return FALSE;
	}

	if (rdr.RdrType != SAHPI_INVENTORY_RDR) {
		VOH_ERROR(err, "Adding inventory area failed", -1);
		return FALSE;
	}

	inventory = rdr.RdrTypeUnion.InventoryRec;

	rv = saHpiIdrAreaAdd(sid, rid, inventory.IdrId, areatype, &aid);

	if (rv != SA_OK) {
		VOH_ERROR(err, "Adding inventory area failed", rv);
		return FALSE;
	}

	if (areaid) {
		*areaid = aid;
	}

	return TRUE;
}

gboolean voh_idr_area_delete(guint sessionid,
			     guint resourceid,
			     guint rdrentryid,
			     guint areaid,
			     gchar *err)
{
	SaErrorT		rv;
	SaHpiResourceIdT	rid = (SaHpiResourceIdT) resourceid;
	SaHpiSessionIdT		sid = (SaHpiSessionIdT) sessionid;
	SaHpiRdrT		rdr;
	SaHpiEntryIdT		nextentryid;
	SaHpiInventoryRecT	inventory;
	SaHpiEntryIdT		rdrid = (SaHpiEntryIdT) rdrentryid;
	SaHpiEntryIdT		aid = (SaHpiEntryIdT) areaid;

	rv = saHpiRdrGet(sid, rid, rdrid, &nextentryid, &rdr);
	if (rv != SA_OK) {
		VOH_ERROR(err, "Deleting inventory area failed", rv);
		return FALSE;
	}

	if (rdr.RdrType != SAHPI_INVENTORY_RDR) {
		VOH_ERROR(err, "Deleting inventory area failed", -1);
		return FALSE;
	}

	inventory = rdr.RdrTypeUnion.InventoryRec;

	rv = saHpiIdrAreaDelete(sid, rid, inventory.IdrId, aid);

	if (rv != SA_OK) {
		VOH_ERROR(err, "Deleting inventory area failed", rv);
		return FALSE;
	}

	return TRUE;
}

gboolean voh_idr_field_add(guint sessionid,
			   guint resourceid,
			   guint rdrentryid,
			   VtDataT *field,
			   gchar *err)
{
	SaErrorT		rv;
	SaHpiResourceIdT	rid = (SaHpiResourceIdT) resourceid;
	SaHpiSessionIdT		sid = (SaHpiSessionIdT) sessionid;
	SaHpiRdrT		rdr;
	SaHpiEntryIdT		nextentryid;
	SaHpiInventoryRecT	inventory;
	SaHpiEntryIdT		rdrid = (SaHpiEntryIdT) rdrentryid;
	SaHpiIdrFieldT		h_field;
	gchar			buffer[1024];

	rv = saHpiRdrGet(sid, rid, rdrid, &nextentryid, &rdr);
	if (rv != SA_OK) {
		VOH_ERROR(err, "Adding inventory field failed", rv);
		return FALSE;
	}

	if (rdr.RdrType != SAHPI_INVENTORY_RDR) {
		VOH_ERROR(err, "Adding inventory field failed", -1);
		return FALSE;
	}

	inventory = rdr.RdrTypeUnion.InventoryRec;

	vt_data_value_get(field, "area_id", &h_field.AreaId);
	vt_data_value_get(field, "type", &h_field.Type);
	vt_data_value_get(field, "read_only", &h_field.ReadOnly);
	vt_data_value_get(field, "field.data", buffer);
	fillstr(&(h_field.Field), buffer);

	rv = saHpiIdrFieldAdd(sid, rid, inventory.IdrId, &h_field);

	if (rv != SA_OK) {
		VOH_ERROR(err, "Adding inventory field failed", rv);
		return FALSE;
	}

	return TRUE;
}

gboolean voh_idr_field_delete(guint sessionid,
			     guint resourceid,
			     guint rdrentryid,
			     guint areaid,
			     guint fieldid,
			     gchar *err)
{
	SaErrorT		rv;
	SaHpiResourceIdT	rid = (SaHpiResourceIdT) resourceid;
	SaHpiSessionIdT		sid = (SaHpiSessionIdT) sessionid;
	SaHpiRdrT		rdr;
	SaHpiEntryIdT		nextentryid;
	SaHpiInventoryRecT	inventory;
	SaHpiEntryIdT		rdrid = (SaHpiEntryIdT) rdrentryid;
	SaHpiEntryIdT		aid = (SaHpiEntryIdT) areaid;
	SaHpiEntryIdT		fid = (SaHpiEntryIdT) fieldid;

	rv = saHpiRdrGet(sid, rid, rdrid, &nextentryid, &rdr);
	if (rv != SA_OK) {
		VOH_ERROR(err, "Deleting inventory field failed", rv);
		return FALSE;
	}

	if (rdr.RdrType != SAHPI_INVENTORY_RDR) {
		VOH_ERROR(err, "Deleting inventory field failed", -1);
		return FALSE;
	}

	inventory = rdr.RdrTypeUnion.InventoryRec;

	rv = saHpiIdrFieldDelete(sid, rid, inventory.IdrId, aid, fid);

	if (rv != SA_OK) {
		VOH_ERROR(err, "Deleting inventory field failed", rv);
		return FALSE;
	}

	return TRUE;
}

gboolean voh_idr_field_get(guint sessionid,
			   guint resourceid,
			   guint rdrentryid,
			   guint areaid,
			   guint fieldtype,
			   guint fieldid,
			   guint *nextfieldid,
			   VtDataT *field,
			   gchar *err)
{
	SaErrorT		rv;
	SaHpiResourceIdT	rid = (SaHpiResourceIdT) resourceid;
	SaHpiSessionIdT		sid = (SaHpiSessionIdT) sessionid;
	SaHpiRdrT		rdr;
	SaHpiEntryIdT		nextentryid;
	SaHpiInventoryRecT	inventory;
	SaHpiEntryIdT		rdrid = (SaHpiEntryIdT) rdrentryid;
	SaHpiIdrFieldT		h_field;
	SaHpiEntryIdT		aid = (SaHpiEntryIdT) areaid;
	SaHpiEntryIdT		fid = (SaHpiEntryIdT) fieldid;
	SaHpiEntryIdT		nfid;
	SaHpiIdrFieldTypeT	ftype = (SaHpiIdrFieldTypeT) fieldtype;
	gchar			dstr[1024];

	rv = saHpiRdrGet(sid, rid, rdrid, &nextentryid, &rdr);
	if (rv != SA_OK) {
		VOH_ERROR(err, "Getting inventory field failed", rv);
		return FALSE;
	}

	if (rdr.RdrType != SAHPI_INVENTORY_RDR) {
		VOH_ERROR(err, "Getting inventory field failed", -1);
		return FALSE;
	}

	inventory = rdr.RdrTypeUnion.InventoryRec;

	rv = saHpiIdrFieldGet(sid, rid, inventory.IdrId, aid, ftype, fid,
			&nfid, &h_field);

	if (rv != SA_OK) {
		VOH_ERROR(err, "Getting inventory field failed", rv);
		return FALSE;
	}

	if (nextfieldid)
		*nextfieldid = nfid;

	vt_data_value_set(field, "area_id", &h_field.AreaId);
	vt_data_value_set(field, "field_id", &h_field.FieldId);
	vt_data_value_set(field, "type", &h_field.Type);
	vt_data_value_set(field, "read_only", &h_field.ReadOnly);
	fixstr(&h_field.Field, dstr);
	vt_data_value_set(field, "field.data", dstr);

	return TRUE;
}

gboolean voh_idr_field_set(guint sessionid,
			   guint resourceid,
			   guint rdrentryid,
			   VtDataT *field,
			   gchar *err)
{
	SaErrorT		rv;
	SaHpiResourceIdT	rid = (SaHpiResourceIdT) resourceid;
	SaHpiSessionIdT		sid = (SaHpiSessionIdT) sessionid;
	SaHpiRdrT		rdr;
	SaHpiEntryIdT		nextentryid;
	SaHpiInventoryRecT	inventory;
	SaHpiEntryIdT		rdrid = (SaHpiEntryIdT) rdrentryid;
	SaHpiIdrFieldT		h_field;
	gchar			buffer[1024];

	rv = saHpiRdrGet(sid, rid, rdrid, &nextentryid, &rdr);
	if (rv != SA_OK) {
		VOH_ERROR(err, "Setting inventory field failed", rv);
		return FALSE;
	}

	if (rdr.RdrType != SAHPI_INVENTORY_RDR) {
		VOH_ERROR(err, "Setting inventory field failed", -1);
		return FALSE;
	}

	inventory = rdr.RdrTypeUnion.InventoryRec;

	vt_data_value_get(field, "area_id", &h_field.AreaId);
	vt_data_value_get(field, "field_id", &h_field.FieldId);
	vt_data_value_get(field, "type", &h_field.Type);
	vt_data_value_get(field, "read_only", &h_field.ReadOnly);
	vt_data_value_get(field, "field.data", buffer);
	fillstr(&(h_field.Field), buffer);

	rv = saHpiIdrFieldSet(sid, rid, inventory.IdrId, &h_field);

	if (rv != SA_OK) {
		VOH_ERROR(err, "Setting inventory field failed", rv);
		return FALSE;
	}

	return TRUE;
}

gboolean voh_get_control_info(guint sessionid,
			      guint resourceid,
			      guint rdrentryid,
			      VtData1T **info,
			      gchar *err)
{
	SaErrorT		rv;
	SaHpiResourceIdT	rid = (SaHpiResourceIdT) resourceid;
	SaHpiSessionIdT		sid = (SaHpiSessionIdT) sessionid;
	SaHpiRdrT		rdr;
	SaHpiEntryIdT		nextentryid;
	SaHpiCtrlRecT		control;
	SaHpiEntryIdT		rdrid = (SaHpiEntryIdT) rdrentryid;
	VtData1T		*data = NULL;
	gchar			name[1024];
	VtHpiDataMap1T		*map;
	VtHpiDataMap1T	map_digital[] = {
		{VT_VAR,	"type",		VT_UINT,
			"Control type",		vt_convert_ctrl_type},
		{VT_VAR,	"write_only",	VT_BOOLEAN,
			"Write only control",	vt_convert_boolean},
		{VT_VAR,	"output_type",	VT_UINT,
			"Control output type",	vt_convert_ctrl_output_type},
		{VT_VAR,	"default_mode",	VT_UINT,
			"Default mode",		vt_convert_ctrl_mode},
		{VT_VAR,	"read_only_mode", VT_BOOLEAN,
			"Read only control mode", vt_convert_boolean},
		{VT_VAR,	"default",	VT_UINT,
			"Control default state", vt_convert_ctrl_state_digital},
		{0,		NULL,		0,	NULL,	NULL}
	};
	VtHpiDataMap1T	map_discrete[] = {
		{VT_VAR,	"type",		VT_UINT,
			"Control type",		vt_convert_ctrl_type},
		{VT_VAR,	"write_only",	VT_BOOLEAN,
			"Write only control",	vt_convert_boolean},
		{VT_VAR,	"output_type",	VT_UINT,
			"Control output type",	vt_convert_ctrl_output_type},
		{VT_VAR,	"default_mode",	VT_UINT,
			"Default mode",		vt_convert_ctrl_mode},
		{VT_VAR,	"read_only_mode", VT_BOOLEAN,
			"Read only control mode", vt_convert_boolean},
		{VT_VAR,	"default",	VT_UINT,
			"Control default state", NULL},
		{0,		NULL,		0,	NULL,	NULL}
	};
	VtHpiDataMap1T	map_analog[] = {
		{VT_VAR,	"type",		VT_UINT,
			"Control type",		vt_convert_ctrl_type},
		{VT_VAR,	"write_only",	VT_BOOLEAN,
			"Write only control",	vt_convert_boolean},
		{VT_VAR,	"output_type",	VT_UINT,
			"Control output type",	vt_convert_ctrl_output_type},
		{VT_VAR,	"default_mode",	VT_UINT,
			"Default mode",		vt_convert_ctrl_mode},
		{VT_VAR,	"read_only_mode", VT_BOOLEAN,
			"Read only control mode", vt_convert_boolean},
		{VT_VAR,	"min",		VT_UINT,
			"Min control state value", NULL},
		{VT_VAR,	"max",		VT_UINT,
			"Max control state value", NULL},
		{VT_VAR,	"default",	VT_UINT,
			"Control default state", NULL},
		{0,		NULL,		0,	NULL,	NULL}
	};
	VtHpiDataMap1T	map_stream[] = {
		{VT_VAR,	"type",		VT_UINT,
			"Control type",		vt_convert_ctrl_type},
		{VT_VAR,	"write_only",	VT_BOOLEAN,
			"Write only control",	vt_convert_boolean},
		{VT_VAR,	"output_type",	VT_UINT,
			"Control output type",	vt_convert_ctrl_output_type},
		{VT_VAR,	"default_mode",	VT_UINT,
			"Default mode",		vt_convert_ctrl_mode},
		{VT_VAR,	"read_only_mode", VT_BOOLEAN,
			"Read only control mode", vt_convert_boolean},
		{VT_VAR,	"default",	VT_BUFFER,
			"Control default state", NULL},
		{0,		NULL,		0,	NULL,	NULL}
	};
	VtHpiDataMap1T	map_text[] = {
		{VT_VAR,	"type",		VT_UINT,
			"Control type",		vt_convert_ctrl_type},
		{VT_VAR,	"write_only",	VT_BOOLEAN,
			"Write only control",	vt_convert_boolean},
		{VT_VAR,	"output_type",	VT_UINT,
			"Control output type",	vt_convert_ctrl_output_type},
		{VT_VAR,	"default_mode",	VT_UINT,
			"Default mode",		vt_convert_ctrl_mode},
		{VT_VAR,	"read_only_mode", VT_BOOLEAN,
			"Read only control mode", vt_convert_boolean},
		{VT_VAR,	"max_chars",	VT_UINT,
			"Maximum of chars per line", NULL},
		{VT_VAR,	"max_lines",	VT_UINT,
			"Maximum of lines",	NULL},
		{VT_VAR,	"line",		VT_UINT,
			"Control default line", NULL},
		{VT_VAR,	"default",	VT_BUFFER,
			"Control default state", NULL},
		{0,		NULL,		0,	NULL,	NULL}
	};
	VtHpiDataMap1T	map_oem[] = {
		{VT_VAR,	"type",		VT_UINT,
			"Control type",		vt_convert_ctrl_type},
		{VT_VAR,	"write_only",	VT_BOOLEAN,
			"Write only control",	vt_convert_boolean},
		{VT_VAR,	"output_type",	VT_UINT,
			"Control output type",	vt_convert_ctrl_output_type},
		{VT_VAR,	"default_mode",	VT_UINT,
			"Default mode",		vt_convert_ctrl_mode},
		{VT_VAR,	"read_only_mode", VT_BOOLEAN,
			"Read only control mode", vt_convert_boolean},
		{VT_VAR,	"m_id", 	VT_UINT,
			"Control manufacturer Id", NULL},
		{VT_VAR,	"config_data",	VT_BUFFER,
			"Control configuration data", NULL},
		{VT_VAR,	"default",	VT_BUFFER,
			"Control default state", NULL},
		{0,		NULL,		0,	NULL,	NULL}
	};


	*info = NULL;


	rv = saHpiRdrGet(sid, rid, rdrid, &nextentryid, &rdr);
	if (rv != SA_OK) {
		VOH_ERROR(err, "Getting control info failed", rv);
		return FALSE;
	}

	if (rdr.RdrType != SAHPI_CTRL_RDR) {
		VOH_ERROR(err, "Getting control info failed", -1);
		return FALSE;
	}

	control = rdr.RdrTypeUnion.CtrlRec;

	switch (control.Type) {
	case SAHPI_CTRL_TYPE_DIGITAL:
		map = map_digital;
		data = vt_data_element_new_by_array1(VT_UNSPECIFIED,
				VT_UNSPECIFIED, NULL, "Control details",
				map);
		vt_data_value_set1(data, "default",
				control.TypeUnion.Digital.Default);
		break;
	case SAHPI_CTRL_TYPE_DISCRETE:
		map = map_discrete;
		data = vt_data_element_new_by_array1(VT_UNSPECIFIED,
				VT_UNSPECIFIED, NULL, "Control details",
				map);
		vt_data_value_set1(data, "default",
				control.TypeUnion.Discrete.Default);
		break;
	case SAHPI_CTRL_TYPE_ANALOG:
		map = map_analog;
		data = vt_data_element_new_by_array1(VT_UNSPECIFIED,
				VT_UNSPECIFIED, NULL, "Control details",
				map);
		vt_data_value_set1(data, "min",
				control.TypeUnion.Analog.Min);
		vt_data_value_set1(data, "max",
				control.TypeUnion.Analog.Max);
		vt_data_value_set1(data, "default",
				control.TypeUnion.Analog.Default);
		break;
	case SAHPI_CTRL_TYPE_STREAM:
		map = map_stream;
		data = vt_data_element_new_by_array1(VT_UNSPECIFIED,
				VT_UNSPECIFIED, NULL, "Control details",
				map);
		memcpy(name, control.TypeUnion.Stream.Default.Stream,
				 control.TypeUnion.Stream.Default.StreamLength);
		name[control.TypeUnion.Stream.Default.StreamLength] = '\0';
		vt_data_value_str_set1(data, "default", name);
		break;
	case SAHPI_CTRL_TYPE_TEXT:
		map = map_text;
		data = vt_data_element_new_by_array1(VT_UNSPECIFIED,
				VT_UNSPECIFIED, NULL, "Control details",
				map);
		vt_data_value_set1(data, "max_chars",
				control.TypeUnion.Text.MaxChars);
		vt_data_value_set1(data, "max_lines",
				control.TypeUnion.Text.MaxLines);
		vt_data_value_set1(data, "line",
				control.TypeUnion.Text.Default.Line);
		fixstr(&control.TypeUnion.Text.Default.Text, name);
		vt_data_value_str_set1(data, "default", name);
		break;
	case SAHPI_CTRL_TYPE_OEM:
		data = vt_data_element_new_by_array1(VT_UNSPECIFIED,
				VT_UNSPECIFIED, NULL, "Control details",
				map_oem);
		vt_data_value_set1(data, "m_id",
				control.TypeUnion.Oem.MId);
		memcpy(name, control.TypeUnion.Oem.ConfigData,
				    SAHPI_CTRL_OEM_CONFIG_LENGTH);
		vt_data_value_str_set1(data, "config_data", name);
		memcpy(name, control.TypeUnion.Oem.Default.Body,
				    control.TypeUnion.Oem.Default.BodyLength);
		name[control.TypeUnion.Oem.Default.BodyLength] = '\0';
		vt_data_value_str_set1(data, "default", name);
		break;
	default:
		return FALSE;
	}

	vt_data_value_set1(data, "type", control.Type);
	vt_data_value_set1(data, "output_type", control.OutputType);
	vt_data_value_set1(data, "write_only", control.WriteOnly);
	vt_data_value_set1(data, "default_mode", control.DefaultMode.Mode);
	vt_data_value_set1(data, "read_only_mode",
				control.DefaultMode.ReadOnly);

	*info = data;

	return TRUE;
}

gboolean voh_control_get(guint sessionid,
			 guint resourceid,
			 guint rdrentryid,
			 VtData1T **mode,
			 VtData1T **state,
			 gchar *err)
{
	SaErrorT		rv;
	SaHpiResourceIdT	rid = (SaHpiResourceIdT) resourceid;
	SaHpiSessionIdT		sid = (SaHpiSessionIdT) sessionid;
	SaHpiRdrT		rdr;
	SaHpiEntryIdT		nextentryid;
	SaHpiCtrlRecT		control;
	SaHpiEntryIdT		rdrid = (SaHpiEntryIdT) rdrentryid;
	VtData1T		*data = NULL;
	SaHpiCtrlStateT		h_state;
	SaHpiCtrlModeT		h_mode;
	gchar			name[1024];
	VtHpiDataMap1T		*map;
	VtHpiDataMap1T	map_digital_state[] = {
		{VT_VAR,	"state",	VT_UINT,
			"Control current state", vt_convert_ctrl_state_digital},
		{0,		NULL,		0,	NULL,	NULL}
	};
	VtHpiDataMap1T	map_discrete_state[] = {
		{VT_VAR,	"state",	VT_UINT,
			"Control current state", NULL},
		{0,		NULL,		0,	NULL,	NULL}
	};
	VtHpiDataMap1T	map_analog_state[] = {
		{VT_VAR,	"state",	VT_UINT,
			"Control current state", NULL},
		{0,		NULL,		0,	NULL,	NULL}
	};
	VtHpiDataMap1T	map_stream_state[] = {
		{VT_VAR,	"repeat",	VT_BOOLEAN,
			"Control repeat flag",	vt_convert_boolean},
		{VT_VAR,	"state",	VT_BUFFER,
			"Control current state", NULL},
		{0,		NULL,		0,	NULL,	NULL}
	};
	VtHpiDataMap1T	map_text_state[] = {
		{VT_VAR,	"line",		VT_UINT,
			"Control current line", NULL},
		{VT_VAR,	"state",	VT_BUFFER,
			"Control current state", NULL},
		{0,		NULL,		0,	NULL,	NULL}
	};
	VtHpiDataMap1T	map_oem_state[] = {
		{VT_VAR,	"state",	VT_BUFFER,
			"Control current state", NULL},
		{0,		NULL,		0,	NULL,	NULL}
	};

	VtHpiDataMap1T map_mode[] = {
		{VT_VAR,	"mode",		VT_UINT,
			"Control current mode", vt_convert_ctrl_mode},
		{0,		NULL,		0,	NULL,	NULL}
	};


	*state = NULL;
	*mode = NULL;

	rv = saHpiRdrGet(sid, rid, rdrid, &nextentryid, &rdr);
	if (rv != SA_OK) {
		VOH_ERROR(err, "Getting control failed", rv);
		return FALSE;
	}

	if (rdr.RdrType != SAHPI_CTRL_RDR) {
		VOH_ERROR(err, "Getting control failed", -1);
		return FALSE;
	}

	control = rdr.RdrTypeUnion.CtrlRec;

	rv = saHpiControlGet(sid, rid, control.Num, &h_mode, &h_state);
	if (rv != SA_OK) {
		VOH_ERROR(err, "Getting control failed", rv);
		return FALSE;
	}

	switch (control.Type) {
	case SAHPI_CTRL_TYPE_DIGITAL:
		map = map_digital_state;
		data = vt_data_element_new_by_array1(VT_UNSPECIFIED,
				VT_UNSPECIFIED, NULL, "Control state",
				map);
		vt_data_value_set1(data, "state",
				h_state.StateUnion.Digital);
		break;
	case SAHPI_CTRL_TYPE_DISCRETE:
		map = map_discrete_state;
		data = vt_data_element_new_by_array1(VT_UNSPECIFIED,
				VT_UNSPECIFIED, NULL, "Control state",
				map);
		vt_data_value_set1(data, "state",
				h_state.StateUnion.Discrete);
		break;
	case SAHPI_CTRL_TYPE_ANALOG:
		map = map_analog_state;
		data = vt_data_element_new_by_array1(VT_UNSPECIFIED,
				VT_UNSPECIFIED, NULL, "Control state",
				map);
		vt_data_value_set1(data, "state",
				h_state.StateUnion.Analog);
		break;
	case SAHPI_CTRL_TYPE_STREAM:
		map = map_stream_state;
		data = vt_data_element_new_by_array1(VT_UNSPECIFIED,
				VT_UNSPECIFIED, NULL, "Control state",
				map);
		memcpy(name, h_state.StateUnion.Stream.Stream,
				 h_state.StateUnion.Stream.StreamLength);
		name[h_state.StateUnion.Stream.StreamLength] = '\0';
		vt_data_value_str_set1(data, "state", name);
		vt_data_value_set1(data, "repeat",
					h_state.StateUnion.Stream.Repeat);
		break;
	case SAHPI_CTRL_TYPE_TEXT:
		map = map_text_state;
		data = vt_data_element_new_by_array1(VT_UNSPECIFIED,
				VT_UNSPECIFIED, NULL, "Control state",
				map);
		vt_data_value_set1(data, "line",
				h_state.StateUnion.Text.Line);
		fixstr(&h_state.StateUnion.Text.Text, name);
		vt_data_value_str_set1(data, "state", name);
		break;
	case SAHPI_CTRL_TYPE_OEM:
		data = vt_data_element_new_by_array1(VT_UNSPECIFIED,
				VT_UNSPECIFIED, NULL, "Control state",
				map_oem_state);
		memcpy(name, h_state.StateUnion.Oem.Body,
				    h_state.StateUnion.Oem.BodyLength);
		name[h_state.StateUnion.Oem.BodyLength] = '\0';
		vt_data_value_str_set1(data, "state", name);
		break;
	default:
		return FALSE;
	}

	*state = data;

	data = vt_data_element_new_by_array1(VT_UNSPECIFIED, VT_UNSPECIFIED,
			NULL, "Control mode", map_mode);
	vt_data_value_set1(data, "mode", h_mode);

	*mode = data;

	return TRUE;
}

GtkListStore *voh_get_ctrl_digital_state_list(void)
{
	GtkListStore	*store;
	GtkTreeIter	iter;

	store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_UINT);
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter,
			   0, vohCtrlStateDigital2String(SAHPI_CTRL_STATE_OFF),
			   1, SAHPI_CTRL_STATE_OFF, 
			   -1);
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter,
			   0, vohCtrlStateDigital2String(SAHPI_CTRL_STATE_ON),
			   1, SAHPI_CTRL_STATE_ON, 
			   -1);
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter,
			   0, vohCtrlStateDigital2String(
				   SAHPI_CTRL_STATE_PULSE_OFF),
			   1, SAHPI_CTRL_STATE_PULSE_OFF,
			   -1);
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter,
			   0, vohCtrlStateDigital2String(
				   SAHPI_CTRL_STATE_PULSE_ON),
			   1, SAHPI_CTRL_STATE_PULSE_ON,
			   -1);

	return store;
}

GtkListStore *voh_get_ctrl_mode_list(void)
{
	GtkListStore	*store;
	GtkTreeIter	iter;

	store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_UINT);
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter,
			   0, vohCtrlMode2String(SAHPI_CTRL_MODE_AUTO),
			   1, SAHPI_CTRL_MODE_AUTO, 
			   -1);
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter,
			   0, vohCtrlMode2String(SAHPI_CTRL_MODE_MANUAL),
			   1, SAHPI_CTRL_MODE_MANUAL, 
			   -1);

	return store;
}

gboolean voh_control_type_get(guint sessionid,
			      guint resourceid,
			      guint rdrentryid,
			      VtData1T **ctrltype,
			      gchar *err)
{
	SaErrorT		rv;
	SaHpiResourceIdT	rid = (SaHpiResourceIdT) resourceid;
	SaHpiSessionIdT		sid = (SaHpiSessionIdT) sessionid;
	SaHpiRdrT		rdr;
	SaHpiEntryIdT		nextentryid;
	SaHpiCtrlRecT		control;
	SaHpiEntryIdT		rdrid = (SaHpiEntryIdT) rdrentryid;
	VtData1T		*data = NULL;
	SaHpiCtrlTypeT		type;
	VtHpiDataMap1T		map_ctrl_type[] = {
		{VT_VAR,	"type",		VT_UINT,
			"Control type", vt_convert_ctrl_type},
		{0,		NULL,		0,	NULL,	NULL}
	};

	*ctrltype = NULL;

	rv = saHpiRdrGet(sid, rid, rdrid, &nextentryid, &rdr);
	if (rv != SA_OK) {
		VOH_ERROR(err, "Getting control type failed", rv);
		return FALSE;
	}

	if (rdr.RdrType != SAHPI_CTRL_RDR) {
		VOH_ERROR(err, "Getting control type failed", -1);
		return FALSE;
	}

	control = rdr.RdrTypeUnion.CtrlRec;

	rv = saHpiControlTypeGet(sid, rid, control.Num, &type);
	if (rv != SA_OK) {
		VOH_ERROR(err, "Getting control type failed", rv);
		return FALSE;
	}

	data = vt_data_element_new_by_array1(VT_UNSPECIFIED,
				VT_UNSPECIFIED, NULL, "Control type",
				map_ctrl_type);
	vt_data_value_set1(data, "type", (gdouble) type);
	*ctrltype = data;

	return TRUE;
}

gboolean voh_control_set(guint sessionid,
			 guint resourceid,
			 guint rdrentryid,
			 VtData1T *mode_data,
			 VtData1T *state_data,
			 gchar *err)
{
	SaErrorT		rv;
	SaHpiResourceIdT	rid = (SaHpiResourceIdT) resourceid;
	SaHpiSessionIdT		sid = (SaHpiSessionIdT) sessionid;
	SaHpiRdrT		rdr;
	SaHpiEntryIdT		nextentryid;
	SaHpiCtrlRecT		control;
	SaHpiEntryIdT		rdrid = (SaHpiEntryIdT) rdrentryid;
	SaHpiCtrlModeT		mode;
	SaHpiCtrlStateT		state;
	gchar			*buffer;

	rv = saHpiRdrGet(sid, rid, rdrid, &nextentryid, &rdr);
	if (rv != SA_OK) {
		VOH_ERROR(err, "Control setting failed", rv);
		return FALSE;
	}

	if (rdr.RdrType != SAHPI_CTRL_RDR) {
		VOH_ERROR(err, "Control setting failed", -1);
		return FALSE;
	}

	control = rdr.RdrTypeUnion.CtrlRec;
	state.Type = control.Type;

	mode = vt_data_value_get_as_int(mode_data, "mode");

	switch (control.Type) {
	case VOH_CONTROL_DIGITAL:
		state.StateUnion.Digital = vt_data_value_get_as_int(state_data,
						"state");
		break;
	case VOH_CONTROL_DISCRETE:
		state.StateUnion.Discrete = vt_data_value_get_as_int(state_data,
						"state");
		break;
	case VOH_CONTROL_ANALOG:
		state.StateUnion.Discrete = vt_data_value_get_as_int(state_data,
						"state");
		break;
	case VOH_CONTROL_STREAM:
		state.StateUnion.Stream.Repeat = vt_data_value_get_as_int(
							state_data, "repeat");
		state.StateUnion.Stream.StreamLength = vt_data_value_get_as_int(
							state_data,
							"stream_length");
		g_stpcpy(state.StateUnion.Stream.Stream,
				vt_data_value_str_get1(state_data, "stream"));
		break;
	case VOH_CONTROL_TEXT:
		state.StateUnion.Text.Line = vt_data_value_get_as_int(
							state_data, "line");
		buffer = g_strdup(vt_data_value_str_get1(state_data,
								"text.data"));
		fillstr(&(state.StateUnion.Text.Text), buffer);
		g_free(buffer);
		break;
	case VOH_CONTROL_OEM:
		state.StateUnion.Oem.BodyLength = vt_data_value_get_as_int(
							state_data,
							"body_length");
		g_stpcpy(state.StateUnion.Oem.Body,
				vt_data_value_str_get1(state_data, "body"));
		break;
	default:
		VOH_ERROR(err, "Setting control failed (unknown control type)",
							-1);
		return FALSE;
	}

	rv = saHpiControlSet(sid, rid, control.Num, mode, &state);
	if (rv != SA_OK) {
		VOH_ERROR(err, "Control setting failed", rv);
		return FALSE;
	}

	return TRUE;
}

gboolean voh_watchdog_timer_get(guint sessionid,
				guint resourceid,
				guint rdrentryid,
				VtData1T **watchdog_data,
				gchar *err)
{
	SaErrorT		rv;
	SaHpiResourceIdT	rid = (SaHpiResourceIdT) resourceid;
	SaHpiSessionIdT		sid = (SaHpiSessionIdT) sessionid;
	SaHpiRdrT		rdr;
	SaHpiEntryIdT		nextentryid;
	SaHpiWatchdogRecT	watchdog;
	SaHpiWatchdogT		watchdog_timer;
	SaHpiEntryIdT		rdrid = (SaHpiEntryIdT) rdrentryid;
	VtData1T		*data = NULL;

	*watchdog_data = NULL;

	rv = saHpiRdrGet(sid, rid, rdrid, &nextentryid, &rdr);
	if (rv != SA_OK) {
		VOH_ERROR(err, "Getting watchdog timer failed", rv);
		return FALSE;
	}

	if (rdr.RdrType != SAHPI_WATCHDOG_RDR) {
		VOH_ERROR(err, "Getting watchdog timer failed", -1);
		return FALSE;
	}

	watchdog = rdr.RdrTypeUnion.WatchdogRec;

	rv = saHpiWatchdogTimerGet(sid, rid, watchdog.WatchdogNum,
							&watchdog_timer);
	if (rv != SA_OK) {
		VOH_ERROR(err, "Getting watchdog timer failed", rv);
		return FALSE;
	}

	data = vt_data_new1(VT_WATCHDOG);
	vt_data_value_set1(data, "log", watchdog_timer.Log);
	vt_data_value_set1(data, "running", watchdog_timer.Running);
	vt_data_value_set1(data, "timer_use", watchdog_timer.TimerUse);
	vt_data_value_set1(data, "timer_action", watchdog_timer.TimerAction);
	vt_data_value_set1(data, "pretimer_interrupt",
					watchdog_timer.PretimerInterrupt);
	vt_data_value_set1(data, "pre_timeout_interval",
					watchdog_timer.PreTimeoutInterval);
	vt_data_value_set1(data, "timer_use_exp_flags",
					watchdog_timer.TimerUseExpFlags);
	vt_data_value_set1(data, "initial_count", watchdog_timer.InitialCount);
	vt_data_value_set1(data, "present_count", watchdog_timer.PresentCount);
	*watchdog_data = data;

	return TRUE;
}

GtkListStore *voh_get_watchdog_timer_use_list(void)
{
	GtkListStore	*store;
	GtkTreeIter	iter;

	store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_UINT);
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter,
			   0, vt_convert_watchdog_timer_use(
				   SAHPI_WTU_NONE),
			   1, SAHPI_WTU_NONE, 
			   -1);
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter,
			   0, vt_convert_watchdog_timer_use(
				   SAHPI_WTU_BIOS_FRB2),
			   1, SAHPI_WTU_BIOS_FRB2, 
			   -1);
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter,
			   0, vt_convert_watchdog_timer_use(
				   SAHPI_WTU_BIOS_POST),
			   1, SAHPI_WTU_BIOS_POST,
			   -1);
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter,
			   0, vt_convert_watchdog_timer_use(
				   SAHPI_WTU_OS_LOAD),
			   1, SAHPI_WTU_OS_LOAD,
			   -1);
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter,
			   0, vt_convert_watchdog_timer_use(
				   SAHPI_WTU_SMS_OS),
			   1, SAHPI_WTU_SMS_OS,
			   -1);
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter,
			   0, vt_convert_watchdog_timer_use(
				   SAHPI_WTU_OEM),
			   1, SAHPI_WTU_OEM,
			   -1);
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter,
			   0, vt_convert_watchdog_timer_use(
				   SAHPI_WTU_UNSPECIFIED),
			   1, SAHPI_WTU_UNSPECIFIED,
			   -1);

	return store;
}

GtkListStore *voh_get_watchdog_action_list(void)
{
	GtkListStore	*store;
	GtkTreeIter	iter;

	store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_UINT);
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter,
			   0, vt_convert_watchdog_action(
				   SAHPI_WA_NO_ACTION),
			   1, SAHPI_WA_NO_ACTION, 
			   -1);
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter,
			   0, vt_convert_watchdog_action(
				   SAHPI_WA_RESET),
			   1, SAHPI_WA_RESET, 
			   -1);
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter,
			   0, vt_convert_watchdog_action(
				   SAHPI_WA_POWER_DOWN),
			   1, SAHPI_WA_POWER_DOWN,
			   -1);
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter,
			   0, vt_convert_watchdog_action(
				   SAHPI_WA_POWER_CYCLE),
			   1, SAHPI_WA_POWER_CYCLE,
			   -1);

	return store;
}

GtkListStore *voh_get_watchdog_pretimer_interrupt_list(void)
{
	GtkListStore	*store;
	GtkTreeIter	iter;

	store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_UINT);
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter,
			   0, vt_convert_watchdog_pretimer_interrupt(
				   SAHPI_WPI_NONE),
			   1, SAHPI_WPI_NONE, 
			   -1);
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter,
			   0, vt_convert_watchdog_pretimer_interrupt(
				   SAHPI_WPI_SMI),
			   1, SAHPI_WPI_SMI, 
			   -1);
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter,
			   0, vt_convert_watchdog_pretimer_interrupt(
				   SAHPI_WPI_NMI),
			   1, SAHPI_WPI_NMI,
			   -1);
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter,
			   0, vt_convert_watchdog_pretimer_interrupt(
				   SAHPI_WPI_MESSAGE_INTERRUPT),
			   1, SAHPI_WPI_MESSAGE_INTERRUPT,
			   -1);
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter,
			   0, vt_convert_watchdog_pretimer_interrupt(
				   SAHPI_WPI_OEM),
			   1, SAHPI_WPI_OEM,
			   -1);

	return store;
}

GList *voh_get_watchdog_exp_flags_list()
{
	GList		*list = NULL;
	VtValNameMapT	*val_name;


	val_name = g_malloc0(sizeof(VtValNameMapT));
	list = g_list_append(list, val_name);
	val_name->m_value = SAHPI_WATCHDOG_EXP_BIOS_FRB2;
	val_name->m_name = g_strdup(vt_convert_watchdog_exp_flags(
					SAHPI_WATCHDOG_EXP_BIOS_FRB2));

	val_name = g_malloc0(sizeof(VtValNameMapT));
	list = g_list_append(list, val_name);
	val_name->m_value = SAHPI_WATCHDOG_EXP_BIOS_POST;
	val_name->m_name = g_strdup(vt_convert_watchdog_exp_flags(
					SAHPI_WATCHDOG_EXP_BIOS_POST));

	val_name = g_malloc0(sizeof(VtValNameMapT));
	list = g_list_append(list, val_name);
	val_name->m_value = SAHPI_WATCHDOG_EXP_OS_LOAD;
	val_name->m_name = g_strdup(vt_convert_watchdog_exp_flags(
					SAHPI_WATCHDOG_EXP_OS_LOAD));

	val_name = g_malloc0(sizeof(VtValNameMapT));
	list = g_list_append(list, val_name);
	val_name->m_value = SAHPI_WATCHDOG_EXP_SMS_OS;
	val_name->m_name = g_strdup(vt_convert_watchdog_exp_flags(
					SAHPI_WATCHDOG_EXP_SMS_OS));

	val_name = g_malloc0(sizeof(VtValNameMapT));
	list = g_list_append(list, val_name);
	val_name->m_value = SAHPI_WATCHDOG_EXP_OEM;
	val_name->m_name = g_strdup(vt_convert_watchdog_exp_flags(
					SAHPI_WATCHDOG_EXP_OEM));

	return list;
}

gboolean voh_watchdog_timer_set(guint sessionid,
				guint resourceid,
				guint rdrentryid,
				VtData1T *watchdog_data,
				gchar *err)
{
	SaErrorT		rv;
	SaHpiResourceIdT	rid = (SaHpiResourceIdT) resourceid;
	SaHpiSessionIdT		sid = (SaHpiSessionIdT) sessionid;
	SaHpiRdrT		rdr;
	SaHpiEntryIdT		nextentryid;
	SaHpiWatchdogRecT	watchdog_entry;
	SaHpiEntryIdT		rdrid = (SaHpiEntryIdT) rdrentryid;
	SaHpiWatchdogT		watchdog;

	rv = saHpiRdrGet(sid, rid, rdrid, &nextentryid, &rdr);
	if (rv != SA_OK) {
		VOH_ERROR(err, "Watchdog timer setting failed", rv);
		return FALSE;
	}

	if (rdr.RdrType != SAHPI_WATCHDOG_RDR) {
		VOH_ERROR(err, "Watchdog timer setting failed", -1);
		return FALSE;
	}

	watchdog_entry = rdr.RdrTypeUnion.WatchdogRec;

	watchdog.Log = vt_data_value_get_as_int(watchdog_data, "log");
	watchdog.Running = vt_data_value_get_as_int(watchdog_data, "running");
	watchdog.TimerUse = vt_data_value_get_as_int(watchdog_data,
							"timer_use");
	watchdog.TimerAction = vt_data_value_get_as_int(watchdog_data,
							"timer_action");
	watchdog.PretimerInterrupt = vt_data_value_get_as_int(watchdog_data,
							"pretimer_interrupt");
	watchdog.PreTimeoutInterval = vt_data_value_get_as_int(watchdog_data,
							"pre_timeout_interval");
	watchdog.InitialCount = vt_data_value_get_as_int(watchdog_data,
							"initial_count");
	watchdog.TimerUseExpFlags = vt_data_value_get_as_int(watchdog_data,
							"timer_use_exp_flags");

	rv = saHpiWatchdogTimerSet(sid, rid, watchdog_entry.WatchdogNum,
								&watchdog);
	if (rv != SA_OK) {
		VOH_ERROR(err, "Watchdog timer setting failed", rv);
		return FALSE;
	}

	return TRUE;
}

gboolean voh_watchdog_timer_reset(guint sessionid,
				  guint resourceid,
				  guint rdrentryid,
				  gchar *err)
{
	SaErrorT		rv;
	SaHpiResourceIdT	rid = (SaHpiResourceIdT) resourceid;
	SaHpiSessionIdT		sid = (SaHpiSessionIdT) sessionid;
	SaHpiRdrT		rdr;
	SaHpiEntryIdT		nextentryid;
	SaHpiWatchdogRecT	watchdog_entry;
	SaHpiEntryIdT		rdrid = (SaHpiEntryIdT) rdrentryid;

	rv = saHpiRdrGet(sid, rid, rdrid, &nextentryid, &rdr);
	if (rv != SA_OK) {
		VOH_ERROR(err, "Watchdog timer reset failed", rv);
		return FALSE;
	}

	if (rdr.RdrType != SAHPI_WATCHDOG_RDR) {
		VOH_ERROR(err, "Watchdog timer reset failed", -1);
		return FALSE;
	}

	watchdog_entry = rdr.RdrTypeUnion.WatchdogRec;

	rv = saHpiWatchdogTimerReset(sid, rid, watchdog_entry.WatchdogNum);
	if (rv != SA_OK) {
		VOH_ERROR(err, "Watchdog timer reset failed", rv);
		return FALSE;
	}

	return TRUE;
}

gboolean voh_domain_info_get(guint sessionid,
			      VtData1T **domain_data,
			      gchar *err)
{
	SaErrorT		rv;
	SaHpiSessionIdT		sid = (SaHpiSessionIdT) sessionid;
	SaHpiDomainInfoT	domain_info;
	VtData1T		*data = NULL;
	gchar			name[1024];

	*domain_data = NULL;

	rv = saHpiDomainInfoGet(sid, &domain_info);
	if (rv != SA_OK) {
		VOH_ERROR(err, "Getting domain info failed", rv);
		return FALSE;
	}

	data = vt_data_new1(VT_DOMAIN_INFO);
	vt_data_value_set1(data, "domain_id", domain_info.DomainId);
	vt_data_value_set1(data, "domain_capabilities",
					domain_info.DomainCapabilities);
	vt_data_value_set1(data, "is_peer", domain_info.IsPeer);
	vt_data_value_set1(data, "drt_update_count",
					domain_info.DrtUpdateCount);
	vt_data_value_set1(data, "drt_update_timestamp",
					domain_info.DrtUpdateTimestamp);
	vt_data_value_set1(data, "rpt_update_count",
					domain_info.RptUpdateCount);
	vt_data_value_set1(data, "rpt_update_timestamp",
					domain_info.RptUpdateTimestamp);
	vt_data_value_set1(data, "dat_update_count",
					domain_info.DatUpdateCount);
	vt_data_value_set1(data, "dat_update_timestamp",
					domain_info.DatUpdateTimestamp);

	vt_data_value_set1(data, "active_alarms",
					domain_info.ActiveAlarms);
	vt_data_value_set1(data, "critical_alarms",
					domain_info.CriticalAlarms);
	vt_data_value_set1(data, "major_alarms",
					domain_info.MajorAlarms);
	vt_data_value_set1(data, "minor_alarms",
					domain_info.MinorAlarms);
	vt_data_value_set1(data, "dat_user_alarm_limit",
					domain_info.DatUserAlarmLimit);
	vt_data_value_set1(data, "dat_overflow",
					domain_info.DatOverflow);
	fixstr(&domain_info.DomainTag, name);
	vt_data_value_str_set1(data, "domain_tag", name);

	*domain_data = data;

	return TRUE;
}

gboolean voh_domain_tag_set(guint sessionid,
			    VtData1T *tag_data,
			    gchar *err)
{
	SaErrorT		rv;
	SaHpiSessionIdT		sid = (SaHpiSessionIdT) sessionid;
	SaHpiTextBufferT	txt_buf;
	gchar			name[1024];

	fillstr(&txt_buf, vt_data_value_str_get1(tag_data, "data"));

	rv = saHpiDomainTagSet(sid, &txt_buf);

	if (rv != SA_OK) {
		VOH_ERROR(err, "Setting domain tag failed", rv);
		return FALSE;
	}

	return TRUE;
}

gboolean voh_alarm_get_next(guint sessionid,
			    guint severity,
			    gboolean unack_only,
			    VtData1T *alarm_data,
			    gchar *err)
{
	SaErrorT		rv;
	SaHpiSessionIdT		sid = (SaHpiSessionIdT) sessionid;
	SaHpiAlarmT		alarm;
	VtData1T		*data = NULL;
	gint			val;
	gchar			name[1024];

	if (alarm_data == NULL) {
		VOH_ERROR(err, "\"alarm get next\" failed (invalid arguments)",
				rv);
		return FALSE;
	}

	data = alarm_data;

	val = vt_data_value_get_as_int(alarm_data, "alarm_id");
	alarm.AlarmId = val;
	val = vt_data_value_get_as_int(alarm_data, "severity");
	alarm.Severity = val;
	rv = saHpiAlarmGetNext(sid, (SaHpiSeverityT) severity,
				(SaHpiBoolT) unack_only, &alarm);
	if (rv != SA_OK) {
		VOH_ERROR(err, "\"alarm get next\" failed", rv);
		return FALSE;
	}

	vt_data_value_set1(data, "alarm_id", alarm.AlarmId);
	vt_data_value_set1(data, "timestamp", alarm.Timestamp);
	vt_data_value_set1(data, "severity", alarm.Severity);
	vt_data_value_set1(data, "acknowledged", alarm.Acknowledged);
	vt_data_value_set1(data, "alarm_cond.type", alarm.AlarmCond.Type);
	vohFullEntityPath2String(&alarm.AlarmCond.Entity, name);
	vt_data_value_str_set1(data, "alarm_cond.entity", name);
	vt_data_value_set1(data, "alarm_cond.domain_id",
					alarm.AlarmCond.DomainId);
	vt_data_value_set1(data, "alarm_cond.resource_id",
					alarm.AlarmCond.ResourceId);
	vt_data_value_set1(data, "alarm_cond.sensor_num",
					alarm.AlarmCond.SensorNum);

	vt_data_value_set1(data, "alarm_cond.event_state",
					alarm.AlarmCond.EventState);
	memcpy(name, alarm.AlarmCond.Name.Value,
				 alarm.AlarmCond.Name.Length);
		name[alarm.AlarmCond.Name.Length] = '\0';
	vt_data_value_str_set1(data, "alarm_cond.name", name);
	vt_data_value_set1(data, "alarm_cond.mid", alarm.AlarmCond.Mid);
	fixstr(&alarm.AlarmCond.Data, name);
	vt_data_value_str_set1(data, "alarm_cond.data", name);

	return TRUE;
}

gboolean voh_alarm_get(guint sessionid,
		       guint entryid,
		       VtData1T **alarm_data,
		       gchar *err)
{
	SaErrorT		rv;
	SaHpiSessionIdT		sid = (SaHpiSessionIdT) sessionid;
	SaHpiAlarmT		alarm;
	VtData1T		*data = NULL;
	gchar			name[1024];

	if (alarm_data == NULL) {
		VOH_ERROR(err, "\"alarm get\" failed (invalid arguments)",
				rv);
		return FALSE;
	}

	*alarm_data = NULL;

	rv = saHpiAlarmGet(sid, (SaHpiAlarmIdT) entryid, &alarm);
	if (rv != SA_OK) {
		VOH_ERROR(err, "\"alarm \" failed", rv);
		return FALSE;
	}

	data = vt_data_new1(VT_ALARM);
	vt_data_value_set1(data, "alarm_id", alarm.AlarmId);
	vt_data_value_set1(data, "timestamp", alarm.Timestamp);
	vt_data_value_set1(data, "severity", alarm.Severity);
	vt_data_value_set1(data, "acknowledged", alarm.Acknowledged);
	vt_data_value_set1(data, "alarm_cond.type", alarm.AlarmCond.Type);
	vohFullEntityPath2String(&alarm.AlarmCond.Entity, name);
	vt_data_value_str_set1(data, "alarm_cond.entity", name);
	vt_data_value_set1(data, "alarm_cond.domain_id",
					alarm.AlarmCond.DomainId);
	vt_data_value_set1(data, "alarm_cond.resource_id",
					alarm.AlarmCond.ResourceId);
	vt_data_value_set1(data, "alarm_cond.sensor_num",
					alarm.AlarmCond.SensorNum);

	vt_data_value_set1(data, "alarm_cond.event_state",
					alarm.AlarmCond.EventState);
	memcpy(name, alarm.AlarmCond.Name.Value,
				 alarm.AlarmCond.Name.Length);
		name[alarm.AlarmCond.Name.Length] = '\0';
	vt_data_value_str_set1(data, "alarm_cond.name", name);
	vt_data_value_set1(data, "alarm_cond.mid", alarm.AlarmCond.Mid);
	fixstr(&alarm.AlarmCond.Data, name);
	vt_data_value_str_set1(data, "alarm_cond.data", name);

	*alarm_data = data;

	return TRUE;
}

GList *voh_get_all_alarms(guint sessionid,
			  gchar *err)
{
	gboolean		res;
	guint			entryid;
	VtData1T		*data = NULL;
	GList			*alarm_list = NULL;

	entryid = SAHPI_FIRST_ENTRY;
	while (entryid != SAHPI_LAST_ENTRY) {
		data = vt_data_new1(VT_ALARM);
		vt_data_value_set1(data, "alarm_id", entryid);
		vt_data_value_set1(data, "timestamp", 0);
		res = voh_alarm_get_next(sessionid, SAHPI_ALL_SEVERITIES, FALSE,
						data, err);
		if (res == FALSE) {
			vt_data_destroy1(data);
			break;
		}
		alarm_list = g_list_append(alarm_list, (gpointer) data);
		entryid = vt_data_value_get_as_int(data, "alarm_id");
	}

	return alarm_list;
}

gboolean voh_event_log_time_get(guint sessionid,
				gint resourceid,
				gfloat *time,
				gchar *err)
{
	SaErrorT		rv;
	SaHpiResourceIdT	rid = (SaHpiResourceIdT) resourceid;
	SaHpiSessionIdT		sid = (SaHpiSessionIdT) sessionid;

	if (time == NULL) {
		VOH_ERROR(err,
			"\"event log time get\" failed (invalid arguments)", -1);
		return FALSE;
	}

	rv = saHpiEventLogTimeGet(sid, rid, (SaHpiTimeT *) time);

	if (rv != SA_OK) {
		VOH_ERROR(err, "\"event log time get\" failed", rv);
		return FALSE;
	}

	return TRUE;
}

gboolean voh_event_log_time_set(guint sessionid,
				gint resourceid,
				gfloat time,
				gchar *err)
{
	SaErrorT		rv;
	SaHpiResourceIdT	rid = (SaHpiResourceIdT) resourceid;
	SaHpiSessionIdT		sid = (SaHpiSessionIdT) sessionid;

	rv = saHpiEventLogTimeSet(sid, rid, (SaHpiTimeT) time);

	if (rv != SA_OK) {
		VOH_ERROR(err, "\"event log time set\" failed", rv);
		return FALSE;
	}

	return TRUE;
}

gboolean voh_event_log_info_get(guint sessionid,
				guint resourceid,
				VtData1T **evlog_data,
				gchar *err)
{
	SaErrorT		rv;
	SaHpiResourceIdT	rid = (SaHpiResourceIdT) resourceid;
	SaHpiSessionIdT		sid = (SaHpiSessionIdT) sessionid;
	SaHpiEventLogInfoT	evlog_info;
	VtData1T		*data = NULL;
	gchar			name[1024];

	*evlog_data = NULL;

	rv = saHpiEventLogInfoGet(sid, rid, &evlog_info);
	if (rv != SA_OK) {
		VOH_ERROR(err, "Getting event log info failed", rv);
		return FALSE;
	}

	data = vt_data_new1(VT_EVENT_LOG_INFO);
	vt_data_value_set1(data, "entries", evlog_info.Entries);
	vt_data_value_set1(data, "size",
					evlog_info.Size);
	vt_data_value_set1(data, "user_event_max_size",
					evlog_info.UserEventMaxSize);
	vt_data_value_set1(data, "update_timestamp",
					evlog_info.UpdateTimestamp);
	vt_data_value_set1(data, "current_time",
					evlog_info.CurrentTime);
	vt_data_value_set1(data, "enabled",
					evlog_info.Enabled);
	vt_data_value_set1(data, "overflow_flag",
					evlog_info.OverflowFlag);
	vt_data_value_set1(data, "overflow_resetable",
					evlog_info.OverflowResetable);
	vt_data_value_set1(data, "overflow_action",
					evlog_info.OverflowAction);

	*evlog_data = data;

	return TRUE;
}

gboolean voh_event_log_state_set(guint sessionid,
				 guint resourceid,
				 gboolean enabled,
				 gchar *err)
{
	SaErrorT		rv;
	SaHpiResourceIdT	rid = (SaHpiResourceIdT) resourceid;
	SaHpiSessionIdT		sid = (SaHpiSessionIdT) sessionid;


	rv = saHpiEventLogStateSet(sid, rid, enabled);

	if (rv != SA_OK) {
		VOH_ERROR(err, "Setting event log enable state failed", rv);
		return FALSE;
	}

	return TRUE;
}

gboolean voh_event_log_clear(guint sessionid,
			     guint resourceid,
			     gchar *err)
{
	SaErrorT		rv;
	SaHpiResourceIdT	rid = (SaHpiResourceIdT) resourceid;
	SaHpiSessionIdT		sid = (SaHpiSessionIdT) sessionid;


	rv = saHpiEventLogClear(sid, rid);

	if (rv != SA_OK) {
		VOH_ERROR(err, "Event log clear failed", rv);
		return FALSE;
	}

	return TRUE;
}

gboolean voh_event_log_overflow_reset(guint sessionid,
				      guint resourceid,
				      gchar *err)
{
	SaErrorT		rv;
	SaHpiResourceIdT	rid = (SaHpiResourceIdT) resourceid;
	SaHpiSessionIdT		sid = (SaHpiSessionIdT) sessionid;


	rv = saHpiEventLogOverflowReset(sid, rid);

	if (rv != SA_OK) {
		VOH_ERROR(err, "Event log overflow reset failed", rv);
		return FALSE;
	}

	return TRUE;
}

GList *voh_get_evlog_entries(guint sessionid,
			     guint resourceid,
			     gchar *err)
{
	SaErrorT		rv;
	SaHpiResourceIdT	rid = (SaHpiResourceIdT) resourceid;
	SaHpiEventLogEntryIdT	entryid,	prev_entryid,	next_entryid;
	SaHpiEventLogEntryT	evlog_entry;
	VtData1T		*data = NULL;
	GList			*evlog_list = NULL;

	entryid = SAHPI_OLDEST_ENTRY;
	while (entryid != SAHPI_NO_MORE_ENTRIES) {
		rv = saHpiEventLogEntryGet(sessionid, rid, entryid,
				&prev_entryid, &next_entryid, &evlog_entry,
				NULL, NULL);
		if (rv != SA_OK) {
			break;
		}
		data = vt_data_new1(VT_EVENT_LOG_ENTRY);
		vt_data_value_set1(data, "entry_id", evlog_entry.EntryId);
		vt_data_value_set1(data, "timestamp",
				evlog_entry.Event.Timestamp);
		vt_data_value_set1(data, "event.source",
					evlog_entry.Event.Source);
		vt_data_value_set1(data, "event.event_type",
					evlog_entry.Event.EventType);
		vt_data_value_set1(data, "event.severity",
					evlog_entry.Event.Severity);

		evlog_list = g_list_append(evlog_list, (gpointer) data);
		entryid = next_entryid;
	}

	return evlog_list;
}

gboolean voh_get_evlog_entry_info(guint sessionid,
				  guint resourceid,
				  guint entryid,
				  VtData1T **entry_data,
				  gchar *err)
{
	SaErrorT		rv;
	SaHpiResourceIdT	rid = (SaHpiResourceIdT) resourceid;
	SaHpiSessionIdT		sid = (SaHpiSessionIdT) sessionid;
	SaHpiEventLogEntryIdT	enid = (SaHpiEventLogEntryIdT) entryid;
	SaHpiEventLogEntryIdT	prev_entryid,	next_entryid;
	SaHpiEventLogEntryT	evlog_info;
	SaHpiEventUnionT	event;
	VtData1T		*data = NULL;
	gchar			name[1024];

	static VtHpiDataMap1T resource_event_map[] = {
	{VT_VAR,	"timestamp",		VT_TIME,
			"Event timestamp",	NULL},
	{VT_VAR,	"severity",		VT_UINT,
			"Event severity",	vt_convert_severity},
	{VT_VAR,	"source",		VT_INT,
			"Source resource id",	NULL},
	{VT_VAR,	"event_type",		VT_UINT,
			"Event type",		vt_convert_resource_event_type},
	{0,		NULL,			0,	NULL,	NULL}
	};

	static VtHpiDataMap1T domain_event_map[] = {
	{VT_VAR,	"timestamp",		VT_TIME,
			"Event timestamp",	NULL},
	{VT_VAR,	"severity",		VT_UINT,
			"Event severity",	vt_convert_severity},
	{VT_VAR,	"source",		VT_INT,
			"Source resource id",	NULL},
	{VT_VAR,	"event_type",		VT_UINT,
			"Event type",		vt_convert_domain_event_type},
	{0,		NULL,			0,	NULL,	NULL}
	};

	static VtHpiDataMap1T sensor_event_map[] = {
	{VT_VAR,	"timestamp",		VT_TIME,
			"Event timestamp",	NULL},
	{VT_VAR,	"severity",		VT_UINT,
			"Event severity",	vt_convert_severity},
	{VT_VAR,	"source",		VT_INT,
			"Source resource id",	NULL},
	{VT_VAR,	"sensor_num",		VT_UINT,
			"Sensor number",	NULL},
	{VT_VAR,	"sensor_type",		VT_UINT,
			"Sensor type",		vt_convert_sensor_type},
	{VT_VAR,	"event_category",	VT_UINT,
			"Event category",	vt_convert_event_category},
	{VT_VAR,	"assertion",		VT_BOOLEAN,
			"Asserted event state",	vt_convert_boolean},
	{0,		NULL,			0,	NULL,	NULL}
	};
	static VtHpiDataMap1T hotswap_event_map[] = {
	{VT_VAR,	"timestamp",		VT_TIME,
			"Event timestamp",	NULL},
	{VT_VAR,	"severity",		VT_UINT,
			"Event severity",	vt_convert_severity},
	{VT_VAR,	"source",		VT_INT,
			"Source resource id",	NULL},
	{VT_VAR,	"hot_swap_state",	VT_UINT,
			"Hot swap state",	vt_convert_hs_state},
	{VT_VAR,	"previous_hot_swap_state",VT_UINT,
			"Previous hot swap state",vt_convert_hs_state},
	{0,		NULL,			0,	NULL,	NULL}
	};

	static VtHpiDataMap1T sensorenable_event_map[] = {
	{VT_VAR,	"timestamp",		VT_TIME,
			"Event timestamp",	NULL},
	{VT_VAR,	"severity",		VT_UINT,
			"Event severity",	vt_convert_severity},
	{VT_VAR,	"source",		VT_INT,
			"Source resource id",	NULL},
	{VT_VAR,	"sensor_num",		VT_UINT,
			"Sensor number",	NULL},
	{VT_VAR,	"sensor_type",		VT_UINT,
			"Sensor type",		vt_convert_sensor_type},
	{VT_VAR,	"event_category",	VT_UINT,
			"Event category",	vt_convert_event_category},
	{VT_VAR,	"sensor_enable",	VT_BOOLEAN,
			"Sensor enable state",	vt_convert_boolean},
	{VT_VAR,	"sensor_event_enable",	VT_BOOLEAN,
			"Sensor event enable state",
						vt_convert_boolean},
	{0,		NULL,			0,	NULL,	NULL}
	};

	static VtHpiDataMap1T watchdog_event_map[] = {
	{VT_VAR,	"timestamp",		VT_TIME,
			"Event timestamp",	NULL},
	{VT_VAR,	"severity",		VT_UINT,
			"Event severity",	vt_convert_severity},
	{VT_VAR,	"source",		VT_INT,
			"Source resource id",	NULL},
	{VT_VAR,	"watchdog_num",		VT_UINT,
			"Watchdog number",	NULL},
	{VT_VAR,	"watchdog_action",	VT_UINT,
			"Watchdog action event",
				vt_convert_watchdog_action_event},
	{VT_VAR,	"watchdog_pre_timer_action",VT_UINT,
			"Watchdog pretimer action",
				vt_convert_watchdog_pretimer_interrupt},
	{VT_VAR,	"watchdog_use",		VT_UINT,
			"Watchdog timer use",	vt_convert_watchdog_timer_use},
	{0,		NULL,			0,	NULL,	NULL}
	};

	static VtHpiDataMap1T hpisw_event_map[] = {
	{VT_VAR,	"timestamp",		VT_TIME,
			"Event timestamp",	NULL},
	{VT_VAR,	"severity",		VT_UINT,
			"Event severity",	vt_convert_severity},
	{VT_VAR,	"source",		VT_INT,
			"Source resource id",	NULL},
	{VT_VAR,	"mid",			VT_UINT,
			"Manufacturer Id",	NULL},
	{VT_VAR,	"type",			VT_UINT,
			"HPI sofware event type",vt_convert_hpi_sw_event_type},
	{VT_VAR,	"event_data",		VT_BUFFER,
			"Event data",		NULL},
	{0,		NULL,			0,	NULL,	NULL}
	};

	static VtHpiDataMap1T oem_event_map[] = {
	{VT_VAR,	"timestamp",		VT_TIME,
			"Event timestamp",	NULL},
	{VT_VAR,	"severity",		VT_UINT,
			"Event severity",	vt_convert_severity},
	{VT_VAR,	"source",		VT_INT,
			"Source resource id",	NULL},
	{VT_VAR,	"mid",			VT_UINT,
			"Manufacturer Id",	NULL},
	{VT_VAR,	"oem_event_data",	VT_BUFFER,
			"Event data",		NULL},
	{0,		NULL,			0,	NULL,	NULL}
	};

	static VtHpiDataMap1T user_event_map[] = {
	{VT_VAR,	"timestamp",		VT_TIME,
			"Event timestamp",	NULL},
	{VT_VAR,	"severity",		VT_UINT,
			"Event severity",	vt_convert_severity},
	{VT_VAR,	"source",		VT_INT,
			"Source resource id",	NULL},
	{VT_VAR,	"user_event_data",	VT_BUFFER,
			"Event data",		NULL},
	{0,		NULL,			0,	NULL,	NULL}
	};

	*entry_data = NULL;

	rv = saHpiEventLogEntryGet(sid, rid, enid,
				&prev_entryid, &next_entryid, &evlog_info,
				NULL, NULL);
	if (rv != SA_OK) {
		VOH_ERROR(err, "Getting event log entry info failed", rv);
		return FALSE;
	}

	event = evlog_info.Event.EventDataUnion;

	switch (evlog_info.Event.EventType) {
	case SAHPI_ET_RESOURCE:
		data = vt_data_element_new_by_array1(VT_UNSPECIFIED,
				VT_UNSPECIFIED, NULL, "Event log entry info",
				resource_event_map);
		vt_data_value_set1(data, "event_type",
					event.ResourceEvent.ResourceEventType);
		break;
	case SAHPI_ET_DOMAIN:
		data = vt_data_element_new_by_array1(VT_UNSPECIFIED,
				VT_UNSPECIFIED, NULL, "Event log entry info",
				domain_event_map);
		vt_data_value_set1(data, "event_type",
					event.DomainEvent.Type);
		break;
	case SAHPI_ET_SENSOR:
		data = vt_data_element_new_by_array1(VT_UNSPECIFIED,
				VT_UNSPECIFIED, NULL, "Event log entry info",
				sensor_event_map);
		vt_data_value_set1(data, "sensor_num",
					event.SensorEvent.SensorNum);
		vt_data_value_set1(data, "sensor_type",
					event.SensorEvent.SensorType);
		vt_data_value_set1(data, "event_category",
					event.SensorEvent.EventCategory);
		vt_data_value_set1(data, "assertion",
					event.SensorEvent.Assertion);
		break;
	case SAHPI_ET_SENSOR_ENABLE_CHANGE:
		data = vt_data_element_new_by_array1(VT_UNSPECIFIED,
				VT_UNSPECIFIED, NULL, "Event log entry info",
				sensorenable_event_map);
		vt_data_value_set1(data, "sensor_num",
				event.SensorEnableChangeEvent.SensorNum);
		vt_data_value_set1(data, "sensor_type",
				event.SensorEnableChangeEvent.SensorType);
		vt_data_value_set1(data, "event_category",
				event.SensorEnableChangeEvent.EventCategory);
		vt_data_value_set1(data, "sensor_enable",
				event.SensorEnableChangeEvent.SensorEnable);
		vt_data_value_set1(data, "sensor_event_enable",
				event.SensorEnableChangeEvent.SensorEventEnable);
		break;
	case SAHPI_ET_HOTSWAP:
		data = vt_data_element_new_by_array1(VT_UNSPECIFIED,
				VT_UNSPECIFIED, NULL, "Event log entry info",
				hotswap_event_map);
		vt_data_value_set1(data, "hot_swap_state",
				event.HotSwapEvent.HotSwapState);
		vt_data_value_set1(data, "previous_hot_swap_state",
				event.HotSwapEvent.PreviousHotSwapState);
		break;
	case SAHPI_ET_WATCHDOG:
		data = vt_data_element_new_by_array1(VT_UNSPECIFIED,
				VT_UNSPECIFIED, NULL, "Event log entry info",
				watchdog_event_map);
		vt_data_value_set1(data, "watchdog_num",
				event.WatchdogEvent.WatchdogNum);
		vt_data_value_set1(data, "watchdog_action",
				event.WatchdogEvent.WatchdogAction);
		vt_data_value_set1(data, "watchdog_pre_timer_action",
				event.WatchdogEvent.WatchdogPreTimerAction);
		vt_data_value_set1(data, "watchdog_use",
				event.WatchdogEvent.WatchdogUse);
		break;
	case SAHPI_ET_HPI_SW:
		data = vt_data_element_new_by_array1(VT_UNSPECIFIED,
				VT_UNSPECIFIED, NULL, "Event log entry info",
				hpisw_event_map);
		vt_data_value_set1(data, "mid",
				event.HpiSwEvent.MId);
		vt_data_value_set1(data, "type",
				event.HpiSwEvent.Type);
		fixstr(&event.HpiSwEvent.EventData, name);
		vt_data_value_str_set1(data, "event_data", name);
		break;
	case SAHPI_ET_OEM:
		data = vt_data_element_new_by_array1(VT_UNSPECIFIED,
				VT_UNSPECIFIED, NULL, "Event log entry info",
				oem_event_map);
		vt_data_value_set1(data, "mid",
				event.OemEvent.MId);
		fixstr(&event.OemEvent.OemEventData, name);
		vt_data_value_str_set1(data, "oem_event_data", name);
		break;
	case SAHPI_ET_USER:
		data = vt_data_element_new_by_array1(VT_UNSPECIFIED,
				VT_UNSPECIFIED, NULL, "Event log entry info",
				user_event_map);
		fixstr(&event.UserEvent.UserEventData, name);
		vt_data_value_str_set1(data, "user_event_data", name);
		break;
	}

	vt_data_value_set1(data, "timestamp", evlog_info.Event.Timestamp);
	vt_data_value_set1(data, "severity",
					evlog_info.Event.Severity);
	vt_data_value_set1(data, "source", evlog_info.Event.Source);

	*entry_data = data;

	return TRUE;
}
