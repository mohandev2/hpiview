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
#include "hview_utils.h"
#include "hview_service.h"
#include "hview_widgets.h"
#include "hview_windows.h"
#include "hview_callbacks.h"

void hview_quit_call(GtkWidget *widget, gpointer data)
{
//      voh_fini(NULL);
      gtk_main_quit();
}

void hview_empty_log_call(GtkWidget *widget, gpointer data)
{
      HviewWidgetsT	*w = (HviewWidgetsT *) data;
      GtkTextIter	start_iter,	end_iter;
      GtkTextBuffer	*buf;
 
      buf = GTK_TEXT_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW
						     	(w->message_view)));
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
      guint		sid;
      guint		num_pages,	i;

      page = gtk_notebook_get_current_page(GTK_NOTEBOOK(w->tab_windows));
      if (page < 0)
	    return;
      sid = w->tab_views[page].sessionid;
      voh_session_close(sid, NULL);
      gtk_notebook_remove_page(GTK_NOTEBOOK(w->tab_windows), page);
      num_pages = gtk_notebook_get_n_pages(GTK_NOTEBOOK(w->tab_windows));

      for (i = page; i <= num_pages; i++) {
	      w->tab_views[i].resource_view = w->tab_views[i + 1].resource_view;
	      w->tab_views[i].detail_view = w->tab_views[i + 1].detail_view;
	      w->tab_views[i].event_subscribed =
		      w->tab_views[i + 1].event_subscribed;
	      w->tab_views[i].sessionid = w->tab_views[i + 1].sessionid;
      }

      gtk_widget_queue_draw(w->tab_windows);
}

gint hview_discover_thread(gpointer data)
{
      HviewWidgetsT	*w = (HviewWidgetsT *) data;
      GtkTreeModel	*list;
      GtkWidget		*tview;
      gint		page;
      guint		sid;
      gboolean		res;
      gchar		err[100];

      page = gtk_notebook_get_current_page(GTK_NOTEBOOK(w->tab_windows));
      if (page < 0)
	    return FALSE;
      if (w->tab_views[page].resource_view == NULL ||
	  w->tab_views[page].detail_view == NULL)
	    return FALSE;
      tview = w->tab_views[page].resource_view;
      sid = w->tab_views[page].sessionid;

      res = voh_discover(sid, err);
      if (res == FALSE) {
	    hview_print(w, err);
	    hview_statusbar_push(w, "discovering failed");
	    gtk_widget_set_sensitive(w->main_window, TRUE);
	    return FALSE;
      }

      list = voh_list_resources(sid, err);
      if (list == NULL) {
	    hview_print(w, err);
      }
      else {
	    gtk_tree_view_set_model(GTK_TREE_VIEW(tview), list);
	    g_object_unref(list);
      }
      hview_statusbar_push(w, "ready");
      
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
      if (w->tab_views[page].resource_view == NULL ||
	  w->tab_views[page].detail_view == NULL)
	    return;
      tview = w->tab_views[page].resource_view;
      dview = w->tab_views[page].detail_view;

      gtk_tree_view_set_model(GTK_TREE_VIEW(tview), NULL);
      gtk_tree_view_set_model(GTK_TREE_VIEW(dview), NULL);

      hview_statusbar_push(w, "discovering (please wait)");
      gtk_widget_set_sensitive(w->main_window, FALSE);
      gtk_timeout_add(100, hview_discover_thread, data);
      
}

void hview_about_call(GtkWidget *widget, gpointer data)
{
	HviewWidgetsT	*w = (HviewWidgetsT *) data;
	GtkWidget	*dialog_window;
	guint		res;

	dialog_window = hview_get_about_window(w);

	gtk_widget_show_all(dialog_window);

	res = gtk_dialog_run(GTK_DIALOG(dialog_window));
	switch (res) {
	case GTK_RESPONSE_CLOSE:
		gtk_widget_destroy(dialog_window);
		break;
	default:
		break;
	}
}


void hview_domain_row_activated_call(GtkWidget *widget,
				     GtkTreeIter *iter,
				     GtkTreePath *path,
				     gpointer data)
{
      HviewWidgetsT		*w = (HviewWidgetsT *) data;


}

