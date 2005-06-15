#ifndef __HVIEW_WIDGETS_H__
#define __HVIEW_WIDGETS_H__

GtkWidget *hwidget_get_menu_item(const gchar *image_file,
				 const gchar *label,
				 gpointer callback,
				 gpointer data);
GtkWidget *hwidget_get_menu_item_from_stock(const gchar *stock_id,
					    gpointer callback,
					    gpointer data);

enum {
	HWIDGET_TOGGLE_TOOL_BUTTON = 1,
	HWIDGET_TOOL_BUTTON,
};

GtkToolItem *hwidget_get_tool_button(const gchar *image_file,
				     const gchar *label,
				     const gchar *tooltips_string,
				     gpointer callback,
				     gpointer data,
				     guint type);


GtkWidget *hwidget_get_toggle_toolbar(HviewWidgetsT *w);
GtkWidget *hwidget_get_toolbar(HviewWidgetsT *w);
GtkWidget *hwidget_get_vtoolbar(HviewWidgetsT *w);


typedef struct HwidgetMenuItemFactory
{
      gchar		*label;
      gchar		*image_file;
      gpointer		callback;
      gpointer		data;
      guint		type;
} HwidgetMenuItemFactoryT;

enum {
	HWIDGET_BRANCH = 1,
	HWIDGET_ITEM,
};

GtkWidget *hwidget_get_menubar(HviewWidgetsT *w);

GtkWidget *hwidget_get_iter_popup(GtkTreeModel *store,
				  GtkTreeIter *iter,
				  gpointer data);

#endif /* __HVIEW_WIDGETS_H__ */
