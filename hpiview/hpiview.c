#include <gtk/gtk.h>
#include "hpiview.h"
#include "voh.h"
#include <pthread.h>
#include <signal.h>

static GtkWidget		*main_window;
static GtkWidget		*tree_window;
static GtkWidget		*detail_window;
static GtkWidget		*log_window;
static GtkWidget		*statusbar;
static GtkWidget		*toolbar;

static GtkWidget		*tree_view;
static GtkWidget		*detail_view;
static GtkWidget		*log_view;

static pthread_t	prog_thread;

	/* Menu itmes */

static GtkItemFactoryEntry	menu_items[] = {
  {"/_Session",			NULL,		NULL,		0, "<Branch>"},
  {"/Session/     _Close",	"<control>Q",	hview_quit_call,0, "<Item>"},
  {"/_Edit",			NULL,		NULL,		0, "<Branch>"},
  {"/Edit/     _Cleare log",	NULL,		hview_clear_log_call,0, "<Item>"},
  {"/_Action",			NULL,		NULL,		0, "<Branch>"},
  {"/Action/     _Redescover",	NULL,		hview_redescover_call,0, "<Item>"},
  {"/Action/     _Load plugin",	NULL,		hview_load_plugin_call,0, "<Item>"},
  {"/Action/     _Unload plugin", NULL,		hview_unload_plugin_call,0, "<Item>"},
  {"/_Help",			NULL,		NULL,		0, "<Branch>"},
  {"/Help/     _About",		"<control>A",	hview_about_call,0, "<Item>"},
};

static gint nmenu_items = sizeof(menu_items)/sizeof(menu_items[0]);


	/* Create menu bar */

static GtkWidget *hview_get_menubar(void)
{
      GtkItemFactory	*item_factory;
      GtkAccelGroup	*accel_group;

      accel_group = gtk_accel_group_new();

      item_factory = gtk_item_factory_new(GTK_TYPE_MENU_BAR, "<main>",
					  accel_group);
      gtk_item_factory_create_items(item_factory, nmenu_items, menu_items,
				     NULL);
      gtk_window_add_accel_group(GTK_WINDOW(main_window), accel_group);

      return gtk_item_factory_get_widget(item_factory, "<main>");
}

	/* Create toolbar */

static GtkWidget *hview_get_toolbar(void)
{
      GtkWidget		*tbar;
      GtkWidget		*iconw;
      GtkIconTheme	*icth;
      gchar		**path[10];
      gint		el;

      tbar = gtk_toolbar_new();
      gtk_toolbar_set_orientation(GTK_TOOLBAR(tbar),
				  GTK_ORIENTATION_HORIZONTAL);
      gtk_toolbar_set_style(GTK_TOOLBAR(tbar), GTK_TOOLBAR_BOTH);

      iconw = gtk_image_new_from_file("nothing so far");
      gtk_toolbar_append_item(GTK_TOOLBAR(tbar),
			      "Close",
			      "Close HpiView",
			      "Private",
			      iconw,
			      hview_quit_call,
			      NULL);
      iconw = gtk_image_new_from_file("nothing so far");
      gtk_toolbar_append_item(GTK_TOOLBAR(tbar),
			      "Descover",
			      "Descover",
			      "Private",
			      iconw,
			      hview_redescover_call,
			      NULL);

      return tbar;
}

	/* Create "tree window" */

static GtkWidget *hview_get_tree_window(void)
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


      tree_view = gtk_tree_view_new();

      renderer = gtk_cell_renderer_text_new();
      gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view),
						  -1,
						  HVIEW_DOMAIN_COLUMN_TITLE,
						  renderer,
						  "text", 0, NULL);
//      col = gtk_tree_view_column_new();
//      gtk_tree_view_insert_column(GTK_TREE_VIEW(tree_view), col, -1);
//      col = gtk_tree_view_get_column(GTK_TREE_VIEW(tree_view), 1);
//      gtk_tree_view_column_set_visible(col, FALSE);