gint hview_entity_get_info_thread(gpointer data)
{
      HviewWidgetsT	*w = (HviewWidgetsT *) data;
      guint		type,	id,	pid;
      GtkTreeModel	*store;
      GtkTreeIter	iter,	parent;
      GtkTreeModel	*details = NULL;
      GtkWidget		*dview,	*tview;
      GtkTreeSelection *selection;
      gint		page;
      guint		sid;
      gchar		err[100];

      err[0] = 0;

      page = gtk_notebook_get_current_page(GTK_NOTEBOOK(w->tab_windows));
      if (page < 0)
	    return;
      if ((tview = w->tab_views[page].resource_view) == NULL ||
	  (dview = w->tab_views[page].detail_view) == NULL)
	    return;
      sid = w->tab_views[page].sessionid;

      selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tview));

      if (gtk_tree_selection_get_selected(selection, &store, &iter)) {
	    gtk_tree_model_get(store, &iter, VOH_LIST_COLUMN_TYPE, &type,
			       VOH_LIST_COLUMN_ID, &id, -1);

	    switch (type) {
	      case VOH_ITER_IS_DOMAIN:
//		  details = voh_domain_info(did, err);
		  break;
	      case VOH_ITER_IS_RPT:
		  details = voh_get_resource_details(sid, id, err);
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
		  details = voh_rdr_info(sid, pid, id, err);
		  if (details == NULL || strlen(err) > 0)
			hview_print(w, err);

		  break;
	    }
	    gtk_tree_view_set_model(GTK_TREE_VIEW(dview), details);
	    if (details)
		  g_object_unref(details);
      }

//      gtk_widget_set_sensitive(w->main_window, TRUE);
      hview_statusbar_push(w, "ready");

      return FALSE;
}

void hview_tree_row_selected_call(GtkTreeSelection *selection,
				  gpointer data)
{
      HviewWidgetsT	*w = (HviewWidgetsT *) data;

      hview_statusbar_push(w, "entry info getting (please wait)");
      gtk_timeout_add(100, hview_entity_get_info_thread, data);
      
}

gboolean hview_butpress_rlist_call(GtkWidget *widget,
				   GdkEventButton *event,
				   gpointer data)
{
      GtkWidget		*menu;
      GtkTreePath	*path;
      GtkTreeViewColumn	*column;
      GtkTreeModel	*store;
      GtkTreeSelection	*selection;
      GtkTreeIter	iter;
      gint		x,	y;
      gchar		*name;

      if (event->type == GDK_BUTTON_PRESS && event->button == 3) {
	    gtk_widget_get_pointer(widget, &x, &y);
	    gtk_tree_view_widget_to_tree_coords(GTK_TREE_VIEW(widget),
						x, y,
						&x, &y);
	    if (gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(widget),
					      x, y,
					      &path, NULL, NULL, NULL)) {
		  store = gtk_tree_view_get_model(GTK_TREE_VIEW(widget));
		  if (store == NULL)
			return TRUE;
		  gtk_tree_model_get_iter(store, &iter, path);
		  gtk_tree_path_free(path);
		  menu = hwidget_get_resource_popup(store, &iter, data);
		  if (menu) {
			gtk_widget_show_all(menu);
			gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL,
				       event->button,
				       gdk_event_get_time((GdkEvent*)event));
		  } else {
			return TRUE;
		  }
	    } else {
		  return TRUE;
	    }
	    return FALSE;
      }

      return FALSE;
}

gint hview_readsensor_thread(gpointer data)
{
      HviewWidgetsT	*w = (HviewWidgetsT *) data;
      GtkTreeModel	*store;
      GtkTreeSelection	*selection;
      GtkTreeIter	iter,	parent;
      guint		id,	pid;
      gint		ret;
      GtkWidget		*tview;
      gint		page;
      guint		sid;
      gchar		err[100];

      page = gtk_notebook_get_current_page(GTK_NOTEBOOK(w->tab_windows));
      if (page < 0)
	    return FALSE;
      if (w->tab_views[page].resource_view == NULL ||
	  w->tab_views[page].detail_view == NULL)
	    return FALSE;
      tview = w->tab_views[page].resource_view;
      sid = w->tab_views[page].sessionid;

      selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tview));

      gtk_tree_selection_get_selected(selection, &store, &iter);
      if (gtk_tree_model_iter_parent(store, &parent, &iter) == FALSE) {
	    hview_statusbar_push(w, "sensor reading failed");
	    gtk_widget_set_sensitive(w->main_window, TRUE);
	    return FALSE;
      }
      gtk_tree_model_get(store, &iter, VOH_LIST_COLUMN_ID, &id, -1);
      gtk_tree_model_get(store, &parent, VOH_LIST_COLUMN_ID, &pid, -1);

      ret = voh_read_sensor(GTK_TREE_STORE(store), sid, pid, id, err);
      if (ret == FALSE) {
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
      if (w->tab_views[page].resource_view == NULL ||
	  w->tab_views[page].detail_view == NULL)
	    return;
      tview = w->tab_views[page].resource_view;

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
				    hutil_find_pixmap_file("show.png"));
      } else {
	    gtk_tool_button_set_label(GTK_TOOL_BUTTON(widget),
				      "hide domain list");
	    gtk_widget_show(w->domain_window);

	    gtk_tool_item_set_tooltip(GTK_TOOL_ITEM(widget), tooltips,
				      "Hide domain list", NULL);
	    gtk_image_set_from_file(GTK_IMAGE(image),
				    hutil_find_pixmap_file("hide.png"));
      }
}

