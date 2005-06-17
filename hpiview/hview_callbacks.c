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
#include "voh_convert_type.h"

void hview_quit_call(GtkWidget *widget, gpointer data)
{
//      voh_fini(NULL);
      gtk_main_quit();
}

gboolean hview_toggled_true_false_call(GtkToggleButton *but, gpointer data)
{
	if (gtk_toggle_button_get_active(but) == FALSE) {
		gtk_button_set_label(GTK_BUTTON(but),
					vt_convert_boolean(FALSE));
	} else {
		gtk_button_set_label(GTK_BUTTON(but),
					vt_convert_boolean(TRUE));
	}
	return TRUE;
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
      guint		num_pages,	i,	state;
      gboolean		res;
      GtkTreeModel	*model;
      GtkTreeIter	iter;

      page = gtk_notebook_get_current_page(GTK_NOTEBOOK(w->tab_windows));
      if (page < 0)
	    return;
      sid = w->tab_views[page].sessionid;
      gtk_notebook_remove_page(GTK_NOTEBOOK(w->tab_windows), page);
      num_pages = gtk_notebook_get_n_pages(GTK_NOTEBOOK(w->tab_windows));

      for (i = page; i <= num_pages; i++) {
	      w->tab_views[i].resource_view = w->tab_views[i + 1].resource_view;
	      w->tab_views[i].detail_view = w->tab_views[i + 1].detail_view;
	      w->tab_views[i].event_subscribed =
		      w->tab_views[i + 1].event_subscribed;
	      w->tab_views[i].sessionid = w->tab_views[i + 1].sessionid;
      }
      model = gtk_tree_view_get_model(GTK_TREE_VIEW(w->domain_view));
      res = hutil_find_iter_by_id(model, VOH_LIST_COLUMN_ID, sid, 0, &iter,
		      					HUTIL_FIRST_ITER);

      if (res == TRUE) {
	      gtk_tree_model_get(model, &iter,
			      		VOH_LIST_COLUMN_STATE, &state,
					-1);
	      state &= ~VOH_DOMAIN_STATE_OPENED;
	      gtk_tree_store_set(GTK_TREE_STORE(model), &iter,
			      		VOH_LIST_COLUMN_STATE, state,
					-1);
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
	    return FALSE;
      if ((tview = w->tab_views[page].resource_view) == NULL ||
	  (dview = w->tab_views[page].detail_view) == NULL)
	    return FALSE;
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

gboolean hview_butpress_list_call(GtkWidget *widget,
				   GdkEventButton *event,
				   gpointer data)
{
      GtkWidget		*menu = NULL;
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
		  if (store == NULL) {
			return TRUE;
		  }
		  gtk_tree_model_get_iter(store, &iter, path);
		  gtk_tree_path_free(path);
		  menu = hwidget_get_iter_popup(store, &iter, data);
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
      guint			did,		sid,	state;
      gchar			err[1024];

      selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(w->domain_view));

      if (gtk_tree_selection_get_selected(selection, &store, &iter)) {

	    if (store) {
		  gtk_tree_model_get(store, &iter,
				     VOH_LIST_COLUMN_NAME, &name,
				     VOH_LIST_COLUMN_ID, &did,
				     VOH_LIST_COLUMN_STATE, &state,
				     -1);
		  if (state & VOH_DOMAIN_STATE_OPENED) {
			  return;
		  }
	    } else {
		  return;
	    }

	    label = gtk_label_new(name);
	    g_free(name);

	    hpaned = gtk_hpaned_new();
	    page = gtk_notebook_append_page(GTK_NOTEBOOK(w->tab_windows),
					    hpaned, label);

	    if (page < 0 || page >= HVIEW_MAX_TAB_WINDOWS)
		  return;

	    w->tab_views[page].sessionid = did;

	    tree_window = hview_get_resource_window(w, page);
	    gtk_paned_add1(GTK_PANED(hpaned), tree_window);
	    
	    detail_window = hview_get_detail_window(w, page);
	    gtk_paned_add2(GTK_PANED(hpaned), detail_window);
	    
	    gtk_widget_show_all(hpaned);
	    w->tab_views[page].event_subscribed = FALSE;
	    gtk_notebook_set_current_page(GTK_NOTEBOOK(w->tab_windows), page);
      } else {
	    hview_print(w, "select domain please");
	    return;
      }

      gtk_tree_store_set(GTK_TREE_STORE(store), &iter,
		      VOH_LIST_COLUMN_STATE, VOH_DOMAIN_STATE_OPENED,
		      -1);
      hview_statusbar_push(w, "discovering (please wait)");
      gtk_widget_set_sensitive(w->main_window, FALSE);
      gtk_timeout_add(100, hview_discover_thread, data);
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

gint hview_parm_ctrl_restore_thread(gpointer data)
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

	res = voh_parm_control(sid, id, 2, err);
	if (res == FALSE) {
		hview_print(w, err);
		hview_statusbar_push(w,
				"restoring resource parameters failed");
	} else {
		hview_statusbar_push(w, "ready");
	}

	gtk_widget_set_sensitive(w->main_window, TRUE);

	return FALSE;
}



void hview_parm_ctrl_restore_call(GtkWidget *widget, gpointer data)
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
			hview_statusbar_push(w,
			"saving resource parameters (please wait)");
			gtk_widget_set_sensitive(w->main_window, FALSE);
			gtk_timeout_add(100, hview_parm_ctrl_restore_thread,
									data);
		}
      }
}

gint hview_parm_ctrl_save_thread(gpointer data)
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

	res = voh_parm_control(sid, id, 1, err);
	if (res == FALSE) {
		hview_print(w, err);
		hview_statusbar_push(w,
				"saving resource parameters failed");
	} else {
		hview_statusbar_push(w, "ready");
	}

	gtk_widget_set_sensitive(w->main_window, TRUE);

	return FALSE;
}

void hview_parm_ctrl_save_call(GtkWidget *widget, gpointer data)
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
			hview_statusbar_push(w,
			"saving resource parameters (please wait)");
			gtk_widget_set_sensitive(w->main_window, FALSE);
			gtk_timeout_add(100, hview_parm_ctrl_save_thread, data);
		}
      }
}

gint hview_parm_ctrl_default_thread(gpointer data)
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

	res = voh_parm_control(sid, id, 0, err);
	if (res == FALSE) {
		hview_print(w, err);
		hview_statusbar_push(w,
				"setting default resource parameters failed");
	} else {
		hview_statusbar_push(w, "ready");
	}

	gtk_widget_set_sensitive(w->main_window, TRUE);

	return FALSE;
}

void hview_parm_ctrl_default_call(GtkWidget *widget, gpointer data)
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
			hview_statusbar_push(w,
			"setting default resource parameters (please wait)");
			gtk_widget_set_sensitive(w->main_window, FALSE);
			gtk_timeout_add(100, hview_parm_ctrl_default_thread,
									data);
		}
      }
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

void hview_sensor_settings_ok_response(gpointer data)
{
	HviewSenDialogWidgetsT	*dw = (HviewSenDialogWidgetsT *) data;
	HviewWidgetsT		*w = dw->parent_widgets;
	GtkTreeModel		*model;
	GtkTreeIter		iter,		parent;
	GtkTreeSelection	*selection;
	gint			page;
	GtkWidget		*tview,	*dview;
	GList			*thrlist = NULL,	*setlist = NULL;
	VohObjectT		*obj,		*setobj;
	gdouble			val;
	guint			id,		pid,	sid,	capability;
	gboolean		res,		status;
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
		if (!gtk_tree_model_iter_parent(model, &parent, &iter))
			return;
		gtk_tree_model_get(model, &parent, VOH_LIST_COLUMN_ID,
				   &pid, -1);
		gtk_tree_model_get(model, &iter, VOH_LIST_COLUMN_ID, &id,
				   VOH_LIST_COLUMN_CAPABILITY, &capability, -1);
	} else {
		return;
	}

	status = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(
						dw->general_tab.enable_status));
	res = voh_set_sensor_enable(sid, pid, id, status, err);

	if (res == FALSE) {
		hview_print(w, err);
	}

	status = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(
					dw->event_tab.event_enable_status));
	res = voh_set_sensor_event_enable(sid, pid, id, status, err);

	if (res == FALSE) {
		hview_print(w, err);
	}

	if (capability & VOH_ITER_SENSOR_CAPABILITY_THRESHOLD) {
		thrlist = dw->threshold_tab.thrlist;
		while (thrlist != NULL) {
			obj = (VohObjectT *) (thrlist)->data;
			if (obj->state & VOH_OBJECT_WRITABLE) {
				val = gtk_spin_button_get_value(
						GTK_SPIN_BUTTON(obj->data));
				setobj = g_malloc(sizeof(VohObjectT));
				setobj->name = NULL;
				setobj->numerical = obj->numerical;
				setobj->value.type = obj->value.type;
				switch (setobj->value.type) {
				case VOH_OBJECT_TYPE_INT:
					setobj->value.vint = (gint) val;
					break;
				case VOH_OBJECT_TYPE_UINT:
					setobj->value.vuint = (guint) val;
					break;
				case VOH_OBJECT_TYPE_FLOAT:
					setobj->value.vfloat = (gfloat) val;
					break;
				default:
					break;
				}
				setlist = g_list_prepend(setlist, setobj);
			}
			thrlist = thrlist->next;
		}

		if (setlist) {
			res = voh_set_sensor_thresholds(sid, pid, id,
							setlist, err);
			if (res == FALSE) {
				hview_print(w, err);
			}
		}
	}
}

void hview_sensor_settings_call(GtkWidget *widget, gpointer data)
{
	HviewWidgetsT		*w = (HviewWidgetsT *) data;
	HviewSenDialogWidgetsT	dw;
	GtkWidget		*dialog_window;
	GtkTreeModel		*model;
	GtkTreeIter		iter,		parent;
	GtkTreeSelection	*selection;
	gint			page;
	GtkWidget		*tview;
	guint			id,		pid,	sid;
	guint			capability;
	GtkWidget		*label;
	GtkWidget		*hbox;
	GtkWidget		*but;
	GList			*sen_info;
	VohObjectT		*obj;
	gboolean		status;
	gint			res;
	gdouble			min = 0,	max = 100,	step = 1;
	gdouble			vl = 0;
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

	if (!gtk_tree_model_iter_parent(model, &parent, &iter))
		return;

	gtk_tree_model_get(model, &parent, VOH_LIST_COLUMN_ID, &pid, -1);
	gtk_tree_model_get(model, &iter, VOH_LIST_COLUMN_ID, &id,
			   VOH_LIST_COLUMN_CAPABILITY, &capability, -1);

	res = voh_check_rdr_presence(sid, pid, id, err);
	if (res == FALSE) {
		dialog_window = hview_get_rpt_empty_dialog_window(w);
		hview_print(w, err);
	} else {
		dw.parent_widgets = w;
		dialog_window = hview_get_sensor_settings_window(&dw);
		dw.dialog_window = dialog_window;

		sen_info = voh_get_sensor_info(sid, pid, id, err);

		if (sen_info == NULL) {
			hview_print(w, err);
		} else {
			while (sen_info != NULL) {
				hbox = gtk_hbox_new(FALSE, 10);
				gtk_box_pack_start(GTK_BOX(
						dw.general_tab.info_box),
						hbox, FALSE, FALSE, 2);
				label = gtk_label_new(sen_info->data);
				gtk_box_pack_start(GTK_BOX(hbox),
						   label, FALSE, FALSE, 10);
				sen_info = sen_info->next;
			}
		}

		res = voh_get_sensor_enable(sid, pid, id, &status, err);
		if (res != FALSE) {
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
						dw.general_tab.enable_status),
						status);
		} else {
			hview_print(w, err);
		}

		sen_info = voh_get_sensor_event_info(sid, pid, id, err);

		if (sen_info == NULL) {
			hview_print(w, err);
		} else {
			while (sen_info != NULL) {
				hbox = gtk_hbox_new(FALSE, 10);
				gtk_box_pack_start(GTK_BOX(
						dw.event_tab.event_info_box),
						hbox, FALSE, FALSE, 2);
				label = gtk_label_new(sen_info->data);
				gtk_box_pack_start(GTK_BOX(hbox),
						   label, FALSE, FALSE, 10);
				sen_info = sen_info->next;
			}
		}

		res = voh_get_sensor_event_enable(sid, pid, id, &status, err);
		if (res != FALSE) {
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
					dw.event_tab.event_enable_status),
					status);
		} else {
			hview_print(w, err);
		}
	}

	if (capability & VOH_ITER_SENSOR_CAPABILITY_THRESHOLD) {

		res = voh_get_sensor_thresholds(sid, pid, id,
						     &sen_info, err);

		if (res == FALSE) {
			hview_print(w, err);
			label = gtk_label_new("UNKNOWN");
			gtk_box_pack_start(GTK_BOX(dw.threshold_tab.info_box),
					   label, TRUE, TRUE, 10);
			dw.threshold_tab.thrlist = NULL;
			gtk_widget_show_all(dialog_window);
		} else {
			dw.threshold_tab.thrlist = sen_info;
			while (sen_info != NULL) {
				obj = (VohObjectT *) sen_info->data;
				if (obj->data)
					g_free(obj->data);

				hbox = gtk_hbox_new(TRUE, 10);
				gtk_box_pack_start(GTK_BOX(
						dw.threshold_tab.info_box),
						hbox, TRUE, FALSE, 2);
				label = gtk_label_new(obj->name);
				gtk_box_pack_start(GTK_BOX(hbox),
						   label, FALSE, FALSE, 10);

				if (obj->state & VOH_OBJECT_WRITABLE) {
					switch (obj->value.type) {
					case VOH_OBJECT_TYPE_INT:
						vl = (gdouble) obj->value.vint;
						min = 0 - G_MAXDOUBLE;
						max = G_MAXDOUBLE;
						step = vl / 100;
						step = 1;
						break;
					case VOH_OBJECT_TYPE_UINT:
						vl = (gdouble) obj->value.vuint;
						min = 0;
						max = G_MAXDOUBLE;
						step = 1;
						break;
					case VOH_OBJECT_TYPE_FLOAT:
						vl = (gdouble)obj->value.vfloat;
						min = 0 - G_MAXFLOAT;
						max = G_MAXFLOAT;
						step = 0.1;
						break;
					default:
						break;
					}
					but = gtk_spin_button_new_with_range(
								min, max, step);
					obj->data = (gpointer) but;
					gtk_spin_button_set_value(
							GTK_SPIN_BUTTON(but),
							vl);
				} else if (obj->state & VOH_OBJECT_READABLE) {
					but = gtk_label_new(obj->value.vbuffer);
				} else {
					but = gtk_label_new ("NOT AVAILABLE");
				}
				gtk_box_pack_start(GTK_BOX(hbox), but,
						   TRUE, FALSE, 10);	

				sen_info = sen_info->next;
			}
		}
	} else {
		gtk_notebook_remove_page(GTK_NOTEBOOK(dw.notebook),
					 dw.threshold_tab.tab_page_num);
	}


	gtk_widget_show_all(dialog_window);

	res = gtk_dialog_run(GTK_DIALOG(dialog_window));
	switch (res) {
	case GTK_RESPONSE_OK:
		hview_sensor_settings_ok_response((gpointer) &dw);
		break;
	default:
		break;
	}

	gtk_widget_destroy(dialog_window);
}

void hview_sensor_masks_apply_response(gpointer data)
{
	HviewSenEventMasksWidgetsT	*mdw = (HviewSenEventMasksWidgetsT *)
									data;
	HviewSenDialogWidgetsT		*dw = mdw->parent_widgets;
	HviewWidgetsT			*w = dw->parent_widgets;
	GtkTreeModel			*model;
	GtkTreeIter			iter,		parent;
	GtkTreeSelection		*selection;
	gint				page;
	GtkWidget			*tview;
	GList				*assert_event_mask;
	GList				*deassert_event_mask;
	VohEventStateT			*evst;
	guint				id,		pid,	sid;
	gboolean			res,		status;
	gchar				err[1024];

	page = gtk_notebook_get_current_page(GTK_NOTEBOOK(w->tab_windows));
	if (page < 0)
		return;
	if (w->tab_views[page].resource_view == NULL ||
			w->tab_views[page].detail_view == NULL)
		return;
	tview = w->tab_views[page].resource_view;
	sid = w->tab_views[page].sessionid;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tview));

	if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
		if (!gtk_tree_model_iter_parent(model, &parent, &iter))
			return;
		gtk_tree_model_get(model, &parent, VOH_LIST_COLUMN_ID,
				   &pid, -1);
		gtk_tree_model_get(model, &iter, VOH_LIST_COLUMN_ID, &id, -1);
	} else {
		return;
	}


	assert_event_mask = mdw->assert_event_mask;
	while (assert_event_mask != NULL) {
		evst = (VohEventStateT *) assert_event_mask->data;
		status = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(
								evst->data));
		evst->active = status;
		assert_event_mask = assert_event_mask->next;
	}

	deassert_event_mask = mdw->deassert_event_mask;
	while (deassert_event_mask != NULL) {
		evst = (VohEventStateT *) deassert_event_mask->data;
		status = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(
								evst->data));
		evst->active = status;
		deassert_event_mask = deassert_event_mask->next;
	}	

	res = voh_set_sensor_event_masks(sid, pid, id,
					 mdw->assert_event_mask,
					 mdw->deassert_event_mask,
					 err);

	if (res == FALSE) {
		hview_print(w, err);
	}

}

