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
#include "hview_callbacks.h"
#include "voh.h"

static GList *pixmaps_directories = NULL;

void add_pixmap_directory(const gchar *directory)
{
      pixmaps_directories = g_list_prepend (pixmaps_directories,
					    g_strdup (directory));
}

gchar* find_pixmap_file(const gchar *filename)
{
      GList *elem;

      elem = pixmaps_directories;
      
      while (elem) {
	    gchar *pathname = g_strdup_printf ("%s%s%s", (gchar*)elem->data,
					       G_DIR_SEPARATOR_S, filename);

	    if (g_file_test (pathname, G_FILE_TEST_EXISTS))
		  return pathname;

      	    g_free (pathname);

      	    elem = elem->next;
      }
  
      return NULL;
}

GtkWidget *create_pixmap(const gchar *filename)
{
      gchar *pathname = NULL;
      GtkWidget *pixmap;

      if (!filename || !filename[0])
	    return gtk_image_new ();

      pathname = find_pixmap_file (filename);

      if (!pathname)
	  {
	      g_warning ("Couldn't find pixmap file: %s", filename);
	      return gtk_image_new ();
	  }

      pixmap = gtk_image_new_from_file (pathname);

      g_free (pathname);

      return pixmap;
}


	/* Create menu bar */

GtkWidget *hview_get_menubar(HviewWidgetsT *w)
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

GtkWidget *hview_get_toolbar(HviewWidgetsT *w)
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
		       G_CALLBACK(hview_session_close_call),
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

      iconw = create_pixmap("viewmag.png");
      w->rsitem = gtk_tool_button_new(iconw, "Read sensor");
      g_signal_connect(G_OBJECT(w->rsitem), "clicked",
		       G_CALLBACK(hview_read_sensor_call),
		       (gpointer) w);
      gtk_toolbar_insert(GTK_TOOLBAR(tbar), w->rsitem, -1);

      tooltips = gtk_tooltips_new();
      gtk_tool_item_set_tooltip(w->rsitem, tooltips, "Read Sensor", NULL);
      gtk_widget_set_state(GTK_WIDGET(w->rsitem), GTK_STATE_INSENSITIVE);

      return tbar;
}

	/* Create vertical toolbar */

GtkWidget *hview_get_vtoolbar(HviewWidgetsT *w)
{
      GtkWidget		*tbar;
      GtkWidget		*iconw;
      GtkToolItem	*titem;
      GtkTooltips	*tooltips;

      tbar = gtk_toolbar_new();

      gtk_toolbar_set_orientation(GTK_TOOLBAR(tbar),
				  GTK_ORIENTATION_VERTICAL);
      gtk_toolbar_set_style(GTK_TOOLBAR(tbar), GTK_TOOLBAR_ICONS);


      iconw = create_pixmap("hide.png");
      titem = gtk_tool_button_new(iconw, "hide domain list");
      g_signal_connect(G_OBJECT(titem), "clicked",
		       G_CALLBACK(hview_show_hide_domain_call),
		       (gpointer) w);
      gtk_toolbar_insert(GTK_TOOLBAR(tbar), titem, -1);

      tooltips = gtk_tooltips_new();
      gtk_tool_item_set_tooltip(titem, tooltips, "Hide domain list", NULL);

      iconw = create_pixmap("open.png");
      titem = gtk_tool_button_new(iconw, "open new session");
      g_signal_connect(G_OBJECT(titem), "clicked",
		       G_CALLBACK(hview_open_session_call),
		       (gpointer) w);
      gtk_toolbar_insert(GTK_TOOLBAR(tbar), titem, -1);

      tooltips = gtk_tooltips_new();
      gtk_tool_item_set_tooltip(titem, tooltips, "Open new session", NULL);

      return tbar;
}

	/* Create "domain tree window" */

