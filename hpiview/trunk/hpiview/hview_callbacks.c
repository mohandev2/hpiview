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
#include "voh.h"
#include "hview_callbacks.h"

void hview_quit_call(GtkWidget *widget, gpointer data)
{
      voh_fini(NULL);
      gtk_main_quit();
}

void hview_empty_log_call(GtkWidget *widget, gpointer data)
{
      HviewWidgetsT	*w = (HviewWidgetsT *) data;
      GtkTextIter	start_iter,	end_iter;
      GtkTextBuffer	*buf;
 
      buf = GTK_TEXT_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW
						     	(w->log_view)));
      gtk_text_buffer_get_start_iter(buf, &start_iter);
      gtk_text_buffer_get_end_iter(buf, &end_iter);
      gtk_text_buffer_delete(buf, &start_iter, &end_iter);

}

void hview_load_plugin_call(GtkWidget *widget, gpointer data)
{
      HviewWidgetsT	*w = (HviewWidgetsT *) data;

      hview_print(w, "\"Load plugin\" is not supported yet");
}

void hview_unload_plugin_call(GtkWidget *widget, gpointer data)
{
      HviewWidgetsT	*w = (HviewWidgetsT *) data;
      hview_print(w, "\"Unload plugin\" is not supported yet");
}

gint hview_discover_thread(gpointer data)
{
      HviewWidgetsT	*w = (HviewWidgetsT *) data;
      GtkTreeModel	*domains;
      gchar		err[100];

      if (voh_init(err) == -1) {
	    hview_statusbar_push(w, "discovering failed");
	    hview_print(w, err);
      } else {
	    domains = voh_list_domains(err);
	    if (domains == NULL)
		  hview_print(w, err);
	    gtk_tree_view_set_model(GTK_TREE_VIEW(w->tree_view), domains);
	    if (domains)
		  g_object_unref(domains);
	    hview_statusbar_push(w, "ready");
      }
      
      gtk_widget_set_sensitive(w->main_window, TRUE);

      return FALSE;
}

void hview_discover_call(GtkWidget *widget, gpointer data)
{
      HviewWidgetsT	*w = (HviewWidgetsT *) data;
      GtkTreeModel	*domains;
      GtkTreeViewColumn	*column;
      gchar		err[100];

      column = gtk_tree_view_get_column(GTK_TREE_VIEW(w->tree_view), 0);
      gtk_tree_view_column_set_title(column, HVIEW_DOMAIN_COLUMN_TITLE);
      gtk_window_set_title(GTK_WINDOW(w->main_window), HVIEW_TITLE);
      gtk_tree_view_set_model(GTK_TREE_VIEW(w->tree_view), NULL);
      gtk_tree_view_set_model(GTK_TREE_VIEW(w->detail_view), NULL);

      voh_fini(NULL);
      hview_statusbar_push(w, "discovering (please wait)");
      gtk_widget_set_sensitive(w->main_window, FALSE);
      gtk_timeout_add(100, hview_discover_thread, data);
      
}

void hview_about_call(void)
{
      GtkWidget		*dialog_window;
      GtkWidget		*button;
      GtkWidget		*label;
      GtkWidget		*text;

      dialog_window = gtk_dialog_new();
      gtk_widget_set_size_request(GTK_WIDGET(dialog_window),
				  HVIEW_ABOUT_WINDOW_WIDTH,
				  HVIEW_ABOUT_WINDOW_HEIGHT);

      button = gtk_button_new_with_label("close");
      gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog_window)->action_area),
			 button, TRUE, TRUE, 0);

      g_signal_connect_swapped(G_OBJECT(button), "clicked",
			       G_CALLBACK(gtk_object_destroy), dialog_window);

      gtk_widget_show(button);

      label = gtk_label_new(HVIEW_ABOUT);
      gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog_window)->vbox),
			 label, TRUE, FALSE, 10);

      gtk_widget_show(label);

      gtk_widget_show(dialog_window);
}

void hview_tree_column_activated_call(GtkTreeViewColumn *column,
					     gpointer data)
{
      HviewWidgetsT	*w = (HviewWidgetsT *) data;
      gchar		*name = NULL;
      int		state;
      GtkTreeSelection	*selection;
      GtkTreeModel	*store;
      GtkTreeModel	*newstore;
      GtkTreeIter	iter;
      gchar		err[100];

      gtk_widget_set_state(GTK_WIDGET(w->rsitem), GTK_STATE_INSENSITIVE);

      state = hview_which_tree_store(column);
      if (state == HVIEW_TREE_STORE_IS_UNKNOWN)
	    return;

      selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(w->tree_view));

      if (gtk_tree_selection_get_selected(selection, &store, &iter)) {
	    gtk_tree_model_get (store, &iter, 0, &name, -1);
      } else {
	    if (state == HVIEW_TREE_STORE_IS_DOMAINS) {
		  hview_print(w, "please select domain");
		  return;	/* domain should be selected */
	    }
      }

      if (state == HVIEW_TREE_STORE_IS_DOMAINS) {
	  gtk_tree_view_column_set_title(column, HVIEW_RESOURCE_COLUMN_TITLE);
	  gtk_window_set_title(GTK_WINDOW(w->main_window), name);
	  newstore = voh_list_resources(err);
	  gtk_tree_view_set_model(GTK_TREE_VIEW(w->tree_view),
				  GTK_TREE_MODEL(newstore));
      } else {
	  gtk_tree_view_column_set_title(column, HVIEW_DOMAIN_COLUMN_TITLE);
	  gtk_window_set_title(GTK_WINDOW(w->main_window), HVIEW_TITLE);
	  newstore = voh_list_domains(err);
	  gtk_tree_view_set_model(GTK_TREE_VIEW(w->tree_view),
				  GTK_TREE_MODEL(newstore));
      }
      gtk_tree_view_set_model(GTK_TREE_VIEW(w->detail_view),
			      NULL);
      if (newstore)
	    g_object_unref(newstore);
      else
	    hview_print(w, err);
      if (name)
	    g_free(name);

}

