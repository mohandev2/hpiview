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
#include "voh.h"
#include "hview_service.h"
#include "hview_windows.h"
#include "hpiview.h"

	/* Initialization all Hpi View widgets */

static void hview_init(void)
{
	HviewWidgetsT		widgets;
	GtkTreeModel		*domains;
	gchar			err[1024];
	gint			i;

	for (i = 0; i < HVIEW_MAX_TAB_WINDOWS; i++) {
		widgets.tab_views[i].resource_view = NULL;
		widgets.tab_views[i].detail_view = NULL;
		widgets.tab_views[i].event_subscribed = 0;
		widgets.tab_views[i].sessionid = 0;
	}

	widgets.main_window = hview_get_main_window(&widgets);

	hview_print(&widgets, HVIEW_NAME_VERSION);

	domains = voh_list_domains(err);
	if (domains == NULL) {
		hview_print(&widgets, err);
	} else {
		gtk_tree_view_set_model(GTK_TREE_VIEW(widgets.domain_view),
				        domains);
		g_object_unref(domains);
	}

	hview_statusbar_push(&widgets, "welcome");

	gtk_widget_show_all(widgets.main_window);
	gtk_widget_hide(widgets.message_window);
	gtk_widget_hide(widgets.event_window);
	gtk_widget_hide(widgets.log_hbox);
}

int main(int argc, char *argv[])
{
	gtk_init(&argc, &argv);

	g_thread_init(NULL);
	hutil_add_pixmap_directory (PACKAGE_DATA_DIR "/" PACKAGE "/pixmaps");
      
	hview_init();

	gtk_main();

	return (0);
}

