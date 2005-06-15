#include <glib.h>
#include "voh_types.h"
#include "voh_convert_type.h"



VtDataT *vt_data_alloc(void)
{
	VtDataT		*d;

	d = (VtDataT *) g_malloc(sizeof(VtDataT));
	d->type = 0;
	d->element = NULL;
	d->name = NULL;
	d->next = NULL;
	d->child = NULL;
	d->value = NULL;

	return d;
}

void *vt_data_free(VtDataT *data)
{

	if (data == NULL) {
		return;
	}

	vt_data_value_destroy(data->value);
	if (data->name)
		g_free(data->name);
	if (data->element)
		g_free(data->element);

	g_free(data);

	return;
}
		

VtDataT *vt_data_append(VtDataT *parent, VtDataT *node)
{
	VtDataT		*d;
	VtDataT		*child,	*pchild;

	child = pchild = NULL;

	if (node)
		d = node;
	else
		d = vt_data_alloc();

	if (parent) {
		if (parent->child == NULL) {
			parent->child = d;
		} else {
			child = parent->child;
			while (child != NULL) {
				pchild = child;
				child = child->next;
			}
			pchild->next = d;
		}
	}

	return d;
}

static VtHpiDataMapT text_buffer_map[] = {
	{VT_VAR,	"data_type",		VT_UINT,
							"data type"},
	{VT_VAR,	"language",		VT_UINT,
							"language"},
	{VT_VAR,	"data_length",		VT_UINT,
							"data length"},
	{VT_VAR,	"data",			VT_BUFFER,
							"data"},
	{0,		NULL,			0,	 NULL}
};

static VtHpiDataMapT idr_field_map[] = {
	{VT_VAR,	"idr_id",		VT_UINT,
							"inventory Id"},
	{VT_VAR,	"area_id",		VT_UINT,
							"area Id"},
	{VT_VAR,	"field_id",		VT_UINT,
							"field Id"},
	{VT_VAR,	"type",			VT_UINT,
							"field type"},
	{VT_VAR,	"read_only",		VT_BOOLEAN,
							"read only"},
	{VT_STRUCT,	"field",		VT_TEXT_BUFFER,
							"field"},
	{0,		NULL,			0, 	 NULL}
};

static VtHpiDataMap1T text_buffer1_map[] = {
	{VT_VAR,	"data_type",		VT_UINT,
			"Data type",		NULL},
	{VT_VAR,	"language",		VT_UINT,
			"Language",		NULL},
	{VT_VAR,	"data_length",		VT_UINT,
			"Data length",		NULL},
	{VT_VAR,	"data",			VT_BUFFER,
			"Data",			NULL},
	{0,		NULL,			0,	 NULL}
};

static VtHpiDataMap1T ctrl_state_oem_map[] = {
	{VT_VAR,	"mid",			VT_UINT,
			"Manufacturer Id",	NULL},
	{VT_VAR,	"body_length",		VT_UINT,
			"Body length",		NULL},
	{VT_VAR,	"body",			VT_BUFFER,
			"Body",			NULL},
	{0,		NULL,			0, 	 NULL,	NULL}
};

static VtHpiDataMap1T ctrl_state_text_map[] = {
	{VT_VAR,	"line",			VT_UINT,
			"Control state line",	NULL},
	{VT_STRUCT,	"text",			VT_TEXT_BUFFER1,
			"Text buffer",		NULL},
	{0,		NULL,			0,	NULL,	NULL}
};

static VtHpiDataMap1T ctrl_state_stream_map[] = {
	{VT_VAR,	"repeat",		VT_BOOLEAN,
			"Repeat flag",		NULL},
	{VT_VAR,	"stream_length",	VT_UINT,
			"Stream length",	NULL},
	{VT_VAR,	"stream",		VT_BUFFER,
			"Stream",		NULL},
	{0,		NULL,			0,	NULL,	NULL}
};

static VtHpiDataMap1T ctrl_state_analog_map[] = {
	{VT_VAR,	"state",			VT_INT,
			"Control state",	NULL},
	{0,		NULL,			0,	NULL,	NULL}
};

static VtHpiDataMap1T ctrl_state_discrete_map[] = {
	{VT_VAR,	"state",		VT_UINT,
			"Control state",	NULL},
	{0,		NULL,			0,	NULL,	NULL}
};

static VtHpiDataMap1T ctrl_state_digital_map[] = {
	{VT_VAR,	"state",		VT_UINT,
			"Control state",	vt_convert_ctrl_state_digital},
	{0,		NULL,			0,	NULL,	NULL}
};

static VtHpiDataMap1T ctrl_mode_map[] = {
	{VT_VAR,	"mode",			VT_UINT,
			"Control mode",		vt_convert_ctrl_mode},
	{0,		NULL,			0,	NULL,	NULL}
};