void hview_tree_row_activated_call(GtkWidget *widget,
					  GtkTreeIter *iter,
					  GtkTreePath *path,
					  gpointer data)
{
      HviewWidgetsT		*w = (HviewWidgetsT *) data;
      GtkTreeViewColumn		*column;
      int			state;

      column = gtk_tree_view_get_column(GTK_TREE_VIEW(w->tree_view), 0);

      state = hview_which_tree_store(column);

      if (state == HVIEW_TREE_STORE_IS_UNKNOWN)
	    return;
      if (state == HVIEW_TREE_STORE_IS_DOMAINS) {
	    hview_tree_column_activated_call(column, (gpointer) w);
      } else {
      }
}

void hview_tree_row_selected_call(GtkTreeSelection *selection,
					 gpointer data)
{
      HviewWidgetsT	*w = (HviewWidgetsT *) data;
      guint		type,	id,	pid;
      GtkTreeModel	*store;
      GtkTreeIter	iter,	parent;
      GtkTreeModel	*details = NULL;
      gchar		err[100];


      gtk_widget_set_state(GTK_WIDGET(w->rsitem), GTK_STATE_INSENSITIVE);

      if (gtk_tree_selection_get_selected(selection, &store, &iter)) {
	    gtk_tree_model_get(store, &iter, VOH_LIST_COLUMN_TYPE, &type, -1);
	    gtk_tree_model_get(store, &iter, VOH_LIST_COLUMN_ID, &id, -1);

	    switch (type) {
	      case VOH_ITER_IS_DOMAIN:
		  details = voh_domain_info(err);
		  break;
	      case VOH_ITER_IS_RPT:
		  details = voh_resource_info(id, err);
		  break;
	      case VOH_ITER_IS_RDR:
		  if (!gtk_tree_model_iter_parent(store, &parent, &iter))
			break;
		  gtk_tree_model_get(store, &parent, VOH_LIST_COLUMN_ID,
				     &pid, -1);
		  details = voh_rdr_info(pid, id, err);

		  gtk_widget_set_sensitive(GTK_WIDGET(w->rsitem), TRUE);
		  break;
	    }
	    gtk_tree_view_set_model(GTK_TREE_VIEW(w->detail_view), details);
	    if (details)
		  g_object_unref(details);
      }
}

gint hview_readsensor_thread(gpointer data)
{
      HviewWidgetsT	*w = (HviewWidgetsT *) data;
      GtkTreeModel	*store;
      GtkTreeViewColumn	*column;
      GtkTreeSelection	*selection;
      GtkTreeIter	iter,	parent,	child;
      guint		id,	pid;
      gchar		err[100],	*val = NULL;


      selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(w->tree_view));

      gtk_tree_selection_get_selected(selection, &store, &iter);
      gtk_tree_model_get(store, &iter, VOH_LIST_COLUMN_ID, &id, -1);

      gtk_tree_model_iter_parent(store, &parent, &iter);
      gtk_tree_model_get(store, &parent, VOH_LIST_COLUMN_ID, &pid, -1);

      val = voh_read_sensor(pid, id, err);
      if (val == NULL) {
	    hview_print(w, err);
	    hview_statusbar_push(w, "sensor reading failed");
      } else {
	    gtk_tree_store_append(GTK_TREE_STORE(store), &child, &iter);
	    gtk_tree_store_set(GTK_TREE_STORE(store), &child,
			       VOH_LIST_COLUMN_NAME, val,
			       VOH_LIST_COLUMN_TYPE,
			       VOH_ITER_IS_PATH,
			       -1);

	    hview_statusbar_push(w, "ready");
	    g_free(val);
      }

      gtk_widget_set_sensitive(w->main_window, TRUE);

      return FALSE;
}

void hview_read_sensor_call(GtkWidget *widget, gpointer data)
{
      HviewWidgetsT	*w = (HviewWidgetsT *) data;
      guint		type;
      GtkTreeModel	*store;
      GtkTreeIter	iter;
      GtkTreeViewColumn		*column;
      GtkTreeSelection	*selection;
      int		state;

      selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(w->tree_view));
      column = gtk_tree_view_get_column(GTK_TREE_VIEW(w->tree_view), 0);
      state = hview_which_tree_store(column);

      switch (state) {
	case HVIEW_TREE_STORE_IS_UNKNOWN:
	case HVIEW_TREE_STORE_IS_DOMAINS:
	    return;
	case HVIEW_TREE_STORE_IS_RESOURCES:
	    if (gtk_tree_selection_get_selected(selection, &store, &iter)) {
		  gtk_tree_model_get(store, &iter, VOH_LIST_COLUMN_TYPE,
				     &type, -1);
		  if (type == VOH_ITER_IS_RDR) {
			hview_statusbar_push(w, "sensor reading (please wait)");
			    gtk_widget_set_sensitive(w->main_window, FALSE);
			    gtk_timeout_add(100, hview_readsensor_thread, data);
		  }
	    } else {
		  hview_print(w, "select sensor please");
	    }
	    break;
      }

}