void hview_switch_page_call(GtkNotebook *notebook,
			    GtkNotebookPage *page,
			    guint pagenum,
			    gpointer data)
{
      HviewWidgetsT		*w = (HviewWidgetsT *) data;
      GtkTooltips		*tooltips;
      GtkWidget			*image;

      if (pagenum < 0)
	    return;
      image = gtk_tool_button_get_icon_widget(GTK_TOOL_BUTTON(w->subev_item));
      tooltips = gtk_tooltips_new();
      if (w->tab_views[pagenum].event_subscribed == FALSE) {
	    gtk_tool_button_set_label(GTK_TOOL_BUTTON(w->subev_item),
				      "Subscribe events");
	    gtk_tool_item_set_tooltip(GTK_TOOL_ITEM(w->subev_item), tooltips,
				      "Subscribe events", NULL);
	    gtk_image_set_from_file(GTK_IMAGE(image),
				    hutil_find_pixmap_file("sub_events.png"));
      } else {
	    gtk_tool_button_set_label(GTK_TOOL_BUTTON(w->subev_item),
				      "Unsubscribe events");
	    gtk_tool_item_set_tooltip(GTK_TOOL_ITEM(w->subev_item), tooltips,
				      "Unsubscribe events", NULL);
	    gtk_image_set_from_file(GTK_IMAGE(image),
				    hutil_find_pixmap_file("unsub_events.png"));
      }
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
      guint			did,		sid;
      gchar			err[1024];

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

	    sid = voh_open_session(did, err);
	    w->tab_views[page].sessionid = sid;
	    if (sid == 0) {
		  hview_print(w, err);
		  return;
	    }

	    tree_window = hview_get_resource_window(w, page);
	    gtk_paned_add1(GTK_PANED(hpaned), tree_window);
	    
	    detail_window = hview_get_detail_window(w, page);
	    gtk_paned_add2(GTK_PANED(hpaned), detail_window);
	    
	    gtk_widget_show_all(hpaned);
	    w->tab_views[page].event_subscribed = FALSE;
	    gtk_notebook_set_current_page(GTK_NOTEBOOK(w->tab_windows), page);
      } else {
	    hview_print(w, "select domain please");
      }
}
/*
gboolean hview_check_events_thread(gpointer data)
{
      HviewEventThrDataT	*ev = (HviewEventThrDataT *) data;
      GtkWidget			*event_view = ev->event_view;
      guint			sid = ev->sessionid;
      GtkTreeModel		*events = NULL;
      gboolean			res;

      printf("TTTT %ld\n", event_view);
      events = gtk_tree_view_get_model(GTK_TREE_VIEW(event_view));
      if (events == NULL)
	    return FALSE;

      res = voh_get_events(GTK_TREE_STORE(events), sid, NULL);
      if (res == TRUE)
	    gtk_tree_view_set_model(GTK_TREE_VIEW(event_view), events);

      return TRUE;
}
*/

void hview_subscribe_events_call(GtkWidget *widget, gpointer data)
{
      HviewWidgetsT		*w = (HviewWidgetsT *) data;
      GtkTooltips		*tooltips;
      GtkWidget			*image;
      gint			page;
      guint			sid;
      gboolean			res;
      gchar			err[100];

      page = gtk_notebook_get_current_page(GTK_NOTEBOOK(w->tab_windows));
      if (page < 0)
	    return;
      if (w->tab_views[page].resource_view == NULL ||
	  w->tab_views[page].detail_view == NULL)
	    return;
      sid = w->tab_views[page].sessionid;

      image = gtk_tool_button_get_icon_widget(GTK_TOOL_BUTTON(widget));
      tooltips = gtk_tooltips_new();
      if (w->tab_views[page].event_subscribed == FALSE) {
	    res = voh_subscribe_events(sid, err);
	    if (res == FALSE) {
		  hview_print(w, err);
	    } else {
		  gtk_tool_button_set_label(GTK_TOOL_BUTTON(widget),
					    "Unsubscribe events");
		  gtk_tool_item_set_tooltip(GTK_TOOL_ITEM(widget), tooltips,
					    "Unsubscribe events", NULL);
		  gtk_image_set_from_file(GTK_IMAGE(image),
					  hutil_find_pixmap_file(
						  "unsub_events.png"));
		  w->tab_views[page].event_subscribed = TRUE;
	    }
      } else {
	    res = voh_unsubscribe_events(sid, err);
	    if (res == FALSE) {
		  hview_print(w, err);
	    } else {
		  gtk_tool_button_set_label(GTK_TOOL_BUTTON(widget),
					    "Subscribe events");
		  gtk_tool_item_set_tooltip(GTK_TOOL_ITEM(widget), tooltips,
					    "Subscribe events", NULL);
		  gtk_image_set_from_file(GTK_IMAGE(image),
					  hutil_find_pixmap_file(
						  "sub_events.png"));
		  w->tab_views[page].event_subscribed = FALSE;
	    }
      }
}