void hview_sensor_assert_mask_set_call(GtkWidget *widget, gpointer data)
{
	HviewSenDialogWidgetsT		*dw = (HviewSenDialogWidgetsT *) data;
	HviewWidgetsT			*w = dw->parent_widgets;
	HviewSenEventMasksWidgetsT	mdw;
	GtkWidget			*dialog;
	GtkTreeModel			*model;
	GtkTreeIter			iter,		parent;
	GtkTreeSelection		*selection;
	gint				page;
	GtkWidget			*tview;
	GtkWidget			*but;
	GList				*mask,	*ev_supported;
	VohEventStateT			*evst,	*evsup;
	guint				id,		pid,	sid;
	gint				res;
	gchar				err[1024];

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

	if (!gtk_tree_model_iter_parent(model, &parent, &iter))
		return;

	gtk_tree_model_get(model, &parent, VOH_LIST_COLUMN_ID, &pid, -1);
	gtk_tree_model_get(model, &iter, VOH_LIST_COLUMN_ID, &id, -1);

	mdw.parent_widgets = dw;
	dialog = hview_get_sensor_assert_mask_window(&mdw);
	mdw.dialog_window = dialog;

	res = voh_get_sensor_assert_event_mask(sid, pid, id, &mask, err);

	mdw.assert_event_mask = mask;
	mdw.deassert_event_mask = NULL;

	if (res == FALSE) {
		hview_print(w, err);
		gtk_widget_destroy(dialog);
		return;
	} else {
		res = voh_get_sensor_event_states_supported(sid, pid, id,
							    &ev_supported, err);
		if (res == FALSE) {
			hview_print(w, err);
			gtk_widget_destroy(dialog);
			return;
		}
		while (mask != NULL) {
			evst = (VohEventStateT *) mask->data;
			evsup = (VohEventStateT *) ev_supported->data;
			but = gtk_check_button_new_with_label(evst->string);
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(but),
						     evst->active);
			gtk_widget_set_sensitive(but, evsup->active);
			gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),
					   but, TRUE, FALSE, 4);
			evst->data = (gpointer) but;
			mask = mask->next;
			ev_supported = ev_supported->next;
		}

	}

	gtk_widget_show_all(dialog);

	res = gtk_dialog_run(GTK_DIALOG(dialog));
	switch (res) {
	case GTK_RESPONSE_APPLY:
		hview_sensor_masks_apply_response((gpointer) &mdw);
		break;
	default:
		break;
	}

	gtk_widget_destroy(dialog);
}

void hview_sensor_deassert_mask_set_call(GtkWidget *widget, gpointer data)
{
	HviewSenDialogWidgetsT		*dw = (HviewSenDialogWidgetsT *) data;
	HviewWidgetsT			*w = dw->parent_widgets;
	HviewSenEventMasksWidgetsT	mdw;
	GtkWidget			*dialog;
	GtkTreeModel			*model;
	GtkTreeIter			iter,		parent;
	GtkTreeSelection		*selection;
	gint				page;
	GtkWidget			*tview;
	GtkWidget			*but;
	GList				*mask,	*ev_supported;
	VohEventStateT			*evst,	*evsup;
	guint				id,		pid,	sid;
	gint				res;
	gchar				err[1024];

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

	if (!gtk_tree_model_iter_parent(model, &parent, &iter))
		return;

	gtk_tree_model_get(model, &parent, VOH_LIST_COLUMN_ID, &pid, -1);
	gtk_tree_model_get(model, &iter, VOH_LIST_COLUMN_ID, &id, -1);

	mdw.parent_widgets = dw;
	dialog = hview_get_sensor_deassert_mask_window(&mdw);
	mdw.dialog_window = dialog;

	res = voh_get_sensor_deassert_event_mask(sid, pid, id, &mask, err);

	mdw.assert_event_mask = NULL;
	mdw.deassert_event_mask = mask;

	if (res == FALSE) {
		hview_print(w, err);
		gtk_widget_destroy(dialog);
		return;
	} else {
		res = voh_get_sensor_event_states_supported(sid, pid, id,
							    &ev_supported, err);
		if (res == FALSE) {
			hview_print(w, err);
			gtk_widget_destroy(dialog);
			return;
		}
		while (mask != NULL) {
			evst = (VohEventStateT *) mask->data;
			evsup = (VohEventStateT *) ev_supported->data;
			but = gtk_check_button_new_with_label(evst->string);
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(but),
						     evst->active);
			gtk_widget_set_sensitive(but, evsup->active);
			gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),
					   but, TRUE, FALSE, 4);
			evst->data = (gpointer) but;
			mask = mask->next;
			ev_supported = ev_supported->next;
		}

	}

	gtk_widget_show_all(dialog);

	res = gtk_dialog_run(GTK_DIALOG(dialog));
	switch (res) {
	case GTK_RESPONSE_APPLY:
		hview_sensor_masks_apply_response((gpointer) &mdw);
		break;
	default:
		break;
	}

	gtk_widget_destroy(dialog);
}

void hview_inventory_settings_ok_response(gpointer data)
{
	HviewInvDialogWidgetsT	*dw = (HviewInvDialogWidgetsT *) data;
	HviewWidgetsT		*w = dw->parent_widgets;
	GtkTreeModel		*model;
	GtkTreeIter		iter,		parent;
	GtkTreeSelection	*selection;
	gint			page;
	GtkWidget		*tview,	*dview;
	guint			id,		pid,	sid;
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
		if (!gtk_tree_model_iter_parent(model, &parent, &iter))
			return;
		gtk_tree_model_get(model, &parent, VOH_LIST_COLUMN_ID,
				   &pid, -1);
		gtk_tree_model_get(model, &iter, VOH_LIST_COLUMN_ID, &id, -1);
	} else {
		return;
	}
}


void hview_inventory_settings_call(GtkWidget *widget, gpointer data)
{
	HviewWidgetsT		*w = (HviewWidgetsT *) data;
	HviewInvDialogWidgetsT	dw;
	GtkWidget		*dialog_window;
	GtkTreeModel		*model;
	GtkTreeIter		iter,		parent;
	GtkTreeSelection	*selection;
	gint			page;
	GtkWidget		*tview;
	GtkWidget		*hbox;
	GtkWidget		*label;
	guint			id,		pid,	sid;
	GList			*info,		*area_list;
	GtkListStore		*area_model;
	VohObjectT		*obj;
	HviewCallDataT		*call_data;
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

	if (!gtk_tree_model_iter_parent(model, &parent, &iter))
		return;

	gtk_tree_model_get(model, &parent, VOH_LIST_COLUMN_ID, &pid, -1);
	gtk_tree_model_get(model, &iter, VOH_LIST_COLUMN_ID, &id, -1);

	call_data = (HviewCallDataT *) g_malloc(sizeof(HviewCallDataT));
	call_data->sessionid = sid;
	call_data->parentid = pid;
	call_data->entryid = id;

	res = voh_check_rdr_presence(sid, pid, id, err);
	if (res == FALSE) {
		dialog_window = hview_get_rpt_empty_dialog_window(w);
		hview_print(w, err);
	} else {
		dw.parent_widgets = w;
		dialog_window = hview_get_inventory_settings_window(&dw);
		dw.dialog_window = dialog_window;
		call_data->data = (gpointer) &dw;

		res = voh_get_inventory_info(sid, pid, id, &info, err);

		if (info == NULL) {
			hview_print(w, err);
			label = gtk_label_new("UNKNOWN");
			gtk_widget_show_all(dialog_window);
			gtk_box_pack_start(GTK_BOX(dw.general_tab.info_box),
					   label, TRUE, TRUE, 0);
		} else {
			while (info != NULL) {
				obj = (VohObjectT *) info->data;
				hbox = gtk_hbox_new(TRUE, 5);
				gtk_box_pack_start(GTK_BOX(
						dw.general_tab.info_box), hbox,
						TRUE, FALSE, 10);
				label = gtk_label_new(obj->name);
				gtk_box_pack_start(GTK_BOX(hbox), label,
						   FALSE, FALSE, 5);
				label = gtk_label_new(obj->value.vbuffer);
				gtk_box_pack_start(GTK_BOX(hbox), label,
						   FALSE, FALSE, 5);

				info = info->next;
			}
		}

		res = voh_get_idr_area_with_field(sid, pid, id,
						  &dw.general_tab.area_list,
						  err);

		if (res == FALSE) {
			hview_print(w, err);
			gtk_widget_destroy(dw.general_tab.fru_view);
			dw.general_tab.fru_view = NULL;
			label = gtk_label_new("unknown");
			gtk_box_pack_end(GTK_BOX(dw.general_tab.invfield_box),
					 label, TRUE, TRUE, 5);
		} else if (dw.general_tab.area_list == NULL) {
			gtk_widget_destroy(dw.general_tab.fru_view);
			dw.general_tab.fru_view = NULL;
			label = gtk_label_new("nothing");
			gtk_box_pack_end(GTK_BOX(dw.general_tab.invfield_box),
					 label, TRUE, TRUE, 5);
		} else {
			g_signal_connect(G_OBJECT(dw.general_tab.fru_view),
			"button-press-event",
			G_CALLBACK(hview_butpress_invareas_call), &dw);

			g_signal_connect(G_OBJECT(dw.fields_tab.add_area),
					 "clicked",
					 G_CALLBACK(hview_invarea_add_call),
					 (gpointer) call_data);

			g_signal_connect(G_OBJECT(dw.fields_tab.remove_area),
					 "clicked",
					 G_CALLBACK(hview_invarea_remove_call),
					 (gpointer) call_data);
			g_signal_connect(G_OBJECT(dw.fields_tab.add_field),
					 "clicked",
					 G_CALLBACK(hview_invfield_add_call),
					 (gpointer) call_data);
			g_signal_connect(G_OBJECT(dw.fields_tab.remove_field),
					 "clicked",
					 G_CALLBACK(hview_invfield_remove_call),
					 (gpointer) call_data);
			g_signal_connect(G_OBJECT(dw.fields_tab.set_field),
					 "clicked",
					 G_CALLBACK(hview_invfield_set_call),
					 (gpointer) call_data);


			area_model = gtk_list_store_new(5, G_TYPE_STRING,
							G_TYPE_POINTER,
							G_TYPE_UINT,
							G_TYPE_UINT,
							G_TYPE_UINT);
			gtk_tree_view_set_model(GTK_TREE_VIEW(
						dw.fields_tab.areas_view),
						GTK_TREE_MODEL(area_model));
			selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(
						dw.fields_tab.areas_view));

			g_signal_connect(G_OBJECT(dw.fields_tab.areas_view),
					 "unselect-all",
					 G_CALLBACK(hview_invarea_unsel_call),
					 (gpointer) &dw);
			g_signal_connect(G_OBJECT(dw.fields_tab.fields_view),
					 "unselect-all",
					 G_CALLBACK(hview_invfield_unsel_call),
					 (gpointer) &dw);

			g_signal_connect(G_OBJECT(selection), "changed",
					G_CALLBACK(hview_invarea_selected_call),
					(gpointer) &dw);

			selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(
						dw.fields_tab.fields_view));

			g_signal_connect(G_OBJECT(selection), "changed",
				G_CALLBACK(hview_invfield_selected_call),
				(gpointer) &dw);

			area_list = dw.general_tab.area_list;
			while (area_list != NULL) {
				obj = (VohObjectT *) area_list->data;
				gtk_list_store_append(area_model, &iter);
				gtk_list_store_set(area_model, &iter,
						   0, obj->name,
						   1, obj->data,
						   2, obj->id,
						   3, obj->state,
						   4, obj->numerical,
						   -1);
				area_list = area_list->next;
			}
				
		}
	}


	gtk_widget_show_all(dialog_window);

	res = gtk_dialog_run(GTK_DIALOG(dialog_window));
	switch (res) {
	case GTK_RESPONSE_OK:
		hview_inventory_settings_ok_response((gpointer) &dw);
		break;
	default:
		break;
	}

	gtk_widget_destroy(dialog_window);
}

gboolean hview_butpress_invareas_call(GtkWidget *widget,
				      GdkEventButton *event,
				      gpointer data)
{
	HviewInvDialogWidgetsT	*dw = (HviewInvDialogWidgetsT *) data;
	HviewWidgetsT		*w = dw->parent_widgets;
	GtkWidget		*menu,		*smenu,		*fmenu;
	GtkWidget		*item,		*sitem,		*fitem;
	GList			*areas =dw->general_tab.area_list;
	GList			*fields;
	VohObjectT		*obj;

      if (event->type != GDK_BUTTON_PRESS || event->button != 1) {
	      return TRUE;
      }

      menu = gtk_menu_new();
      while (areas != NULL) {
	      obj = (VohObjectT *) areas->data;
	      item = hwidget_get_menu_item(NULL, obj->name, NULL, NULL);
	      gtk_container_add(GTK_CONTAINER(menu), item);
	      if (obj->data != NULL) {
		      fields = (GList *) obj->data;
		      smenu = gtk_menu_new();
		      gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), smenu);

		      while (fields != NULL) {
			      obj = (VohObjectT *) fields->data;
			      sitem = hwidget_get_menu_item(NULL, obj->name,
					      		    NULL, NULL);
			      gtk_container_add(GTK_CONTAINER(smenu), sitem);

			      fmenu = gtk_menu_new();
			      gtk_menu_item_set_submenu(GTK_MENU_ITEM(sitem),
					      		fmenu);
			      fitem = hwidget_get_menu_item(NULL, obj->data,
					      		    NULL, NULL);
			      gtk_container_add(GTK_CONTAINER(fmenu), fitem);

			      fields = fields->next;
		      }
	      }
	      areas = areas->next;
      }

      gtk_widget_show_all(menu);
      
      gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL, event->button,
		      gdk_event_get_time((GdkEvent*)event));

      return TRUE;
}