GtkWidget *hview_get_domain_window(HviewWidgetsT *w)
{
      GtkWidget		*window;
      GtkCellRenderer	*renderer;
      GtkTreeViewColumn	*col;
      GtkTreeModel	*domainstore = NULL;
      gchar		err[1024];

      window = gtk_scrolled_window_new(NULL, NULL);
      gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(window),
				     GTK_POLICY_AUTOMATIC,
				     GTK_POLICY_AUTOMATIC);

      gtk_widget_set_size_request(GTK_WIDGET(window),
				  175,
				  HVIEW_TREE_WINDOW_HEIGHT);

      w->domain_view = gtk_tree_view_new();

      renderer = gtk_cell_renderer_text_new();
      gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(w->domain_view),
						  VOH_LIST_COLUMN_NAME,
						  HVIEW_DOMAIN_COLUMN_TITLE,
						  renderer,
						  "text",
						  VOH_LIST_COLUMN_NAME, NULL);
      col = gtk_tree_view_get_column(GTK_TREE_VIEW(w->domain_view),
				     VOH_LIST_COLUMN_NAME);
      gtk_tree_view_column_set_cell_data_func(col, renderer,
					      hview_tree_cell_func, NULL, NULL);

      renderer = gtk_cell_renderer_pixbuf_new();
      gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(w->domain_view),
						  VOH_LIST_COLUMN_ICON,
						  NULL,
						  renderer,
						  "pixbuf",
						  VOH_LIST_COLUMN_ICON, NULL);
      col = gtk_tree_view_get_column(GTK_TREE_VIEW(w->domain_view),
				     VOH_LIST_COLUMN_ICON);
      gtk_tree_view_column_set_cell_data_func(col, renderer,
					      hview_tree_pixbuf_cell_func,
					      NULL, NULL);

//      gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(w->domain_view),
//					FALSE);

      domainstore = voh_list_domains(err);
      if (domainstore == NULL) {
//	    hview_print(w, err);
	    domainstore = GTK_TREE_MODEL(gtk_tree_store_new(1,
							    G_TYPE_STRING));
	    gtk_tree_view_set_model(GTK_TREE_VIEW(w->domain_view),
				    domainstore);
	    g_object_unref(domainstore);
      } else {
	    gtk_tree_view_set_model(GTK_TREE_VIEW(w->domain_view),
				    domainstore);
	    g_object_unref(domainstore);
      }

      g_signal_connect(G_OBJECT(w->domain_view), "row-activated",
		       G_CALLBACK(hview_domain_row_activated_call),
		       (gpointer) w);

      gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(window),
					    w->domain_view);
      return window;
}

	/* Create "tree window" */

GtkWidget *hview_get_tree_window(HviewWidgetsT *w, gint page)
{
      GtkWidget		*window;
      GtkCellRenderer	*renderer;
      GtkTreeViewColumn	*col;
      GtkTreeSelection	*selection;
      GtkWidget		*view;

      window = gtk_scrolled_window_new(NULL, NULL);
      gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(window),
				     GTK_POLICY_AUTOMATIC,
				     GTK_POLICY_AUTOMATIC);

      gtk_widget_set_size_request(GTK_WIDGET(window),
				  HVIEW_TREE_WINDOW_WIDTH,
				  HVIEW_TREE_WINDOW_HEIGHT);


      w->tab_views[page].tree_view = view = gtk_tree_view_new();

      renderer = gtk_cell_renderer_text_new();
      gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view),
						  VOH_LIST_COLUMN_NAME,
						  HVIEW_RESOURCE_COLUMN_TITLE,
						  renderer,
						  "text",
						  VOH_LIST_COLUMN_NAME, NULL);
      col = gtk_tree_view_get_column(GTK_TREE_VIEW(view), VOH_LIST_COLUMN_NAME);
      gtk_tree_view_column_set_cell_data_func(col, renderer,
					      hview_tree_cell_func, NULL, NULL);

      renderer = gtk_cell_renderer_pixbuf_new();
      gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view),
						  VOH_LIST_COLUMN_ICON,
						  NULL,
						  renderer,
						  "pixbuf",
						  VOH_LIST_COLUMN_ICON, NULL);
      col = gtk_tree_view_get_column(GTK_TREE_VIEW(view),
				     VOH_LIST_COLUMN_ICON);
      gtk_tree_view_column_set_cell_data_func(col, renderer,
					      hview_tree_pixbuf_cell_func,
					      NULL, NULL);

      gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(view),
					FALSE);

      selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(view));

      g_signal_connect(G_OBJECT(selection), "changed",
		       G_CALLBACK(hview_tree_row_selected_call),
		       (gpointer) w);

      g_signal_connect(G_OBJECT(view), "button-press-event",
		       G_CALLBACK(hview_button_prees_treeview_call),
		       (gpointer) w);


      gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(window),
					    view);
      return window;
}

	/* Create "detail window" */