void hview_get_events_call(GtkWidget *widget, gpointer data)
{
      HviewWidgetsT		*w = (HviewWidgetsT *) data;
      GtkWidget			*tview;
      GtkTreeModel		*events = NULL;
      gint			page;
      guint			sid;
      gboolean			res;
      gchar			err[100];

      page = gtk_notebook_get_current_page(GTK_NOTEBOOK(w->tab_windows));
      if (page < 0)
	    return;
      if (w->tab_views[page].resource_view == NULL ||
	  w->tab_views[page].detail_view == NULL)
	    return;
      tview = w->tab_views[page].resource_view;
      sid = w->tab_views[page].sessionid;

      events = gtk_tree_view_get_model(GTK_TREE_VIEW(w->event_view));

      res = voh_get_events(GTK_TREE_STORE(events), sid, err);
      if (res == FALSE) {
	    hview_print(w, err);
      }
      gtk_tree_view_set_model(GTK_TREE_VIEW(w->event_view), events);
//      if (events)
//	    g_object_unref(events);
}

gint hview_set_power_off_thread(gpointer data)
{
	HviewWidgetsT		*w = (HviewWidgetsT *) data;
	GtkTreeModel		*store;
	GtkTreeSelection	*selection;
	GtkTreeIter		iter;
	guint			id;
	gint			res;
	GtkWidget		*tview;
	gint			page;
	guint			sid;
	gchar			err[100];

	page = gtk_notebook_get_current_page(GTK_NOTEBOOK(w->tab_windows));
	if (page < 0)
		return FALSE;
	if (w->tab_views[page].resource_view == NULL ||
			w->tab_views[page].detail_view == NULL)
		return FALSE;
	tview = w->tab_views[page].resource_view;
	sid = w->tab_views[page].sessionid;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tview));

	gtk_tree_selection_get_selected(selection, &store, &iter);
	gtk_tree_model_get(store, &iter, VOH_LIST_COLUMN_ID, &id, -1);

	res = voh_set_power_off(sid, id, GTK_TREE_STORE(store), err);
	if (res == FALSE) {
		hview_print(w, err);
		hview_statusbar_push(w, "power off failed");
	} else {
		hview_statusbar_push(w, "ready");
	}

	gtk_widget_set_sensitive(w->main_window, TRUE);

	return FALSE;
}

void hview_set_power_off_call(GtkWidget *widget, gpointer data)
{
	HviewWidgetsT		*w = (HviewWidgetsT *) data;
	GtkWidget		*tview;
	GtkTreeModel		*model;
	GtkTreeIter		iter;
	guint			type;
	GtkTreeSelection	*selection;
	gint			page;

	page = gtk_notebook_get_current_page(GTK_NOTEBOOK(w->tab_windows));
	if (page < 0)
		return;
	if (w->tab_views[page].resource_view == NULL ||
			w->tab_views[page].detail_view == NULL)
		return;
	tview = w->tab_views[page].resource_view;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tview));

	if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
		gtk_tree_model_get(model, &iter,
				   VOH_LIST_COLUMN_TYPE, &type, -1);
		if (type == VOH_ITER_IS_RPT) {
			hview_statusbar_push(w, "power off (please wait)");
			gtk_widget_set_sensitive(w->main_window, FALSE);
			gtk_timeout_add(100, hview_set_power_off_thread, data);
		}
      }
}

gint hview_set_power_on_thread(gpointer data)
{
	HviewWidgetsT		*w = (HviewWidgetsT *) data;
	GtkTreeModel		*store;
	GtkTreeSelection	*selection;
	GtkTreeIter		iter;
	guint			id;
	gint			res;
	GtkWidget		*tview;
	gint			page;
	guint			sid;
	gchar			err[100];

	page = gtk_notebook_get_current_page(GTK_NOTEBOOK(w->tab_windows));
	if (page < 0)
		return FALSE;
	if (w->tab_views[page].resource_view == NULL ||
			w->tab_views[page].detail_view == NULL)
		return FALSE;
	tview = w->tab_views[page].resource_view;
	sid = w->tab_views[page].sessionid;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tview));

	gtk_tree_selection_get_selected(selection, &store, &iter);
	gtk_tree_model_get(store, &iter, VOH_LIST_COLUMN_ID, &id, -1);

	res = voh_set_power_on(sid, id, GTK_TREE_STORE(store), err);
	if (res == FALSE) {
		hview_print(w, err);
		hview_statusbar_push(w, "power on failed");
	} else {
		hview_statusbar_push(w, "ready");
	}

	gtk_widget_set_sensitive(w->main_window, TRUE);

	return FALSE;
}

