#include <gtk/gtk.h>

#include "hview_utils.h"


static GList *pixmaps_directories = NULL;

gboolean hutil_find_iter_by_id(GtkTreeModel *model, guint column_num,
			       guint req_id, guint par_id,
			       GtkTreeIter *iter, guint flag)
{
	gboolean	res = TRUE;
	guint		id;
	GtkTreeIter	child,	parent;

	if (model == NULL || iter == NULL) {
		return FALSE;
	}

	if (flag & HUTIL_FIRST_ITER) {
		res = gtk_tree_model_get_iter_first(model, iter);
	}

	while (res != FALSE) {
		gtk_tree_model_get(model, iter, column_num, &id, -1);

		if (req_id == id) {
			if (flag & HUTIL_PARENT_NEEDED) {
				res = gtk_tree_model_iter_parent(model, &parent,
								 iter);
				if (res == TRUE) {
					gtk_tree_model_get(model, &parent,
							column_num, &id, -1);
					if (par_id == id) {
						return TRUE;
					}
				}
			} else {
				return TRUE;
			}
		}

		if (gtk_tree_model_iter_children(model, &child, iter) == TRUE) {
			flag &= ~HUTIL_FIRST_ITER; 
			res = hutil_find_iter_by_id(model, column_num, req_id,
					            par_id, &child, flag);
			if (res == TRUE) {
				*iter = child;
				return TRUE;
			}
		}
		res = gtk_tree_model_iter_next(model, iter);
	}
	return FALSE;
}

gboolean hutil_find_iter_by_name(GtkTreeModel *model, guint column_num,
			         const gchar *req_name, const gchar *par_name,
			         GtkTreeIter *iter, guint flag)
{
	gboolean	res = TRUE;
	gchar		*name;
	GtkTreeIter	child,	parent;

	if (model == NULL || iter == NULL)
		return FALSE;

	if (req_name == NULL)
		return FALSE;

	if (flag & HUTIL_PARENT_NEEDED) {
		if (par_name == NULL)
			return FALSE;
	}

	if (flag & HUTIL_FIRST_ITER) {
		res = gtk_tree_model_get_iter_first(model, iter);
	}

	while (res == TRUE) {
		gtk_tree_model_get(model, iter, column_num, &name, -1);
		if (strcmp(name, req_name) == 0) {
			g_free(name);
			if (flag & HUTIL_PARENT_NEEDED) {
				res = gtk_tree_model_iter_parent(model, &parent,
								 iter);
				if (res == TRUE) {
					gtk_tree_model_get(model, &parent,
							column_num, name, -1);
					if (strcmp(name, req_name) == 0) {
						g_free(name);
						return TRUE;
					}
				}
				g_free(name);
			} else {
				return TRUE;
			}
		}
		g_free(name);

		if (gtk_tree_model_iter_children(model, &child, iter) == TRUE) {
			flag &= ~HUTIL_FIRST_ITER;
			res = hutil_find_iter_by_name(model, column_num,
						      req_name, par_name,
						      &child, flag);
			if (res == TRUE) {
				*iter = child;
				return TRUE;
			}
		}

		res = gtk_tree_model_iter_next(model, iter);
	}
	return FALSE;
}


void hutil_add_pixmap_directory(const gchar *directory)
{
	pixmaps_directories = g_list_prepend (pixmaps_directories,
					      g_strdup (directory));
}

gchar* hutil_find_pixmap_file(const gchar *filename)
{
	GList *elem;

	elem = pixmaps_directories;
      
	while (elem) {
		gchar *pathname = g_strdup_printf ("%s%s%s", (gchar*)elem->data,
						   G_DIR_SEPARATOR_S, filename);

		if (g_file_test (pathname, G_FILE_TEST_EXISTS))
			return pathname;

		g_free (pathname);

		elem = elem->next;
	}
  
	return NULL;
}

GtkWidget *hutil_create_pixmap(const gchar *filename)
{
	gchar *pathname = NULL;
	GtkWidget *pixmap;

	if (!filename || !filename[0])
		return gtk_image_new();

	pathname = hutil_find_pixmap_file (filename);

	if (!pathname) {
		g_warning ("Couldn't find pixmap file: %s", filename);
		return gtk_image_new ();
	}

	pixmap = gtk_image_new_from_file (pathname);

	g_free (pathname);

	return pixmap;
} 

