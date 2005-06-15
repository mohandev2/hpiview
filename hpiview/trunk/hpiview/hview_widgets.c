#include <gtk/gtk.h>

#include "hview_utils.h"
#include "hview_service.h"
#include "hview_callbacks.h"
#include "hview_widgets.h"
#include "voh.h"

GtkWidget *hwidget_get_menu_item(const gchar *image_file,
				 const gchar *label,
				 gpointer callback,
				 gpointer data)
{
	GtkWidget	*item;
	GtkWidget	*pixmap;

	item = gtk_image_menu_item_new_with_mnemonic(label);

	if (image_file) {
		pixmap = hutil_create_pixmap(image_file);
		gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item),
					      pixmap);
	}

	if (callback) {
		g_signal_connect(G_OBJECT(item), "activate",
				 G_CALLBACK(callback), data);
	}

	return item;
}

GtkWidget *hwidget_get_menu_item_from_stock(const gchar *stock_id,
					    gpointer callback,
					    gpointer data)
{
	GtkWidget	*item;

	item = gtk_image_menu_item_new_from_stock(stock_id, NULL);

	if (callback) {
		g_signal_connect(G_OBJECT(item), "activate",
				 G_CALLBACK(callback), data);
	}

	return item;
}

GtkToolItem *hwidget_get_tool_button(const gchar *image_file,
				     const gchar *label,
				     const gchar *tooltips_string,
				     gpointer callback,
				     gpointer data,
				     guint type)
{
	GtkToolItem	*button;
	GtkWidget	*pixmap;
	GtkTooltips	*tooltips;

	switch (type) {
		case HWIDGET_TOGGLE_TOOL_BUTTON:
			button = gtk_toggle_tool_button_new();
			if (callback) {
				g_signal_connect(G_OBJECT(button), "toggled",
						 G_CALLBACK(callback), data);
			}
			break;
		case HWIDGET_TOOL_BUTTON:
			button = gtk_tool_button_new(NULL, NULL);
			if (callback) {
				g_signal_connect(G_OBJECT(button), "clicked",
						 G_CALLBACK(callback), data);
			}
			break;
		default:
			return NULL;
	}

	if (image_file) {
		pixmap = hutil_create_pixmap(image_file);
		gtk_tool_button_set_icon_widget(GTK_TOOL_BUTTON(button),
						pixmap);
	}

	if (label) {
		gtk_tool_button_set_label(GTK_TOOL_BUTTON(button), label);
	}

	if (tooltips_string) {
		tooltips = gtk_tooltips_new();
		gtk_tool_item_set_tooltip(button, tooltips, tooltips_string,
					  NULL);
	}

	return button;
}


GtkWidget *hwidget_get_toggle_toolbar(HviewWidgetsT *w)
{
	GtkWidget	*bar;
	GtkToolItem	*but;

	bar = gtk_toolbar_new();
	gtk_toolbar_set_style(GTK_TOOLBAR(bar), GTK_TOOLBAR_BOTH_HORIZ);

	but = hwidget_get_tool_button("message_log.png", "Messages", NULL,
				      hview_toggled_call, w,
				      HWIDGET_TOGGLE_TOOL_BUTTON);
	gtk_tool_item_set_is_important(GTK_TOOL_ITEM(but), TRUE);
	gtk_toolbar_insert(GTK_TOOLBAR(bar), but, -1);

	but = hwidget_get_tool_button("event_log.png", "Events", NULL,
				      hview_toggled_call, w,
				      HWIDGET_TOGGLE_TOOL_BUTTON);
	gtk_tool_item_set_is_important(GTK_TOOL_ITEM(but), TRUE);
	gtk_toolbar_insert(GTK_TOOLBAR(bar), but, -1);

      return bar;
}

GtkWidget *hwidget_get_toolbar(HviewWidgetsT *w)
{
	GtkWidget		*tbar;
	GtkToolItem		*but;

	tbar = gtk_toolbar_new();
	gtk_toolbar_set_style(GTK_TOOLBAR(tbar), GTK_TOOLBAR_BOTH);

	but = hwidget_get_tool_button("close.png", "Close", "Close session",
		      		      hview_session_close_call, w,
				      HWIDGET_TOOL_BUTTON);
	gtk_toolbar_insert(GTK_TOOLBAR(tbar), but, -1);

	but = hwidget_get_tool_button("discover.png", "Discover",
				      "Discover domain", hview_discover_call,
				      w, HWIDGET_TOOL_BUTTON);
	gtk_toolbar_insert(GTK_TOOLBAR(tbar), but, -1);

	but = hwidget_get_tool_button("sub_events.png", "Subscribe",
				       "Subscribe events",
				       hview_subscribe_events_call,
				       w,
				       HWIDGET_TOOL_BUTTON);
	gtk_toolbar_insert(GTK_TOOLBAR(tbar), but, -1);
	w->subev_item = but;

	but = hwidget_get_tool_button("get_events.png", "Get event",
				      "Get event", hview_get_events_call,
				      w, HWIDGET_TOOL_BUTTON);
	gtk_toolbar_insert(GTK_TOOLBAR(tbar), but, -1);

	return tbar;
}