void hview_set_power_on_call(GtkWidget *widget, gpointer data)
{
	HviewWidgetsT		*w = (HviewWidgetsT *) data;
	GtkWidget		*tview;
	GtkTreeModel		*model;
	GtkTreeIter		iter;
	guint			type;
	GtkTreeSelection	*selection;
	gint			page;

	page = gtk_notebook_get_current_page(GTK_NOTEBOOK(w->tab_windows));
	if (page < 0)
		return;
	if (w->tab_views[page].resource_view == NULL ||
			w->tab_views[page].detail_view == NULL)
		return;
	tview = w->tab_views[page].resource_view;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tview));

	if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
		gtk_tree_model_get(model, &iter,
				   VOH_LIST_COLUMN_TYPE, &type, -1);
		if (type == VOH_ITER_IS_RPT) {
			hview_statusbar_push(w, "power on (please wait)");
			gtk_widget_set_sensitive(w->main_window, FALSE);
			gtk_timeout_add(100, hview_set_power_on_thread, data);
		}
      }
}

gint hview_set_power_cycle_thread(gpointer data)
{
	HviewWidgetsT		*w = (HviewWidgetsT *) data;
	GtkTreeModel		*store;
	GtkTreeSelection	*selection;
	GtkTreeIter		iter;
	guint			id;
	gint			res;
	GtkWidget		*tview;
	gint			page;
	guint			sid;
	gchar			err[100];

	page = gtk_notebook_get_current_page(GTK_NOTEBOOK(w->tab_windows));
	if (page < 0)
		return FALSE;
	if (w->tab_views[page].resource_view == NULL ||
			w->tab_views[page].detail_view == NULL)
		return FALSE;
	tview = w->tab_views[page].resource_view;
	sid = w->tab_views[page].sessionid;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tview));

	gtk_tree_selection_get_selected(selection, &store, &iter);
	gtk_tree_model_get(store, &iter, VOH_LIST_COLUMN_ID, &id, -1);

	res = voh_set_power_cycle(sid, id, GTK_TREE_STORE(store), err);
	if (res == FALSE) {
		hview_print(w, err);
		hview_statusbar_push(w, "power off/on failed");
	} else {
		hview_statusbar_push(w, "ready");
	}

	gtk_widget_set_sensitive(w->main_window, TRUE);

	return FALSE;
}

void hview_set_power_cycle_call(GtkWidget *widget, gpointer data)
{
	HviewWidgetsT		*w = (HviewWidgetsT *) data;
	GtkWidget		*tview;
	GtkTreeModel		*model;
	GtkTreeIter		iter;
	GtkTreeSelection	*selection;
	guint			type;
	gint			page;

	page = gtk_notebook_get_current_page(GTK_NOTEBOOK(w->tab_windows));
	if (page < 0)
		return;
	if (w->tab_views[page].resource_view == NULL ||
			w->tab_views[page].detail_view == NULL)
		return;
	tview = w->tab_views[page].resource_view;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tview));

	if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
		gtk_tree_model_get(model, &iter,
				   VOH_LIST_COLUMN_TYPE, &type, -1);
		if (type == VOH_ITER_IS_RPT) {
			hview_statusbar_push(w, "power off/on (please wait)");
			gtk_widget_set_sensitive(w->main_window, FALSE);
			gtk_timeout_add(100, hview_set_power_cycle_thread,
					data);
		}
      }
}

gint hview_reset_cold_thread(gpointer data)
{
	HviewWidgetsT		*w = (HviewWidgetsT *) data;
	GtkTreeModel		*store;
	GtkTreeSelection	*selection;
	GtkTreeIter		iter;
	guint			id;
	gint			res;
	GtkWidget		*tview;
	gint			page;
	guint			sid;
	gchar			err[100];

	page = gtk_notebook_get_current_page(GTK_NOTEBOOK(w->tab_windows));
	if (page < 0)
		return FALSE;
	if (w->tab_views[page].resource_view == NULL ||
			w->tab_views[page].detail_view == NULL)
		return FALSE;
	tview = w->tab_views[page].resource_view;
	sid = w->tab_views[page].sessionid;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tview));

	gtk_tree_selection_get_selected(selection, &store, &iter);
	gtk_tree_model_get(store, &iter, VOH_LIST_COLUMN_ID, &id, -1);

	res = voh_set_reset_cold(sid, id, GTK_TREE_STORE(store), err);
	if (res == FALSE) {
		hview_print(w, err);
		hview_statusbar_push(w, "reset cold failed");
	} else {
		hview_statusbar_push(w, "ready");
	}

	gtk_widget_set_sensitive(w->main_window, TRUE);

	return FALSE;
}

