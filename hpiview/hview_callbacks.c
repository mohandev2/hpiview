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

void hview_session_close_call(GtkWidget *widget, gpointer data)
{
      HviewWidgetsT	*w = (HviewWidgetsT *) data;
      gint		page;

      page = gtk_notebook_get_current_page(GTK_NOTEBOOK(w->tab_windows));
      if (page < 0)
	    return;
      gtk_notebook_remove_page(GTK_NOTEBOOK(w->tab_windows), page);

      w->tab_views[page].tree_view = w->tab_views[page].detail_view = NULL;

      gtk_widget_queue_draw(w->tab_windows);
}

gint hview_discover_thread(gpointer data)
{
      HviewWidgetsT	*w = (HviewWidgetsT *) data;
      GtkTreeModel	*res;
      GtkWidget		*tview;
      gint		page;
      gchar		err[100];

      page = gtk_notebook_get_current_page(GTK_NOTEBOOK(w->tab_windows));
      if (page < 0)
	    return FALSE;
      if (w->tab_views[page].tree_view == NULL ||
	  w->tab_views[page].detail_view == NULL)
	    return FALSE;
      tview = w->tab_views[page].tree_view;

      if (voh_init(err) == -1) {
	    hview_statusbar_push(w, "discovering failed");
	    hview_print(w, err);
      } else {
	    res = voh_list_resources(err);
	    if (res == NULL)
		  hview_print(w, err);
	    else {
		  gtk_tree_view_set_model(GTK_TREE_VIEW(tview), res);
		  g_object_unref(res);
	    }
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
      GtkWidget		*tview,	*dview;
      gint		page;
      gchar		err[100];

      page = gtk_notebook_get_current_page(GTK_NOTEBOOK(w->tab_windows));
      if (page < 0)
	    return;
      if (w->tab_views[page].tree_view == NULL ||
	  w->tab_views[page].detail_view == NULL)
	    return;
      tview = w->tab_views[page].tree_view;
      dview = w->tab_views[page].detail_view;

      gtk_tree_view_set_model(GTK_TREE_VIEW(tview), NULL);
      gtk_tree_view_set_model(GTK_TREE_VIEW(dview), NULL);

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


void hview_domain_row_activated_call(GtkWidget *widget,
				     GtkTreeIter *iter,
				     GtkTreePath *path,
				     gpointer data)
{
      HviewWidgetsT		*w = (HviewWidgetsT *) data;


}

void hview_tree_row_selected_call(GtkTreeSelection *selection,
					 gpointer data)
{
      HviewWidgetsT	*w = (HviewWidgetsT *) data;
      guint		type,	id,	pid;
      GtkTreeModel	*store;
      GtkTreeIter	iter,	parent;
      GtkTreeModel	*details = NULL;
      GtkWidget		*dview;
      gint		page;
      gchar		err[100];

      page = gtk_notebook_get_current_page(GTK_NOTEBOOK(w->tab_windows));
      if (page < 0)
	    return;
      if (w->tab_views[page].tree_view == NULL ||
	  w->tab_views[page].detail_view == NULL)
	    return;
      dview = w->tab_views[page].detail_view;

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
	      case VOH_ITER_IS_CONTROL:
	      case VOH_ITER_IS_SENSOR:
	      case VOH_ITER_IS_INVENTORY:
	      case VOH_ITER_IS_WATCHDOG:
	      case VOH_ITER_IS_ANNUNCIATOR:
		  if (!gtk_tree_model_iter_parent(store, &parent, &iter))
			break;
		  gtk_tree_model_get(store, &parent, VOH_LIST_COLUMN_ID,
				     &pid, -1);
		  details = voh_rdr_info(pid, id, err);

		  if (type == VOH_ITER_IS_SENSOR)
			gtk_widget_set_sensitive(GTK_WIDGET(w->rsitem), TRUE);

		  break;
	    }
	    gtk_tree_view_set_model(GTK_TREE_VIEW(dview), details);
	    if (details)
		  g_object_unref(details);
      }
}