GtkWidget *hview_get_detail_window(HviewWidgetsT *w, gint page)
{
      GtkWidget		*window;
      GtkCellRenderer	*renderer;
      GtkTreeViewColumn	*col;
      GtkWidget		*view;

      window = gtk_scrolled_window_new(NULL, NULL);
      gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(window),
				     GTK_POLICY_AUTOMATIC,
				     GTK_POLICY_AUTOMATIC);

      gtk_widget_set_size_request(GTK_WIDGET(window),
				  HVIEW_DETAIL_WINDOW_WIDTH,
				  HVIEW_DETAIL_WINDOW_HEIGHT);

      w->tab_views[page].detail_view = view = gtk_tree_view_new();
      gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(view),
					FALSE);

      renderer = gtk_cell_renderer_text_new();
      gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view),
						  -1,
						  NULL,
						  renderer,
						  "text", 0, NULL);
      col = gtk_tree_view_get_column(GTK_TREE_VIEW(view), 0);
      gtk_tree_view_column_set_cell_data_func(col, renderer,
					      hview_detail_cell_func,
					      NULL, NULL);

      renderer = gtk_cell_renderer_text_new();
      gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view),
						  -1,
						  NULL,
						  renderer,
						  "text", 1, NULL);
      
      gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(window),
					    view);
      return window;
}

	/* Create "log window" */

GtkWidget *hview_get_log_window(HviewWidgetsT *w)
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

	/* Create "event window" */

GtkWidget *hview_get_event_window(HviewWidgetsT *w)
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

      w->event_view = gtk_text_view_new();

      buf = gtk_text_buffer_new(NULL);
      gtk_text_view_set_buffer(GTK_TEXT_VIEW(w->event_view), buf);
      gtk_text_view_set_editable(GTK_TEXT_VIEW(w->event_view), FALSE);
      gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(w->event_view), FALSE);
      gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(window),
					    w->event_view);

      hview_event_print(w, "no events (not supported yet)");

      return window;
}

GtkWidget *hview_get_tree_popup(GtkTreeModel *store,
				GtkTreeIter *iter,
				gpointer data)
{
      HviewWidgetsT	*w = (HviewWidgetsT *) data;
      GtkWidget		*menu,	*smenu;
      GtkWidget		*item,	*sitem;
      GtkWidget		*image;
      guint		type;
      guint		id;
      guint		state;
      gboolean		rv;

      if (store == NULL || iter == NULL)
	    return NULL;
      gtk_tree_model_get(store, iter,
			 VOH_LIST_COLUMN_TYPE, &type, -1);

      switch (type) {
	case VOH_ITER_IS_RPT:
	    gtk_tree_model_get(store, iter, VOH_LIST_COLUMN_ID, &id,
			      VOH_LIST_COLUMN_STATE, &state, -1);
	    w->iter_id = id;
	    menu = gtk_menu_new();
	    if (state & VOH_ITER_RPT_STATE_POWER_ON ||
				state & VOH_ITER_RPT_STATE_POWER_OFF) {
		  smenu = gtk_menu_new();
		  item = gtk_image_menu_item_new_with_mnemonic("power");
			image = create_pixmap("power.png");

      		  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item),
						image);
		  gtk_container_add(GTK_CONTAINER(menu), item);
		  gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), smenu);

		  sitem = gtk_image_menu_item_new_with_mnemonic("on");
		  g_signal_connect(G_OBJECT(sitem), "activate",
					 G_CALLBACK(hview_set_power_on_call),
					 data);
		  image = create_pixmap("on.png");
		  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(sitem),
						image);
		  gtk_container_add(GTK_CONTAINER(smenu), sitem);

		  sitem = gtk_image_menu_item_new_with_mnemonic("off");
		  g_signal_connect(G_OBJECT(sitem), "activate",
					 G_CALLBACK(hview_set_power_off_call),
					 data);
		  image = create_pixmap("off.png");
		  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(sitem),
						image);
		  gtk_container_add(GTK_CONTAINER(smenu), sitem);

		  sitem = gtk_menu_item_new_with_mnemonic("off/on");
		  g_signal_connect(G_OBJECT(sitem), "activate",
					 G_CALLBACK(hview_set_power_cycle_call),
					 data);
		  gtk_container_add(GTK_CONTAINER(smenu), sitem);
	    }

	    if (state & VOH_ITER_RPT_STATE_RESET_ASSERT ||
				state & VOH_ITER_RPT_STATE_RESET_DEASSERT) {
		  smenu = gtk_menu_new();
		  item = gtk_image_menu_item_new_with_mnemonic("reset");
			image = create_pixmap("reset.png");

      		  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item),
						image);
		  gtk_container_add(GTK_CONTAINER(menu), item);
		  gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), smenu);

		  sitem = gtk_image_menu_item_new_with_mnemonic("cold");
		  g_signal_connect(G_OBJECT(sitem), "activate",
					 G_CALLBACK(hview_set_reset_cold_call),
					 data);
		  image = create_pixmap("cold_reset.png");
		  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(sitem),
						image);
		  gtk_container_add(GTK_CONTAINER(smenu), sitem);

		  sitem = gtk_image_menu_item_new_with_mnemonic("warm");
		  g_signal_connect(G_OBJECT(sitem), "activate",
					 G_CALLBACK(hview_set_reset_warm_call),
					 data);
		  image = create_pixmap("warm_reset.png");
		  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(sitem),
						image);
		  gtk_container_add(GTK_CONTAINER(smenu), sitem);

		  sitem = gtk_menu_item_new_with_mnemonic("assert");
		  g_signal_connect(G_OBJECT(sitem), "activate",
					 G_CALLBACK(hview_reset_assert_call),
					 data);
		  gtk_container_add(GTK_CONTAINER(smenu), sitem);

		  sitem = gtk_menu_item_new_with_mnemonic("deassert");
		  g_signal_connect(G_OBJECT(sitem), "activate",
					 G_CALLBACK(hview_reset_deassert_call),
					 data);
		  gtk_container_add(GTK_CONTAINER(smenu), sitem);
	    }
	    return menu;
	case VOH_ITER_IS_SENSOR:
	    menu = gtk_menu_new();
	    sitem = gtk_image_menu_item_new_with_mnemonic("read sensor");
	    image = create_pixmap("viewmag.png");
	    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(sitem), image);
	    g_signal_connect(G_OBJECT(sitem), "activate",
			     G_CALLBACK(hview_read_sensor_call), data);
	    gtk_container_add(GTK_CONTAINER(menu), sitem);
	    break;
	case VOH_ITER_IS_CONTROL:
	case VOH_ITER_IS_INVENTORY:
	case VOH_ITER_IS_WATCHDOG:
	case VOH_ITER_IS_ANNUNCIATOR:
	default:
	    return NULL;
      }
      return menu;
}


	/* hview_print() puts message to "log window"
	 * arguments:
	 * 	const gchar string: null-terminated string
	 * 			    that should be in "log window"
	 * return value: none
	 */

