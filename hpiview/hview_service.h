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


#define HVIEW_MAX_TAB_WINDOWS		100


typedef struct HviewTabs {
      GtkWidget		*resource_view;
      GtkWidget		*detail_view;
      guint		sessionid;
      gboolean		event_subscribed;
} HviewTabsT;

typedef struct HviewWidgets
{
      GtkWidget		*main_window;
      GtkWidget		*domain_window;
      GtkWidget		*message_window;
      GtkWidget		*event_window;
      GtkWidget		*statusbar;
      GtkWidget		*toolbar;
      GtkWidget		*vtoolbar;
      GtkWidget		*tglbar;

      GtkWidget		*log_hbox;

      GtkWidget		*tab_windows;
      HviewTabsT	tab_views[HVIEW_MAX_TAB_WINDOWS];

      GtkWidget		*domain_view;
      GtkWidget		*message_view;
      GtkWidget		*event_view;

      GtkTreeStore	*event_list;

      GtkToolItem	*subev_item;

} HviewWidgetsT;

typedef struct HviewRptDialogWidgets {
	HviewWidgetsT	*parent_widgets;
	GtkWidget	*dialog_window;
	GtkWidget	*info_box;
	GtkWidget	*severity_box;
	GtkWidget	*tag_entry;
} HviewRptDialogWidgetsT;

typedef struct HviewSenGeneralTab {
	gint		tab_page_num;
	GtkWidget	*info_box;
	GtkWidget	*enable_status;
} HviewSenGeneralTabT;

typedef struct HviewSenEventTabT {
	gint		tab_page_num;
	GtkWidget	*event_info_box;
	GtkWidget	*event_enable_status;
} HviewSenEventTabT;

typedef struct HviewSenThrTab {
	gint		tab_page_num;
	GtkWidget	*info_box;
	GList		*thrlist;
} HviewSenThrTabT;

typedef struct HviewSenDialogWidgets {
	HviewWidgetsT		*parent_widgets;
	GtkWidget		*dialog_window;
	GtkWidget		*notebook;
	HviewSenGeneralTabT	general_tab;
	HviewSenEventTabT	event_tab;
	HviewSenThrTabT		threshold_tab;
} HviewSenDialogWidgetsT;

typedef struct HviewSenEventMasksWidgets {
	HviewSenDialogWidgetsT	*parent_widgets;
	GtkWidget		*dialog_window;
	GList			*assert_event_mask;
	GList			*deassert_event_mask;
} HviewSenEventMasksWidgetsT;

typedef struct HviewInvGeneralTab {
	gint		tab_page_num;
	GtkWidget	*info_box;
	GtkWidget	*invfield_box;
	GtkWidget	*fru_view;
	GList		*area_list;
} HviewInvGeneralTabT;

typedef struct HviewInvFieldsTab {
	gint		tab_page_num;
	GtkWidget	*areas_view;
	GtkWidget	*fields_view;
	GtkWidget	*add_area;
	GtkWidget	*remove_area;
	GtkWidget	*add_field;
	GtkWidget	*remove_field;
	GtkWidget	*set_field;
	GtkWidget	*data_label;
} HviewInvFieldsTabT;

typedef struct HviewInvDialogWidgets {
	HviewWidgetsT		*parent_widgets;
	GtkWidget		*dialog_window;
	GtkWidget		*notebook;
	HviewInvGeneralTabT	general_tab;
	HviewInvFieldsTabT	fields_tab;
} HviewInvDialogWidgetsT;

typedef struct HviewInvAreaAddWidgets {
	HviewInvDialogWidgetsT	*parent_widgets;
	GtkWidget		*dialog_window;
	GtkWidget		*area_types;
} HviewInvAreaAddWidgetsT;

typedef struct HviewInvFieldAddWidgets {
	HviewInvDialogWidgetsT	*parent_widgets;
	GtkWidget		*dialog_window;
	GtkWidget		*field_types;
	GtkWidget		*read_only;
	GtkTextBuffer		*data_buffer;
	GtkWidget		*data_view;
} HviewInvFieldAddWidgetsT;


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
			    GtkTreeModel	*model,
			    GtkTreeIter		*iter,
			    gpointer		data);

#endif