static VtHpiDataMap1T watchdog_map[] = {
	{VT_VAR,	"log",			VT_BOOLEAN,
			"Watchdog log",		vt_convert_boolean},
	{VT_VAR,	"running",		VT_BOOLEAN,
			"Watchdog running",	vt_convert_boolean},
	{VT_VAR,	"timer_use",		VT_UINT,
			"Timer use",		vt_convert_watchdog_timer_use},
	{VT_VAR,	"timer_action",		VT_UINT,
			"Timer action",
					vt_convert_watchdog_action},
	{VT_VAR,	"pretimer_interrupt",	VT_UINT,
			"Pretimer interrupt",
					vt_convert_watchdog_pretimer_interrupt},
	{VT_VAR,	"pre_timeout_interval",	VT_UINT,
			"Pretimeout interval",	NULL},
	{VT_VAR,	"timer_use_exp_flags",	VT_UINT,
			"Timer use expiration flags",
					vt_convert_watchdog_exp_flags},
	{VT_VAR,	"initial_count",	VT_UINT,
			"Initial counter",	vt_convert_watchdog_counter},
	{VT_VAR,	"present_count",	VT_UINT,
			"Present counter",	vt_convert_watchdog_counter},
	{0,		NULL,			0,	NULL,	NULL}
};

static VtHpiDataMap1T domain_info_map[] = {
	{VT_VAR,	"domain_id",		VT_UINT,
			"Domain ID",		NULL},
	{VT_VAR,	"domain_capabilities",	VT_UINT,
			"Domain capabilities",	vt_convert_domain_capabilities},
	{VT_VAR,	"is_peer",		VT_BOOLEAN,
			"Peer domain",		vt_convert_boolean},
	{VT_VAR,	"domain_tag",		VT_BUFFER,
			"Domain tag",		NULL},
	{VT_VAR,	"drt_update_count",	VT_UINT,
			"DRT update count",	NULL},
	{VT_VAR,	"drt_update_timestamp",VT_TIME,
			"DRT update timestamp",NULL},
	{VT_VAR,	"rpt_update_count",	VT_UINT,
			"RPT update count",	NULL},
	{VT_VAR,	"rpt_update_timestamp",	VT_TIME,
			"RPT update timestamp",	NULL},
	{VT_VAR,	"dat_update_count",	VT_UINT,
			"DAT update count",	NULL},
	{VT_VAR,	"dat_update_timestamp",	VT_TIME,
			"DAT update timestamp",	NULL},
	{VT_VAR,	"active_alarms",	VT_UINT,
			"Active alarms count",	NULL},
	{VT_VAR,	"critical_alarms",	VT_UINT,
			"Critical alarms count",NULL},
	{VT_VAR,	"major_alarms",		VT_UINT,
			"Major alarms count",	NULL},
	{VT_VAR,	"minor_alarms",		VT_UINT,
			"Minor alarms count",	NULL},
	{VT_VAR,	"dat_user_alarm_limit",	VT_UINT,
			"Maximum of user alarms",NULL},
	{VT_VAR,	"dat_overflow",		VT_BOOLEAN,
			"DAT overflow",		vt_convert_boolean},
	{0,		NULL,			0,	NULL,	NULL}
};

static VtHpiDataMap1T condition_map[] = {
	{VT_VAR,	"type",			VT_UINT,
			"Status condition type",vt_convert_status_cond_type},
	{VT_VAR,	"entity",		VT_BUFFER,
			"Associated entity",	NULL},
	{VT_VAR,	"domain_id",		VT_UINT,
			"Domain Id",		NULL},
	{VT_VAR,	"resource_id",		VT_UINT,
			"Resource Id",		NULL},
	{VT_VAR,	"sensor_num",		VT_UINT,
			"Sensor number",	NULL},
	{VT_VAR,	"event_state",		VT_UINT,
			"Sensor event state",	vt_convert_event_state},
	{VT_VAR,	"name",			VT_BUFFER,
			"Status condition name",NULL},
	{VT_VAR,	"mid",			VT_UINT,
			"Manufacturer Id",	NULL},
	{VT_VAR,	"data",			VT_BUFFER,
			"Optional data",	NULL},
	{0,		NULL,			0,	NULL,	NULL}
};

static VtHpiDataMap1T alarm_map[] = {
	{VT_VAR,	"alarm_id",		VT_UINT,
			"Alarm Id",		NULL},
	{VT_VAR,	"timestamp",		VT_TIME,
			"Alarm adding time",	NULL},
	{VT_VAR,	"severity",		VT_UINT,
			"Severity of alarm",	vt_convert_severity},
	{VT_VAR,	"acknowledged",		VT_BOOLEAN,
			"Alarm acknowledged flag",vt_convert_boolean},
	{VT_STRUCT,	"alarm_cond",		VT_CONDITION,
			"Alarm condition details",NULL},
	{0,		NULL,			0,	NULL,	NULL}
};

