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

#ifndef __HVIEW_CALLBACKS_H__
#define __HVIEW_CALLBACKS_H__

void hview_quit_call(GtkWidget *widget, gpointer data);
void hview_empty_log_call(GtkWidget *widget, gpointer data);
void hview_load_plugin_call(GtkWidget *widget, gpointer data);
void hview_unload_plugin_call(GtkWidget *widget, gpointer data);
void hview_session_close_call(GtkWidget *widget, gpointer data);
gint hview_discover_thread(gpointer data);
void hview_discover_call(GtkWidget *widget, gpointer data);
void hview_about_call(void);
void hview_tree_column_activated_call(GtkTreeViewColumn *column,
					     gpointer data);
void hview_tree_row_activated_call(GtkWidget *widget,
					  GtkTreeIter *iter,
					  GtkTreePath *path,
					  gpointer data);
gboolean hview_button_prees_treeview_call(GtkWidget *widget,
				      GdkEventButton *event,
				      gpointer data);
void hview_domain_row_activated_call(GtkWidget *widget,
				     GtkTreeIter *iter,
				     GtkTreePath *path,
				     gpointer data);
void hview_tree_row_selected_call(GtkTreeSelection *selection,
					 gpointer data);
gint hview_readsensor_thread(gpointer data);
void hview_read_sensor_call(GtkWidget *widget, gpointer data);
void hview_show_hide_domain_call(GtkWidget *widget, gpointer data);
void hview_open_session_call(GtkWidget *widget, gpointer data);
void hview_switch_page_call(GtkNotebook *notebook,
			    GtkNotebookPage *page,
			    guint pagenum,
			    gpointer data);
void hview_set_power_off_call(GtkWidget *widget, gpointer data);
void hview_set_power_on_call(GtkWidget *widget, gpointer data);
void hview_set_power_cycle_call(GtkWidget *widget, gpointer data);

#endif

