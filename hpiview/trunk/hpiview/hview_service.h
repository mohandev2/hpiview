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

#ifndef __HVIEW_SERVICE_H__
#define __HVIEW_SERVICE_H__

#define HVIEW_VERSION		2.0
#define HVIEW_NAME_VERSION	"OpenHpi View version: 2.0"

#define HVIEW_ABOUT	"OpenHpi View ver. 2.0\n Author: Denis Sadykov, 2004"

#define HVIEW_TITLE			"OpenHpi View"
#define HVIEW_DOMAIN_COLUMN_TITLE	"\tDomains"
#define HVIEW_RESOURCE_COLUMN_TITLE	"\tResources"

#define HVIEW_MAIN_WINDOW_WIDTH		700
#define HVIEW_MAIN_WINDOW_HEIGHT	600
#define HVIEW_LOG_WINDOW_WIDTH		HVIEW_MAIN_WINDOW_WIDTH
#define HVIEW_LOG_WINDOW_HEIGHT		150
#define HVIEW_DETAIL_WINDOW_WIDTH	450
#define HVIEW_DETAIL_WINDOW_HEIGHT	300
#define HVIEW_TREE_WINDOW_WIDTH		250
#define HVIEW_TREE_WINDOW_HEIGHT	HVIEW_DETAIL_WINDOW_HEIGHT
#define HVIEW_ABOUT_WINDOW_WIDTH	200
#define HVIEW_ABOUT_WINDOW_HEIGHT	100

#define HVIEW_MAX_TAB_WINDOWS		100


typedef struct HviewTabs
{
      GtkWidget		*tree_view;
      GtkWidget		*detail_view;
} HviewTabsT;

typedef struct HviewWidgets
{
      GtkWidget		*main_window;
      GtkWidget		*domain_window;
      GtkWidget		*log_window;
      GtkWidget		*event_window;
      GtkWidget		*statusbar;
      GtkWidget		*toolbar;
      GtkWidget		*vtoolbar;

      GtkWidget		*tab_windows;
      HviewTabsT	tab_views[HVIEW_MAX_TAB_WINDOWS];

      GtkWidget		*domain_view;
      GtkWidget		*log_view;
      GtkWidget		*event_view;

      GtkToolItem	*rsitem;
} HviewWidgetsT;

enum {
      HVIEW_BRANCH = 1,
      HVIEW_ITEM,
};

typedef struct HviewMenuItemFactory
{
      gchar		*label;
      gchar		*image_file;
      gpointer		callback;
      gpointer		data;
      guint		type;
} HviewMenuItemFactoryT;

void add_pixmap_directory(const gchar *directory);
gchar* find_pixmap_file(const gchar *filename);
GtkWidget *create_pixmap(const gchar *filename);

GtkWidget *hview_get_log_window(HviewWidgetsT *w);
GtkWidget *hview_get_event_window(HviewWidgetsT *w);
GtkWidget *hview_get_domain_window(HviewWidgetsT *w);
GtkWidget *hview_get_detail_window(HviewWidgetsT *w, gint page);
GtkWidget *hview_get_tree_window(HviewWidgetsT *w, gint page);
GtkWidget *hview_get_menubar(HviewWidgetsT *w);
GtkWidget *hview_get_toolbar(HviewWidgetsT *w);
GtkWidget *hview_get_vtoolbar(HviewWidgetsT *w);

void hview_print(HviewWidgetsT *w, const gchar *string);
void hview_event_print(HviewWidgetsT *w, const gchar *string);
void hview_statusbar_push(HviewWidgetsT *w, const gchar *string);

void hview_tree_cell_func(GtkTreeViewColumn	*col,
			 GtkCellRenderer	*renderer,
			 GtkTreeModel		*model,
			 GtkTreeIter		*iter,
			 gpointer		data);
void hview_detail_cell_func(GtkTreeViewColumn	*col,
			 GtkCellRenderer	*renderer,
			 GtkTreeModel		*model,
			 GtkTreeIter		*iter,
			 gpointer		data);

#endif