void hview_invfield_set_apply_response(HviewCallDataT *cdata)
{
	HviewInvFieldAddWidgetsT	*dw = (HviewInvFieldAddWidgetsT *)
								cdata->data;
	HviewInvDialogWidgetsT	*idw = dw->parent_widgets;
	HviewWidgetsT		*mw = idw->parent_widgets;
	guint			sid = cdata->sessionid;
	guint			rid = cdata->parentid;
	guint			iid = cdata->entryid;
	guint			aid,	fid;
	GtkTreeSelection	*selection;
	GtkTreeModel		*field_store,	*area_model,	*model;
	GtkListStore		*store;
	GtkTreeIter		iter,	aiter;
	GList			*field_list,	*area_list;
	VohObjectT		*obj;
	VtDataT			*field_data;
	guint			type;
	gchar			*type_name = NULL;
	GtkTextBuffer		*buf;
	GtkTextIter		st_iter,	end_iter;
	gboolean		res;
	gchar			*buffer;
	gchar			err[1024];


	field_store = gtk_combo_box_get_model(GTK_COMBO_BOX(dw->field_types));
	res = gtk_combo_box_get_active_iter(GTK_COMBO_BOX(dw->field_types),
							  &iter);
	if (res == FALSE) {
		return;
	}
	gtk_tree_model_get(field_store, &iter, 0, &type_name, 1, &type, -1);

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(
						idw->fields_tab.areas_view));
	area_model = gtk_tree_view_get_model(GTK_TREE_VIEW(
						idw->fields_tab.areas_view));
	if (gtk_tree_selection_get_selected(selection, &area_model, &aiter)) {
		gtk_tree_model_get(area_model, &aiter, 2, &aid, -1);
	} else {
		return;
	}

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(
						idw->fields_tab.fields_view));
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(
						idw->fields_tab.fields_view));
	if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
		gtk_tree_model_get(model, &iter, 2, &fid, -1);
	} else {
		return;
	}

	field_data = vt_data_new(VT_IDR_FIELD);

	vt_data_value_set(field_data, "area_id", &aid);
	vt_data_value_set(field_data, "field_id", &fid);
	vt_data_value_set(field_data, "type", &type);

	res = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(dw->read_only));
	vt_data_value_set(field_data, "read_only", &res);

	buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(dw->data_view));
	gtk_text_buffer_get_start_iter(buf, &st_iter);
	gtk_text_buffer_get_end_iter(buf, &end_iter);
	buffer = gtk_text_buffer_get_text(buf, &st_iter, &end_iter, FALSE);
	vt_data_value_set(field_data, "field.data", buffer);
	if (buffer)
		g_free(buffer);

	res = voh_idr_field_set(sid, rid, iid, field_data, err);
	vt_data_destroy(field_data);

	if (res == FALSE) {
		hview_print(mw, err);
	} else {
		res = voh_get_idr_area_with_field(sid, rid, iid,
						  &area_list,
						  err);
		if (res != FALSE) {
			idw->general_tab.area_list = area_list;
			while (area_list != NULL) {
				obj = (VohObjectT *) area_list->data;
				if (obj->id != aid) {
					area_list = area_list->next;
					continue;
				}
				gtk_list_store_set(GTK_LIST_STORE(area_model),
						&aiter,
						1, obj->data,
						-1);
				gtk_tree_view_set_model(GTK_TREE_VIEW(
						idw->fields_tab.fields_view),
						NULL);
				field_list = obj->data;
				store = gtk_list_store_new(5, G_TYPE_STRING,
							G_TYPE_POINTER,
							G_TYPE_UINT,
							G_TYPE_UINT,
							G_TYPE_UINT);
				while (field_list != NULL) {
					obj = (VohObjectT *) field_list->data;
					gtk_list_store_append(store, &iter);
					gtk_list_store_set(store, &iter,
							0, obj->name,
							1, obj->data,
							2, obj->id,
							3, obj->state,
							4, obj->numerical,
							-1);
					field_list = field_list->next;
				}
				break;
			}

			gtk_tree_view_set_model(GTK_TREE_VIEW(
						idw->fields_tab.fields_view),
						GTK_TREE_MODEL(store));
			selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(
						idw->fields_tab.fields_view));
			gtk_tree_selection_select_iter(selection, &iter);
		}
	}

	if (type_name) {
		g_free(type_name);
	}	
}

gboolean hview_invfield_set_call(GtkWidget *widget, gpointer data)
{
	HviewCallDataT			*cdata = (HviewCallDataT *) data;
	HviewInvDialogWidgetsT		*idw = (HviewInvDialogWidgetsT *)
							cdata->data;
	HviewInvFieldAddWidgetsT	dw;
	HviewWidgetsT			*mw = idw->parent_widgets;
	HviewCallDataT			call_data;
	GtkTreeSelection		*selection;
	GtkTreeModel			*amodel,	*model;
	GtkTreeIter			aiter,		iter;
	guint				aid,		fid;
	guint				type;
	GtkTextBuffer			*buf;
	VtDataT				*field_s;
	gint				res,		val;
	gchar				buffer[1024];
	gchar				err[1024];
	
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(
						idw->fields_tab.areas_view));
	amodel = gtk_tree_view_get_model(GTK_TREE_VIEW(
						idw->fields_tab.areas_view));
	if (gtk_tree_selection_get_selected(selection, &amodel, &aiter)) {
		gtk_tree_model_get(amodel, &aiter, 2, &aid, -1);
	} else {
		return FALSE;
	}

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(
						idw->fields_tab.fields_view));
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(
						idw->fields_tab.fields_view));
	if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
			gtk_tree_model_get(model, &iter, 2, &fid,
					   4, &type, -1);
	} else {
		return FALSE;
	}

	field_s = vt_data_new(VT_IDR_FIELD);

	res = voh_idr_field_get(cdata->sessionid, cdata->parentid,
			cdata->entryid, aid, type, fid, NULL, field_s, err);
	if (res == FALSE) {
		hview_print(mw, err);
		return FALSE;
	}

	dw.parent_widgets = idw;
	dw.dialog_window = hview_get_invfield_add_window(&dw);

	vt_data_value_get(field_s, "field.data", buffer);
	buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(dw.data_view));
	gtk_text_buffer_set_text(buf, buffer, strlen(buffer));

	vt_data_value_get(field_s, "read_only", &val);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(dw.read_only), val);

	vt_data_value_get(field_s, "type", &val);
	model = gtk_combo_box_get_model(GTK_COMBO_BOX(dw.field_types));
	if (hutil_find_iter_by_id(model, 1, val, 0, &iter, HUTIL_FIRST_ITER)
								== TRUE) {
		gtk_combo_box_set_active_iter(GTK_COMBO_BOX(dw.field_types),
								&iter);
	}
	vt_data_destroy(field_s);

	gtk_widget_show_all(dw.dialog_window);

	call_data.sessionid = cdata->sessionid;
	call_data.parentid = cdata->parentid;
	call_data.entryid = cdata->entryid;
	call_data.data = (gpointer) &dw;

	res = gtk_dialog_run(GTK_DIALOG(dw.dialog_window));
	switch (res) {
	case GTK_RESPONSE_APPLY:
		hview_invfield_set_apply_response(&call_data);
		break;
	default:
		 break;
	}

	gtk_widget_destroy(dw.dialog_window);

	return TRUE;
}

gboolean hview_butpress_invdata_view_call(GtkWidget *widget,
					  GdkEventButton *event,
					  gpointer data)
{
      HviewWidgetsT	*w = (HviewWidgetsT *) data;

      if (event->type == GDK_BUTTON_PRESS && event->button == 1) {
	      return FALSE;
      }
      if (event->type != GDK_BUTTON_PRESS || event->button != 3) {
	      return TRUE;
      }

      return TRUE;

}

gboolean hview_invarea_selected_call(GtkTreeSelection *selection,
				     gpointer data)
{
	HviewInvDialogWidgetsT	*dw = (HviewInvDialogWidgetsT *) data;
	GtkTreeModel		*astore;
	GtkListStore		*fstore = NULL;
	GtkTreeIter		iter;
	GList			*fields;
	VohObjectT		*obj;
	guint			state;
 
	gtk_label_set_text(GTK_LABEL(dw->fields_tab.data_label), "");

	gtk_widget_set_sensitive(dw->fields_tab.add_field, TRUE);
	gtk_widget_set_sensitive(dw->fields_tab.remove_area, TRUE);

	if (gtk_tree_selection_get_selected(selection, &astore, &iter)) {
		gtk_tree_model_get(astore, &iter, 1, &fields,
			           3, &state, -1);
	} else {
		hview_invarea_unsel_call(GTK_TREE_VIEW(
						dw->fields_tab.areas_view),
					(gpointer) dw);
		return TRUE;
	}

	gtk_tree_view_set_model(GTK_TREE_VIEW(dw->fields_tab.fields_view),
				NULL);
	astore = gtk_tree_view_get_model(GTK_TREE_VIEW(
						dw->fields_tab.areas_view));


	if (!(state & VOH_OBJECT_WRITABLE)) {
		gtk_widget_set_sensitive(dw->fields_tab.add_field, FALSE);
		gtk_widget_set_sensitive(dw->fields_tab.remove_area, FALSE);
	}

	fstore = gtk_list_store_new(5, G_TYPE_STRING, G_TYPE_POINTER,
					G_TYPE_UINT, G_TYPE_UINT, G_TYPE_UINT);
	while (fields != NULL) {
		obj = (VohObjectT *) fields->data;

		gtk_list_store_append(fstore, &iter);
		gtk_list_store_set(fstore, &iter,
				   0, obj->name,
				   1, obj->data,
				   2, obj->id,
				   3, obj->state,
				   4, obj->numerical,
				   -1);
		fields = fields->next;
	}

	gtk_tree_view_set_model(GTK_TREE_VIEW(dw->fields_tab.fields_view),
				GTK_TREE_MODEL(fstore));

	return TRUE;
}

gboolean hview_invfield_selected_call(GtkTreeSelection *selection,
				      gpointer data)
{
	HviewInvDialogWidgetsT	*dw = (HviewInvDialogWidgetsT *) data;
	GtkTreeSelection	*area_selection;
	GtkTreeModel		*fstore,	*astore;
	GtkTreeIter		iter;
	gchar			*fru_data;
	guint			astate,		fstate;
 
	gtk_label_set_text(GTK_LABEL(dw->fields_tab.data_label), "");
	area_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(
						dw->fields_tab.areas_view));
	astore = gtk_tree_view_get_model(GTK_TREE_VIEW(
				dw->fields_tab.fields_view));
	if (gtk_tree_selection_get_selected(area_selection, &astore, &iter)) {
		gtk_tree_model_get(astore, &iter, 3, &astate, -1);
	} else {
		hview_invarea_unsel_call(GTK_TREE_VIEW(
						dw->fields_tab.fields_view),
					(gpointer) dw);
		return TRUE;
	}

	if (astate & VOH_OBJECT_WRITABLE) {
		gtk_widget_set_sensitive(dw->fields_tab.remove_field, TRUE);
		gtk_widget_set_sensitive(dw->fields_tab.set_field, TRUE);
	}

	fstore = gtk_tree_view_get_model(GTK_TREE_VIEW(
						dw->fields_tab.fields_view));

	if (gtk_tree_selection_get_selected(selection, &fstore, &iter)) {
		gtk_tree_model_get(fstore, &iter, 1, &fru_data,
				   3, &fstate, -1);
	} else {
		hview_invfield_unsel_call(GTK_TREE_VIEW(
						dw->fields_tab.fields_view),
					(gpointer) dw);
		return TRUE;
	}
	gtk_label_set_text(GTK_LABEL(dw->fields_tab.data_label), fru_data);
	if (!(fstate & VOH_OBJECT_WRITABLE)) {
		gtk_widget_set_sensitive(dw->fields_tab.set_field, FALSE);
		gtk_widget_set_sensitive(dw->fields_tab.remove_field, FALSE);
	}

	return TRUE;
}

void hview_invarea_add_apply_response(HviewCallDataT *cdata)
{
	HviewInvAreaAddWidgetsT	*dw = (HviewInvAreaAddWidgetsT *) cdata->data;
	HviewInvDialogWidgetsT	*idw = dw->parent_widgets;
	HviewWidgetsT		*mw = idw->parent_widgets;
	guint			sid = cdata->sessionid;
	guint			rid = cdata->parentid;
	guint			iid = cdata->entryid;
	guint			aid;
	GtkTreeSelection	*selection;
	GtkTreeModel		*area_store;
	GtkListStore		*store;
	GtkTreeIter		iter;
	GList			*area_list;
	VohObjectT		*obj;
	guint			type;
	gchar			*type_name = NULL;
	gboolean		res;
	gchar			err[1024];

	area_store = gtk_combo_box_get_model(GTK_COMBO_BOX(dw->area_types));
	res = gtk_combo_box_get_active_iter(GTK_COMBO_BOX(dw->area_types),
							  &iter);
	if (res == FALSE) {
		return;
	}

	gtk_tree_model_get(area_store, &iter, 0, &type_name, 1, &type, -1);

	res = voh_idr_area_add(sid, rid, iid, type, &aid, err);

	if (res == FALSE) {
		hview_print(mw, err);
	} else {
		res = voh_get_idr_area_with_field(sid, rid, iid,
						  &area_list,
						  err);
		if (res != FALSE) {
			idw->general_tab.area_list = area_list;
			gtk_tree_view_set_model(GTK_TREE_VIEW(
						idw->fields_tab.areas_view),
						NULL);
			store = gtk_list_store_new(5, G_TYPE_STRING,
						   G_TYPE_POINTER,
						   G_TYPE_UINT,
						   G_TYPE_UINT,
						   G_TYPE_UINT);

			while (area_list != NULL) {
				obj = (VohObjectT *) area_list->data;
				gtk_list_store_append(store, &iter);
				gtk_list_store_set(store, &iter,
				   0, obj->name,
				   1, obj->data,
				   2, obj->id,
				   3, obj->state,
				   4, obj->numerical,
				   -1);

				area_list = area_list->next;
			}

			gtk_tree_view_set_model(GTK_TREE_VIEW(
						idw->fields_tab.areas_view),
						GTK_TREE_MODEL(store));
			selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(
						idw->fields_tab.areas_view));
			gtk_tree_selection_select_iter(selection, &iter);
		}
	}

	if (type_name) {
		g_free(type_name);
	}	
}

gboolean hview_invarea_add_call(GtkWidget *button, gpointer data)
{
	HviewCallDataT		*cdata = (HviewCallDataT *) data;
	HviewInvDialogWidgetsT	*pw = (HviewInvDialogWidgetsT *)
							cdata->data;
	HviewInvAreaAddWidgetsT	dw;
	HviewCallDataT		*call_data;
	gint			res;
	
	dw.parent_widgets = pw;
	dw.dialog_window = hview_get_invarea_add_window(&dw);

	gtk_widget_show_all(dw.dialog_window);

	call_data = (HviewCallDataT *) g_malloc(sizeof(HviewCallDataT));
	call_data->sessionid = cdata->sessionid;
	call_data->parentid = cdata->parentid;
	call_data->entryid = cdata->entryid;
	call_data->data = (gpointer) &dw;

	res = gtk_dialog_run(GTK_DIALOG(dw.dialog_window));
	switch (res) {
	case GTK_RESPONSE_APPLY:
		hview_invarea_add_apply_response(call_data);
		break;
	default:
		break;
	}

	gtk_widget_destroy(dw.dialog_window);

	return TRUE;
}

gboolean hview_invarea_remove_call(GtkWidget *button, gpointer data)
{
	HviewCallDataT		*cdata = (HviewCallDataT *) data;
	HviewInvDialogWidgetsT	*dw = (HviewInvDialogWidgetsT *)
							cdata->data;
	HviewWidgetsT		*mw = dw->parent_widgets;
	guint			sid = cdata->sessionid;
	guint			rid = cdata->parentid;
	guint			iid = cdata->entryid;
	GtkTreeSelection	*selection;
	GtkTreeModel		*model;
	GtkListStore		*store;
	GtkTreeIter		iter;
	GList			*area_list;
	VohObjectT		*obj;
	guint			aid;
	gboolean		res;
	gchar			err[1024];
	

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(
					dw->fields_tab.areas_view));
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(
					dw->fields_tab.areas_view));

	if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
		gtk_tree_model_get(model, &iter, 2, &aid, -1);

		res = voh_idr_area_delete(sid, rid, iid, aid, err);

		if (res == FALSE) {
			hview_print(mw, err);
		}
		res = voh_get_idr_area_with_field(sid, rid, iid,
						  &area_list,
						  err);
		if (res != FALSE) {
			dw->general_tab.area_list = area_list;
			gtk_tree_view_set_model(GTK_TREE_VIEW(
						dw->fields_tab.areas_view),
						NULL);
			hview_invarea_unsel_call(GTK_TREE_VIEW(
						 dw->fields_tab.areas_view),
						(gpointer) dw);
			store = gtk_list_store_new(5, G_TYPE_STRING,
						   G_TYPE_POINTER,
						   G_TYPE_UINT,
						   G_TYPE_UINT,
						   G_TYPE_UINT);

			while (area_list != NULL) {
				obj = (VohObjectT *) area_list->data;
				gtk_list_store_append(store, &iter);
				gtk_list_store_set(store, &iter,
				   0, obj->name,
				   1, obj->data,
				   2, obj->id,
				   3, obj->state,
				   4, obj->numerical,
				   -1);

				area_list = area_list->next;
			}

			gtk_tree_view_set_model(GTK_TREE_VIEW(
						dw->fields_tab.areas_view),
						GTK_TREE_MODEL(store));
		}
	}

	return TRUE;
}

