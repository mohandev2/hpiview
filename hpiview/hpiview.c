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
#include "hview_service.h"
#include "hview_callbacks.h"
#include "hpiview.h"

	/* Create menu bar */

static GtkWidget *hview_get_menubar(HviewWidgetsT *w)
{

      GtkWidget		*mbar;
      GtkWidget		*menu;
      GtkWidget		*item,	*sitem;
      GtkWidget		*image;

      gchar		*label,	*image_file;
      guint		type,	number_items;
      gpointer		callback, data;
      int		i;

      HviewMenuItemFactoryT menu_items[] = {
	{"_Session",	NULL, NULL, NULL, HVIEW_BRANCH},
        {"_Quit",	"exit.png", hview_quit_call, w, HVIEW_ITEM},
        {"_Edit",	NULL, NULL, NULL, HVIEW_BRANCH},
        {"_Clear log",	"clear.png", hview_empty_log_call, w, HVIEW_ITEM},
	{"_Action",	NULL, NULL, NULL, HVIEW_BRANCH},
        {"_Discover",	"discover.png", hview_discover_call, w, HVIEW_ITEM},
        {"_Load plugin", NULL, hview_load_plugin_call, w, HVIEW_ITEM},
        {"_Unload plugin", NULL, hview_unload_plugin_call, w, HVIEW_ITEM},
        {"_Help",	NULL, NULL, NULL, HVIEW_BRANCH},
        {"_About",	"info.png", hview_about_call,	NULL, HVIEW_ITEM}
      };

      number_items = sizeof(menu_items)/sizeof(menu_items[0]);

      mbar = gtk_menu_bar_new();

      for (i = 0; i < number_items; i++) {
	    type = menu_items[i].type;
	    label = menu_items[i].label;
	    image_file = menu_items[i].image_file;
	    callback = menu_items[i].callback;
	    data = menu_items[i].data;
	    switch (menu_items[i].type) {
	      case HVIEW_BRANCH:
		  item = gtk_menu_item_new_with_mnemonic(label);
		  gtk_container_add(GTK_CONTAINER(mbar), item);
		  menu = gtk_menu_new();
		  gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), menu);
		  break;
	      case HVIEW_ITEM:
		  sitem = gtk_image_menu_item_new_with_mnemonic(label);
		  if (image_file) {
			image = create_pixmap(image_file);
			gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM
						            (sitem), image);
		  }
		  if (callback)
			g_signal_connect(G_OBJECT(sitem), "activate",
					 G_CALLBACK(callback),
					 data);
		  gtk_container_add(GTK_CONTAINER(menu), sitem);
		  break;
	    }
      }
      return mbar;
}

	/* Create toolbar */

static GtkWidget *hview_get_toolbar(HviewWidgetsT *w)
{
      GtkWidget		*tbar;
      GtkWidget		*iconw;
      GtkToolItem	*titem;
      GtkTooltips	*tooltips;

      tbar = gtk_toolbar_new();

      gtk_toolbar_set_orientation(GTK_TOOLBAR(tbar),
				  GTK_ORIENTATION_HORIZONTAL);
      gtk_toolbar_set_style(GTK_TOOLBAR(tbar), GTK_TOOLBAR_BOTH);

      iconw = create_pixmap("close.png");
      titem = gtk_tool_button_new(iconw, "Close");
      g_signal_connect(G_OBJECT(titem), "clicked",
		       G_CALLBACK(hview_quit_call),
		       (gpointer) w);
      gtk_toolbar_insert(GTK_TOOLBAR(tbar), titem, -1);

      tooltips = gtk_tooltips_new();
      gtk_tool_item_set_tooltip(titem, tooltips, "Close session", NULL);

      iconw = create_pixmap("discover.png");
      titem = gtk_tool_button_new(iconw, "Discover");
      g_signal_connect(G_OBJECT(titem), "clicked",
		       G_CALLBACK(hview_discover_call),
		       (gpointer) w);
      gtk_toolbar_insert(GTK_TOOLBAR(tbar), titem, -1);

      tooltips = gtk_tooltips_new();
      gtk_tool_item_set_tooltip(titem, tooltips, "Discover", NULL);

      return tbar;
}

	/* Create "tree window" */

static GtkWidget *hview_get_tree_window(HviewWidgetsT *w)
{
      GtkWidget		*window;
      GtkCellRenderer	*renderer;
      GtkTreeViewColumn	*col;
      GtkTreeSelection	*selection;

      window = gtk_scrolled_window_new(NULL, NULL);
      gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(window),
				     GTK_POLICY_AUTOMATIC,
				     GTK_POLICY_AUTOMATIC);
      gtk_widget_set_size_request(GTK_WIDGET(window),
				  HVIEW_TREE_WINDOW_WIDTH,
				  HVIEW_TREE_WINDOW_HEIGHT);


      w->tree_view = gtk_tree_view_new();

      renderer = gtk_cell_renderer_text_new();
      gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(w->tree_view),
						  -1,
						  HVIEW_DOMAIN_COLUMN_TITLE,
						  renderer,
						  "text", 0, NULL);

      col = gtk_tree_view_get_column(GTK_TREE_VIEW(w->tree_view), 0);
      gtk_tree_view_column_set_clickable(col, TRUE);


      g_signal_connect(G_OBJECT(col), "clicked",
		       G_CALLBACK(hview_tree_column_activated_call),
		       (gpointer) w);

      g_signal_connect(G_OBJECT(w->tree_view), "row-activated",
		       G_CALLBACK(hview_tree_row_activated_call),
		       (gpointer) w);

      selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(w->tree_view));

      g_signal_connect(G_OBJECT(selection), "changed",
		       G_CALLBACK(hview_tree_row_selected_call),
		       (gpointer) w);


      gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(window),
					    w->tree_view);
      return window;
}

	/* Create "detail window" */

