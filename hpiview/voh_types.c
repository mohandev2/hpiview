#include <glib.h>
#include "voh_types.h"



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

VtDataT	*vt_data_new(guint value_type)
{
	return vt_data_element_new(value_type, NULL);
}

VtDataT *vt_data_element_new(guint value_type, const gchar *element)
{
	VtDataT		*data,	*d;
	VtHpiDataMapT	*map;
	gchar		func_str[] = "(VOH) vt_data_new";

	data = vt_data_alloc();
	switch (value_type) {
	case VT_IDR_FIELD:
		data->type = VT_STRUCT;
		if (element)
			data->element = g_strdup(element);
		else
			data->element = g_strdup("idr_field");
		data->name = g_strdup("Inventory data record field");
		map = idr_field_map;
		break;
	case VT_TEXT_BUFFER:
		data->type = VT_STRUCT;
		if (element)
			data->element = g_strdup(element);
		else
			data->element = g_strdup("text_buffer");
		data->name = g_strdup("Text buffer");
		map = text_buffer_map;
		break;
	default:
		vt_data_free(data);
		g_warning("%s : wrong data type \"%d\"",
				func_str, value_type);
		return NULL;
	}

	data->value_type = value_type;

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

void vt_print_structure(VtDataT *structure, guint num)
{

	VtDataT		*child;
	VtDataValT	*val;
	guint		i;

	for (i = 0; i < num; i++) {
		printf("  ");
	}
	printf("%s:\n", structure->element);

	child = structure->child;
	while (child != NULL) {
		switch (child->type) {
		case VT_VAR:
			val = child->value;
			for (i = 0; i <= num; i++)
				printf("  ");
			printf("%s = ", child->element);
			if (val->buffer) {
				printf("%s", val->buffer);
			}
			printf("\n");
			break;
		case VT_MASK:
			break;
		case VT_STRUCT:
			vt_print_structure(child, num + 1);
			break;
		case VT_UNION:
			break;
		default:
			break;
		}
		child = child->next;
	}
}