void hview_reset_cold_call(GtkWidget *widget, gpointer data)
{
	HviewWidgetsT		*w = (HviewWidgetsT *) data;
	GtkWidget		*tview;
	GtkTreeModel		*model;
	GtkTreeIter		iter;
	GtkTreeSelection	*selection;
	guint			type;
	gint			page;

	page = gtk_notebook_get_current_page(GTK_NOTEBOOK(w->tab_windows));
	if (page < 0)
		return;
	if (w->tab_views[page].resource_view == NULL ||
			w->tab_views[page].detail_view == NULL)
		return;
	tview = w->tab_views[page].resource_view;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tview));

	if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
		gtk_tree_model_get(model, &iter,
				   VOH_LIST_COLUMN_TYPE, &type, -1);
		if (type == VOH_ITER_IS_RPT) {
			hview_statusbar_push(w, "reset cold (please wait)");
			gtk_widget_set_sensitive(w->main_window, FALSE);
			gtk_timeout_add(100, hview_reset_cold_thread, data);
		}
      }
}

gint hview_reset_warm_thread(gpointer data)
{
	HviewWidgetsT		*w = (HviewWidgetsT *) data;
	GtkTreeModel		*store;
	GtkTreeSelection	*selection;
	GtkTreeIter		iter;
	guint			id;
	gint			res;
	GtkWidget		*tview;
	gint			page;
	guint			sid;
	gchar			err[100];

	page = gtk_notebook_get_current_page(GTK_NOTEBOOK(w->tab_windows));
	if (page < 0)
		return FALSE;
	if (w->tab_views[page].resource_view == NULL ||
			w->tab_views[page].detail_view == NULL)
		return FALSE;
	tview = w->tab_views[page].resource_view;
	sid = w->tab_views[page].sessionid;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tview));

	gtk_tree_selection_get_selected(selection, &store, &iter);
	gtk_tree_model_get(store, &iter, VOH_LIST_COLUMN_ID, &id, -1);

	res = voh_set_reset_warm(sid, id, GTK_TREE_STORE(store), err);
	if (res == FALSE) {
		hview_print(w, err);
		hview_statusbar_push(w, "reset warm failed");
	} else {
		hview_statusbar_push(w, "ready");
	}

	gtk_widget_set_sensitive(w->main_window, TRUE);

	return FALSE;
}

void hview_reset_warm_call(GtkWidget *widget, gpointer data)
{
	HviewWidgetsT		*w = (HviewWidgetsT *) data;
	GtkWidget		*tview;
	GtkTreeModel		*model;
	GtkTreeIter		iter;
	GtkTreeSelection	*selection;
	guint			type;
	gint			page;

	page = gtk_notebook_get_current_page(GTK_NOTEBOOK(w->tab_windows));
	if (page < 0)
		return;
	if (w->tab_views[page].resource_view == NULL ||
			w->tab_views[page].detail_view == NULL)
		return;
	tview = w->tab_views[page].resource_view;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tview));

	if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
		gtk_tree_model_get(model, &iter,
				   VOH_LIST_COLUMN_TYPE, &type, -1);
		if (type == VOH_ITER_IS_RPT) {
			hview_statusbar_push(w, "reset warm (please wait)");
			gtk_widget_set_sensitive(w->main_window, FALSE);
			gtk_timeout_add(100, hview_reset_warm_thread, data);
		}
      }
}

gint hview_reset_assert_thread(gpointer data)
{
	HviewWidgetsT		*w = (HviewWidgetsT *) data;
	GtkTreeModel		*store;
	GtkTreeSelection	*selection;
	GtkTreeIter		iter;
	guint			id;
	gint			res;
	GtkWidget		*tview;
	gint			page;
	guint			sid;
	gchar			err[100];

	page = gtk_notebook_get_current_page(GTK_NOTEBOOK(w->tab_windows));
	if (page < 0)
		return FALSE;
	if (w->tab_views[page].resource_view == NULL ||
			w->tab_views[page].detail_view == NULL)
		return FALSE;
	tview = w->tab_views[page].resource_view;
	sid = w->tab_views[page].sessionid;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tview));

	gtk_tree_selection_get_selected(selection, &store, &iter);
	gtk_tree_model_get(store, &iter, VOH_LIST_COLUMN_ID, &id, -1);

	res = voh_set_reset_assert(sid, id, GTK_TREE_STORE(store), err);
	if (res == FALSE) {
		hview_print(w, err);
		hview_statusbar_push(w, "reset assert failed");
	} else {
		hview_statusbar_push(w, "ready");
	}

	gtk_widget_set_sensitive(w->main_window, TRUE);

	return FALSE;
}