gboolean hview_invarea_unsel_call(GtkTreeView *area_view, gpointer data)
{
	HviewInvDialogWidgetsT	*dw = (HviewInvDialogWidgetsT *) data;
 
	gtk_widget_set_sensitive(dw->fields_tab.remove_area, FALSE);
	gtk_widget_set_sensitive(dw->fields_tab.add_field, FALSE);
	gtk_widget_set_sensitive(dw->fields_tab.remove_field, FALSE);
	gtk_widget_set_sensitive(dw->fields_tab.set_field, FALSE);
	gtk_tree_view_set_model(GTK_TREE_VIEW(dw->fields_tab.fields_view),
		       		NULL);
	gtk_label_set_text(GTK_LABEL(dw->fields_tab.data_label), "");

	return TRUE;
}

gboolean hview_invfield_unsel_call(GtkTreeView *field_view, gpointer data)
{
	HviewInvDialogWidgetsT	*dw = (HviewInvDialogWidgetsT *) data;

	gtk_widget_set_sensitive(dw->fields_tab.remove_field, FALSE);
	gtk_widget_set_sensitive(dw->fields_tab.set_field, FALSE);
	gtk_label_set_text(GTK_LABEL(dw->fields_tab.data_label), "");

	return TRUE;
}

void hview_invfield_add_apply_response(HviewCallDataT *cdata)
{
	HviewInvFieldAddWidgetsT	*dw = (HviewInvFieldAddWidgetsT *)
								cdata->data;
	HviewInvDialogWidgetsT	*idw = dw->parent_widgets;
	HviewWidgetsT		*mw = idw->parent_widgets;
	guint			sid = cdata->sessionid;
	guint			rid = cdata->parentid;
	guint			iid = cdata->entryid;
	guint			aid;
	GtkTreeSelection	*selection;
	GtkTreeModel		*field_store,	*area_model;
	GtkListStore		*store;
	GtkTreeIter		iter,	aiter;
	GList			*field_list,	*area_list;
	VohObjectT		*obj;
	VtDataT			*field_data;
	guint			type;
	gchar			*type_name = NULL;
	GtkTextBuffer		*buf;
	GtkTextIter		st_iter,	end_iter;
	gboolean		res;
	gchar			*buffer;
	gchar			err[1024];


	field_store = gtk_combo_box_get_model(GTK_COMBO_BOX(dw->field_types));
	res = gtk_combo_box_get_active_iter(GTK_COMBO_BOX(dw->field_types),
							  &iter);
	if (res == FALSE) {
		return;
	}
	gtk_tree_model_get(field_store, &iter, 0, &type_name, 1, &type, -1);

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(
						idw->fields_tab.areas_view));
	area_model = gtk_tree_view_get_model(GTK_TREE_VIEW(
						idw->fields_tab.areas_view));
	if (gtk_tree_selection_get_selected(selection, &area_model, &aiter)) {
		gtk_tree_model_get(area_model, &aiter, 2, &aid, -1);
	} else {
		return;
	}


	field_data = vt_data_new(VT_IDR_FIELD);

	vt_data_value_set(field_data, "area_id", &aid);
	vt_data_value_set(field_data, "type", &type);

	res = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(dw->read_only));
	vt_data_value_set(field_data, "read_only", &res);

	buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(dw->data_view));
	gtk_text_buffer_get_start_iter(buf, &st_iter);
	gtk_text_buffer_get_end_iter(buf, &end_iter);
	buffer = gtk_text_buffer_get_text(buf, &st_iter, &end_iter, FALSE);
	vt_data_value_set(field_data, "field.data", buffer);
	if (buffer)
		g_free(buffer);

	res = voh_idr_field_add(sid, rid, iid, field_data, err);
	vt_data_destroy(field_data);

	if (res == FALSE) {
		hview_print(mw, err);
	} else {
		res = voh_get_idr_area_with_field(sid, rid, iid,
						  &area_list,
						  err);
		if (res != FALSE) {
			idw->general_tab.area_list = area_list;
			while (area_list != NULL) {
				obj = (VohObjectT *) area_list->data;
				if (obj->id != aid) {
					area_list = area_list->next;
					continue;
				}
				gtk_list_store_set(GTK_LIST_STORE(area_model),
						&aiter,
						1, obj->data,
						-1);
				gtk_tree_view_set_model(GTK_TREE_VIEW(
						idw->fields_tab.fields_view),
						NULL);
				field_list = obj->data;
				store = gtk_list_store_new(5, G_TYPE_STRING,
							G_TYPE_POINTER,
							G_TYPE_UINT,
							G_TYPE_UINT,
							G_TYPE_UINT);
				while (field_list != NULL) {
					obj = (VohObjectT *) field_list->data;
					gtk_list_store_append(store, &iter);
					gtk_list_store_set(store, &iter,
							0, obj->name,
							1, obj->data,
							2, obj->id,
							3, obj->state,
							4, obj->numerical,
							-1);
					field_list = field_list->next;
				}
				break;
			}

			gtk_tree_view_set_model(GTK_TREE_VIEW(
						idw->fields_tab.fields_view),
						GTK_TREE_MODEL(store));
			selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(
						idw->fields_tab.fields_view));
			gtk_tree_selection_select_iter(selection, &iter);
		}
	}

	if (type_name) {
		g_free(type_name);
	}	
}

gboolean hview_invfield_add_call(GtkWidget *button, gpointer data)
{
	HviewCallDataT			*cdata = (HviewCallDataT *) data;
	HviewInvDialogWidgetsT		*pw = (HviewInvDialogWidgetsT *)
							cdata->data;
	HviewInvFieldAddWidgetsT	dw;
	HviewCallDataT			call_data;
	gint				res;
	
	dw.parent_widgets = pw;
	dw.dialog_window = hview_get_invfield_add_window(&dw);

	gtk_widget_show_all(dw.dialog_window);

	call_data.sessionid = cdata->sessionid;
	call_data.parentid = cdata->parentid;
	call_data.entryid = cdata->entryid;
	call_data.data = (gpointer) &dw;

	res = gtk_dialog_run(GTK_DIALOG(dw.dialog_window));
	switch (res) {
	case GTK_RESPONSE_APPLY:
		hview_invfield_add_apply_response(&call_data);
		break;
	default:
		break;
	}

	gtk_widget_destroy(dw.dialog_window);

	return TRUE;
}

gboolean hview_invfield_remove_call(GtkWidget *button, gpointer data)
{
	HviewCallDataT		*cdata = (HviewCallDataT *) data;
	HviewInvDialogWidgetsT	*dw = (HviewInvDialogWidgetsT *)
							cdata->data;
	HviewWidgetsT		*mw = dw->parent_widgets;
	guint			sid = cdata->sessionid;
	guint			rid = cdata->parentid;
	guint			iid = cdata->entryid;
	GtkTreeSelection	*selection;
	GtkTreeModel		*model,		*area_model;
	GtkListStore		*store;
	GtkTreeIter		iter,		aiter;
	GList			*area_list,	*field_list;
	VohObjectT		*obj;
	guint			aid,		fid;
	gboolean		res;
	gchar			err[1024];
	

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(
					dw->fields_tab.areas_view));
	area_model = gtk_tree_view_get_model(GTK_TREE_VIEW(
					dw->fields_tab.areas_view));

	if (gtk_tree_selection_get_selected(selection, &model, &aiter)) {
		gtk_tree_model_get(model, &aiter, 2, &aid, -1);
	} else {
		return FALSE;
	}

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(
					dw->fields_tab.fields_view));
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(
					dw->fields_tab.fields_view));

	if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
		gtk_tree_model_get(model, &iter, 2, &fid, -1);
	} else {
		return FALSE;
	}

	res = voh_idr_field_delete(sid, rid, iid, aid, fid, err);

	if (res == FALSE) {
		hview_print(mw, err);
	}
	res = voh_get_idr_area_with_field(sid, rid, iid, &area_list, err);
	if (res != FALSE) {
		dw->general_tab.area_list = area_list;
		gtk_tree_view_set_model(GTK_TREE_VIEW(
					dw->fields_tab.fields_view), NULL);
		hview_invfield_unsel_call(GTK_TREE_VIEW(
						 dw->fields_tab.fields_view),
						(gpointer) dw);
		while (area_list != NULL) {
			obj = (VohObjectT *) area_list->data;
			if (obj->id != aid) {
				area_list = area_list->next;
				continue;
			}
			gtk_list_store_set(GTK_LIST_STORE(area_model),
					&aiter,
					1, obj->data,
					-1);

			gtk_tree_view_set_model(GTK_TREE_VIEW(
						dw->fields_tab.fields_view),
						NULL);
			field_list = obj->data;
			store = gtk_list_store_new(5, G_TYPE_STRING,
						G_TYPE_POINTER,
						G_TYPE_UINT,
						G_TYPE_UINT,
						G_TYPE_UINT);
			while (field_list != NULL) {
				obj = (VohObjectT *) field_list->data;
				gtk_list_store_append(store, &iter);
				gtk_list_store_set(store, &iter,
						0, obj->name,
						1, obj->data,
						2, obj->id,
						3, obj->state,
						4, obj->numerical,
						-1);
				field_list = field_list->next;
			}
			gtk_tree_view_set_model(GTK_TREE_VIEW(
						dw->fields_tab.fields_view),
						GTK_TREE_MODEL(store));
			break;
			}
	}

	return TRUE;
}

void hview_control_settings_ok_response(gpointer data)
{
	HviewConDialogWidgetsT	*dw = (HviewConDialogWidgetsT *) data;
	HviewWidgetsT		*w = dw->parent_widgets;
	GtkTreeModel		*model;
	GtkTreeIter		iter,		parent;
	GtkTreeSelection	*selection;
	gint			page;
	GtkWidget		*tview,	*dview;
	guint			id,		pid,	sid;
	guint			vu;
	gboolean		res;
	VtData1T		*ctrl_type,	*stated,	*moded;
	guint			vl;
	gchar			*name;
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
		if (!gtk_tree_model_iter_parent(model, &parent, &iter))
			return;
		gtk_tree_model_get(model, &parent, VOH_LIST_COLUMN_ID,
				   &pid, -1);
		gtk_tree_model_get(model, &iter, VOH_LIST_COLUMN_ID, &id, -1);
	} else {
		return;
	}

	res = voh_check_rdr_presence(sid, pid, id, err);
	if (res == FALSE) {
		hview_print(w, err);
		return;
	}

	res = voh_control_type_get(sid, pid, id, &ctrl_type, err);

	if (res == FALSE) {
		hview_print(w, err);
		return;
	}

	vl = vt_data_value_get_as_int(ctrl_type, "type");
	switch (vl) {
	case VOH_CONTROL_DIGITAL:
		stated = vt_data_new1(VT_CTRL_STATE_DIGITAL);
		model = gtk_combo_box_get_model(GTK_COMBO_BOX(
					dw->state_tab.state_widget));
		res = gtk_combo_box_get_active_iter(GTK_COMBO_BOX(
					dw->state_tab.state_widget), &iter);
		if (res == TRUE) {
			gtk_tree_model_get(model, &iter, 1, &vu, -1);
		} else {
			hview_print(w, "unknown control state to set");
			return;
		}
		vt_data_value_set1(stated, "state", vu);
		break;
	case VOH_CONTROL_DISCRETE:
		stated = vt_data_new1(VT_CTRL_STATE_DISCRETE);
		vu = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(
						dw->state_tab.state_widget));
		vt_data_value_set1(stated, "state", vu);
		break;
	case VOH_CONTROL_ANALOG:
		stated = vt_data_new1(VT_CTRL_STATE_ANALOG);
		vu = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(
						dw->state_tab.state_widget));
		vt_data_value_set1(stated, "state", vu);
		break;
	case VOH_CONTROL_STREAM:
		stated = vt_data_new1(VT_CTRL_STATE_STREAM);
		vu = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(
						dw->state_tab.flag_widget));
		vt_data_value_set1(stated, "repeat", vu);
		name = g_strdup(gtk_entry_get_text(GTK_ENTRY(
					dw->state_tab.state_widget)));
		vt_data_value_str_set1(stated, "stream", name);
		vu = strlen(name);
		g_free(name);
		vt_data_value_set1(stated, "stream_length", vu);

		break;
	case VOH_CONTROL_TEXT:
		stated = vt_data_new1(VT_CTRL_STATE_TEXT);
		vu = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(
						dw->state_tab.line_widget));
		vt_data_value_set1(stated, "line", vu);
		name = g_strdup(gtk_entry_get_text(GTK_ENTRY(
					dw->state_tab.state_widget)));
		vt_data_value_str_set1(stated, "text.data", name);
		vu = strlen(name);
		g_free(name);
		vt_data_value_set1(stated, "text.data_length", vu);
		break;
	case VOH_CONTROL_OEM:
		stated = vt_data_new1(VT_CTRL_STATE_OEM);
		name = g_strdup(gtk_entry_get_text(GTK_ENTRY(
					dw->state_tab.state_widget)));
		vt_data_value_str_set1(stated, "body", name);
		vu = strlen(name);
		g_free(name);
		vt_data_value_set1(stated, "body_length", vu);
		break;
	default:
		return;
	}

	model = gtk_combo_box_get_model(GTK_COMBO_BOX(
					dw->state_tab.mode_widget));
	res = gtk_combo_box_get_active_iter(GTK_COMBO_BOX(
					dw->state_tab.mode_widget), &iter);
	if (res == TRUE) {
		gtk_tree_model_get(model, &iter, 1, &vu, -1);
	} else {
		hview_print(w, "unknown control mode to set");
		return;
	}

	moded = vt_data_new1(VT_CTRL_MODE);
	vt_data_value_set1(moded, "mode", vu);

	res = voh_control_set(sid, pid, id, moded, stated, err);

	if (res == FALSE) {
		hview_print(w, err);
		return;
	}

}