static VtHpiDataMap1T event_log_info_map[] = {
	{VT_VAR,	"entries",		VT_UINT,
			"Active entries count",	NULL},
	{VT_VAR,	"size",			VT_UINT,
			"Maximum entries",	NULL},
	{VT_VAR,	"user_event_max_size",	VT_UINT,
			"Maximum size of user buffer",NULL},
	{VT_VAR,	"update_timestamp",	VT_TIME,
			"Last update time",	NULL},
	{VT_VAR,	"current_time",		VT_TIME,
			"Current event log timestamp",NULL},
	{VT_VAR,	"enabled",		VT_BOOLEAN,
			"Event log enable",	vt_convert_boolean},
	{VT_VAR,	"overflow_flag",	VT_BOOLEAN,
			"Event log is overflowed",vt_convert_boolean},
	{VT_VAR,	"overflow_resetable",	VT_BOOLEAN,
			"Overflow is resetable",vt_convert_boolean},
	{VT_VAR,	"overflow_action",	VT_UINT,
			"Event log behavior while overflow",
				vt_convert_event_log_overflow_action},
	{0,		NULL,			0,	NULL,	NULL}
};

static VtHpiDataMap1T event_log_entry_map[] = {
	{VT_VAR,	"entry_id",		VT_UINT,
			"Event log entry Id",	NULL},
	{VT_VAR,	"timestamp",		VT_TIME,
			"Event log entry timestamp",NULL},
	{VT_STRUCT,	"event",		VT_EVENT,
			"Event",		NULL},
	{0,		NULL,			0,	NULL,	NULL}
};

static VtHpiDataMap1T event_map[] = {
	{VT_VAR,	"source",		VT_UINT,
			"Event source",		NULL},
	{VT_VAR,	"event_type",		VT_UINT,
			"Event type",		vt_convert_event_type},
	{VT_VAR,	"severity",		VT_UINT,
			"Event severity",	vt_convert_severity},
	{0,		NULL,			0,	NULL,	NULL}
};


static VtHpiDataMap1T resource_event_map[] = {
	{VT_VAR,	"resource_event_type",	VT_UINT,
			"Resource event type",	vt_convert_resource_event_type},
	{0,		NULL,			0,	NULL,	NULL}
};

static VtHpiDataMap1T domain_event_map[] = {
	{VT_VAR,	"domain",		VT_UINT,
			"Domain event type",	vt_convert_domain_event_type},
	{VT_VAR,	"domain_id",		VT_UINT,
			"Domain Id",		NULL},
	{0,		NULL,			0,	NULL,	NULL}
};

static VtHpiDataMap1T sensor_event_map[] = {
	{VT_VAR,	"sensor_num",		VT_UINT,
			"Sensor number",	NULL},
	{VT_VAR,	"sensor_type",		VT_UINT,
			"Sensor type",		vt_convert_sensor_type},
	{VT_VAR,	"assertion",		VT_BOOLEAN,
			"Asserted event state",	vt_convert_boolean},
	{0,		NULL,			0,	NULL,	NULL}
};

static VtHpiDataMap1T hot_swap_event_map[] = {
	{VT_VAR,	"hot_swap_state",	VT_UINT,
			"Hot swap event state",	vt_convert_hs_state},
	{VT_VAR,	"previous_hot_swap_state",VT_UINT,
			"Previous hot swap state",vt_convert_hs_state},
	{0,		NULL,			0,	NULL,	NULL}
};

static VtHpiDataMap1T sensor_enable_change_event_map[] = {
	{VT_VAR,	"sensor_num",		VT_UINT,
			"Sensor number",	NULL},
	{VT_VAR,	"sensor_type",		VT_UINT,
			"Sensor type",		vt_convert_sensor_type},
	{VT_VAR,	"sensor_enable",	VT_BOOLEAN,
			"Sensor enable state",	vt_convert_boolean},
	{VT_VAR,	"sensor_event_enable",	VT_BOOLEAN,
			"Sensor event enable state",
						vt_convert_boolean},
	{0,		NULL,			0,	NULL,	NULL}
};

