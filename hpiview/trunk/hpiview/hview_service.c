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

#include "voh.h"
#include "hview_service.h"


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
			    		(GTK_TEXT_VIEW(w->message_view)));
      gtk_text_buffer_get_end_iter(buf, &iter);
      gtk_text_buffer_insert(buf, &iter, string, -1);
      gtk_text_buffer_insert(buf, &iter, "\n", -1);
      gtk_text_view_set_buffer(GTK_TEXT_VIEW(w->message_view), buf);
}

void hview_event_print(HviewWidgetsT *w, const gchar *string)
{
      GtkTextBuffer	*buf;
      GtkTextIter	iter;

      buf = GTK_TEXT_BUFFER(gtk_text_view_get_buffer
			    		(GTK_TEXT_VIEW(w->event_view)));
      gtk_text_buffer_get_end_iter(buf, &iter);
      gtk_text_buffer_insert(buf, &iter, string, -1);
      gtk_text_buffer_insert(buf, &iter, "\n", -1);
      gtk_text_view_set_buffer(GTK_TEXT_VIEW(w->event_view), buf);
}

void hview_statusbar_push(HviewWidgetsT *w, const gchar *str)
{
      guint	contid;

      gtk_statusbar_push(GTK_STATUSBAR(w->statusbar), contid, str);
}

void hview_tree_cell_func(GtkTreeViewColumn	*col,
			 GtkCellRenderer	*renderer,
			 GtkTreeModel		*model,
			 GtkTreeIter		*iter,
			 gpointer		data)
{
      guint		type,	state;

      g_object_set(renderer, "foreground-set", FALSE,
		   "weight-set", FALSE, "style-set", FALSE, NULL);

      gtk_tree_model_get(model, iter, VOH_LIST_COLUMN_TYPE, &type,
			 VOH_LIST_COLUMN_STATE, &state, -1);

      switch (type) {
	case VOH_ITER_IS_DOMAIN:
	    g_object_set(renderer, "weight", PANGO_WEIGHT_BOLD,
			 "weight-set", TRUE, NULL);
	    break;
	case VOH_ITER_IS_PATH:
	    g_object_set(renderer, "weight", PANGO_WEIGHT_BOLD,
			 "weight-set", TRUE, NULL);
	    break;
	case VOH_ITER_IS_RPT:
	    g_object_set(renderer, "weight", PANGO_WEIGHT_BOLD,
			 "weight-set", TRUE, NULL);
	    break;
	case VOH_ITER_IS_SENSOR:
	    switch (state) {
	      case VOH_ITER_SENSOR_STATE_NOMINAL:
		  g_object_set(renderer, "foreground", "Green",
			       "foreground-set", TRUE, NULL);
		  break;
	      case VOH_ITER_SENSOR_STATE_NORMAL:
		  g_object_set(renderer, "foreground", "Orange",
			       "foreground-set", TRUE, NULL);
		  break;
	      case VOH_ITER_SENSOR_STATE_CRITICAL:
		  g_object_set(renderer, "foreground", "Red",
			       "foreground-set", TRUE, NULL);
		  break;
	      default:
		  break;
	    }
	case VOH_ITER_IS_CONTROL:
	case VOH_ITER_IS_INVENTORY:
	case VOH_ITER_IS_WATCHDOG:
	case VOH_ITER_IS_ANNUNCIATOR:
	    g_object_set(renderer, "style", PANGO_STYLE_ITALIC,
			 "style-set", TRUE, NULL);
	    break;
	default:
	    break;
      }
}

void hview_detail_cell_func(GtkTreeViewColumn	*col,
			 GtkCellRenderer	*renderer,
			 GtkTreeModel		*model,
			 GtkTreeIter		*iter,
			 gpointer		data)
{
      GtkTreeIter	child;
      guint		type,	state;

      g_object_set(renderer, "foreground-set", FALSE,
		   "weight-set", FALSE, "style-set", FALSE, NULL);

      if (gtk_tree_model_iter_children(model, &child, iter) == TRUE) {
	    g_object_set(renderer, "foreground", "Blue",
			 "foreground-set", TRUE, NULL);
      }

      g_object_set(renderer, "weight", PANGO_WEIGHT_BOLD,
		   "weight-set", TRUE,
		   "style", PANGO_STYLE_ITALIC,
		   "style-set", TRUE, NULL);

}