void hview_control_settings_call(GtkWidget *widget, gpointer data)
{
	HviewWidgetsT		*w = (HviewWidgetsT *) data;
	HviewConDialogWidgetsT	dw;
	GtkWidget		*dialog_window;
	GtkTreeModel		*model;
	GtkTreeIter		iter,		parent;
	GtkTreeSelection	*selection;
	gint			page;
	GtkWidget		*tview;
	GtkWidget		*hbox,		*flhbox,	*lnhbox;
	GtkWidget		*label;
	GtkWidget		*statew,	*flw,		*linew,	*modew;
	GtkCellRenderer		*renderer;
	GtkListStore		*store;
	guint			id,		pid,	sid;
	GtkListStore		*area_model;
	VtData1T		*info_data,	*state,	*mode;
	HviewCallDataT		*call_data;
	guint			ctrl_type;
	gint			res;
	gdouble			vl;
	guint			vl1;
	GList			*var_val_l;
	VtVarValT		*var_val;
	gchar			err[1024];

	statew = flw = linew = modew = NULL;
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

	if (!gtk_tree_model_iter_parent(model, &parent, &iter))
		return;

	gtk_tree_model_get(model, &parent, VOH_LIST_COLUMN_ID, &pid, -1);
	gtk_tree_model_get(model, &iter, VOH_LIST_COLUMN_ID, &id, -1);

	call_data = (HviewCallDataT *) g_malloc(sizeof(HviewCallDataT));
	call_data->sessionid = sid;
	call_data->parentid = pid;
	call_data->entryid = id;

	res = voh_check_rdr_presence(sid, pid, id, err);
	if (res == FALSE) {
		dialog_window = hview_get_rpt_empty_dialog_window(w);
		hview_print(w, err);
	} else {
		dw.parent_widgets = w;
		dialog_window = hview_get_control_settings_window(&dw);
		dw.dialog_window = dialog_window;
		call_data->data = (gpointer) &dw;

		res = voh_get_control_info(sid, pid, id, &info_data, err);
		if (res == FALSE) {
			hview_print(w, err);
			label = gtk_label_new("UNKNOWN");
			gtk_box_pack_start(GTK_BOX(dw.general_tab.info_box),
					   label, TRUE, TRUE, 0);
			gtk_widget_show_all(dialog_window);
		} else {
			var_val_l = vt_get_var_val_list(info_data);
			while (var_val_l != NULL) {
				var_val = (VtVarValT *) var_val_l->data;
				hbox = gtk_hbox_new(TRUE, 5);
				gtk_box_pack_start(GTK_BOX(
						dw.general_tab.info_box), hbox,
						FALSE, FALSE, 5);
				label = gtk_label_new(var_val->var);
				gtk_box_pack_start(GTK_BOX(hbox), label,
						   TRUE, TRUE, 10);
				label = gtk_label_new(var_val->val);
				gtk_box_pack_start(GTK_BOX(hbox), label,
						   TRUE, TRUE, 1);

				var_val_l = var_val_l->next;
			}
			ctrl_type = vt_data_value_get_as_int(info_data, "type");
			vt_data_destroy1(info_data);
		}

		hbox = gtk_hbox_new(TRUE, 5);
		gtk_box_pack_start(GTK_BOX(dw.state_tab.state_box),
						hbox, FALSE, FALSE, 5);
		label = gtk_label_new("Control current state");
		gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 10);
		
		res = voh_control_get(sid, pid, id, &mode, &state, err);
		if (res == FALSE) {
			hview_print(w, err);
			label = gtk_label_new("UNKNOWN");
			gtk_box_pack_start(GTK_BOX(hbox),
					   label, TRUE, TRUE, 0);
			gtk_widget_show_all(dialog_window);
		} else {
			switch (ctrl_type) {
			case VOH_CONTROL_DIGITAL:
				store = voh_get_ctrl_digital_state_list();
				statew = gtk_combo_box_new_with_model(
						GTK_TREE_MODEL(store));
				renderer = gtk_cell_renderer_text_new();
				gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(
						statew), renderer, TRUE);
				gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(
						statew), renderer, "text",
						0, NULL);
			
				gtk_box_pack_start(GTK_BOX(hbox), statew,
								TRUE, TRUE, 1);
				vl = vt_data_value_get_as_int(state, "state");
				if (hutil_find_iter_by_id(GTK_TREE_MODEL(store),
					1, vl, 0, &iter, HUTIL_FIRST_ITER) ==
									TRUE) {
					gtk_combo_box_set_active_iter(
						GTK_COMBO_BOX(statew), &iter);
				}
				break;
			case VOH_CONTROL_DISCRETE:
				vl = vt_data_value_get_as_double(state,
								"state");
				statew = gtk_spin_button_new_with_range(0,
							G_MAXUINT, 1);
				gtk_box_pack_start(GTK_BOX(hbox), statew,
								TRUE, TRUE, 1);
				gtk_spin_button_set_value(
						GTK_SPIN_BUTTON(statew), vl);
				break;
			case VOH_CONTROL_ANALOG:
				vl = vt_data_value_get_as_double(state,
								"state");
				statew = gtk_spin_button_new_with_range(
						0 - G_MAXINT, G_MAXINT, 1);
				gtk_box_pack_start(GTK_BOX(hbox), statew,
								TRUE, TRUE, 1);
				gtk_spin_button_set_value(
						GTK_SPIN_BUTTON(statew), vl);
				break;
			case VOH_CONTROL_STREAM:
				statew = gtk_entry_new();
				gtk_entry_set_max_length(GTK_ENTRY(statew), 4);
				gtk_box_pack_start(GTK_BOX(hbox), statew,
								TRUE, TRUE, 1);
				gtk_entry_set_text(GTK_ENTRY(statew),
					vt_data_value_str_get1(state, "state"));

				label = gtk_label_new("Repeat flag");
				flw = gtk_check_button_new_with_label(
						vt_convert_boolean(FALSE));
				g_signal_connect(G_OBJECT(flw), "toggled",
					G_CALLBACK(
						hview_toggled_true_false_call),
							NULL);
				vl = vt_data_value_get_as_int(state, "repeat");
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
							flw), vl);

				flhbox = gtk_hbox_new(TRUE, 5);
				gtk_box_pack_start(GTK_BOX(
							dw.state_tab.state_box),
						flhbox, FALSE, FALSE, 5);
				gtk_box_pack_start(GTK_BOX(flhbox), label,
						TRUE, TRUE, 10);
				gtk_box_pack_start(GTK_BOX(flhbox), flw,
								TRUE, TRUE, 1);
				break;
			case VOH_CONTROL_TEXT:
				statew = gtk_entry_new();
				gtk_box_pack_start(GTK_BOX(hbox), statew,
								TRUE, TRUE, 1);
				gtk_entry_set_text(GTK_ENTRY(statew),
					vt_data_value_str_get1(state, "state"));

				linew = gtk_spin_button_new_with_range(
						0, G_MAXUINT8, 1);
				vl = vt_data_value_get_as_double(state, "line");
				gtk_spin_button_set_value(
						GTK_SPIN_BUTTON(linew), vl);
				label = gtk_label_new("Control current line");

				lnhbox = gtk_hbox_new(TRUE, 5);
				gtk_box_pack_start(GTK_BOX(
							dw.state_tab.state_box),
						lnhbox, FALSE, FALSE, 5);
				gtk_box_pack_start(GTK_BOX(lnhbox), label,
						TRUE, TRUE, 10);
				gtk_box_pack_start(GTK_BOX(lnhbox), linew,
								TRUE, TRUE, 1);
				break;
			case VOH_CONTROL_OEM:
				statew = gtk_entry_new();
				gtk_entry_set_max_length(GTK_ENTRY(statew),
									255);
				gtk_box_pack_start(GTK_BOX(hbox), statew,
								TRUE, TRUE, 1);
				gtk_entry_set_text(GTK_ENTRY(statew),
					vt_data_value_str_get1(state, "state"));
			break;
			default:
				gtk_container_remove(GTK_CONTAINER(
						dw.state_tab.state_box), hbox);
				var_val_l = vt_get_var_val_list(state);
				while (var_val_l != NULL) {
					var_val = (VtVarValT *) var_val_l->data;
					hbox = gtk_hbox_new(TRUE, 5);
					gtk_box_pack_start(GTK_BOX(
						dw.state_tab.state_box), hbox,
						FALSE, FALSE, 5);
					label = gtk_label_new(var_val->var);
					gtk_box_pack_start(GTK_BOX(hbox), label,
						   TRUE, TRUE, 10);
					label = gtk_label_new(var_val->val);
					gtk_box_pack_start(GTK_BOX(hbox), label,
						   TRUE, TRUE, 1);

					var_val_l = var_val_l->next;
				}
				var_val_l = vt_get_var_val_list(mode);
				while (var_val_l != NULL) {
					var_val = (VtVarValT *) var_val_l->data;
					hbox = gtk_hbox_new(TRUE, 5);
					gtk_box_pack_start(GTK_BOX(
						dw.state_tab.state_box), hbox,
						FALSE, FALSE, 5);
					label = gtk_label_new(var_val->var);
					gtk_box_pack_start(GTK_BOX(hbox), label,
						   TRUE, TRUE, 10);
					label = gtk_label_new(var_val->val);
					gtk_box_pack_start(GTK_BOX(hbox), label,
							   TRUE, TRUE, 1);

					var_val_l = var_val_l->next;
				}
				vt_data_destroy1(state);
				vt_data_destroy1(mode);
				gtk_widget_show_all(dialog_window);

				res = gtk_dialog_run(GTK_DIALOG(dialog_window));
				gtk_widget_destroy(dialog_window);
				return;
			}
			hbox = gtk_hbox_new(TRUE, 5);
			gtk_box_pack_start(GTK_BOX(dw.state_tab.state_box),
					hbox, FALSE, FALSE, 5);
			label = gtk_label_new("Control current mode");
			gtk_box_pack_start(GTK_BOX(hbox), label,
							TRUE, TRUE, 10);
			store = voh_get_ctrl_mode_list();
			modew = gtk_combo_box_new_with_model(
						GTK_TREE_MODEL(store));
			renderer = gtk_cell_renderer_text_new();
			gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(
						modew), renderer, TRUE);
			gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(
						modew), renderer, "text",
						0, NULL);
			gtk_box_pack_start(GTK_BOX(hbox), modew, TRUE, TRUE, 1);
			vl = vt_data_value_get_as_int(mode, "mode");
			if (hutil_find_iter_by_id(GTK_TREE_MODEL(store),
				1, vl, 0, &iter, HUTIL_FIRST_ITER) == TRUE) {
				gtk_combo_box_set_active_iter(
						GTK_COMBO_BOX(modew), &iter);
			}
			vt_data_destroy1(state);
			vt_data_destroy1(mode);
		}
	}

	dw.state_tab.state_widget = statew;
	dw.state_tab.line_widget = linew;
	dw.state_tab.flag_widget = flw;
	dw.state_tab.mode_widget = modew;

	gtk_widget_show_all(dialog_window);

	res = gtk_dialog_run(GTK_DIALOG(dialog_window));
	switch (res) {
	case GTK_RESPONSE_OK:
		hview_control_settings_ok_response((gpointer) &dw);
		break;
	default:
		break;
	}

	gtk_widget_destroy(dialog_window);
}


void hview_watchdog_settings_ok_response(gpointer data)
{
	HviewWatchDialogWidgetsT	*dw = (HviewWatchDialogWidgetsT *)
									data;
	HviewWidgetsT			*w = dw->parent_widgets;
	GtkTreeModel			*model;
	GtkTreeIter			iter,		parent;
	GtkTreeSelection		*selection;
	gint				page;
	GtkWidget			*tview,	*dview;
	guint				id,		pid,	sid;
	gboolean			res;
	guint				vu;
	VtData1T			*watchdog_data;
	VtValNameMapT			*val_name;
	GList				*flags;
	gchar				err[1024];

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
		if (!gtk_tree_model_iter_parent(model, &parent, &iter))
			return;
		gtk_tree_model_get(model, &parent, VOH_LIST_COLUMN_ID,
				   &pid, -1);
		gtk_tree_model_get(model, &iter, VOH_LIST_COLUMN_ID, &id, -1);
	} else {
		return;
	}

	res = voh_check_rdr_presence(sid, pid, id, err);
	if (res == FALSE) {
		hview_print(w, err);
		return;
	}

	watchdog_data = vt_data_new1(VT_WATCHDOG);
	vt_data_value_set1(watchdog_data, "log",
				gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(
						dw->general_tab.log)));
	vt_data_value_set1(watchdog_data, "running",
				gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(
						dw->general_tab.running)));

	model = gtk_combo_box_get_model(GTK_COMBO_BOX(
					dw->general_tab.timer_use));
	res = gtk_combo_box_get_active_iter(GTK_COMBO_BOX(
					dw->general_tab.timer_use), &iter);
	if (res == TRUE) {
		gtk_tree_model_get(model, &iter, 1, &vu, -1);
	} else {
		hview_print(w, "unknown watchdog \"timer use\" to set");
		return;
	}
	vt_data_value_set1(watchdog_data, "timer_use", vu);

	model = gtk_combo_box_get_model(GTK_COMBO_BOX(
					dw->general_tab.timer_action));
	res = gtk_combo_box_get_active_iter(GTK_COMBO_BOX(
					dw->general_tab.timer_action), &iter);
	if (res == TRUE) {
		gtk_tree_model_get(model, &iter, 1, &vu, -1);
	} else {
		hview_print(w, "unknown watchdog \"timer action\" to set");
		return;
	}
	vt_data_value_set1(watchdog_data, "timer_action", vu);

	model = gtk_combo_box_get_model(GTK_COMBO_BOX(
					dw->general_tab.pretimer_inter));
	res = gtk_combo_box_get_active_iter(GTK_COMBO_BOX(
					dw->general_tab.pretimer_inter), &iter);
	if (res == TRUE) {
		gtk_tree_model_get(model, &iter, 1, &vu, -1);
	} else {
		hview_print(w, "unknown watchdog (pretimer interrupt) to set");
		return;
	}
	vt_data_value_set1(watchdog_data, "pretimer_interrupt", vu);

	vu = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(
						dw->general_tab.pretimeout));
	vt_data_value_set1(watchdog_data, "pre_timeout_interval", vu);

	vu = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(
						dw->general_tab.initial_count));
	vt_data_value_set1(watchdog_data, "initial_count", vu);

	vu = 0;
	flags = dw->flags_tab.flags;
	while (flags != NULL) {
		val_name = (VtValNameMapT *) flags->data;
		res = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(
							val_name->data));
		if (res == TRUE) {
			vu |= (guint) val_name->m_value;
		}
		flags = flags->next;
	}
	vt_data_value_set1(watchdog_data, "timer_use_exp_flags", vu);

	res = voh_watchdog_timer_set(sid, pid, id, watchdog_data, err);

	vt_data_destroy1(watchdog_data);

	if (res == FALSE) {
		hview_print(w, err);
		return;
	}

}

