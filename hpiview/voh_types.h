
#ifndef __VOH_TYPES_H__
#define __VOH_TYPES_H__

#include <glib.h>

typedef struct VohEventState {
	gchar		*string;
	guint		value;
	gboolean	active;
	gpointer	data;
} VohEventStateT;

#endif /* __VOH_TYPES_H__ */