void hview_print(HviewWidgetsT *w, const gchar *string)
{
      GtkTextBuffer	*buf;
      GtkTextIter	iter;

      buf = GTK_TEXT_BUFFER(gtk_text_view_get_buffer
			    		(GTK_TEXT_VIEW(w->log_view)));
      gtk_text_buffer_get_end_iter(buf, &iter);
      gtk_text_buffer_insert(buf, &iter, string, -1);
      gtk_text_buffer_insert(buf, &iter, "\n", -1);
      gtk_text_view_set_buffer(GTK_TEXT_VIEW(w->log_view), buf);
}

void hview_event_print(HviewWidgetsT *w, const gchar *string)
{
      GtkTextBuffer	*buf;
      GtkTextIter	iter;

      buf = GTK_TEXT_BUFFER(gtk_text_view_get_buffer
			    		(GTK_TEXT_VIEW(w->event_view)));
      gtk_text_buffer_get_end_iter(buf, &iter);
      gtk_text_buffer_insert(buf, &iter, string, -1);
      gtk_text_buffer_insert(buf, &iter, "\n", -1);
      gtk_text_view_set_buffer(GTK_TEXT_VIEW(w->event_view), buf);
}

void hview_statusbar_push(HviewWidgetsT *w, const gchar *str)
{
      guint	contid;

      gtk_statusbar_push(GTK_STATUSBAR(w->statusbar), contid, str);
}

