#ifndef __HPIVIEW_H__
#define __HPIVIEW_H__

#define HVIEW_VERSION		2.0
#define HVIEW_NAME_VERSION	"OpenHpi View version: 2.0"

#define HVIEW_ABOUT	"OpenHpi View ver. 2.0\n Author: Denis Sadykov, 2004"

#define HVIEW_TITLE			"OpenHpi View"
#define HVIEW_DOMAIN_COLUMN_TITLE	"\tDomains"
#define HVIEW_RESOURCE_COLUMN_TITLE	"\tResources"

#define HVIEW_MAIN_WINDOW_WIDTH		500
#define HVIEW_MAIN_WINDOW_HEIGHT	400
#define HVIEW_LOG_WINDOW_WIDTH		HVIEW_MAIN_WINDOW_WIDTH
#define HVIEW_LOG_WINDOW_HEIGHT		150
#define HVIEW_DETAIL_WINDOW_WIDTH	300
#define HVIEW_DETAIL_WINDOW_HEIGHT	250
#define HVIEW_TREE_WINDOW_WIDTH		200
#define HVIEW_TREE_WINDOW_HEIGHT	HVIEW_DETAIL_WINDOW_HEIGHT
#define HVIEW_ABOUT_WINDOW_WIDTH	200
#define HVIEW_ABOUT_WINDOW_HEIGHT	100

#define HVIEW_TREE_STORE_IS_DOMAINS	0
#define HVIEW_TREE_STORE_IS_RESOURCES	1
#define HVIEW_TREE_STORE_IS_UNKNOWN	-1


static int hview_init(void);

static GtkWidget *hview_get_log_window(void);
static GtkWidget *hview_get_detail_window(void);
static GtkWidget *hview_get_tree_window(void);
static GtkWidget *hview_get_menubar();

static void hview_quit_call(void);
static void hview_clear_log_call(void);
static void hview_load_plugin_call(void);
static void hview_unload_plugin_call(void);
static void hview_about_call(void);
static void hview_tree_column_activated_call(GtkTreeViewColumn *column);
static void hview_tree_row_activated_call(void);
static void hview_tree_row_selected_call(GtkTreeSelection *selection);

static int hview_which_tree_store(GtkTreeViewColumn *column);
static void hview_print(const gchar *string);

#endif /* __HPIVIEW_H__ */
