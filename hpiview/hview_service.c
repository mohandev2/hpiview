/*      -*- linux-c -*-
 *
 * Copyright (c) 2004 by Intel Corp.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *	   Denis Sadykov
 *
 *
 */

#include <gtk/gtk.h>
#include "hview_service.h"

static GList *pixmaps_directories = NULL;

void add_pixmap_directory(const gchar *directory)
{
      pixmaps_directories = g_list_prepend (pixmaps_directories,
					    g_strdup (directory));
}

static gchar* find_pixmap_file(const gchar *filename)
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

GtkWidget *create_pixmap(const gchar *filename)
{
      gchar *pathname = NULL;
      GtkWidget *pixmap;

      if (!filename || !filename[0])
	    return gtk_image_new ();

      pathname = find_pixmap_file (filename);

      if (!pathname)
	  {
	      g_warning ("Couldn't find pixmap file: %s", filename);
	      return gtk_image_new ();
	  }

      pixmap = gtk_image_new_from_file (pathname);

      g_free (pathname);

      return pixmap;
}


int hview_which_tree_store(GtkTreeViewColumn *column)
{
      gchar			*title = NULL;
      int			res;

      title = gtk_tree_view_column_get_title(column);

      if (strcmp(title, HVIEW_DOMAIN_COLUMN_TITLE) == 0) {
	    res = HVIEW_TREE_STORE_IS_DOMAINS;
      } else if (strcmp(title, HVIEW_RESOURCE_COLUMN_TITLE) == 0) {
	   res = HVIEW_TREE_STORE_IS_RESOURCES;
      } else {
	    res = HVIEW_TREE_STORE_IS_UNKNOWN;
      }

      return res;
}

	/* hview_print() puts message to "log window"
	 * arguments:
	 * 	const gchar string: null-terminated string
	 * 			    that should be in "log window"
	 * return value: none
	 */

void hview_print(HviewWidgetsT *w, const gchar *string)
{
      GtkTextBuffer	*buf;
      GtkTextIter	iter;

      buf = GTK_TEXT_BUFFER(gtk_text_view_get_buffer
			    		(GTK_TEXT_VIEW(w->log_view)));
      gtk_text_buffer_get_end_iter(buf, &iter);
      gtk_text_buffer_insert(buf, &iter, string, -1);
      gtk_text_buffer_insert(buf, &iter, "\n", -1);
      gtk_text_view_set_buffer(GTK_TEXT_VIEW(w->log_view), buf);
}

void hview_statusbar_push(HviewWidgetsT *w, const gchar *str)
{
      guint	contid;

      gtk_statusbar_push(GTK_STATUSBAR(w->statusbar), contid, str);
}