void hview_tree_cell_func(GtkTreeViewColumn	*col,
			 GtkCellRenderer	*renderer,
			 GtkTreeModel		*model,
			 GtkTreeIter		*iter,
			 gpointer		data)
{
      guint		type,	state;

      g_object_set(renderer, "foreground-set", FALSE,
		   "weight-set", FALSE, "style-set", FALSE, NULL);

      gtk_tree_model_get(model, iter, VOH_LIST_COLUMN_TYPE, &type,
			 VOH_LIST_COLUMN_STATE, &state, -1);

      switch (type) {
	case VOH_ITER_IS_DOMAIN:
	    g_object_set(renderer, "weight", PANGO_WEIGHT_BOLD,
			 "weight-set", TRUE, NULL);
	    break;
	case VOH_ITER_IS_PATH:
	    g_object_set(renderer, "weight", PANGO_WEIGHT_BOLD,
			 "weight-set", TRUE, NULL);
	    break;
	case VOH_ITER_IS_RPT:
	    g_object_set(renderer, "weight", PANGO_WEIGHT_BOLD,
			 "weight-set", TRUE, NULL);
	    break;
	case VOH_ITER_IS_SENSOR:
	    switch (state) {
	      case VOH_ITER_SENSOR_STATE_NOMINAL:
		  g_object_set(renderer, "foreground", "Green",
			       "foreground-set", TRUE, NULL);
		  break;
	      case VOH_ITER_SENSOR_STATE_NORMAL:
		  g_object_set(renderer, "foreground", "Orange",
			       "foreground-set", TRUE, NULL);
		  break;
	      case VOH_ITER_SENSOR_STATE_CRITICAL:
		  g_object_set(renderer, "foreground", "Red",
			       "foreground-set", TRUE, NULL);
		  break;
	      default:
		  break;
	    }
	case VOH_ITER_IS_CONTROL:
	case VOH_ITER_IS_INVENTORY:
	case VOH_ITER_IS_WATCHDOG:
	case VOH_ITER_IS_ANNUNCIATOR:
	    g_object_set(renderer, "style", PANGO_STYLE_ITALIC,
			 "style-set", TRUE, NULL);
	    break;
	case VOH_ITER_IS_VALUE:

	    g_object_set(renderer, "weight", PANGO_WEIGHT_BOLD,
			 "weight-set", TRUE, NULL);

	    switch (state) {
	      case VOH_ITER_SENSOR_STATE_NOMINAL:
		  g_object_set(renderer, "foreground", "Green",
			       "foreground-set", TRUE, NULL);
		  break;
	      case VOH_ITER_SENSOR_STATE_NORMAL:
		  g_object_set(renderer, "foreground", "Orange",
			       "foreground-set", TRUE, NULL);
		  break;
	      case VOH_ITER_SENSOR_STATE_CRITICAL:
		  g_object_set(renderer, "foreground", "Red",
			       "foreground-set", TRUE, NULL);
		  break;
	      default:
		  break;
	    }
	    break;
	default:
	    break;
      }
}

void hview_tree_pixbuf_cell_func(GtkTreeViewColumn	*col,
			 GtkCellRenderer	*renderer,
			 GtkTreeModel		*model,
			 GtkTreeIter		*iter,
			 gpointer		data)
{
      GdkPixbuf		*image = NULL;
      guint		type,	state;
      GError		*error = NULL;

      gtk_tree_model_get(model, iter, VOH_LIST_COLUMN_TYPE, &type,
			 VOH_LIST_COLUMN_STATE, &state, -1);

      switch (type) {
	case VOH_ITER_IS_DOMAIN:
	    break;
	case VOH_ITER_IS_PATH:
	    break;
	case VOH_ITER_IS_RPT:
	    if (state & VOH_ITER_RPT_STATE_POWER_ON) {
		  image = gdk_pixbuf_new_from_file(find_pixmap_file("on.png"),
						   &error);
		  if (error) {
			g_critical ("Could not load pixbuf: %s\n",
				    error->message);
			g_error_free(error);
		  }
	    } else if (state & VOH_ITER_RPT_STATE_POWER_OFF) {
		  image = gdk_pixbuf_new_from_file(find_pixmap_file("off.png"),
						   &error);
		  if (error) {
			g_critical ("Could not load pixbuf: %s\n",
				    error->message);
			g_error_free(error);
		  }
	    }

	    if (image) {
		  gtk_tree_store_set(GTK_TREE_STORE(model), iter,
				     VOH_LIST_COLUMN_ICON, image, -1);
		  g_object_unref(image);
	    }

	    break;
	case VOH_ITER_IS_SENSOR:
	    break;
	case VOH_ITER_IS_CONTROL:
	case VOH_ITER_IS_INVENTORY:
	case VOH_ITER_IS_WATCHDOG:
	case VOH_ITER_IS_ANNUNCIATOR:
	    break;
	case VOH_ITER_IS_VALUE:
	    break;
	default:
	    break;
      }
}

void hview_detail_cell_func(GtkTreeViewColumn	*col,
			 GtkCellRenderer	*renderer,
			 GtkTreeModel		*model,
			 GtkTreeIter		*iter,
			 gpointer		data)
{
      GtkTreeIter	child;
      guint		type,	state;

      g_object_set(renderer, "foreground-set", FALSE,
		   "weight-set", FALSE, "style-set", FALSE, NULL);

      if (gtk_tree_model_iter_children(model, &child, iter) == TRUE) {
	    g_object_set(renderer, "foreground", "Blue",
			 "foreground-set", TRUE, NULL);
      }

      g_object_set(renderer, "weight", PANGO_WEIGHT_BOLD,
		   "weight-set", TRUE,
		   "style", PANGO_STYLE_ITALIC,
		   "style-set", TRUE, NULL);

}