void hview_watchdog_settings_call(GtkWidget *widget, gpointer data)
{
	HviewWidgetsT			*w = (HviewWidgetsT *) data;
	HviewWatchDialogWidgetsT	dw;
	GtkWidget			*dialog_window;
	GtkTreeModel			*model;
	GtkTreeIter			iter,		parent;
	GtkTreeSelection		*selection;
	gint				page;
	GtkCellRenderer			*renderer;
	GtkListStore			*store;
	GtkWidget			*tview;
	GtkWidget			*hbox,		*hbox_add;
	GtkWidget			*label;
	guint				id,		pid,	sid;
	VtData1T			*watchdog_data;
	HviewCallDataT			*call_data;
	GList				*flags;
	VtValNameMapT			*val_name;
	guint				vu;
	gdouble				vl;
	gint				res;
	gchar				err[1024];

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

	if (!gtk_tree_model_iter_parent(model, &parent, &iter))
		return;

	gtk_tree_model_get(model, &parent, VOH_LIST_COLUMN_ID, &pid, -1);
	gtk_tree_model_get(model, &iter, VOH_LIST_COLUMN_ID, &id, -1);

	call_data = (HviewCallDataT *) g_malloc(sizeof(HviewCallDataT));
	call_data->sessionid = sid;
	call_data->parentid = pid;
	call_data->entryid = id;

	res = voh_check_rdr_presence(sid, pid, id, err);
	if (res == FALSE) {
		dialog_window = hview_get_rpt_empty_dialog_window(w);
		hview_print(w, err);
		gtk_widget_show_all(dialog_window);
		res = gtk_dialog_run(GTK_DIALOG(dialog_window));
		gtk_widget_destroy(dialog_window);
		return;
	}

	dw.parent_widgets = w;
	dialog_window = hview_get_watchdog_settings_window(&dw);
	dw.dialog_window = dialog_window;
	call_data->data = (gpointer) &dw;

	res = voh_watchdog_timer_get(sid, pid, id, &watchdog_data, err);

	if (res == FALSE) {
		hview_print(w, err);
		label = gtk_label_new("UNKNOWN");
		gtk_box_pack_start(GTK_BOX(dw.general_tab.info_box),
					   	label, TRUE, TRUE, 0);
		gtk_widget_show_all(dialog_window);
		res = gtk_dialog_run(GTK_DIALOG(dialog_window));
		gtk_widget_destroy(dialog_window);
		return;
	}

	label = gtk_label_new("Watchdog log");
	dw.general_tab.log = gtk_check_button_new_with_label(
					vt_convert_boolean(FALSE));
	g_signal_connect(G_OBJECT(dw.general_tab.log), "toggled",
			G_CALLBACK(hview_toggled_true_false_call), NULL);
	vu = vt_data_value_get_as_int(watchdog_data, "log");

	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(dw.general_tab.log),
							(gboolean) vu);

	hbox = gtk_hbox_new(TRUE, 5);
	gtk_box_pack_start(GTK_BOX(dw.general_tab.info_box), hbox,
							FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 10);
	gtk_box_pack_start(GTK_BOX(hbox), dw.general_tab.log, TRUE, TRUE, 1);

	label = gtk_label_new("Watchdog running");
	dw.general_tab.running = gtk_check_button_new_with_label(
					vt_convert_boolean(FALSE));
	g_signal_connect(G_OBJECT(dw.general_tab.running), "toggled",
			G_CALLBACK(hview_toggled_true_false_call), NULL);
	vu = vt_data_value_get_as_int(watchdog_data, "running");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(dw.general_tab.running),
									vu);	
	hbox = gtk_hbox_new(TRUE, 5);
	gtk_box_pack_start(GTK_BOX(dw.general_tab.info_box), hbox,
							FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 10);
	gtk_box_pack_start(GTK_BOX(hbox), dw.general_tab.running,
								TRUE, TRUE, 1);

	hbox = gtk_hbox_new(TRUE, 5);
	gtk_box_pack_start(GTK_BOX(dw.general_tab.info_box), hbox, FALSE,
								FALSE, 5);
	label = gtk_label_new("Timer use");
	gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 10);
	store = voh_get_watchdog_timer_use_list();
	dw.general_tab.timer_use = gtk_combo_box_new_with_model(
						GTK_TREE_MODEL(store));
	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(dw.general_tab.timer_use),
					renderer, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(
					dw.general_tab.timer_use),
						renderer, "text", 0, NULL);
	gtk_box_pack_start(GTK_BOX(hbox), dw.general_tab.timer_use, TRUE,
								TRUE, 1);
	vu = vt_data_value_get_as_int(watchdog_data, "timer_use");
	if (hutil_find_iter_by_id(GTK_TREE_MODEL(store),
				1, vu, 0, &iter, HUTIL_FIRST_ITER) == TRUE) {
		gtk_combo_box_set_active_iter(GTK_COMBO_BOX(
						dw.general_tab.timer_use),
									&iter);
	}

	hbox = gtk_hbox_new(TRUE, 5);
	gtk_box_pack_start(GTK_BOX(dw.general_tab.info_box), hbox, FALSE,
								FALSE, 5);
	label = gtk_label_new("Timer action");
	gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 10);
	store = voh_get_watchdog_action_list();
	dw.general_tab.timer_action = gtk_combo_box_new_with_model(
						GTK_TREE_MODEL(store));
	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(dw.general_tab.timer_action),
					renderer, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(
					dw.general_tab.timer_action),
						renderer, "text", 0, NULL);
	gtk_box_pack_start(GTK_BOX(hbox), dw.general_tab.timer_action, TRUE,
								TRUE, 1);
	vu = vt_data_value_get_as_int(watchdog_data, "timer_action");
	if (hutil_find_iter_by_id(GTK_TREE_MODEL(store),
				1, vu, 0, &iter, HUTIL_FIRST_ITER) == TRUE) {
		gtk_combo_box_set_active_iter(GTK_COMBO_BOX(
						dw.general_tab.timer_action),
									&iter);
	}

	hbox = gtk_hbox_new(TRUE, 5);
	gtk_box_pack_start(GTK_BOX(dw.general_tab.info_box), hbox, FALSE,
								FALSE, 5);
	label = gtk_label_new("Pretimer interrupt");
	gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 10);
	store = voh_get_watchdog_pretimer_interrupt_list();
	dw.general_tab.pretimer_inter = gtk_combo_box_new_with_model(
						GTK_TREE_MODEL(store));
	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(
					dw.general_tab.pretimer_inter),
						renderer, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(
					dw.general_tab.pretimer_inter),
						renderer, "text", 0, NULL);
	gtk_box_pack_start(GTK_BOX(hbox), dw.general_tab.pretimer_inter, TRUE,
								TRUE, 1);
	vu = vt_data_value_get_as_int(watchdog_data, "pretimer_interrupt");
	if (hutil_find_iter_by_id(GTK_TREE_MODEL(store),
				1, vu, 0, &iter, HUTIL_FIRST_ITER) == TRUE) {
		gtk_combo_box_set_active_iter(GTK_COMBO_BOX(
						dw.general_tab.pretimer_inter),
									&iter);
	}

	dw.general_tab.pretimeout = gtk_spin_button_new_with_range(
						0, G_MAXUINT32, 1);
	vl = vt_data_value_get_as_double(watchdog_data, "pre_timeout_interval");
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(dw.general_tab.pretimeout),
									vl);
	label = gtk_label_new("Pretimeout interval");
	hbox = gtk_hbox_new(TRUE, 5);
	gtk_box_pack_start(GTK_BOX(dw.general_tab.info_box), hbox,
							FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 10);
	hbox_add = gtk_hbox_new(TRUE, 2);
	gtk_box_pack_start(GTK_BOX(hbox), hbox_add, TRUE, TRUE, 1);
	gtk_box_pack_start(GTK_BOX(hbox_add), dw.general_tab.pretimeout,
							TRUE, TRUE, 1);
	label = gtk_label_new("(ms)");
	gtk_box_pack_start(GTK_BOX(hbox_add), label, TRUE, TRUE, 1);

	dw.general_tab.initial_count = gtk_spin_button_new_with_range(
						0, G_MAXUINT32, 1);
	vl = vt_data_value_get_as_double(watchdog_data, "initial_count");
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(dw.general_tab.initial_count),
									vl);
	label = gtk_label_new("Initial counter");
	hbox = gtk_hbox_new(TRUE, 5);
	gtk_box_pack_start(GTK_BOX(dw.general_tab.info_box), hbox,
							FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 10);
	hbox_add = gtk_hbox_new(TRUE, 2);
	gtk_box_pack_start(GTK_BOX(hbox), hbox_add, TRUE, TRUE, 1);
	gtk_box_pack_start(GTK_BOX(hbox_add), dw.general_tab.initial_count,
							TRUE, TRUE, 1);
	label = gtk_label_new("(ms)");
	gtk_box_pack_start(GTK_BOX(hbox_add), label, TRUE, TRUE, 1);

	hbox = gtk_hbox_new(TRUE, 5);
	gtk_box_pack_start(GTK_BOX(dw.general_tab.info_box), hbox,
							FALSE, FALSE, 5);
	label = gtk_label_new("Present counter");
	gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 10);
	label = gtk_label_new(vt_data_value_str_get1(watchdog_data,
						"present_count"));
	gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 1);

	flags = voh_get_watchdog_exp_flags_list();
	dw.flags_tab.flags = flags;
	vu = vt_data_value_get_as_int(watchdog_data, "timer_use_exp_flags");
	while (flags != NULL) {
		val_name = (VtValNameMapT *) flags->data;
		val_name->data = (gpointer) gtk_check_button_new_with_label(
							val_name->m_name);
		res = vu & (guint) val_name->m_value;
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(val_name->data),
									res);
		gtk_box_pack_start(GTK_BOX(dw.flags_tab.flags_box),
					GTK_WIDGET(val_name->data),
						TRUE, TRUE, 5);
		flags = flags->next;
	}

	vt_data_destroy1(watchdog_data);

	gtk_widget_show_all(dialog_window);
	res = gtk_dialog_run(GTK_DIALOG(dialog_window));
	switch (res) {
	case GTK_RESPONSE_OK:
		hview_watchdog_settings_ok_response((gpointer) &dw);
		break;
	default:
		break;
	}

	gtk_widget_destroy(dialog_window);
}

gint hview_watchdog_reset_thread(gpointer data)
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
	    hview_statusbar_push(w, "watchdog reset failed");
	    gtk_widget_set_sensitive(w->main_window, TRUE);
	    return FALSE;
      }
      gtk_tree_model_get(store, &iter, VOH_LIST_COLUMN_ID, &id, -1);
      gtk_tree_model_get(store, &parent, VOH_LIST_COLUMN_ID, &pid, -1);

      ret = voh_watchdog_timer_reset(sid, pid, id, err);
      if (ret == FALSE) {
	    hview_print(w, err);
	    hview_statusbar_push(w, "watchdog reset failed");
      } else {
	    hview_statusbar_push(w, "ready");
      }

      gtk_widget_set_sensitive(w->main_window, TRUE);

      return FALSE;
}

void hview_watchdog_reset_call(GtkWidget *widget, gpointer data)
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
	    if (type == VOH_ITER_IS_WATCHDOG) {
		  hview_statusbar_push(w, "watchdog reset (please wait)");
		  gtk_widget_set_sensitive(w->main_window, FALSE);
		  gtk_timeout_add(100, hview_watchdog_reset_thread, data);
	    }
      } else {
	    hview_print(w, "select watchdog please");
      }

}

void hview_domain_settings_ok_response(gpointer data)
{
	HviewDomainDialogWidgetsT	*dw = (HviewDomainDialogWidgetsT *)
									data;
	HviewWidgetsT			*w = dw->parent_widgets;
	GtkTreeModel			*model;
	GtkTreeModel			*sevlist;
	GtkTreeIter			iter;
	GtkTreeSelection		*selection;
	gint				page,	num_pages;
	GtkWidget			*tview,	*dview;
	guint				id,	sid,	severity;
	gboolean			res;
	gint				i;
	VtData1T			*buffer_data;
	gchar				*name;
	gchar				err[1024];

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(w->domain_view));
	res = gtk_tree_selection_get_selected(selection, &model, &iter);
	if (res == FALSE) {
		return;
	}

	gtk_tree_model_get(model, &iter, VOH_LIST_COLUMN_ID, &id, -1);

	name = g_strdup(gtk_entry_get_text(GTK_ENTRY(
						dw->general_tab.tag_entry)));
	buffer_data = vt_data_new1(VT_TEXT_BUFFER1);
	vt_data_value_str_set1(buffer_data, "data", name);
	
	res = voh_domain_tag_set(id, buffer_data, err);
	if (res == FALSE) {
		hview_print(w, err);
	} else {
		gtk_tree_store_set(GTK_TREE_STORE(model), &iter,
					VOH_LIST_COLUMN_NAME,
					name,
					-1);
		num_pages = gtk_notebook_get_n_pages(GTK_NOTEBOOK(
							w->tab_windows));

		res = FALSE;
		for (i = 0; i < num_pages; i++) {
			if (w->tab_views[i].sessionid == id) {
				res = TRUE;
				page = i;
				break;
			}
		}
		if (res == TRUE) {
			gtk_notebook_set_tab_label_text(GTK_NOTEBOOK(
								w->tab_windows),
					gtk_notebook_get_nth_page(GTK_NOTEBOOK(
								w->tab_windows),
						page), name);
		}
      }

	if (name)
		g_free(name);
	vt_data_destroy1(buffer_data);

}

void hview_domain_settings_call(GtkWidget *widget, gpointer data)
{
	HviewWidgetsT			*w = (HviewWidgetsT *) data;
	HviewDomainDialogWidgetsT	dw;
	GtkWidget			*dialog_window;
	GtkTreeModel			*model;
	GtkTreeIter			iter;
	GtkTreeSelection		*selection;
	GtkWidget			*hbox;
	GtkWidget			*label;
	GtkTreeStore			*store;
	VtData1T			*domain_info,	*alarm_data;
	GList				*var_val_l,	*alarm_list;
	VtVarValT			*var_val;
	guint				id, sid;
	gint				res;
	gchar				name[1024];
	gchar				err[1024];

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(w->domain_view));
	res = gtk_tree_selection_get_selected(selection, &model, &iter);
	if (res == FALSE) {
		return;
	}

	gtk_tree_model_get(model, &iter, VOH_LIST_COLUMN_ID, &id, -1);

	dw.parent_widgets = w;
	dialog_window = hview_get_domain_settings_window(&dw);
	dw.dialog_window = dialog_window;

	res = voh_domain_info_get(id, &domain_info, err);

	if (res == FALSE) {
		label = gtk_label_new("UNKNOWN");
		gtk_box_pack_start(GTK_BOX(dw.general_tab.info_box), label,
								TRUE, TRUE, 0);
		gtk_widget_show_all(dialog_window);
		res = gtk_dialog_run(GTK_DIALOG(dialog_window));
		gtk_widget_destroy(dialog_window);
		return;
	}
 
	var_val_l = vt_get_var_val_list(domain_info);
	while (var_val_l != NULL) {
		var_val = (VtVarValT *) var_val_l->data;
		if (strcmp(var_val->var, "Domain tag") == 0) {
			var_val_l = var_val_l->next;
			continue;
		}
		hbox = gtk_hbox_new(TRUE, 5);
		gtk_box_pack_start(GTK_BOX(dw.general_tab.info_box), hbox,
							FALSE, FALSE, 3);
		label = gtk_label_new(var_val->var);
		gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 10);
		label = gtk_label_new(var_val->val);
		gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 1);
		var_val_l = var_val_l->next;
	}
	gtk_entry_set_text(GTK_ENTRY(dw.general_tab.tag_entry),
				vt_data_value_str_get1(domain_info,
							"domain_tag"));

	alarm_list = voh_get_all_alarms(id, err);

	store = gtk_tree_store_new(VOH_LIST_NUM_COL, G_TYPE_STRING,
                                 GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_UINT,
                                 G_TYPE_UINT, G_TYPE_UINT, G_TYPE_UINT);

	while (alarm_list != NULL) {
		alarm_data = (VtData1T *) alarm_list->data;
		gtk_tree_store_append(store, &iter, NULL);
		id = vt_data_value_get_as_int(alarm_data, "alarm_id");
		sprintf(name, "alarm #%d", id);
		gtk_tree_store_set(store, &iter,
				VOH_LIST_COLUMN_NAME, name,
				VOH_LIST_COLUMN_ID, id,
				-1);
		alarm_list = alarm_list->next;
	}

	gtk_tree_view_set_model(GTK_TREE_VIEW(dw.dat_tab.alarms_view),
				GTK_TREE_MODEL(store));
	g_object_unref(store);

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(
						dw.dat_tab.alarms_view));
//	g_signal_connect(G_OBJECT(dw.evlog_tab.evlog_view), "unselect-all",
//			G_CALLBACK(hview_domain_evlog_unsel_call),
//			(gpointer) &dw);
	g_signal_connect(G_OBJECT(selection), "changed",
			G_CALLBACK(hview_domain_alarm_selected_call),
			(gpointer) &dw);

	gtk_widget_show_all(dialog_window);
	res = gtk_dialog_run(GTK_DIALOG(dialog_window));
	switch (res) {
	case GTK_RESPONSE_OK:
		hview_domain_settings_ok_response((gpointer) &dw);
		break;
	default:
		break;
	}

	gtk_widget_destroy(dialog_window);
	
}

void hview_domain_evlog_time_ok_response(gpointer data)
{
	HviewDomainEvLogTSWidgetsT	*dw = (HviewDomainEvLogTSWidgetsT *)
									data;
	HviewWidgetsT			*w = dw->parent_widgets;
	GtkTreeModel			*model;
	GtkTreeIter			iter;
	GtkTreeSelection		*selection;
	guint				id;
	guint				year,	month,	day;
	guint				hour,	min,	sec;
	gint				res;
	gdouble				time;
	gchar				err[1024];

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(w->domain_view));
	res = gtk_tree_selection_get_selected(selection, &model, &iter);
	if (res == FALSE) {
		return;
	}

	gtk_tree_model_get(model, &iter, VOH_LIST_COLUMN_ID, &id, -1);

	gtk_calendar_get_date(GTK_CALENDAR(dw->calendar), &year, &month, &day);
	hour = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(dw->hour));
	min = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(dw->min));
	sec = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(dw->sec));
	time = vt_dmyhms_to_time(year, month, day, hour, min, sec);

	res = voh_event_log_time_set(id, -1, time, err);

	if (res == FALSE) {
		hview_print(w, err);
	}
}

void hview_domain_evlog_time_call(GtkWidget *widget, gpointer data)
{
	HviewWidgetsT			*w = (HviewWidgetsT *) data;
	HviewDomainEvLogTSWidgetsT	dw;
	GtkWidget			*dialog_window;
	GtkTreeModel			*model;
	GtkTreeIter			iter;
	GtkTreeSelection		*selection;
	GtkWidget			*hbox;
	GtkWidget			*label;
	VtData1T			*domain_info;
	GList				*var_val_l,	*alarm_list;
	VtVarValT			*var_val;
	guint				id, sid;
	guint				year,	month,	day;
	guint				hour,	min,	sec;
	gint				res;
	gfloat				time;
	gchar				err[1024];

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(w->domain_view));
	res = gtk_tree_selection_get_selected(selection, &model, &iter);
	if (res == FALSE) {
		return;
	}

	gtk_tree_model_get(model, &iter, VOH_LIST_COLUMN_ID, &id, -1);

	dw.parent_widgets = w;
	dialog_window = hview_get_domain_evlogtime_window(&dw);
	dw.dialog_window = dialog_window;

	label = gtk_label_new("Current timestamp");
	gtk_box_pack_start(GTK_BOX(dw.time_box), label, TRUE, TRUE, 5);

	res = voh_event_log_time_get(id, -1, &time, err);
	if(res == FALSE) {
		hview_print(w, err);
		label = gtk_label_new("unknown");
	} else {
		label = gtk_label_new(vt_convert_time(time));
		res = vt_get_dmyhms(time, &year, &month, &day, &hour,
								&min, &sec);
		if (res == TRUE) {
			gtk_spin_button_set_value(GTK_SPIN_BUTTON(dw.hour),
									hour);
			gtk_spin_button_set_value(GTK_SPIN_BUTTON(dw.min),
									min);
			gtk_spin_button_set_value(GTK_SPIN_BUTTON(dw.sec),
									sec);
			gtk_calendar_select_month(GTK_CALENDAR(dw.calendar),
							month, year);
			gtk_calendar_select_day(GTK_CALENDAR(dw.calendar),
							day);
		}
	}
	gtk_box_pack_start(GTK_BOX(dw.time_box), label, TRUE, TRUE, 5);

	gtk_widget_show_all(dialog_window);
	res = gtk_dialog_run(GTK_DIALOG(dialog_window));
	switch (res) {
	case GTK_RESPONSE_OK:
		hview_domain_evlog_time_ok_response((gpointer) &dw);
		break;
	default:
		break;
	}

	gtk_widget_destroy(dialog_window);
	
}