void hview_reset_assert_call(GtkWidget *widget, gpointer data)
{
	HviewWidgetsT		*w = (HviewWidgetsT *) data;
	GtkWidget		*tview;
	GtkTreeModel		*model;
	GtkTreeIter		iter;
	GtkTreeSelection	*selection;
	guint			type;
	gint			page;

	page = gtk_notebook_get_current_page(GTK_NOTEBOOK(w->tab_windows));
	if (page < 0)
		return;
	if (w->tab_views[page].resource_view == NULL ||
			w->tab_views[page].detail_view == NULL)
		return;
	tview = w->tab_views[page].resource_view;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tview));

	if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
		gtk_tree_model_get(model, &iter,
				   VOH_LIST_COLUMN_TYPE, &type, -1);
		if (type == VOH_ITER_IS_RPT) {
			hview_statusbar_push(w, "reset assert (please wait)");
			gtk_widget_set_sensitive(w->main_window, FALSE);
			gtk_timeout_add(100, hview_reset_assert_thread, data);
		}
      }
}

gint hview_reset_deassert_thread(gpointer data)
{
	HviewWidgetsT		*w = (HviewWidgetsT *) data;
	GtkTreeModel		*store;
	GtkTreeSelection	*selection;
	GtkTreeIter		iter;
	guint			id;
	gint			res;
	GtkWidget		*tview;
	gint			page;
	guint			sid;
	gchar			err[100];

	page = gtk_notebook_get_current_page(GTK_NOTEBOOK(w->tab_windows));
	if (page < 0)
		return FALSE;
	if (w->tab_views[page].resource_view == NULL ||
			w->tab_views[page].detail_view == NULL)
		return FALSE;
	tview = w->tab_views[page].resource_view;
	sid = w->tab_views[page].sessionid;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tview));

	gtk_tree_selection_get_selected(selection, &store, &iter);
	gtk_tree_model_get(store, &iter, VOH_LIST_COLUMN_ID, &id, -1);

	res = voh_set_reset_deassert(sid, id, GTK_TREE_STORE(store), err);
	if (res == FALSE) {
		hview_print(w, err);
		hview_statusbar_push(w, "reset deassert failed");
	} else {
		hview_statusbar_push(w, "ready");
	}

	gtk_widget_set_sensitive(w->main_window, TRUE);

	return FALSE;
}

void hview_reset_deassert_call(GtkWidget *widget, gpointer data)
{
	HviewWidgetsT		*w = (HviewWidgetsT *) data;
	GtkWidget		*tview;
	GtkTreeModel		*model;
	GtkTreeIter		iter;
	GtkTreeSelection	*selection;
	guint			type;
	gint			page;

	page = gtk_notebook_get_current_page(GTK_NOTEBOOK(w->tab_windows));
	if (page < 0)
		return;
	if (w->tab_views[page].resource_view == NULL ||
			w->tab_views[page].detail_view == NULL)
		return;
	tview = w->tab_views[page].resource_view;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tview));

	if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
		gtk_tree_model_get(model, &iter,
				   VOH_LIST_COLUMN_TYPE, &type, -1);
		if (type == VOH_ITER_IS_RPT) {
			hview_statusbar_push(w, "reset deassert (please wait)");
			gtk_widget_set_sensitive(w->main_window, FALSE);
			gtk_timeout_add(100, hview_reset_deassert_thread, data);
		}
	}
}

void hview_toggled_call(GtkToggleToolButton *but, gpointer data)
{
      HviewWidgetsT		*w = (HviewWidgetsT *) data;
      GtkWidget			*sh_obj = NULL;

      if (strcmp(gtk_tool_button_get_label(GTK_TOOL_BUTTON(but)),
		 "Messages") == 0) {
	    sh_obj = w->message_window;
      } else if (strcmp(gtk_tool_button_get_label(GTK_TOOL_BUTTON(but)),
			"Events") == 0) {
	    sh_obj = w->event_window;
      }

      if (gtk_toggle_tool_button_get_active(but) == TRUE) {
	    gtk_container_foreach(GTK_CONTAINER(w->tglbar),
				  hview_disactivate_toggle_call,
				  but);
	    gtk_widget_show(sh_obj);
	    gtk_widget_show(w->log_hbox);
      }
      else {
	    gtk_widget_hide(sh_obj);
	    gtk_widget_hide(w->log_hbox);
      }
      
}

void hview_disactivate_toggle_call(GtkWidget *widget, gpointer data)
{
      if ((gpointer)widget != data) {
	    if (gtk_toggle_tool_button_get_active(
				GTK_TOGGLE_TOOL_BUTTON(widget)) == TRUE) {
		  gtk_toggle_tool_button_set_active(
				GTK_TOGGLE_TOOL_BUTTON(widget), FALSE);
	    }
      }

}