static GtkWidget *hview_get_detail_window(HviewWidgetsT *w)
{
      GtkWidget		*window;
      GtkCellRenderer	*renderer;
      GtkTreeViewColumn	*col;

      window = gtk_scrolled_window_new(NULL, NULL);
      gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(window),
				     GTK_POLICY_AUTOMATIC,
				     GTK_POLICY_AUTOMATIC);

      gtk_widget_set_size_request(GTK_WIDGET(window),
				  HVIEW_DETAIL_WINDOW_WIDTH,
				  HVIEW_DETAIL_WINDOW_HEIGHT);

      w->detail_view = gtk_tree_view_new();
      gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(w->detail_view),
					FALSE);

      renderer = gtk_cell_renderer_text_new();
      gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(w->detail_view),
						  -1,
						  NULL,
						  renderer,
						  "text", 0, NULL);
      col = gtk_tree_view_get_column(GTK_TREE_VIEW(w->detail_view), 0);
      gtk_tree_view_column_set_min_width(col, 150);
      

      renderer = gtk_cell_renderer_text_new();
      gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(w->detail_view),
						  -1,
						  NULL,
						  renderer,
						  "text", 1, NULL);
      
      gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(window),
					    w->detail_view);
      return window;
}

	/* Create "log window" */

static GtkWidget *hview_get_log_window(HviewWidgetsT *w)
{
      GtkWidget		*window;
      GtkTextBuffer	*buf;

      window = gtk_scrolled_window_new(NULL, NULL);
      gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(window),
				     GTK_POLICY_AUTOMATIC,
				     GTK_POLICY_AUTOMATIC);
      
      gtk_widget_set_size_request(GTK_WIDGET(window),
				  HVIEW_LOG_WINDOW_WIDTH,
				  HVIEW_LOG_WINDOW_HEIGHT);

      w->log_view = gtk_text_view_new();

      buf = gtk_text_buffer_new(NULL);
      gtk_text_view_set_buffer(GTK_TEXT_VIEW(w->log_view), buf);
      gtk_text_view_set_editable(GTK_TEXT_VIEW(w->log_view), FALSE);
      gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(w->log_view), FALSE);
      gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(window),
					    w->log_view);

      hview_print(w, HVIEW_NAME_VERSION);

      return window;
}

	/* Initialization all Hpi View widgets */

static void hview_init(void)
{
      HviewWidgetsT	widgets;
      GtkWidget		*menubar;
      GtkWidget		*vpaned;
      GtkWidget		*hpaned;
      GtkWidget		*main_vbox;
      GtkTreeModel	*domains;
      gchar		err[100];

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
      gtk_container_set_border_width(GTK_CONTAINER(main_vbox), 1);
      gtk_container_add(GTK_CONTAINER(widgets.main_window), main_vbox);

      menubar = hview_get_menubar(&widgets);
      gtk_box_pack_start(GTK_BOX(main_vbox), menubar, FALSE, FALSE, 0);

      widgets.toolbar = hview_get_toolbar(&widgets);
      gtk_box_pack_start(GTK_BOX(main_vbox), widgets.toolbar, FALSE, FALSE, 0);

      vpaned = gtk_vpaned_new();
      hpaned = gtk_hpaned_new();

// Create scrolled "tree window" and add it to horizontal paned window --------
      widgets.tree_window = hview_get_tree_window(&widgets);
      gtk_paned_add1(GTK_PANED(hpaned), widgets.tree_window);
//-----------------------------------------------------------------------------

// Create scrolled "detail window" and add it to hpaned window ----------------
      widgets.detail_window = hview_get_detail_window(&widgets);
      gtk_paned_add2(GTK_PANED(hpaned), widgets.detail_window);
//-----------------------------------------------------------------------------

// Create scrolled "log window" and add it with hpaned window to vertical paned
      widgets.log_window = hview_get_log_window(&widgets);
      gtk_paned_add1(GTK_PANED(vpaned), hpaned);
      gtk_paned_add2(GTK_PANED(vpaned), widgets.log_window);
//-----------------------------------------------------------------------------

      gtk_box_pack_start(GTK_BOX(main_vbox), vpaned, TRUE, TRUE, 0);

// Create statusbar------------------------------------------------------------
      widgets.statusbar = gtk_statusbar_new();
      gtk_box_pack_start(GTK_BOX(main_vbox), widgets.statusbar,
			 FALSE, FALSE, 0);
      hview_statusbar_push(&widgets, "wellcome");
//-----------------------------------------------------------------------------

      gtk_widget_show_all(widgets.main_window);

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

