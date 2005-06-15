
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

#define VT_UNSPECIFIED	0

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

	VT_TIME,

	VT_DOMAIN_INFO,

	VT_IDR_FIELD,
	VT_TEXT_BUFFER,
	VT_TEXT_BUFFER1,
	VT_CTRL_STATE_DIGITAL,
	VT_CTRL_STATE_DISCRETE,
	VT_CTRL_STATE_ANALOG,
	VT_CTRL_STATE_STREAM,
	VT_CTRL_STATE_TEXT,
	VT_CTRL_STATE_OEM,
	VT_CTRL_MODE,

	VT_CONDITION,
	VT_ALARM,

	VT_WATCHDOG,

	VT_EVENT_LOG_INFO,

	VT_EVENT_LOG_ENTRY,
	VT_EVENT,
	VT_RESOURCE_EVENT,
	VT_DOMAIN_EVENT,
	VT_SENSOR_EVENT,
	VT_SENSOR_ENABLE_CHANGE_EVENT,
	VT_HOT_SWAP_EVENT,
	VT_WATCHDOG_EVENT,
	VT_HPI_SW_EVENT,
	VT_OEM_EVENT,
	VT_USER_EVENT,
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
VtDataT *vt_data_element_new_by_array(guint type,
				      guint value_type,
				      const gchar *element,
				      const gchar *name,
				      VtHpiDataMapT *map);

gchar *vt_data_to_buffer(VtDataT *data);

/*----------------------------------------------------------------------------*/

typedef struct VtVarVal {
	gchar	*var;
	gchar	*val;
} VtVarValT;

typedef struct VtHpiDataMap1 {
	guint		type;
	const gchar	*element;
	guint		value_type;
	const gchar	*name;
	const gchar	*(*converter_to_str)(guint val);
} VtHpiDataMap1T;

typedef struct VtDataProp {
	gchar		*name;
	guint		value_type;
	gchar		*buffer;
	gdouble		value;
	const gchar *(*converter_to_str)(guint val);
} VtDataPropT;

typedef struct VtData1 {
	guint			type;
	gchar			*element;
	struct VtData1		*next;
	struct VtData1		*child;
	VtDataPropT		property;
} VtData1T;


VtData1T *vt_data_alloc1(void);
void *vt_data_free1(VtData1T *data);
void vt_data_destroy1(VtData1T *data);
VtData1T *vt_data_append1(VtData1T *parent, VtData1T *node);
VtData1T *vt_data_find1(VtData1T *data, const gchar *element);
gboolean vt_data_value_str_set1(VtData1T *data,
			   	const gchar *element,
			   	gchar *buffer);
const gchar *vt_data_value_str_get1(VtData1T *data, const gchar *element);
gboolean vt_data_value_set1(VtData1T *data,
			   const gchar *element,
			   gdouble value);
VtData1T *vt_data_new1(guint value_type);
VtData1T *vt_data_element_new1(guint value_type, const gchar *element);
VtData1T *vt_data_element_new_by_array1(guint type,
				      guint value_type,
				      const gchar *element,
				      const gchar *name,
				      VtHpiDataMap1T *map);
GList *vt_get_var_val_list(VtData1T *data);
gdouble vt_data_value_get_as_double(VtData1T *data,
			   	     const gchar *element);
gint vt_data_value_get_as_int(VtData1T *data,
			   	     const gchar *element);
guint vt_val_max_len(GList *var_val_list);
guint vt_var_max_len(GList *var_val_list);
void vt_var_align(GList *var_val_list, guint len);

#endif /* __VOH_TYPES_H__ */