static VtHpiDataMap1T watchdog_event_map[] = {
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

static VtHpiDataMap1T hpi_sw_event_map[] = {
	{VT_VAR,	"mid",			VT_UINT,
			"Manufacturer Id",	NULL},
	{VT_VAR,	"type",			VT_UINT,
			"HPI sofware event type",vt_convert_hpi_sw_event_type},
	{VT_VAR,	"event_data",		VT_BUFFER,
			"Event data",		NULL},
	{0,		NULL,			0,	NULL,	NULL}
};

static VtHpiDataMap1T oem_event_map[] = {
	{VT_VAR,	"mid",			VT_UINT,
			"Manufacturer Id",	NULL},
	{VT_VAR,	"oem_event_data",	VT_BUFFER,
			"Event data",		NULL},
	{0,		NULL,			0,	NULL,	NULL}
};

static VtHpiDataMap1T user_event_map[] = {
	{VT_VAR,	"user_event_data",	VT_BUFFER,
			"Event data",		NULL},
	{0,		NULL,			0,	NULL,	NULL}
};

VtDataValT *vt_data_value_new(guint type)
{
	VtDataValT	*val;

	val = (VtDataValT *) g_malloc(sizeof(VtDataValT));

	val->type = type;
	val->value = NULL;
	val->buffer = NULL;

	return val;
}

void vt_data_value_destroy(VtDataValT *val)
{
	if (val == NULL)
		return;
	if (val->value)
		g_free(val->value);
	if (val->buffer)
		g_free(val->buffer);
	g_free(val);
	return;
}

void vt_data_destroy(VtDataT *data)
{
	if (data == NULL)
		return;
	vt_data_destroy(data->next);
	vt_data_destroy(data->child);
	vt_data_free(data);
}

VtDataT *vt_data_find(VtDataT *data, const gchar *element)
{
	VtDataT		*d;

	if (element == NULL)
		return;

	while (data != NULL) {
		if (strcmp(element, data->element) == 0)
			return data;
		d = vt_data_find(data->child, element);
		if (d)
			return d;
		data = data->next;
	}

	return NULL;
}

gboolean vt_data_value_get(VtDataT *data,
			   const gchar *element,
			   gpointer value)
{
	VtDataT		*node;
	VtDataValT	*nval;
	guint		len;
	gchar		func_str[] = "(VOH) vt_data_value_get";

	if (data == NULL || element == NULL || value == NULL)
		return FALSE;

	node = vt_data_find(data, element);
	if (node == NULL) {
		g_error("%s : data element \"%s\" doesn't contain \"%s\"",
				func_str, data->element, element);
	}
	if (node->type != VT_VAR) {
		g_error("%s : wrong data element \"%s\"",
				func_str, element);
	}

	nval = node->value;
	if (nval == NULL) {
		g_error("%s : data element \"%s\" has been corrupted",
				func_str, data->element);
	}

	switch (nval->type) {
	case VT_UINT:
		len = sizeof(guint);
		break;
	case VT_INT:
		len = sizeof(gint);
		break;
	case VT_BOOLEAN:
		len = sizeof(gboolean);
		break;
	case VT_FLOAT:
		len = sizeof(gfloat);
		break;
	case VT_BUFFER:
		len = strlen(nval->value) + 1;
		break;
	default:
		g_error("%s : data element \"%s\" has been corrupted",
				func_str, data->element);
	}
	g_memmove(value, nval->value, len);

	return TRUE;
}

gchar *vt_data_value_buffer_get(VtDataT *data,
				const gchar *element)
{
	VtDataT		*node;
	VtDataValT	*nval;
	gchar		func_str[] = "(VOH) vt_data_value_buffer_get";

	if (data == NULL || element == NULL)
		return NULL;

	node = vt_data_find(data, element);
	if (node == NULL) {
		g_error("%s : data element \"%s\" doesn't contain \"%s\"",
				func_str, data->element, element);
	}
	if (node->type != VT_VAR) {
		g_error("%s : wrong data element \"%s\"",
				func_str, element);
	}

	nval = node->value;
	if (nval == NULL) {
		g_error("%s : data element \"%s\" has been corrupted",
				func_str, data->element);
	}

	return g_strdup(nval->buffer);
}

gboolean vt_data_value_set(VtDataT *data,
			   const gchar *element,
			   gpointer value)
{
	VtDataT		*node;
	VtDataValT	*val;
	guint		len;
	gchar		func_str[] = "(VOH) vt_data_value_set";

	if (data == NULL || element == NULL)
		return FALSE;

	node = vt_data_find(data, element);
	if (node == NULL) {
		g_error("%s : data element \"%s\" doesn't contain \"%s\"",
				func_str, data->element, element);
		return FALSE;
	}
	if (node->type != VT_VAR) {
		g_error("%s : wrong data element \"%s\"",
				func_str, element);
		return FALSE;
	}

	val = node->value;
	if (val->value)
		g_free(val->value);
	if (val->buffer)
		g_free(val->buffer);

	switch (val->type) {
	case VT_BOOLEAN:
		len = sizeof(gboolean);
		if (*((gboolean *) value) == TRUE)
			val->buffer = g_strdup("true");
		else
			val->buffer = g_strdup("false");
	case VT_UINT:
		len = sizeof(guint);
		val->buffer = g_strdup_printf("%d", *((guint *) value));
		break;
	case VT_INT:
		len = sizeof(gint);
		val->buffer = g_strdup_printf("%d", *((gint *) value));
		break;
	case VT_FLOAT:
		len = sizeof(gfloat);
		val->buffer = g_strdup_printf("%lld", *((gfloat *) value));
		break;
	case VT_BUFFER:
		val->value = g_strdup((const gchar *) value);
		val->buffer = g_strdup((const gchar *) value);
		return TRUE;
	default:
		return FALSE;
	}

	val->value = g_memdup(value, len);

	return TRUE;
}

gboolean vt_data_value_buffer_set(VtDataT *data,
				  const gchar *element,
				  const gchar *buffer)
{
	VtDataT		*node;
	VtDataValT	*val;
	guint		len;
	gchar		func_str[] = "(VOH) vt_data_value_buffer_set";

	if (data == NULL || element == NULL)
		return FALSE;

	node = vt_data_find(data, element);
	if (node == NULL) {
		g_error("%s : data element \"%s\" doesn't contain \"%s\"",
				func_str, data->element, element);
		return FALSE;
	}
	if (node->type != VT_VAR) {
		g_error("%s : wrong data element \"%s\"",
				func_str, element);
		return FALSE;
	}

	val = node->value;
	if (val->buffer)
		g_free(val->buffer);
	val->buffer = g_strdup((const gchar *) buffer);

	return TRUE;
}

VtDataT	*vt_data_new(guint value_type)
{
	return vt_data_element_new(value_type, NULL);
}

VtDataT *vt_data_element_new(guint value_type, const gchar *element)
{
	VtDataT		*data;
	VtHpiDataMapT	*map;
	guint		type;
	gchar		*name;
	gchar		func_str[] = "(VOH) vt_data_element_new";

	switch (value_type) {
	case VT_IDR_FIELD:
		type = VT_STRUCT;
		name = g_strdup("Inventory data record field");
		map = idr_field_map;
		break;
	case VT_TEXT_BUFFER:
		type = VT_STRUCT;
		name = g_strdup("Text buffer");
		map = text_buffer_map;
		break;
	default:
		g_warning("%s : wrong data type \"%d\"",
				func_str, value_type);
		return NULL;
	}

	data = vt_data_element_new_by_array(type, value_type,
							element, name, map);

	g_free(name);

	return data;
}

VtDataT *vt_data_element_new_by_array(guint type,
				      guint value_type,
				      const gchar *element,
				      const gchar *name,
				      VtHpiDataMapT *map)
{
	VtDataT		*data,	*d;
	gchar		func_str[] = "(VOH) vt_data_element_new_by_array";

	if (map == NULL) {
		g_error("%s : Invalid parameters", func_str);
	}

	if (type <= 0)
		type = VT_STRUCT;

	data = vt_data_alloc();
	data->type = type;
	data->value_type = value_type;

	if (element) {
		data->element = g_strdup(element);
		if (name == NULL)
			data->name = g_strdup(element);
	} else {
		if (name) {
			data->element = g_strdup(name);
			data->name = g_strdup(name);
		} else {
			data->element = g_strdup("unspecified");
			data->name = g_strdup("unspecified");
		}
	}

	d = data;

	while (map->element != NULL) {
		switch (map->type) {
		case VT_VAR:
			d = vt_data_append(data, NULL);
			d->type = VT_VAR;
			if (element)
				d->element = g_strconcat(element, ".",
						map->element, NULL);
			else
				d->element = g_strdup(map->element);
			d->name = g_strdup(map->name);
			d->value = vt_data_value_new(map->value_type);

			break;
		case VT_MASK:
			break;
		case VT_STRUCT:
			d = vt_data_element_new(map->value_type,
							map->element);
			if (d)
				d = vt_data_append(data, d);
			break;
		case VT_UNION:
			break;
		default:
			break;
		}
		map++;
	}
	return data;
}

gchar *vt_data_to_buffer(VtDataT *data)
{

	VtDataT		*child;
	VtDataValT	*val;
	gchar		*str = NULL,	*str1;


	if (data->type == VT_STRUCT) {
		child = data->child;
	} else if (data->type == VT_VAR) {
		child = data;
	} else {
		return NULL;
	}
	while (child != NULL) {
		val = child->value;
		switch (child->type) {
		case VT_VAR:
			if (str == NULL) {
				str = g_strdup(child->name);
			} else {
				str1 = str;
				str = g_strconcat(str, "\n", child->name, NULL);
				g_free(str1);
			}
			if (val->buffer) {
				str1 = str;
				str = g_strconcat(str, "\t", val->buffer, NULL);
				g_free(str1);
			}
			break;
		case VT_MASK:
			break;
		case VT_STRUCT:
			if (str == NULL) {
				str = g_strdup(vt_data_to_buffer(child));
			} else {
				str1 = str;
				str = g_strconcat(str, "\n",
						vt_data_to_buffer(child), NULL);
				g_free(str1);
			}
			break;
		case VT_UNION:
			break;
		default:
			break;
		}
		printf("\n");
		child = child->next;
	}
	return str;
}


/*---------------------------------------------------------------------------*/

VtData1T *vt_data_alloc1(void)
{

	return g_malloc0(sizeof(VtData1T));
}

void *vt_data_free1(VtData1T *data)
{

	if (data == NULL) {
		return;
	}

	if (data->element) {
		g_free(data->element);
	}
	if (data->property.name) {
		g_free(data->property.name);
	}
	if (data->property.buffer) {
		g_free(data->property.buffer);
	}
//	if (data->property.converter_to_str) {
//		g_free(data->property.converter_to_str);
//	}

	g_free(data);

	return;
}

void vt_data_destroy1(VtData1T *data)
{
	if (data == NULL)
		return;
	vt_data_destroy1(data->next);
	vt_data_destroy1(data->child);
	vt_data_free1(data);
}

VtData1T *vt_data_append1(VtData1T *parent, VtData1T *node)
{
	VtData1T		*d;
	VtData1T		*child,	*pchild;

	child = pchild = NULL;

	if (node)
		d = node;
	else
		d = vt_data_alloc1();

	if (parent) {
		if (parent->child == NULL) {
			parent->child = d;
		} else {
			child = parent->child;
			while (child != NULL) {
				pchild = child;
				child = child->next;
			}
			pchild->next = d;
		}
	}

	return d;
}

VtData1T *vt_data_find1(VtData1T *data, const gchar *element)
{
	VtData1T	*d;
	gchar		func_str[] = "(VOH) vt_data_find";

	if (element == NULL)
		return NULL;

	while (data != NULL) {
		if (data->type == VT_STRUCT) {
			d = vt_data_find1(data->child, element);
			if (d)
				return d;
		}
		if (data->element == NULL) {
			g_error("%s : data element \"%s\" has been corrupted",
					func_str, element);
		}
		if (strcmp(element, data->element) == 0)
			return data;
		data = data->next;
	}

	return NULL;
}

gdouble vt_data_value_get_as_double(VtData1T *data,
			   	     const gchar *element)
{
	VtData1T	*node;
	guint		len;
	gchar		func_str[] = "(VOH) vt_data_value_get";
	int		i;

	node = vt_data_find1(data, element);
	if (node == NULL) {
		g_error("%s : data element \"%s\" doesn't contain \"%s\"",
				func_str, data->element, element);
	}
	if (node->type != VT_VAR) {
		g_error("%s : wrong data element \"%s\"",
				func_str, element);
	}

	switch (node->property.value_type) {
	case VT_UINT:
	case VT_INT:
	case VT_BOOLEAN:
	case VT_FLOAT:
	case VT_TIME:
		return ((gdouble) node->property.value);
	case VT_BUFFER:	
		g_error("%s : data element \"%s\" has baffer type value",
				func_str, element);
	default:
		g_error("%s : data element \"%s\" has been corrupted",
				func_str, node->element);
	}
}

gint vt_data_value_get_as_int(VtData1T *data,
			   	     const gchar *element)
{
	VtData1T	*node;
	guint		len;
	gchar		func_str[] = "(VOH) vt_data_value_get";
	int		i;
	gboolean	vb;

	node = vt_data_find1(data, element);
	if (node == NULL) {
		g_error("%s : data element \"%s\" doesn't contain \"%s\"",
				func_str, data->element, element);
	}
	if (node->type != VT_VAR) {
		g_error("%s : wrong data element \"%s\"",
				func_str, element);
	}

	switch (node->property.value_type) {
	case VT_UINT:
	case VT_INT:
	case VT_BOOLEAN:
	case VT_FLOAT:
	case VT_TIME:
		return ((gint) node->property.value);
	case VT_BUFFER:	
		g_error("%s : data element \"%s\" has baffer type value",
				func_str, node->element);
	default:
		g_error("%s : data element \"%s\" has been corrupted",
				func_str, node->element);
	}
}

const gchar *vt_data_value_str_get1(VtData1T *data, const gchar *element)
{
	VtData1T	*node;
	guint		v_uint;
	gint		v_int;
	gfloat		v_float;
	static gchar	valstr[1024];
	gchar		func_str[] = "(VOH) vt_data_value_buffer_get";

	if (data == NULL || element == NULL)
		return NULL;

	node = vt_data_find1(data, element);
	if (node == NULL) {
		g_error("%s : data element \"%s\" doesn't contain \"%s\"",
				func_str, data->element, element);
	}
	if (node->type != VT_VAR) {
		g_error("%s : wrong data element \"%s\"",
				func_str, element);
	}

	if (node->property.converter_to_str == NULL) {
		switch (node->property.value_type) {
		case VT_UINT:
		case VT_BOOLEAN:
			v_uint = (guint) node->property.value;
			g_sprintf(valstr, "%d", v_uint);
			return valstr;
		case VT_INT:
			v_int = (gint) node->property.value;
			g_sprintf(valstr, "%d", v_int);
			return valstr;
		case VT_FLOAT:
		case VT_TIME:
			g_sprintf(valstr, "%f", node->property.value);
			return valstr;
		case VT_BUFFER:
			return (const gchar *) node->property.buffer;
		}
	} else {
		v_uint = (guint) node->property.value;
		return node->property.converter_to_str(v_uint);
	}
}

gboolean vt_data_value_set1(VtData1T *data,
			   const gchar *element,
			   gdouble value)
{
	VtData1T	*node;
	guint		len;
	gchar		func_str[] = "(VOH) vt_data_value_set";

	if (data == NULL || element == NULL)
		return FALSE;

	node = vt_data_find1(data, element);
	if (node == NULL) {
		g_error("%s : data element \"%s\" doesn't contain \"%s\"",
				func_str, data->element, element);
	}
	if (node->type != VT_VAR) {
		g_error("%s : wrong data element \"%s\"",
				func_str, element);
	}
	switch (node->property.value_type) {
	case VT_BOOLEAN:
	case VT_UINT:
	case VT_INT:
	case VT_TIME:
	case VT_FLOAT:
		node->property.value = value;
		break;
	case VT_BUFFER:
		g_error("%s : data element \"%s\" has buffer type value",
				func_str, node->element);
	default:
		g_error("%s : data element \"%s\" has been corrupted",
				func_str, node->element);
	}

	return TRUE;
}

gboolean vt_data_value_str_set1(VtData1T *data,
			   	const gchar *element,
			   	gchar *buffer)
{
	VtData1T	*node;
	guint		len;
	gchar		func_str[] = "(VOH) vt_data_value_str_set";

	if (data == NULL || element == NULL || buffer == NULL)
		return FALSE;

	node = vt_data_find1(data, element);
	if (node == NULL) {
		g_error("%s : data element \"%s\" doesn't contain \"%s\"",
				func_str, data->element, element);
	}
	if (node->type != VT_VAR) {
		g_error("%s : wrong data element \"%s\"",
				func_str, element);
	}

	switch (node->property.value_type) {
	case VT_BOOLEAN:
	case VT_UINT:
	case VT_INT:
	case VT_FLOAT:
	case VT_TIME:
		g_error("%s : data element \"%s\" has not buffer type value",
				func_str, node->element);
	case VT_BUFFER:
		len = strlen(buffer);
		node->property.buffer = g_memdup(buffer, len + 1);
		break;
	default:
		g_error("%s : data element \"%s\" has been corrupted",
				func_str, node->element);
	}

	return TRUE;
}


VtData1T *vt_data_new1(guint value_type)
{
	return vt_data_element_new1(value_type, NULL);
}

VtData1T *vt_data_element_new1(guint value_type, const gchar *element)
{
	VtData1T	*data;
	VtHpiDataMap1T	*map;
	guint		type = VT_STRUCT;
	gchar		*name;
	gchar		func_str[] = "(VOH) vt_data_element_new";

	switch (value_type) {
	case VT_IDR_FIELD:
		break;
	case VT_TEXT_BUFFER:
		break;
	case VT_TEXT_BUFFER1:
		map = text_buffer1_map;
		break;
	case VT_CTRL_STATE_DIGITAL:
		map = ctrl_state_digital_map;
		break;
	case VT_CTRL_STATE_DISCRETE:
		map = ctrl_state_discrete_map;
		break;
	case VT_CTRL_STATE_ANALOG:
		map = ctrl_state_analog_map;
		break;
	case VT_CTRL_STATE_STREAM:
		map = ctrl_state_stream_map;
		break;
	case VT_CTRL_STATE_TEXT:
		map = ctrl_state_text_map;
		break;
	case VT_CTRL_STATE_OEM:
		map = ctrl_state_oem_map;
		break;
	case VT_CTRL_MODE:
		map = ctrl_mode_map;
		break;
	case VT_WATCHDOG:
		map = watchdog_map;
		break;
	case VT_DOMAIN_INFO:
		map = domain_info_map;
		break;
	case VT_CONDITION:
		map = condition_map;
		break;
	case VT_ALARM:
		map = alarm_map;
		break;
	case VT_EVENT_LOG_INFO:
		map = event_log_info_map;
		break;
	case VT_EVENT_LOG_ENTRY:
		map = event_log_entry_map;
		break;
	case VT_EVENT:
		map = event_map;
		break;
	case VT_RESOURCE_EVENT:
		map = resource_event_map;
		break;
	case VT_DOMAIN_EVENT:
		map = domain_event_map;
		break;
	case VT_SENSOR_EVENT:
		map = sensor_event_map;
		break;
	case VT_SENSOR_ENABLE_CHANGE_EVENT:
		map = sensor_enable_change_event_map;
		break;
	case VT_HOT_SWAP_EVENT:
		map = hot_swap_event_map;
		break;
	case VT_WATCHDOG_EVENT:
		map = watchdog_event_map;
		break;
	case VT_HPI_SW_EVENT:
		map = hpi_sw_event_map;
		break;
	case VT_OEM_EVENT:
		map = oem_event_map;
		break;
	case VT_USER_EVENT:
		map = user_event_map;
		break;
	default:
		g_warning("%s : wrong data type \"%d\"",
				func_str, value_type);
		return NULL;
	}

	data = vt_data_element_new_by_array1(type, value_type,
							element, NULL, map);


	return data;
}

VtData1T *vt_data_element_new_by_array1(guint type,
				      guint value_type,
				      const gchar *element,
				      const gchar *name,
				      VtHpiDataMap1T *map)
{
	VtData1T	*data,	*d;
	gchar		func_str[] = "(VOH) vt_data_element_new_by_array";

	if (map == NULL) {
		g_error("%s : Invalid parameters", func_str);
	}

	if (type <= 0)
		type = VT_STRUCT;

	data = vt_data_alloc1();
	data->type = type;
	data->property.value_type = value_type;

	if (element) {
		data->element = g_strdup(element);
		if (name == NULL)
			data->property.name = g_strdup(element);
	} else {
		if (name) {
			data->element = g_strdup(name);
			data->property.name = g_strdup(name);
		} else {
			data->element = g_strdup("unspecified");
			data->property.name = g_strdup("unspecified");
		}
	}

	d = data;

	while (map->element != NULL) {
		switch (map->type) {
		case VT_VAR:
			d = vt_data_append1(data, NULL);
			d->type = VT_VAR;
			if (element)
				d->element = g_strconcat(element, ".",
						map->element, NULL);
			else
				d->element = g_strdup(map->element);
			d->property.name = g_strdup(map->name);
			d->property.value_type = map->value_type;
			d->property.converter_to_str = map->converter_to_str;
			break;
		case VT_MASK:
			break;
		case VT_STRUCT:
			d = vt_data_element_new1(map->value_type,
							map->element);
			if (d)
				d = vt_data_append1(data, d);
			break;
		case VT_UNION:
			break;
		default:
			break;
		}
		map++;
	}
	return data;
}

GList *vt_get_var_val_list(VtData1T *data)
{
	VtVarValT	*var_val;
	GList		*list = NULL;
	guint		max_var_len,	max_val_len;
	gchar		func_str[] = "(VOH) vt_get_var_val_list";

	if (data == NULL) {
		return NULL;
	}

	while (data) {
		switch (data->type) {
		case VT_STRUCT:
			list = g_list_concat(list, vt_get_var_val_list(
								data->child));
			break;
		case VT_VAR:
			var_val = g_malloc0(sizeof(VtVarValT));
			if (data->property.name) {
				var_val->var = g_strdup(data->property.name);
				if (data->property.value_type == VT_TIME) {
					var_val->val = g_strdup(
							vt_convert_time(
							data->property.value));
				} else {
					var_val->val = g_strdup(
							vt_data_value_str_get1(
							data, data->element));
				}
			} else {
				var_val->var = g_strdup("unspecified");
				var_val->val = g_strdup("unspecified");
			}
			list = g_list_append(list, (gpointer) var_val);
			break;
		default:
			g_error("%s : Invalid data type (%d)", func_str,
								data->type);
		}
		data = data->next;
	}

//	max_var_len = vt_var_max_len(list);
//	max_val_len = vt_val_max_len(list);

	return list;
}

guint vt_var_max_len(GList *var_val_list)
{
	guint		len = 0,	l = 0;
	VtVarValT	*var_val;

	while (var_val_list) {
		if (var_val_list->data == NULL)
			continue;
		var_val = (VtVarValT *) var_val_list->data;
		l = strlen(var_val->var);
		if (l > len)
			len = l;
		var_val_list = var_val_list->next;
	}

	return len;
}

guint vt_val_max_len(GList *var_val_list)
{
	guint		len = 0,	l = 0;
	VtVarValT	*var_val;

	while (var_val_list) {
		if (var_val_list->data == NULL)
			continue;
		var_val = (VtVarValT *) var_val_list->data;
		l = strlen(var_val->val);
		if (l > len)
			len = l;
		var_val_list = var_val_list->next;
	}

	return len;
}

void vt_var_align(GList *var_val_list, guint len)
{
	VtVarValT	*var_val;
	gchar		*var;

	while (var_val_list) {
		if (var_val_list->data == NULL)
			continue;
		var_val = (VtVarValT *) var_val_list->data;
		if (var_val->var == NULL)
			continue;
		while ((len - strlen(var_val->var)) > 0) {
			var = var_val->var;
			var_val->var = g_strconcat(var, " ", NULL);
			g_free(var);
		}

		var_val_list = var_val_list->next;
	}
}
