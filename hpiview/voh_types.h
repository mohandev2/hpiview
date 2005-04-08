
#ifndef __VOH_TYPES_H__
#define __VOH_TYPES_H__

#include <glib.h>

typedef struct VohEventState {
	gchar		*string;
	guint		value;
	gboolean	active;
	gpointer	data;
} VohEventStateT;

enum {
	VOH_OBJECT_TYPE_UNSPECIFIED = 0,
	VOH_OBJECT_TYPE_INT,
	VOH_OBJECT_TYPE_UINT,
	VOH_OBJECT_TYPE_FLOAT,
	VOH_OBJECT_TYPE_BUFFER
};

typedef struct VohValue {
	guint		type;
	gint		vint;
	guint		vuint;
	gdouble		vfloat;
	gchar		*vbuffer;
} VohValueT;

#define VOH_OBJECT_NOT_AVAILABLE	0x1
#define VOH_OBJECT_READABLE		0x2
#define VOH_OBJECT_WRITABLE		0x4

typedef struct VohObject {
	gchar		*name;
	guint		numerical;
	VohValueT	value;
	guint		state;
	guint		id;
	gpointer	data;
} VohObjectT;

enum {
	VT_VAR = 1,
	VT_MASK,
	VT_STRUCT,
	VT_UNION,
};

enum {
	VT_UINT = 1,
	VT_UINT64,
	VT_INT,
	VT_INT64,
	VT_FLOAT,
	VT_BUFFER,
	VT_BOOLEAN,

	VT_IDR_FIELD,
	VT_TEXT_BUFFER,
};

typedef struct VtHpiDataMap {
	guint		type;
	const gchar	*element;
	guint		value_type;
	const gchar	*name;
} VtHpiDataMapT;

typedef struct VtDataVal {
	guint		type;
	gpointer	value;
	gchar		*buffer;
} VtDataValT;

typedef struct VtData {
	guint		value_type;
	guint		type;
	gchar		*element;
	gchar		*name;
	struct VtData	*next;
	struct VtData	*child;
	VtDataValT	*value;
} VtDataT;


VtDataT *vt_data_alloc(void);
void *vt_data_free(VtDataT *data);
VtDataT *vt_data_append(VtDataT *parent, VtDataT *node);
VtDataT *vt_data_find(VtDataT *data, const gchar *element);

VtDataValT *vt_data_value_new(guint type);
void vt_data_destroy(VtDataT *data);
void vt_data_value_destroy(VtDataValT *val);
gboolean vt_data_value_set(VtDataT *data,
			   const gchar *element,
			   gpointer value);
gboolean vt_data_value_get(VtDataT *data,
			   const gchar *element,
			   gpointer val);
gchar *vt_data_value_buffer_get(VtDataT *data,
				const gchar *element);

VtDataT	*vt_data_new(guint value_type);
VtDataT *vt_data_element_new(guint value_type, const gchar *element);

void vt_print_structure(VtDataT *structure, guint num);

#endif /* __VOH_TYPES_H__ */
