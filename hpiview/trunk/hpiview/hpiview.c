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
#include <config.h>

#include <gtk/gtk.h>
#include "hpiview.h"
#include "hview_service.h"
#include "hview_callbacks.h"
#include "voh.h"


	/* Initialization all Hpi View widgets */

static void hview_init(void)
{
      HviewWidgetsT	widgets;
      GtkWidget		*menubar;
      GtkWidget		*vpaned;
      GtkWidget		*hpaned,	*hpaned1;
      GtkWidget		*main_vbox,	*hbox;
      GtkTreeModel	*domains;
      GtkWidget		*notebook;
      GtkWidget		*separator;
      gint		i;

      for (i = 0; i < HVIEW_MAX_TAB_WINDOWS; i++) {
	    widgets.tab_views[i].tree_view = NULL;
	    widgets.tab_views[i].detail_view = NULL;
      }

// Create main window ---------------------------------------------------------
      widgets.main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
      g_signal_connect(G_OBJECT(widgets.main_window), "destroy",
		       G_CALLBACK(gtk_main_quit),
		       NULL);
      gtk_window_set_title(GTK_WINDOW(widgets.main_window), HVIEW_TITLE);
      gtk_widget_set_size_request(GTK_WIDGET(widgets.main_window),
				  HVIEW_MAIN_WINDOW_WIDTH,
				  HVIEW_MAIN_WINDOW_HEIGHT);
//-----------------------------------------------------------------------------

      main_vbox = gtk_vbox_new(FALSE, 1);
      hbox = gtk_hbox_new(FALSE, 1);
      gtk_container_set_border_width(GTK_CONTAINER(main_vbox), 1);
      gtk_container_add(GTK_CONTAINER(widgets.main_window), main_vbox);

      menubar = hview_get_menubar(&widgets);
      gtk_box_pack_start(GTK_BOX(main_vbox), menubar, FALSE, FALSE, 0);

      widgets.toolbar = hview_get_toolbar(&widgets);
      gtk_box_pack_start(GTK_BOX(main_vbox), widgets.toolbar, FALSE, FALSE, 0);

      separator = gtk_hseparator_new();
      gtk_box_pack_start(GTK_BOX(main_vbox), separator, FALSE, FALSE, 0);

      widgets.vtoolbar = hview_get_vtoolbar(&widgets);
      gtk_box_pack_start(GTK_BOX(hbox), widgets.vtoolbar, FALSE, FALSE, 0);

      separator = gtk_vseparator_new();
      gtk_box_pack_start(GTK_BOX(hbox), separator, FALSE, FALSE, 0);

      vpaned = gtk_vpaned_new();
      hpaned1 = gtk_hpaned_new();
      hpaned = gtk_hpaned_new();

      gtk_box_pack_start(GTK_BOX(hbox), hpaned1, TRUE, TRUE, 0);

      widgets.domain_window = hview_get_domain_window(&widgets);

      gtk_paned_add1(GTK_PANED(hpaned1), widgets.domain_window);

// Create scrolled "tree window" and add it to horizontal paned window --------
      widgets.tab_windows = gtk_notebook_new();
      g_signal_connect(G_OBJECT(widgets.tab_windows), "switch-page",
		       G_CALLBACK(hview_switch_page_call), &widgets);
//      gtk_paned_add2(GTK_PANED(hpaned1), widgets.tab_windows);
      gtk_notebook_set_scrollable(GTK_NOTEBOOK(widgets.tab_windows), TRUE);
      gtk_notebook_set_tab_pos(GTK_NOTEBOOK(widgets.tab_windows), GTK_POS_TOP);
//      widgets.tree_window = hview_get_tree_window(&widgets);
//      gtk_paned_add1(GTK_PANED(hpaned), widgets.tree_window);
//-----------------------------------------------------------------------------

// Create scrolled "detail window" and add it to hpaned window ----------------
//      widgets.detail_window = hview_get_detail_window(&widgets);
//      gtk_paned_add2(GTK_PANED(hpaned), widgets.detail_window);
//-----------------------------------------------------------------------------

      gtk_paned_add2(GTK_PANED(hpaned1), widgets.tab_windows);

// Create tab windows ("log window", "event window") and add it with
// hpaned window to vertical paned window -------------------------------------
      gtk_paned_add1(GTK_PANED(vpaned), hbox);
      widgets.message_window = hview_get_message_window(&widgets);
      widgets.event_window = hview_get_event_window(&widgets);
      widgets.tglbar = hview_get_toggle_bar(&widgets);
      widgets.log_hbox = gtk_hbox_new(FALSE, -1);
      gtk_box_pack_start(GTK_BOX(widgets.log_hbox), widgets.message_window,
			 TRUE, TRUE, 0);
      gtk_box_pack_start(GTK_BOX(widgets.log_hbox), widgets.event_window,
			 TRUE, TRUE, 0);

      gtk_paned_add2(GTK_PANED(vpaned), widgets.log_hbox);
//-----------------------------------------------------------------------------

      gtk_box_pack_start(GTK_BOX(main_vbox), vpaned, TRUE, TRUE, 0);
      separator = gtk_hseparator_new();
      gtk_box_pack_start(GTK_BOX(main_vbox), separator, FALSE, FALSE, 0);
      gtk_box_pack_start(GTK_BOX(main_vbox), widgets.tglbar, FALSE, FALSE, 0);

// Create statusbar -----------------------------------------------------------
      widgets.statusbar = gtk_statusbar_new();
      gtk_box_pack_start(GTK_BOX(main_vbox), widgets.statusbar,
			 FALSE, FALSE, 0);
      hview_statusbar_push(&widgets, "welcome");
//-----------------------------------------------------------------------------

      gtk_widget_show_all(widgets.main_window);
      gtk_widget_hide(widgets.message_window);
      gtk_widget_hide(widgets.event_window);
      gtk_widget_hide(widgets.log_hbox);

}

int main(int argc, char *argv[])
{
      gtk_init(&argc, &argv);

      g_thread_init(NULL);
      add_pixmap_directory (PACKAGE_DATA_DIR "/" PACKAGE "/pixmaps");
      
      hview_init();

      gtk_main();

      return (0);
}