//      col = gtk_tree_view_column_new();
//      gtk_tree_view_insert_column(GTK_TREE_VIEW(tree_view), col, -1);
//      col = gtk_tree_view_get_column(GTK_TREE_VIEW(tree_view), 2);
//      gtk_tree_view_column_set_visible(col, FALSE);

      col = gtk_tree_view_get_column(GTK_TREE_VIEW(tree_view), 0);
      gtk_tree_view_column_set_clickable(col, TRUE);


      g_signal_connect(G_OBJECT(col), "clicked",
		       G_CALLBACK(hview_tree_column_activated_call), NULL);

      g_signal_connect(G_OBJECT(tree_view), "row-activated",
		       G_CALLBACK(hview_tree_row_activated_call), NULL);

      selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));

      g_signal_connect(G_OBJECT(selection), "changed",
		       G_CALLBACK(hview_tree_row_selected_call), NULL);


      gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(window),
					    tree_view);
      return window;
}

	/* Create "detail window" */

static GtkWidget *hview_get_detail_window(void)
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
      detail_view = gtk_tree_view_new();
      gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(detail_view), FALSE);

      renderer = gtk_cell_renderer_text_new();
      gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(detail_view),
						  -1,
						  NULL,
						  renderer,
						  "text", 0, NULL);
      col = gtk_tree_view_get_column(GTK_TREE_VIEW(detail_view), 0);
      gtk_tree_view_column_set_min_width(col, 150);
      

      renderer = gtk_cell_renderer_text_new();
      gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(detail_view),
						  -1,
						  NULL,
						  renderer,
						  "text", 1, NULL);
      
      gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(window),
					    detail_view);
      return window;
}

	/* Create "log window" */

static GtkWidget *hview_get_log_window(void)
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

      log_view = gtk_text_view_new();

      buf = gtk_text_buffer_new(NULL);
      gtk_text_view_set_buffer(GTK_TEXT_VIEW(log_view), buf);

      gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(window),
					    log_view);

      return window;
}

	/* Initialization all Hpi View widgets */

static void hview_init(void)
{
      GtkWidget		*menubar;
      GtkWidget		*vpaned;
      GtkWidget		*hpaned;
      GtkWidget		*main_vbox;
      GtkTreeModel	*domains;
      gchar		err[100];

// Create main window ---------------------------------------------------------
      main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
      g_signal_connect(G_OBJECT(main_window), "destroy",
		       G_CALLBACK(gtk_main_quit),
		       NULL);
      gtk_window_set_title(GTK_WINDOW(main_window), HVIEW_TITLE);
      gtk_widget_set_size_request(GTK_WIDGET(main_window),
				  HVIEW_MAIN_WINDOW_WIDTH,
				  HVIEW_MAIN_WINDOW_HEIGHT);
//-----------------------------------------------------------------------------

      main_vbox = gtk_vbox_new(FALSE, 1);
      gtk_container_set_border_width(GTK_CONTAINER(main_vbox), 1);
      gtk_container_add(GTK_CONTAINER(main_window), main_vbox);

      menubar = hview_get_menubar();
      gtk_box_pack_start(GTK_BOX(main_vbox), menubar, FALSE, FALSE, 0);

      toolbar = hview_get_toolbar();
      gtk_box_pack_start(GTK_BOX(main_vbox), toolbar, FALSE, FALSE, 0);

      vpaned = gtk_vpaned_new();
      hpaned = gtk_hpaned_new();

// Create scrolled "tree window" and add it to horizontal paned window --------
      tree_window = hview_get_tree_window();
      gtk_paned_add1(GTK_PANED(hpaned), tree_window);
//-----------------------------------------------------------------------------

// Create scrolled "detail window" and add it to hpaned window ----------------
      detail_window = hview_get_detail_window();
      gtk_paned_add2(GTK_PANED(hpaned), detail_window);
//-----------------------------------------------------------------------------

// Create scrolled "log window" and add it with hpaned window to vertical paned
      log_window = hview_get_log_window();
      gtk_text_view_set_editable(GTK_TEXT_VIEW(log_view), FALSE);
      gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(log_view), FALSE);
      hview_print(HVIEW_NAME_VERSION);
      gtk_paned_add1(GTK_PANED(vpaned), hpaned);
      gtk_paned_add2(GTK_PANED(vpaned), log_window);
//-----------------------------------------------------------------------------

      gtk_box_pack_start(GTK_BOX(main_vbox), vpaned, TRUE, TRUE, 0);