GtkWidget *hwidget_get_vtoolbar(HviewWidgetsT *w)
{
      GtkWidget		*tbar;
      GtkToolItem	*but;

      tbar = gtk_toolbar_new();

      gtk_toolbar_set_orientation(GTK_TOOLBAR(tbar),
				  GTK_ORIENTATION_VERTICAL);
      gtk_toolbar_set_style(GTK_TOOLBAR(tbar), GTK_TOOLBAR_ICONS);

      but = hwidget_get_tool_button("hide.png", "Hide", "Hide domain list",
		      		    hview_show_hide_domain_call,
				    w,
				    HWIDGET_TOOL_BUTTON);
      gtk_toolbar_insert(GTK_TOOLBAR(tbar), but, -1);

      but = hwidget_get_tool_button("open.png", "Open", "Open new session",
		      		    hview_open_session_call, w,
				    HWIDGET_TOOL_BUTTON);
      gtk_toolbar_insert(GTK_TOOLBAR(tbar), but, -1);

      return tbar;
}

GtkWidget *hwidget_get_menubar(HviewWidgetsT *w)
{
	GtkWidget		*mbar;
	GtkWidget		*menu;
	GtkWidget		*item,	*sitem;
	gchar			*label,	*image_file;
	guint			type,	number_items;
	gpointer		callback, data;
	gint			i;

	HwidgetMenuItemFactoryT menu_items[] = {
	{"_Session",	NULL, 		NULL,		NULL, HWIDGET_BRANCH},
	{"_Quit",	"exit.png",	hview_quit_call, w, HWIDGET_ITEM},
	{"_Edit",	NULL,		NULL, 		NULL, HWIDGET_BRANCH},
	{"_Clear log",	"clear.png",	hview_empty_log_call, w, HWIDGET_ITEM},
	{"_Action",	NULL,		NULL,		NULL, HWIDGET_BRANCH},
	{"_Discover",	"discover.png",	hview_discover_call, w, HWIDGET_ITEM},
	{"_Load plugin", NULL,		hview_load_plugin_call, w,
								HWIDGET_ITEM},
	{"_Unload plugin", NULL,	hview_unload_plugin_call, w,
								HWIDGET_ITEM},
	{"_Help",	NULL,		NULL,		NULL, HWIDGET_BRANCH},
	{"_About",	"info.png",	hview_about_call, w, HWIDGET_ITEM}
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
		case HWIDGET_BRANCH:
			item = hwidget_get_menu_item(NULL, label, NULL, NULL);
			gtk_container_add(GTK_CONTAINER(mbar), item);
			menu = gtk_menu_new();
			gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), menu);
			break;
		case HWIDGET_ITEM:
			sitem = hwidget_get_menu_item(image_file, label,
						      G_CALLBACK(callback),
						      data);
			gtk_container_add(GTK_CONTAINER(menu), sitem);
			break;
		}
	}

	return mbar;
}