void hview_domain_evlog_ok_response(gpointer data)
{
	HviewDomainEvLogWidgetsT	*dw = (HviewDomainEvLogWidgetsT *)
									data;
	HviewWidgetsT			*w = dw->parent_widgets;
	GtkTreeModel			*model;
	GtkTreeIter			iter;
	GtkTreeSelection		*selection;
	guint				id;
	gint				res;
	gchar				err[1024];

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(w->domain_view));
	res = gtk_tree_selection_get_selected(selection, &model, &iter);
	if (res == FALSE) {
		return;
	}

	gtk_tree_model_get(model, &iter, VOH_LIST_COLUMN_ID, &id, -1);

	res = voh_event_log_state_set(id, -1,
			gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(
						dw->general_tab.enable_state)),
			err);
	if (res == FALSE) {
		hview_print(w, err);
	}

}

void hview_domain_evlog_call(GtkWidget *widget, gpointer data)
{
	HviewWidgetsT			*w = (HviewWidgetsT *) data;
	HviewDomainEvLogWidgetsT	dw;
	GtkWidget			*dialog_window;
	GtkTreeModel			*model;
	GtkTreeStore			*store;
	GtkTreeIter			iter;
	GtkTreeSelection		*selection;
	GtkWidget			*hbox;
	GtkWidget			*label;
	VtData1T			*evlog_data;
	GList				*evlog_list;
	GList				*var_val_l;
	VtVarValT			*var_val;
	guint				id;
	gint				res;
	gchar				name[1024];
	gchar				err[1024];

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(w->domain_view));
	res = gtk_tree_selection_get_selected(selection, &model, &iter);
	if (res == FALSE) {
		return;
	}

	gtk_tree_model_get(model, &iter, VOH_LIST_COLUMN_ID, &id, -1);

	dw.parent_widgets = w;
	dialog_window = hview_get_domain_evlog_window(&dw);
	dw.dialog_window = dialog_window;

	res = voh_event_log_info_get(id, -1, &evlog_data, err);
	if (res == FALSE) {
		label = gtk_label_new("UNKNOWN");
		gtk_box_pack_start(GTK_BOX(dw.general_tab.info_box), label,
								TRUE, TRUE, 0);
		gtk_widget_show_all(dialog_window);
		res = gtk_dialog_run(GTK_DIALOG(dialog_window));
		gtk_widget_destroy(dialog_window);
		return;
	}
 
	var_val_l = vt_get_var_val_list(evlog_data);
	while (var_val_l != NULL) {
		var_val = (VtVarValT *) var_val_l->data;
		if (strcmp(var_val->var, "Event log enable") == 0) {
			var_val_l = var_val_l->next;
			continue;
		}
		hbox = gtk_hbox_new(TRUE, 5);
		gtk_box_pack_start(GTK_BOX(dw.general_tab.info_box), hbox,
							FALSE, FALSE, 5);
		label = gtk_label_new(var_val->var);
		gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 10);
		label = gtk_label_new(var_val->val);
		gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 1);
		var_val_l = var_val_l->next;
	} 

	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
						dw.general_tab.enable_state),
					vt_data_value_get_as_int(evlog_data,
								"enabled"));

	gtk_widget_set_sensitive(dw.general_tab.overflow_reset,
			vt_data_value_get_as_int(evlog_data,
				"overflow_resetable"));

	vt_data_destroy1(evlog_data);


	evlog_list = voh_get_evlog_entries(id, -1, err);

	store = gtk_tree_store_new(VOH_LIST_NUM_COL, G_TYPE_STRING,
                                 GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_UINT,
                                 G_TYPE_UINT, G_TYPE_UINT, G_TYPE_UINT);

	while (evlog_list != NULL) {
		evlog_data = (VtData1T *) evlog_list->data;
		gtk_tree_store_append(store, &iter, NULL);
		id = vt_data_value_get_as_int(evlog_data, "entry_id");
		sprintf(name, "%s event #%d", vt_data_value_str_get1(evlog_data,
							"event.event_type"), id);
		gtk_tree_store_set(store, &iter,
				VOH_LIST_COLUMN_NAME, name,
				VOH_LIST_COLUMN_ID, id,
				-1);
		evlog_list = evlog_list->next;
	}

	gtk_tree_view_set_model(GTK_TREE_VIEW(dw.evlog_tab.evlog_view),
				GTK_TREE_MODEL(store));
	g_object_unref(store);

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(
						dw.evlog_tab.evlog_view));
//	g_signal_connect(G_OBJECT(dw.evlog_tab.evlog_view), "unselect-all",
//			G_CALLBACK(hview_domain_evlog_unsel_call),
//			(gpointer) &dw);
	g_signal_connect(G_OBJECT(selection), "changed",
			G_CALLBACK(hview_domain_evlog_selected_call),
			(gpointer) &dw);

	g_signal_connect(G_OBJECT(dw.evlog_tab.clear_evlog), "clicked",
			G_CALLBACK(hview_clear_domain_event_log_call),
			(gpointer) &dw);

	gtk_widget_show_all(dialog_window);
	res = gtk_dialog_run(GTK_DIALOG(dialog_window));
	switch (res) {
	case GTK_RESPONSE_OK:
		hview_domain_evlog_ok_response((gpointer) &dw);
		break;
	default:
		break;
	}

	gtk_widget_destroy(dialog_window);
	
}

void hview_domain_evlog_clear_call(GtkWidget *widget, gpointer data)
{
	HviewWidgetsT			*w = (HviewWidgetsT *) data;
	GtkTreeModel			*model;
	GtkTreeIter			iter;
	GtkTreeSelection		*selection;
	guint				id;
	gint				res;
	gchar				err[1024];

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(w->domain_view));
	res = gtk_tree_selection_get_selected(selection, &model, &iter);
	if (res == FALSE) {
		return;
	}

	gtk_tree_model_get(model, &iter, VOH_LIST_COLUMN_ID, &id, -1);

	res = voh_event_log_clear(id, -1, err);

	if (res == FALSE) {
		hview_print(w, err);
	}
}

void hview_domain_evlog_overflow_reset_call(GtkWidget *widget, gpointer data)
{
	HviewWidgetsT			*w = (HviewWidgetsT *) data;
	GtkTreeModel			*model;
	GtkTreeIter			iter;
	GtkTreeSelection		*selection;
	guint				id;
	gint				res;
	gchar				err[1024];

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(w->domain_view));
	res = gtk_tree_selection_get_selected(selection, &model, &iter);
	if (res == FALSE) {
		return;
	}

	gtk_tree_model_get(model, &iter, VOH_LIST_COLUMN_ID, &id, -1);

	res = voh_event_log_overflow_reset(id, -1, err);

	if (res == FALSE) {
		hview_print(w, err);
	}
}

gboolean hview_domain_evlog_selected_call(GtkTreeSelection *selection,
					  gpointer data)
{
	HviewDomainEvLogWidgetsT	*dw = (HviewDomainEvLogWidgetsT *) data;
	HviewWidgetsT			*w = dw->parent_widgets;
	GList				*childs_list;
	GtkTreeModel			*model;
	GtkTreeIter			iter;
	guint				sid,		id;
	gint				res;
	gchar				err[1024];
	GtkWidget			*widget;
	GList				*var_val_l;
	VtVarValT			*var_val;
	VtData1T			*entry_data;
	GtkWidget			*label;
	GtkWidget			*hbox;
	GtkTreeSelection		*dom_sel;

	childs_list = gtk_container_get_children(GTK_CONTAINER(
						dw->evlog_tab.entryinfo_box));
	while (childs_list != NULL) {
		widget = GTK_WIDGET(childs_list->data);
		gtk_container_remove(GTK_CONTAINER(dw->evlog_tab.entryinfo_box),
					widget);
		childs_list = childs_list->next;
	}

	dom_sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(w->domain_view));
	res = gtk_tree_selection_get_selected(dom_sel, &model, &iter);
	if (res == FALSE) {
		return;
	}

	gtk_tree_model_get(model, &iter, VOH_LIST_COLUMN_ID, &sid, -1);

	res = gtk_tree_selection_get_selected(selection, &model, &iter);
	if (res == FALSE) {
		return FALSE;
	}

	gtk_tree_model_get(model, &iter, VOH_LIST_COLUMN_ID, &id, -1);

	res = voh_get_evlog_entry_info(sid, -1, id, &entry_data, err);

	if (res == FALSE) {
		hview_print(w, err);
		label = gtk_label_new("UNKNOWN");
		gtk_box_pack_start(GTK_BOX(dw->evlog_tab.entryinfo_box),
					label, TRUE, TRUE, 0);
		gtk_widget_show_all(dw->evlog_tab.entryinfo_box);
		return FALSE;
	}

	var_val_l = vt_get_var_val_list(entry_data);
	while (var_val_l != NULL) {
		var_val = (VtVarValT *) var_val_l->data;
		hbox = gtk_hbox_new(TRUE, 5);
		gtk_box_pack_start(GTK_BOX(dw->evlog_tab.entryinfo_box), hbox,
							FALSE, FALSE, 0);
		label = gtk_label_new(var_val->var);
		gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 5);
		label = gtk_label_new(var_val->val);
		gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 1);
		var_val_l = var_val_l->next;
	}

	vt_data_destroy1(entry_data);

	gtk_widget_show_all(dw->evlog_tab.entryinfo_box);

	return TRUE;
}

gboolean hview_domain_alarm_selected_call(GtkTreeSelection *selection,
					  gpointer data)
{
	HviewDomainDialogWidgetsT	*dw = (HviewDomainDialogWidgetsT *) data;
	HviewWidgetsT			*w = dw->parent_widgets;
	GList				*childs_list;
	GtkTreeModel			*model;
	GtkTreeIter			iter;
	guint				sid,		id;
	gint				res;
	gchar				err[1024];
	GtkWidget			*widget;
	GList				*var_val_l;
	VtVarValT			*var_val;
	VtData1T			*entry_data;
	GtkWidget			*label;
	GtkWidget			*hbox;
	GtkTreeSelection		*dom_sel;

	childs_list = gtk_container_get_children(GTK_CONTAINER(
						dw->dat_tab.alarminfo_box));
	while (childs_list != NULL) {
		widget = GTK_WIDGET(childs_list->data);
		gtk_container_remove(GTK_CONTAINER(dw->dat_tab.alarminfo_box),
					widget);
		childs_list = childs_list->next;
	}

	dom_sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(w->domain_view));
	res = gtk_tree_selection_get_selected(dom_sel, &model, &iter);
	if (res == FALSE) {
		return;
	}

	gtk_tree_model_get(model, &iter, VOH_LIST_COLUMN_ID, &sid, -1);

	res = gtk_tree_selection_get_selected(selection, &model, &iter);
	if (res == FALSE) {
		return FALSE;
	}

	gtk_tree_model_get(model, &iter, VOH_LIST_COLUMN_ID, &id, -1);

	res = voh_alarm_get(sid, id, &entry_data, err);

	if (res == FALSE) {
		hview_print(w, err);
		label = gtk_label_new("UNKNOWN");
		gtk_box_pack_start(GTK_BOX(dw->dat_tab.alarminfo_box),
					label, TRUE, TRUE, 0);
		gtk_widget_show_all(dw->dat_tab.alarminfo_box);
		return FALSE;
	}

	var_val_l = vt_get_var_val_list(entry_data);
	while (var_val_l != NULL) {
		var_val = (VtVarValT *) var_val_l->data;
		hbox = gtk_hbox_new(TRUE, 5);
		gtk_box_pack_start(GTK_BOX(dw->dat_tab.alarminfo_box), hbox,
							FALSE, FALSE, 0);
		label = gtk_label_new(var_val->var);
		gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 3);
		label = gtk_label_new(var_val->val);
		gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 1);
		var_val_l = var_val_l->next;
	}

	vt_data_destroy1(entry_data);

	gtk_widget_show_all(dw->dat_tab.alarminfo_box);

	return TRUE;
}

gboolean hview_clear_domain_event_log_call(GtkWidget *widget,
					   gpointer data)
{
	HviewDomainEvLogWidgetsT	*dw = (HviewDomainEvLogWidgetsT *) data;
	HviewWidgetsT			*w = dw->parent_widgets;
	GList				*childs_list;
	GtkTreeModel			*model;
	GtkTreeIter			iter;
	GtkTreeStore			*store;
	guint				id;
	gint				res;
	gchar				err[1024];
	GList				*var_val_l,	*evlog_list;
	VtVarValT			*var_val;
	VtData1T			*evlog_data;
	GtkTreeSelection		*dom_sel;
	gchar				name[1024];

	childs_list = gtk_container_get_children(GTK_CONTAINER(
						dw->evlog_tab.entryinfo_box));
	while (childs_list != NULL) {
		widget = GTK_WIDGET(childs_list->data);
		gtk_container_remove(GTK_CONTAINER(dw->evlog_tab.entryinfo_box),
					widget);
		childs_list = childs_list->next;
	}

	gtk_tree_view_set_model(GTK_TREE_VIEW(dw->evlog_tab.evlog_view),
				NULL);

	dom_sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(w->domain_view));
	res = gtk_tree_selection_get_selected(dom_sel, &model, &iter);
	if (res == FALSE) {
		return FALSE;
	}

	gtk_tree_model_get(model, &iter, VOH_LIST_COLUMN_ID, &id, -1);

	res = voh_event_log_clear(id, -1, err);

	if (res == FALSE) {
		hview_print(w, err);
	}

	evlog_list = voh_get_evlog_entries(id, -1, err);

	store = gtk_tree_store_new(VOH_LIST_NUM_COL, G_TYPE_STRING,
                                 GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_UINT,
                                 G_TYPE_UINT, G_TYPE_UINT, G_TYPE_UINT);

	while (evlog_list != NULL) {
		evlog_data = (VtData1T *) evlog_list->data;
		gtk_tree_store_append(store, &iter, NULL);
		id = vt_data_value_get_as_int(evlog_data, "entry_id");
		sprintf(name, "%s event #%d", vt_data_value_str_get1(evlog_data,
							"event.event_type"), id);
		gtk_tree_store_set(store, &iter,
				VOH_LIST_COLUMN_NAME, name,
				VOH_LIST_COLUMN_ID, id,
				-1);
		evlog_list = evlog_list->next;
	}

	gtk_tree_view_set_model(GTK_TREE_VIEW(dw->evlog_tab.evlog_view),
				GTK_TREE_MODEL(store));
	g_object_unref(store);

	return TRUE;
}

void hview_sys_evlog_time_ok_response(gpointer data)
{
	HviewDomainEvLogTSWidgetsT	*dw = (HviewDomainEvLogTSWidgetsT *)
									data;
	HviewWidgetsT			*w = dw->parent_widgets;
	GtkTreeModel			*model;
	GtkTreeIter			iter;
	GtkTreeSelection		*selection;
	GtkWidget			*tview;
	gint				page;
	guint				rid,	sid;
	guint				year,	month,	day;
	guint				hour,	min,	sec;
	gint				res;
	gdouble				time;
	gchar				err[1024];

	page = gtk_notebook_get_current_page(GTK_NOTEBOOK(w->tab_windows));
	if (page < 0)
		return;
	if (w->tab_views[page].resource_view == NULL ||
			w->tab_views[page].detail_view == NULL)
		return;
	tview = w->tab_views[page].resource_view;
	sid = w->tab_views[page].sessionid;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tview));

	if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
		gtk_tree_model_get(model, &iter, VOH_LIST_COLUMN_ID, &rid, -1);
	} else {
		return;
	}

	gtk_calendar_get_date(GTK_CALENDAR(dw->calendar), &year, &month, &day);
	hour = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(dw->hour));
	min = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(dw->min));
	sec = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(dw->sec));
	time = vt_dmyhms_to_time(year, month, day, hour, min, sec);

	res = voh_event_log_time_set(sid, rid, time, err);

	if (res == FALSE) {
		hview_print(w, err);
	}
}