// Create statusbar------------------------------------------------------------
      statusbar = gtk_statusbar_new();
      gtk_box_pack_start(GTK_BOX(main_vbox), statusbar, FALSE, FALSE, 0);
//-----------------------------------------------------------------------------

      gtk_widget_show_all(main_window);

}

int main(int argc, char *argv[])
{
      gtk_init(&argc, &argv);

      g_thread_init(NULL);

      hview_init();

      gtk_main();

      return 0;
}


	/* CALLBACKS */

static void hview_quit_call(void)
{
      voh_fini(NULL);
      gtk_main_quit();
}

static void hview_clear_log_call(void)
{
      GtkTextIter	start_iter,	end_iter;
      GtkTextBuffer	*buf;
 
      buf = GTK_TEXT_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(log_view)));
      gtk_text_buffer_get_start_iter(buf, &start_iter);
      gtk_text_buffer_get_end_iter(buf, &end_iter);
      gtk_text_buffer_delete(buf, &start_iter, &end_iter);

}

static void hview_load_plugin_call(void)
{
      hview_print("\"Load plugin\" is not supported yet");
}

static void hview_unload_plugin_call(void)
{
      hview_print("\"Unload plugin\" is not supported yet");
}

void *des(void)
{
      GtkTreeModel	*domains;
      gchar		err[100];
      if (voh_init(err) == -1) {
	    hview_statusbar_push("discovering failed");
	    hview_print(err);
      } else {
	    domains = voh_list_domains(err);
	    gtk_tree_view_set_model(GTK_TREE_VIEW(tree_view), domains);
	    g_object_unref(domains);
	    hview_statusbar_push("ready");
      }
      
      gtk_widget_set_sensitive(main_window, TRUE);
}

static void hview_redescover_call(void)
{
      GtkTreeModel	*domains;
      GtkTreeViewColumn	*column;
      gchar		err[100];
      GtkWidget		*win;

      column = gtk_tree_view_get_column(GTK_TREE_VIEW(tree_view), 0);
      gtk_tree_view_column_set_title(column, HVIEW_DOMAIN_COLUMN_TITLE);
      gtk_window_set_title(GTK_WINDOW(main_window), HVIEW_TITLE);
      gtk_tree_view_set_model(GTK_TREE_VIEW(tree_view), NULL);
      gtk_tree_view_set_model(GTK_TREE_VIEW(detail_view), NULL);

      voh_fini(NULL);
      pthread_create(&prog_thread, NULL, des, NULL);
      hview_statusbar_push("discovering (please wait)");

      gtk_widget_set_sensitive(main_window, FALSE);
}

void progress_window(void)
{
      GtkWidget		*win;

      win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
      gtk_widget_show(win);
}

static void hview_about_call(void)
{
      GtkWidget		*dialog_window;
      GtkWidget		*button;
      GtkWidget		*label;
      GtkWidget		*text;

      dialog_window = gtk_dialog_new();
      gtk_widget_set_size_request(GTK_WIDGET(dialog_window),
				  HVIEW_ABOUT_WINDOW_WIDTH,
				  HVIEW_ABOUT_WINDOW_HEIGHT);

      button = gtk_button_new_with_label("close");
      gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog_window)->action_area),
			 button, TRUE, TRUE, 0);

      g_signal_connect_swapped(G_OBJECT(button), "clicked",
			       G_CALLBACK(gtk_object_destroy), dialog_window);

      gtk_widget_show(button);

      label = gtk_label_new(HVIEW_ABOUT);
      gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog_window)->vbox),
			 label, TRUE, FALSE, 10);

      gtk_widget_show(label);

      gtk_widget_show(dialog_window);
}