void hview_rpt_settings_ok_response(gpointer data)
{
	HviewRptDialogWidgetsT	*dw = (HviewRptDialogWidgetsT *) data;
	HviewWidgetsT		*w = dw->parent_widgets;
	GtkTreeModel		*model;
	GtkTreeModel		*sevlist;
	GtkTreeIter		iter;
	GtkTreeSelection	*selection;
	gint			page;
	GtkWidget		*tview,	*dview;
	guint			id,	sid,	severity;
	gboolean		res;
	gchar			err[1024];

	page = gtk_notebook_get_current_page(GTK_NOTEBOOK(w->tab_windows));
	if (page < 0)
		return;
	if (w->tab_views[page].resource_view == NULL ||
			w->tab_views[page].detail_view == NULL)
		return;
	tview = w->tab_views[page].resource_view;
	dview = w->tab_views[page].detail_view;
	sid = w->tab_views[page].sessionid;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tview));

	if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
		gtk_tree_model_get(model, &iter, VOH_LIST_COLUMN_ID, &id, -1);
	} else {
		return;
	}

	sevlist = gtk_combo_box_get_model(GTK_COMBO_BOX(dw->severity_box));
	res = gtk_combo_box_get_active_iter(GTK_COMBO_BOX(dw->severity_box),
							  &iter);
	if (res == TRUE) {
		gtk_tree_model_get(sevlist, &iter, 1, &severity, -1);

		res = voh_set_resource_severity(sid, id, severity, err);
		if (res == FALSE) {
			hview_print(w, err);
		}
	}

	res = voh_set_resource_tag(sid, id, gtk_entry_get_text(
						GTK_ENTRY(dw->tag_entry)), err);
	if (res == FALSE) {
		hview_print(w, err);
	}

	gtk_tree_view_set_model(GTK_TREE_VIEW(dview), NULL);
	model = voh_get_resource_details(sid, id, err);
	if (model == NULL) {
		hview_print(w, err);
	} else {
		gtk_tree_view_set_model(GTK_TREE_VIEW(dview), model);
		g_object_unref(model);
	}

}

void hview_rpt_settings_call(GtkWidget *widget, gpointer data)
{
	HviewWidgetsT		*w = (HviewWidgetsT *) data;
	HviewRptDialogWidgetsT	dw;
	GtkWidget		*dialog_window;
	GtkTreeModel		*model;
	GtkTreeModel		*sevlist;
	GtkTreeIter		iter;
	GtkTreeSelection	*selection;
	GList			*res_info;
	GtkWidget		*hbox;
	GtkWidget		*label;
	gint			page;
	GtkWidget		*tview;
	guint			id, sid;
	gchar			*severity,	*res_tag;
	gint			res;
	gchar			err[1024];

	page = gtk_notebook_get_current_page(GTK_NOTEBOOK(w->tab_windows));
	if (page < 0)
		return;
	if (w->tab_views[page].resource_view == NULL ||
			w->tab_views[page].detail_view == NULL)
		return;
	tview = w->tab_views[page].resource_view;
	sid = w->tab_views[page].sessionid;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tview));
	res = gtk_tree_selection_get_selected(selection, &model, &iter);
	if (res == FALSE) {
		return;
	}

	gtk_tree_model_get(model, &iter, VOH_LIST_COLUMN_ID, &id, -1);

	res = voh_check_resource_presence(sid, id, err);
	if (res == FALSE) {
		dialog_window = hview_get_rpt_empty_dialog_window(w);
		hview_print(w, err);
	} else {
		dw.parent_widgets = w;
		dialog_window = hview_get_rpt_settings_window(&dw);
		dw.dialog_window = dialog_window;

		res_info = voh_get_resource_info(sid, id, err);

		if (res_info == NULL) {
			hview_print(w, err);
		} else {
			while (res_info != NULL) {
				hbox = gtk_hbox_new(FALSE, 10);
				gtk_box_pack_start(GTK_BOX(dw.info_box),
						   hbox, FALSE, FALSE, 2);
				label = gtk_label_new(res_info->data);
				gtk_box_pack_start(GTK_BOX(hbox),
						   label, FALSE, FALSE, 10);
				res_info = res_info->next;
			}
		}	

		severity = voh_get_resource_severity(sid, id, err);
		if (severity) {
			sevlist = gtk_combo_box_get_model(GTK_COMBO_BOX(
						dw.severity_box));
			res = hutil_find_iter_by_name(sevlist,
						      0, severity,
						      NULL, &iter,
						      HUTIL_FIRST_ITER);
			if (res == TRUE) {
				gtk_combo_box_set_active_iter(GTK_COMBO_BOX(
							dw.severity_box),
							&iter);
			}
			g_free(severity);
		}

		res_tag = voh_get_resource_tag(sid, id, err);
		if (res_tag) {
			gtk_entry_set_text(GTK_ENTRY(dw.tag_entry), res_tag);
			g_free(res_tag);
		}
	}

	gtk_widget_show_all(dialog_window);

	res = gtk_dialog_run(GTK_DIALOG(dialog_window));
	switch (res) {
	case GTK_RESPONSE_OK:
		hview_rpt_settings_ok_response((gpointer) &dw);
		break;
	default:
		break;
	}

	gtk_widget_destroy(dialog_window);
}