void hview_sys_evlog_time_call(GtkWidget *widget, gpointer data)
{
	HviewWidgetsT			*w = (HviewWidgetsT *) data;
	HviewDomainEvLogTSWidgetsT	dw;
	GtkWidget			*dialog_window;
	GtkTreeModel			*model;
	GtkTreeIter			iter;
	GtkTreeSelection		*selection;
	GtkWidget			*tview;
	gint				page;
	GtkWidget			*hbox;
	GtkWidget			*label;
	VtData1T			*domain_info;
	GList				*var_val_l,	*alarm_list;
	VtVarValT			*var_val;
	guint				rid, sid;
	guint				year,	month,	day;
	guint				hour,	min,	sec;
	gint				res;
	gfloat				time;
	gchar				err[1024];

	page = gtk_notebook_get_current_page(GTK_NOTEBOOK(w->tab_windows));
	if (page < 0)
		return;
	if (w->tab_views[page].resource_view == NULL ||
			w->tab_views[page].detail_view == NULL)
		return;
	tview = w->tab_views[page].resource_view;
	sid = w->tab_views[page].sessionid;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tview));

	if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
		gtk_tree_model_get(model, &iter, VOH_LIST_COLUMN_ID, &rid, -1);
	} else {
		return;
	}

	dw.parent_widgets = w;
	dialog_window = hview_get_domain_evlogtime_window(&dw);
	dw.dialog_window = dialog_window;

	label = gtk_label_new("Current timestamp");
	gtk_box_pack_start(GTK_BOX(dw.time_box), label, TRUE, TRUE, 5);

	res = voh_event_log_time_get(sid, rid, &time, err);
	if(res == FALSE) {
		hview_print(w, err);
		label = gtk_label_new("unknown");
	} else {
		label = gtk_label_new(vt_convert_time(time));
		res = vt_get_dmyhms(time, &year, &month, &day, &hour,
								&min, &sec);
		if (res == TRUE) {
			gtk_spin_button_set_value(GTK_SPIN_BUTTON(dw.hour),
									hour);
			gtk_spin_button_set_value(GTK_SPIN_BUTTON(dw.min),
									min);
			gtk_spin_button_set_value(GTK_SPIN_BUTTON(dw.sec),
									sec);
			gtk_calendar_select_month(GTK_CALENDAR(dw.calendar),
							month, year);
			gtk_calendar_select_day(GTK_CALENDAR(dw.calendar),
							day);
		}
	}
	gtk_box_pack_start(GTK_BOX(dw.time_box), label, TRUE, TRUE, 5);

	gtk_widget_show_all(dialog_window);
	res = gtk_dialog_run(GTK_DIALOG(dialog_window));
	switch (res) {
	case GTK_RESPONSE_OK:
		hview_sys_evlog_time_ok_response((gpointer) &dw);
		break;
	default:
		break;
	}

	gtk_widget_destroy(dialog_window);
	
}

void hview_sys_evlog_ok_response(gpointer data)
{
	HviewDomainEvLogWidgetsT	*dw = (HviewDomainEvLogWidgetsT *)
									data;
	HviewWidgetsT			*w = dw->parent_widgets;
	GtkTreeModel			*model;
	GtkTreeIter			iter;
	GtkTreeSelection		*selection;
	GtkWidget			*tview;
	gint				page;
	guint				sid,	rid;
	gint				res;
	gchar				err[1024];

	page = gtk_notebook_get_current_page(GTK_NOTEBOOK(w->tab_windows));
	if (page < 0)
		return;
	if (w->tab_views[page].resource_view == NULL ||
			w->tab_views[page].detail_view == NULL)
		return;
	tview = w->tab_views[page].resource_view;
	sid = w->tab_views[page].sessionid;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tview));

	if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
		gtk_tree_model_get(model, &iter, VOH_LIST_COLUMN_ID, &rid, -1);
	} else {
		return;
	}

	res = voh_event_log_state_set(sid, rid,
			gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(
						dw->general_tab.enable_state)),
			err);
	if (res == FALSE) {
		hview_print(w, err);
	}

}

void hview_sys_evlog_call(GtkWidget *widget, gpointer data)
{
	HviewWidgetsT			*w = (HviewWidgetsT *) data;
	HviewDomainEvLogWidgetsT	dw;
	GtkWidget			*dialog_window;
	GtkTreeModel			*model;
	GtkTreeStore			*store;
	GtkTreeIter			iter;
	GtkTreeSelection		*selection;
	GtkWidget			*tview;
	GtkWidget			*hbox;
	GtkWidget			*label;
	gint				page;
	VtData1T			*evlog_data;
	GList				*evlog_list;
	GList				*var_val_l;
	VtVarValT			*var_val;
	guint				sid,		rid;
	gint				res;
	gchar				name[1024];
	gchar				err[1024];

	page = gtk_notebook_get_current_page(GTK_NOTEBOOK(w->tab_windows));
	if (page < 0)
		return;
	if (w->tab_views[page].resource_view == NULL ||
			w->tab_views[page].detail_view == NULL)
		return;
	tview = w->tab_views[page].resource_view;
	sid = w->tab_views[page].sessionid;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tview));

	if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
		gtk_tree_model_get(model, &iter, VOH_LIST_COLUMN_ID, &rid, -1);
	} else {
		return;
	}

	dw.parent_widgets = w;
	dialog_window = hview_get_domain_evlog_window(&dw);
	dw.dialog_window = dialog_window;

	res = voh_event_log_info_get(sid, rid, &evlog_data, err);
	if (res == FALSE) {
		label = gtk_label_new("UNKNOWN");
		gtk_box_pack_start(GTK_BOX(dw.general_tab.info_box), label,
								TRUE, TRUE, 0);
		gtk_widget_show_all(dialog_window);
		res = gtk_dialog_run(GTK_DIALOG(dialog_window));
		gtk_widget_destroy(dialog_window);
		return;
	}
 
	var_val_l = vt_get_var_val_list(evlog_data);
	while (var_val_l != NULL) {
		var_val = (VtVarValT *) var_val_l->data;
		if (strcmp(var_val->var, "Event log enable") == 0) {
			var_val_l = var_val_l->next;
			continue;
		}
		hbox = gtk_hbox_new(TRUE, 5);
		gtk_box_pack_start(GTK_BOX(dw.general_tab.info_box), hbox,
							FALSE, FALSE, 5);
		label = gtk_label_new(var_val->var);
		gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 10);
		label = gtk_label_new(var_val->val);
		gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 1);
		var_val_l = var_val_l->next;
	} 

	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
						dw.general_tab.enable_state),
					vt_data_value_get_as_int(evlog_data,
								"enabled"));

	gtk_widget_set_sensitive(dw.general_tab.overflow_reset,
			vt_data_value_get_as_int(evlog_data,
				"overflow_resetable"));

	vt_data_destroy1(evlog_data);


	evlog_list = voh_get_evlog_entries(sid, rid, err);

	store = gtk_tree_store_new(VOH_LIST_NUM_COL, G_TYPE_STRING,
                                 GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_UINT,
                                 G_TYPE_UINT, G_TYPE_UINT, G_TYPE_UINT);

	while (evlog_list != NULL) {
		evlog_data = (VtData1T *) evlog_list->data;
		gtk_tree_store_append(store, &iter, NULL);
		sid = vt_data_value_get_as_int(evlog_data, "entry_id");
		sprintf(name, "%s event #%d", vt_data_value_str_get1(evlog_data,
							"event.event_type"),
							sid);
		gtk_tree_store_set(store, &iter,
				VOH_LIST_COLUMN_NAME, name,
				VOH_LIST_COLUMN_ID, sid,
				-1);
		evlog_list = evlog_list->next;
	}

	gtk_tree_view_set_model(GTK_TREE_VIEW(dw.evlog_tab.evlog_view),
				GTK_TREE_MODEL(store));
	g_object_unref(store);

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(
						dw.evlog_tab.evlog_view));
//	g_signal_connect(G_OBJECT(dw.evlog_tab.evlog_view), "unselect-all",
//			G_CALLBACK(hview_sys_evlog_unsel_call),
//			(gpointer) &dw);
	g_signal_connect(G_OBJECT(selection), "changed",
			G_CALLBACK(hview_sys_evlog_selected_call),
			(gpointer) &dw);
	g_signal_connect(G_OBJECT(dw.evlog_tab.clear_evlog), "clicked",
			G_CALLBACK(hview_clear_sys_event_log_call),
			(gpointer) &dw);

	gtk_widget_show_all(dialog_window);
	res = gtk_dialog_run(GTK_DIALOG(dialog_window));
	switch (res) {
	case GTK_RESPONSE_OK:
		hview_sys_evlog_ok_response((gpointer) &dw);
		break;
	default:
		break;
	}

	gtk_widget_destroy(dialog_window);
	
}

void hview_sys_evlog_clear_call(GtkWidget *widget, gpointer data)
{
	HviewWidgetsT			*w = (HviewWidgetsT *) data;
	GtkTreeModel			*model;
	GtkTreeIter			iter;
	GtkTreeSelection		*selection;
	GtkWidget			*tview;
	gint				page;
	guint				sid,		rid;
	gint				res;
	gchar				err[1024];

	page = gtk_notebook_get_current_page(GTK_NOTEBOOK(w->tab_windows));
	if (page < 0)
		return;
	if (w->tab_views[page].resource_view == NULL ||
			w->tab_views[page].detail_view == NULL)
		return;
	tview = w->tab_views[page].resource_view;
	sid = w->tab_views[page].sessionid;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tview));

	if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
		gtk_tree_model_get(model, &iter, VOH_LIST_COLUMN_ID, &rid, -1);
	} else {
		return;
	}

	res = voh_event_log_clear(sid, rid, err);

	if (res == FALSE) {
		hview_print(w, err);
	}
}

void hview_sys_evlog_overflow_reset_call(GtkWidget *widget, gpointer data)
{
	HviewWidgetsT			*w = (HviewWidgetsT *) data;
	GtkTreeModel			*model;
	GtkTreeIter			iter;
	GtkTreeSelection		*selection;
	GtkWidget			*tview;
	gint				page;
	guint				sid,		rid;
	gint				res;
	gchar				err[1024];

	page = gtk_notebook_get_current_page(GTK_NOTEBOOK(w->tab_windows));
	if (page < 0)
		return;
	if (w->tab_views[page].resource_view == NULL ||
			w->tab_views[page].detail_view == NULL)
		return;
	tview = w->tab_views[page].resource_view;
	sid = w->tab_views[page].sessionid;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tview));

	if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
		gtk_tree_model_get(model, &iter, VOH_LIST_COLUMN_ID, &rid, -1);
	} else {
		return;
	}

	res = voh_event_log_overflow_reset(sid, rid, err);

	if (res == FALSE) {
		hview_print(w, err);
	}
}

gboolean hview_sys_evlog_selected_call(GtkTreeSelection *selection,
					  gpointer data)
{
	HviewDomainEvLogWidgetsT	*dw = (HviewDomainEvLogWidgetsT *) data;
	HviewWidgetsT			*w = dw->parent_widgets;
	GList				*childs_list;
	GtkTreeModel			*model;
	GtkTreeIter			iter;
	GtkWidget			*tview;
	gint				page;
	guint				sid,		rid,	id;
	gint				res;
	gchar				err[1024];
	GtkWidget			*widget;
	GList				*var_val_l;
	VtVarValT			*var_val;
	VtData1T			*entry_data;
	GtkWidget			*label;
	GtkWidget			*hbox;
	GtkTreeSelection		*dom_sel;

	childs_list = gtk_container_get_children(GTK_CONTAINER(
						dw->evlog_tab.entryinfo_box));
	while (childs_list != NULL) {
		widget = GTK_WIDGET(childs_list->data);
		gtk_container_remove(GTK_CONTAINER(dw->evlog_tab.entryinfo_box),
					widget);
		childs_list = childs_list->next;
	}

	page = gtk_notebook_get_current_page(GTK_NOTEBOOK(w->tab_windows));
	if (page < 0)
		return;
	if (w->tab_views[page].resource_view == NULL ||
			w->tab_views[page].detail_view == NULL)
		return;
	tview = w->tab_views[page].resource_view;
	sid = w->tab_views[page].sessionid;

	dom_sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(tview));

	if (gtk_tree_selection_get_selected(dom_sel, &model, &iter)) {
		gtk_tree_model_get(model, &iter, VOH_LIST_COLUMN_ID, &rid, -1);
	} else {
		return;
	}

	res = gtk_tree_selection_get_selected(selection, &model, &iter);
	if (res == FALSE) {
		return FALSE;
	}

	gtk_tree_model_get(model, &iter, VOH_LIST_COLUMN_ID, &id, -1);

	res = voh_get_evlog_entry_info(sid, rid, id, &entry_data, err);

	if (res == FALSE) {
		hview_print(w, err);
		label = gtk_label_new("UNKNOWN");
		gtk_box_pack_start(GTK_BOX(dw->evlog_tab.entryinfo_box),
					label, TRUE, TRUE, 0);
		gtk_widget_show_all(dw->evlog_tab.entryinfo_box);
		return FALSE;
	}

	var_val_l = vt_get_var_val_list(entry_data);
	while (var_val_l != NULL) {
		var_val = (VtVarValT *) var_val_l->data;
		hbox = gtk_hbox_new(TRUE, 5);
		gtk_box_pack_start(GTK_BOX(dw->evlog_tab.entryinfo_box), hbox,
							FALSE, FALSE, 0);
		label = gtk_label_new(var_val->var);
		gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 5);
		label = gtk_label_new(var_val->val);
		gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 1);
		var_val_l = var_val_l->next;
	}

	vt_data_destroy1(entry_data);

	gtk_widget_show_all(dw->evlog_tab.entryinfo_box);

	return TRUE;
}

gboolean hview_clear_sys_event_log_call(GtkWidget *widget,
					   gpointer data)
{
	HviewDomainEvLogWidgetsT	*dw = (HviewDomainEvLogWidgetsT *) data;
	HviewWidgetsT			*w = dw->parent_widgets;
	GList				*childs_list;
	GtkTreeModel			*model;
	GtkTreeIter			iter;
	GtkTreeStore			*store;
	GtkWidget			*tview;
	gint				page;
	guint				sid,		rid;
	gint				res;
	gchar				err[1024];
	GList				*var_val_l,	*evlog_list;
	VtVarValT			*var_val;
	VtData1T			*evlog_data;
	GtkTreeSelection		*selection;
	gchar				name[1024];

	childs_list = gtk_container_get_children(GTK_CONTAINER(
						dw->evlog_tab.entryinfo_box));
	while (childs_list != NULL) {
		widget = GTK_WIDGET(childs_list->data);
		gtk_container_remove(GTK_CONTAINER(dw->evlog_tab.entryinfo_box),
					widget);
		childs_list = childs_list->next;
	}

	gtk_tree_view_set_model(GTK_TREE_VIEW(dw->evlog_tab.evlog_view),
				NULL);

	page = gtk_notebook_get_current_page(GTK_NOTEBOOK(w->tab_windows));
	if (page < 0)
		return;
	if (w->tab_views[page].resource_view == NULL ||
			w->tab_views[page].detail_view == NULL)
		return;
	tview = w->tab_views[page].resource_view;
	sid = w->tab_views[page].sessionid;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tview));

	if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
		gtk_tree_model_get(model, &iter, VOH_LIST_COLUMN_ID, &rid, -1);
	} else {
		return;
	}

	res = voh_event_log_clear(sid, rid, err);

	if (res == FALSE) {
		hview_print(w, err);
	}

	evlog_list = voh_get_evlog_entries(sid, rid, err);

	store = gtk_tree_store_new(VOH_LIST_NUM_COL, G_TYPE_STRING,
                                 GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_UINT,
                                 G_TYPE_UINT, G_TYPE_UINT, G_TYPE_UINT);

	while (evlog_list != NULL) {
		evlog_data = (VtData1T *) evlog_list->data;
		gtk_tree_store_append(store, &iter, NULL);
		sid = vt_data_value_get_as_int(evlog_data, "entry_id");
		sprintf(name, "%s event #%d", vt_data_value_str_get1(evlog_data,
							"event.event_type"),
							sid);
		gtk_tree_store_set(store, &iter,
				VOH_LIST_COLUMN_NAME, name,
				VOH_LIST_COLUMN_ID, sid,
				-1);
		evlog_list = evlog_list->next;
	}

	gtk_tree_view_set_model(GTK_TREE_VIEW(dw->evlog_tab.evlog_view),
				GTK_TREE_MODEL(store));
	g_object_unref(store);

	return TRUE;
}