static void hview_tree_column_activated_call(GtkTreeViewColumn *column)
{
      gchar		*name = NULL;
      int		state;
      GtkTreeSelection	*selection;
      GtkTreeModel	*store;
      GtkTreeModel	*newstore;
      GtkTreeIter	iter;
      gchar		err[100];

      state = hview_which_tree_store(column);
      if (state == HVIEW_TREE_STORE_IS_UNKNOWN)
	    return;

      selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));

      if (gtk_tree_selection_get_selected(selection, &store, &iter)) {
	    gtk_tree_model_get (store, &iter, 0, &name, -1);
      } else {
	    if (state == HVIEW_TREE_STORE_IS_DOMAINS) {
		  hview_print("please select domain");
		  return;	/* domain should be selected */
	    }
      }

      if (state == HVIEW_TREE_STORE_IS_DOMAINS) {
	  gtk_tree_view_column_set_title(column, HVIEW_RESOURCE_COLUMN_TITLE);
	  gtk_window_set_title(GTK_WINDOW(main_window), name);
	  newstore = voh_list_resources(err);
	  gtk_tree_view_set_model(GTK_TREE_VIEW(tree_view),
				  GTK_TREE_MODEL(newstore));
      } else {
	  gtk_tree_view_column_set_title(column, HVIEW_DOMAIN_COLUMN_TITLE);
	  gtk_window_set_title(GTK_WINDOW(main_window), HVIEW_TITLE);
	  newstore = voh_list_domains(err);
	  gtk_tree_view_set_model(GTK_TREE_VIEW(tree_view),
				  GTK_TREE_MODEL(newstore));
      }
      gtk_tree_view_set_model(GTK_TREE_VIEW(detail_view),
			      NULL);
      g_object_unref(newstore);
      if (name)
	    g_free(name);

}
static void hview_tree_row_activated_call(void)
{
      GtkTreeViewColumn		*column;
      int			state;

      column = gtk_tree_view_get_column(GTK_TREE_VIEW(tree_view), 0);

      state = hview_which_tree_store(column);

      if (state == HVIEW_TREE_STORE_IS_UNKNOWN)
	    return;
      if (state == HVIEW_TREE_STORE_IS_DOMAINS) {
	    hview_tree_column_activated_call(column);
      } else {
      }
}

static void hview_tree_row_selected_call(GtkTreeSelection *selection)
{
      guint		type,	id;
      GtkTreeModel	*store;
      GtkTreeIter	iter;
      GtkTreeModel	*details = NULL;
      gchar		err[100];

      if (gtk_tree_selection_get_selected(selection, &store, &iter)) {
	    gtk_tree_model_get(store, &iter, VOH_LIST_COLUMN_TYPE, &type, -1);
	    gtk_tree_model_get(store, &iter, VOH_LIST_COLUMN_ID, &id, -1);

	    if (type == VOH_ITER_IS_DOMAIN) {
		  details = voh_domain_info(err);
	    } else if (type == VOH_ITER_IS_RPT) {
		  details = voh_resource_info(id, err);
	    } else if (type == VOH_ITER_IS_RDR) {
		  details = voh_rdr_info(id, err);
	    }

	    gtk_tree_view_set_model(GTK_TREE_VIEW(detail_view), details);
	    if (details)
		  g_object_unref(details);
      }
}

	/* some useful "hvew" functions */

static int hview_which_tree_store(GtkTreeViewColumn *column)
{
      gchar			*title = NULL;
      int			res;

      title = gtk_tree_view_column_get_title(column);

      if (strcmp(title, HVIEW_DOMAIN_COLUMN_TITLE) == 0) {
	    res = HVIEW_TREE_STORE_IS_DOMAINS;
      } else if (strcmp(title, HVIEW_RESOURCE_COLUMN_TITLE) == 0) {
	   res = HVIEW_TREE_STORE_IS_RESOURCES;
      } else {
	    res = HVIEW_TREE_STORE_IS_UNKNOWN;
      }

      return res;
}

	/* hview_print() puts message to "log window"
	 * arguments:
	 * 	const gchar string: null-terminated string
	 * 			    that should be in "log window"
	 * return value: none
	 */

static void hview_print(const gchar *string)
{
      GtkTextBuffer	*buf;
      GtkTextIter	iter;

      buf = GTK_TEXT_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(log_view)));
      gtk_text_buffer_get_end_iter(buf, &iter);
      gtk_text_buffer_insert(buf, &iter, string, -1);
      gtk_text_buffer_insert(buf, &iter, "\n", -1);
      gtk_text_view_set_buffer(GTK_TEXT_VIEW(log_view), buf);
}

static void hview_statusbar_push(const gchar *str)
{
      guint	contid;

      gtk_statusbar_push(GTK_STATUSBAR(statusbar), contid, str);
}