GtkWidget *hwidget_get_iter_popup(GtkTreeModel *store,
				  GtkTreeIter *iter,
				  gpointer data)
{
	HviewWidgetsT	*w = (HviewWidgetsT *) data;
	GtkWidget	*menu,	*smenu;
	GtkWidget	*item,	*sitem;
	guint		type;
	guint		capability;

	if (store == NULL || iter == NULL)
		return NULL;

	gtk_tree_model_get(store, iter, VOH_LIST_COLUMN_TYPE, &type, -1);

	switch (type) {
	case VOH_ITER_IS_DOMAIN:
		menu = gtk_menu_new();

		sitem = hwidget_get_menu_item(NULL, "Domain event log",
					      hview_domain_evlog_call,
					      data);
		gtk_container_add(GTK_CONTAINER(menu), sitem);

		sitem = hwidget_get_menu_item(NULL, "Event log timestamp",
					      hview_domain_evlog_time_call,
					      data);
		gtk_container_add(GTK_CONTAINER(menu), sitem);

		sitem = hwidget_get_menu_item(NULL, "Event log clear",
					      hview_domain_evlog_clear_call,
					      data);
		gtk_container_add(GTK_CONTAINER(menu), sitem);

		sitem = gtk_separator_menu_item_new();
		gtk_container_add(GTK_CONTAINER(menu), sitem);

		sitem = hwidget_get_menu_item_from_stock(GTK_STOCK_PREFERENCES,
						hview_domain_settings_call,
						data);
		gtk_container_add(GTK_CONTAINER(menu), sitem);
		
		return menu;
	case VOH_ITER_IS_RPT:
		gtk_tree_model_get(store, iter,
				   VOH_LIST_COLUMN_CAPABILITY, &capability, -1);
		menu = gtk_menu_new();
		if (capability & VOH_ITER_CAPABILITY_POWER) {
			smenu = gtk_menu_new();
			item = hwidget_get_menu_item("power.png", "power",
						     NULL, NULL);
			gtk_container_add(GTK_CONTAINER(menu), item);
			gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), smenu);

			sitem = hwidget_get_menu_item("on.png", "on",
						      hview_set_power_on_call,
						      data);
			gtk_container_add(GTK_CONTAINER(smenu), sitem);

			sitem = hwidget_get_menu_item("off.png", "off",
						      hview_set_power_off_call,
						      data);
			gtk_container_add(GTK_CONTAINER(smenu), sitem);

			sitem = hwidget_get_menu_item(NULL, "off/on",
						     hview_set_power_cycle_call,
						     data);
			gtk_container_add(GTK_CONTAINER(smenu), sitem);
		}

		if (capability & VOH_ITER_CAPABILITY_RESET) {
			smenu = gtk_menu_new();

			item = hwidget_get_menu_item("reset.png", "reset",
						     NULL, NULL);
			gtk_container_add(GTK_CONTAINER(menu), item);
			gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), smenu);

			sitem = hwidget_get_menu_item("cold_reset.png", "cold",
						      hview_reset_cold_call,
						      data);
			gtk_container_add(GTK_CONTAINER(smenu), sitem);

			sitem = hwidget_get_menu_item("warm_reset.png", "warm",
						      hview_reset_warm_call,
						      data);
			gtk_container_add(GTK_CONTAINER(smenu), sitem);

			sitem = hwidget_get_menu_item(NULL, "assert",
						      hview_reset_assert_call,
						      data);
			gtk_container_add(GTK_CONTAINER(smenu), sitem);

			sitem = hwidget_get_menu_item(NULL, "deassert",
						      hview_reset_deassert_call,
						      data);
			gtk_container_add(GTK_CONTAINER(smenu), sitem);
		}

		sitem = gtk_separator_menu_item_new();
		gtk_container_add(GTK_CONTAINER(menu), sitem);

		sitem = hwidget_get_menu_item_from_stock(GTK_STOCK_PREFERENCES,
							hview_rpt_settings_call,
							data);
		gtk_container_add(GTK_CONTAINER(menu), sitem);
		return menu;
	case VOH_ITER_IS_SENSOR:
		menu = gtk_menu_new();

		sitem = hwidget_get_menu_item("viewmag.png", "Read sensor",
					      hview_read_sensor_call, data);
		gtk_container_add(GTK_CONTAINER(menu), sitem);

		sitem = gtk_separator_menu_item_new();
		gtk_container_add(GTK_CONTAINER(menu), sitem);

		sitem = hwidget_get_menu_item_from_stock(GTK_STOCK_PREFERENCES,
						hview_sensor_settings_call,
						data);
		gtk_container_add(GTK_CONTAINER(menu), sitem); 
		return menu;
	case VOH_ITER_IS_CONTROL:
		menu = gtk_menu_new();

		sitem = gtk_separator_menu_item_new();
		gtk_container_add(GTK_CONTAINER(menu), sitem);

		sitem = hwidget_get_menu_item_from_stock(GTK_STOCK_PREFERENCES,
						hview_control_settings_call,
						data);
		gtk_container_add(GTK_CONTAINER(menu), sitem);
		return menu;
	case VOH_ITER_IS_INVENTORY:
		menu = gtk_menu_new();

		sitem = gtk_separator_menu_item_new();
		gtk_container_add(GTK_CONTAINER(menu), sitem);

		sitem = hwidget_get_menu_item_from_stock(GTK_STOCK_PREFERENCES,
						hview_inventory_settings_call,
						data);
		gtk_container_add(GTK_CONTAINER(menu), sitem); 
		return menu;
	case VOH_ITER_IS_WATCHDOG:
		menu = gtk_menu_new();

		sitem = hwidget_get_menu_item(NULL, "Reset watchdog",
					      hview_watchdog_reset_call, data);
		gtk_container_add(GTK_CONTAINER(menu), sitem);

		sitem = gtk_separator_menu_item_new();
		gtk_container_add(GTK_CONTAINER(menu), sitem);

		sitem = hwidget_get_menu_item_from_stock(GTK_STOCK_PREFERENCES,
						hview_watchdog_settings_call,
						data);
		gtk_container_add(GTK_CONTAINER(menu), sitem);
		return menu;
	case VOH_ITER_IS_ANNUNCIATOR:
	default:
		return NULL;
	}
}