gint hview_readsensor_thread(gpointer data)
{
      HviewWidgetsT	*w = (HviewWidgetsT *) data;
      GtkTreeModel	*store;
      GtkTreeSelection	*selection;
      GtkTreeIter	iter;
      guint		id;
      gint		ret;
      GtkWidget		*tview;
      gint		page;
      gchar		err[100];

      page = gtk_notebook_get_current_page(GTK_NOTEBOOK(w->tab_windows));
      if (page < 0)
	    return FALSE;
      if (w->tab_views[page].tree_view == NULL ||
	  w->tab_views[page].detail_view == NULL)
	    return FALSE;
      tview = w->tab_views[page].tree_view;

      selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tview));

      gtk_tree_selection_get_selected(selection, &store, &iter);
      gtk_tree_model_get(store, &iter, VOH_LIST_COLUMN_ID, &id, -1);

      ret = voh_read_sensor(GTK_TREE_STORE(store), id, err);
      if (ret == -1) {
	    hview_print(w, err);
	    hview_statusbar_push(w, "sensor reading failed");
      } else {

	    hview_statusbar_push(w, "ready");
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
      GtkTreeSelection	*selection;
      GtkWidget		*tview;
      gint		page;
      int		state;

      page = gtk_notebook_get_current_page(GTK_NOTEBOOK(w->tab_windows));
      if (page < 0)
	    return;
      if (w->tab_views[page].tree_view == NULL ||
	  w->tab_views[page].detail_view == NULL)
	    return;
      tview = w->tab_views[page].tree_view;

      selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tview));

      if (gtk_tree_selection_get_selected(selection, &store, &iter)) {
	    gtk_tree_model_get(store, &iter, VOH_LIST_COLUMN_TYPE, &type, -1);
	    if (type == VOH_ITER_IS_SENSOR) {
		  hview_statusbar_push(w, "sensor reading (please wait)");
		  gtk_widget_set_sensitive(w->main_window, FALSE);
		  gtk_timeout_add(100, hview_readsensor_thread, data);
	    }
      } else {
	    hview_print(w, "select sensor please");
      }

}

void hview_show_hide_domain_call(GtkWidget *widget, gpointer data)
{
      HviewWidgetsT	*w = (HviewWidgetsT *) data;
      GtkTooltips	*tooltips;
      GtkWidget		*image;

      tooltips = gtk_tooltips_new();

      image = gtk_tool_button_get_icon_widget(GTK_TOOL_BUTTON(widget));

      if (strcmp(gtk_tool_button_get_label(GTK_TOOL_BUTTON(widget)),
		 "hide domain list") == 0) {
	    gtk_tool_button_set_label(GTK_TOOL_BUTTON(widget),
				      "show domain list");
	    gtk_widget_hide(w->domain_window);

	    gtk_tool_item_set_tooltip(GTK_TOOL_ITEM(widget), tooltips,
				      "Show domain list", NULL);
	    gtk_image_set_from_file(GTK_IMAGE(image),
				    find_pixmap_file("show.png"));
      } else {
	    gtk_tool_button_set_label(GTK_TOOL_BUTTON(widget),
				      "hide domain list");
	    gtk_widget_show(w->domain_window);

	    gtk_tool_item_set_tooltip(GTK_TOOL_ITEM(widget), tooltips,
				      "Hide domain list", NULL);
	    gtk_image_set_from_file(GTK_IMAGE(image),
				    find_pixmap_file("hide.png"));
      }
}

void hview_switch_page_call(GtkNotebook *notebook,
			    GtkNotebookPage *page,
			    guint pagenum,
			    gpointer data)
{
      HviewWidgetsT	*w = (HviewWidgetsT *) data;

      gtk_widget_set_state(GTK_WIDGET(w->rsitem), GTK_STATE_INSENSITIVE);
}

void hview_open_session_call(GtkWidget *widget, gpointer data)
{
      HviewWidgetsT		*w = (HviewWidgetsT *) data;
      GtkWidget			*hpaned;
      GtkWidget			*label;
      GtkWidget			*detail_window;
      GtkWidget			*tree_window;
      GtkTreeModel		*store = NULL;
      GtkTreeSelection		*selection;
      GtkTreeIter		iter;
      gchar			*name;
      gint			page;
      guint			did;

      selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(w->domain_view));

      if (gtk_tree_selection_get_selected(selection, &store, &iter)) {
	    hpaned = gtk_hpaned_new();

	    if (store) {
		  gtk_tree_model_get(store, &iter,
				     VOH_LIST_COLUMN_NAME, &name,
				     VOH_LIST_COLUMN_ID, &did, -1);
	    } else {
		  return;
	    }

	    label = gtk_label_new(name);
	    g_free(name);

	    page = gtk_notebook_append_page(GTK_NOTEBOOK(w->tab_windows),
					    hpaned, label);

	    if (page < 0 || page >= HVIEW_MAX_TAB_WINDOWS)
		  return;

	    tree_window = hview_get_tree_window(w, page);
	    gtk_paned_add1(GTK_PANED(hpaned), tree_window);
	    
	    detail_window = hview_get_detail_window(w, page);
	    gtk_paned_add2(GTK_PANED(hpaned), detail_window);
	    
	    gtk_widget_show_all(hpaned);
      } else {
	    hview_print(w, "select domain please");
      }
}

