#include <gtk/gtk.h>


#include "voh.h"
#include "hview_service.h"
#include "hview_widgets.h"
#include "hview_callbacks.h"
#include "hview_windows.h"

GtkWidget *hview_get_domain_window(HviewWidgetsT *w)
{
	GtkWidget		*window;
	GtkCellRenderer		*renderer;
	GtkTreeViewColumn	*col;

	window = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(window),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);

	gtk_widget_set_size_request(GTK_WIDGET(window),
				    175,
				    HVIEW_TREE_WINDOW_HEIGHT);

	w->domain_view = gtk_tree_view_new();

	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(
							w->domain_view),
						    VOH_LIST_COLUMN_NAME,
						    HVIEW_DOMAIN_COLUMN_TITLE,
						    renderer,
						    "text",
						    VOH_LIST_COLUMN_NAME, NULL);
	col = gtk_tree_view_get_column(GTK_TREE_VIEW(w->domain_view),
				       VOH_LIST_COLUMN_NAME);
	gtk_tree_view_column_set_cell_data_func(col, renderer,
						hview_tree_cell_func,
						NULL, NULL);

	renderer = gtk_cell_renderer_pixbuf_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(
							w->domain_view),
						    VOH_LIST_COLUMN_ICON,
						    NULL,
						    renderer,
						    "pixbuf",
						    VOH_LIST_COLUMN_ICON, NULL);
	col = gtk_tree_view_get_column(GTK_TREE_VIEW(w->domain_view),
				       VOH_LIST_COLUMN_ICON);

	g_signal_connect(G_OBJECT(w->domain_view), "row-activated",
			 G_CALLBACK(hview_domain_row_activated_call),
			 (gpointer) w);

	g_signal_connect(G_OBJECT(w->domain_view), "button-press-event",
			G_CALLBACK(hview_butpress_list_call),
			(gpointer) w);

	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(w->domain_view), FALSE);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(window),
					      w->domain_view);

	return window;
}

GtkWidget *hview_get_resource_window(HviewWidgetsT *w, gint page)
{
	GtkWidget		*window;
	GtkCellRenderer		*renderer;
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


	w->tab_views[page].resource_view = view = gtk_tree_view_new();

	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view),
						    VOH_LIST_COLUMN_NAME,
						    HVIEW_RESOURCE_COLUMN_TITLE,
						    renderer,
						    "text",
						    VOH_LIST_COLUMN_NAME, NULL);
	col = gtk_tree_view_get_column(GTK_TREE_VIEW(view),
				       VOH_LIST_COLUMN_NAME);
	gtk_tree_view_column_set_cell_data_func(col, renderer,
						hview_tree_cell_func,
						NULL, NULL);

	renderer = gtk_cell_renderer_pixbuf_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view),
						    VOH_LIST_COLUMN_ICON,
						    NULL,
						    renderer,
						    "pixbuf",
						    VOH_LIST_COLUMN_ICON, NULL);

	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view),
						    VOH_LIST_COLUMN_VALUE,
						    NULL,
						    renderer,
						    "text",
						    VOH_LIST_COLUMN_VALUE,
						    NULL);

	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(view), FALSE);

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(view));

	g_signal_connect(G_OBJECT(selection), "changed",
			 G_CALLBACK(hview_tree_row_selected_call),
			 (gpointer) w);

	g_signal_connect(G_OBJECT(view), "button-press-event",
			G_CALLBACK(hview_butpress_list_call),
			(gpointer) w);


	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(window),
					      view);

	return window;
}

GtkWidget *hview_get_detail_window(HviewWidgetsT *w, gint page)
{
	GtkWidget		*window;
	GtkCellRenderer		*renderer;
	GtkTreeViewColumn	*col;
	GtkWidget		*view;

	window = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(window),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);

//	gtk_widget_set_size_request(GTK_WIDGET(window),
//				    HVIEW_DETAIL_WINDOW_WIDTH,
//				    HVIEW_DETAIL_WINDOW_HEIGHT);

	w->tab_views[page].detail_view = view = gtk_tree_view_new();
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(view), FALSE);

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

GtkWidget *hview_get_message_window(HviewWidgetsT *w)
{
	GtkWidget		*window;
	GtkWidget		*log_view;
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
	gtk_text_view_set_editable(GTK_TEXT_VIEW(log_view), FALSE);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(log_view), FALSE);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(window),
					      log_view);

	w->message_view = log_view;

	return window;
}

GtkWidget *hview_get_event_window(HviewWidgetsT *w)
{
	GtkWidget			*window;
	GtkCellRenderer			*renderer;
	GtkTreeViewColumn		*col;
	GtkTreeStore			*model;

	window = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(window),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);
      
	gtk_widget_set_size_request(GTK_WIDGET(window),
				    HVIEW_LOG_WINDOW_WIDTH,
				    HVIEW_LOG_WINDOW_HEIGHT);

	model = gtk_tree_store_new(VOH_EVENT_LIST_NUM_COL, G_TYPE_STRING,
				   G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
				   G_TYPE_STRING, G_TYPE_STRING);
	w->event_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(model));
	g_object_unref(model);

	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(
							w->event_view),
						    VOH_EVENT_LIST_COLUMN_TIME,
						    HVIEW_EVENT_TIME_TITLE,
						    renderer,
						    "text",
						    VOH_EVENT_LIST_COLUMN_TIME,
						    NULL);
	col = gtk_tree_view_get_column(GTK_TREE_VIEW(w->event_view),
				       VOH_EVENT_LIST_COLUMN_TIME);
//      gtk_tree_view_column_set_cell_data_func(col, renderer,
//					      hview_event_cell_func,
//					      NULL, NULL);

	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(
							w->event_view),
						    VOH_EVENT_LIST_COLUMN_NAME,
						    HVIEW_EVENT_LIST_TITLE,
						    renderer,
						    "text",
						    VOH_EVENT_LIST_COLUMN_NAME,
						    NULL);
	col = gtk_tree_view_get_column(GTK_TREE_VIEW(w->event_view),
				       VOH_EVENT_LIST_COLUMN_NAME);
//      gtk_tree_view_column_set_cell_data_func(col, renderer,
//					      hview_event_cell_func,
//					      NULL, NULL);

	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(
							w->event_view),
						   VOH_EVENT_LIST_COLUMN_SOURCE,
						   HVIEW_EVENT_SOURCE_TITLE,
						   renderer,
						   "text",
						   VOH_EVENT_LIST_COLUMN_SOURCE,
						   NULL);
	col = gtk_tree_view_get_column(GTK_TREE_VIEW(w->event_view),
				       VOH_EVENT_LIST_COLUMN_SOURCE);
//      gtk_tree_view_column_set_cell_data_func(col, renderer,
//					      hview_event_cell_func,
//					      NULL, NULL);

	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(
							w->event_view),
						    VOH_EVENT_LIST_COLUMN_SEVER,
						    HVIEW_EVENT_SEVERITY_TITLE,
						    renderer,
						    "text",
						    VOH_EVENT_LIST_COLUMN_SEVER,
						    NULL);
	col = gtk_tree_view_get_column(GTK_TREE_VIEW(w->event_view),
				       VOH_EVENT_LIST_COLUMN_SEVER);
//      gtk_tree_view_column_set_cell_data_func(col, renderer,
//					      hview_event_cell_func,
//					      NULL, NULL);

	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(
							w->event_view),
						    VOH_EVENT_LIST_COLUMN_TYPE,
						    HVIEW_EVENT_TYPE_TITLE,
						    renderer,
						    "text",
						    VOH_EVENT_LIST_COLUMN_TYPE,
						    NULL);
	col = gtk_tree_view_get_column(GTK_TREE_VIEW(w->event_view),
				       VOH_EVENT_LIST_COLUMN_TYPE);
//      gtk_tree_view_column_set_cell_data_func(col, renderer,
//					      hview_event_cell_func,
//					      NULL, NULL);

	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(
							w->event_view),
						   VOH_EVENT_LIST_COLUMN_DOMAIN,
						   HVIEW_EVENT_DOMAIN_TITLE,
						   renderer,
						   "text",
						   VOH_EVENT_LIST_COLUMN_DOMAIN,
						   NULL);
	col = gtk_tree_view_get_column(GTK_TREE_VIEW(w->event_view),
				       VOH_EVENT_LIST_COLUMN_DOMAIN);
//      gtk_tree_view_column_set_cell_data_func(col, renderer,
//					      hview_event_cell_func,
//					      NULL, NULL);


	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(window),
					      w->event_view);

	return window;
}

GtkWidget *hview_get_main_window(HviewWidgetsT *w)
{
	GtkWidget		*main_window;
	GtkWidget		*menubar;
	GtkWidget		*vpaned;
	GtkWidget		*hpaned,	*hpaned1;
	GtkWidget		*main_vbox,	*hbox;
	GtkWidget		*separator;

/* Create main window ------------------------------------------------------- */
	main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(G_OBJECT(main_window), "destroy",
			 G_CALLBACK(gtk_main_quit),
			 NULL);
	gtk_window_set_title(GTK_WINDOW(main_window), HVIEW_TITLE);
	gtk_widget_set_size_request(GTK_WIDGET(main_window),
				    HVIEW_MAIN_WINDOW_WIDTH,
				    HVIEW_MAIN_WINDOW_HEIGHT);
/* -------------------------------------------------------------------------- */

	main_vbox = gtk_vbox_new(FALSE, 1);
	hbox = gtk_hbox_new(FALSE, 1);
	gtk_container_set_border_width(GTK_CONTAINER(main_vbox), 1);
	gtk_container_add(GTK_CONTAINER(main_window), main_vbox);

/* Create menu bar ---------------------------------------------------------- */
	menubar = hwidget_get_menubar(w);
	gtk_box_pack_start(GTK_BOX(main_vbox), menubar, FALSE, FALSE, 0);
/* -------------------------------------------------------------------------- */

/* Create tool bar ---------------------------------------------------------- */
	w->toolbar = hwidget_get_toolbar(w);
	gtk_box_pack_start(GTK_BOX(main_vbox), w->toolbar, FALSE, FALSE, 0);
/* -------------------------------------------------------------------------- */

	separator = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(main_vbox), separator, FALSE, FALSE, 0);

/* Create the first vertical tool bar --------------------------------------- */
	w->vtoolbar = hwidget_get_vtoolbar(w);
	gtk_box_pack_start(GTK_BOX(hbox), w->vtoolbar, FALSE, FALSE, 0);
/* -------------------------------------------------------------------------- */

	separator = gtk_vseparator_new();
	gtk_box_pack_start(GTK_BOX(hbox), separator, FALSE, FALSE, 0);

	vpaned = gtk_vpaned_new();
	hpaned1 = gtk_hpaned_new();
	hpaned = gtk_hpaned_new();

	gtk_box_pack_start(GTK_BOX(hbox), hpaned1, TRUE, TRUE, 0);

/* Create scrolled "domain" and tab windows, which contains "resourse" and -- */
/* "detail" windows --------------------------------------------------------- */
	w->domain_window = hview_get_domain_window(w);
	gtk_paned_pack1(GTK_PANED(hpaned1), w->domain_window, FALSE, FALSE);

	w->tab_windows = gtk_notebook_new();
	g_signal_connect(G_OBJECT(w->tab_windows), "switch-page",
			 G_CALLBACK(hview_switch_page_call), (gpointer)w);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(w->tab_windows), TRUE);
	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(w->tab_windows), GTK_POS_TOP);
	gtk_paned_pack2(GTK_PANED(hpaned1), w->tab_windows, TRUE, TRUE);

/* Create "message" and "event" windows ------------------------------------- */
	gtk_paned_pack1(GTK_PANED(vpaned), hbox, TRUE, TRUE);
	w->message_window = hview_get_message_window(w);
	w->event_window = hview_get_event_window(w);

	w->log_hbox = gtk_hbox_new(FALSE, -1);
	gtk_box_pack_start(GTK_BOX(w->log_hbox), w->message_window,
			   TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(w->log_hbox), w->event_window,
			   TRUE, TRUE, 0);

	gtk_paned_pack2(GTK_PANED(vpaned), w->log_hbox, TRUE, FALSE);
//-----------------------------------------------------------------------------

	gtk_box_pack_start(GTK_BOX(main_vbox), vpaned, TRUE, TRUE, 0);
	separator = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(main_vbox), separator, FALSE, FALSE, 0);

/* Create toolbar with toggle buttons, that permits to switch between ------- */
/* "message" and "event" windows -------------------------------------------- */
	w->tglbar = hwidget_get_toggle_toolbar(w);
	gtk_box_pack_start(GTK_BOX(main_vbox), w->tglbar, FALSE, FALSE, 0);
/* -------------------------------------------------------------------------- */

/* Create statusbar --------------------------------------------------------- */
	w->statusbar = gtk_statusbar_new();
	gtk_box_pack_start(GTK_BOX(main_vbox), w->statusbar, FALSE, FALSE, 0);
/* -------------------------------------------------------------------------- */

	return main_window;
}


GtkWidget *hview_get_about_window(HviewWidgetsT *w)
{
	GtkWidget	*dialog;
	GtkWidget	*label;

	dialog = gtk_dialog_new_with_buttons ("About",
					      GTK_WINDOW(w->main_window),
					      GTK_DIALOG_MODAL |
					        GTK_DIALOG_DESTROY_WITH_PARENT,
					      GTK_STOCK_CLOSE,
					      GTK_RESPONSE_CLOSE,
					      NULL);

	gtk_widget_set_size_request(GTK_WIDGET(dialog),
				    HVIEW_ABOUT_WINDOW_WIDTH,
				    HVIEW_ABOUT_WINDOW_HEIGHT);

	label = gtk_label_new(HVIEW_ABOUT);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),
			   label, TRUE, FALSE, 10);

	return dialog;
}

GtkWidget *hview_get_rpt_settings_window(HviewRptDialogWidgetsT *w)
{
	HviewWidgetsT	*pw = w->parent_widgets;
	GtkWidget	*dialog;
	GtkWidget	*frame;
	GtkWidget	*entry;
	GtkWidget	*combo;
	GtkListStore	*store;
	GtkCellRenderer	*renderer;
	GtkWidget	*label;

	dialog = gtk_dialog_new_with_buttons ("Resource preferences",
					      GTK_WINDOW(pw->main_window),
					      GTK_DIALOG_MODAL |
					        GTK_DIALOG_DESTROY_WITH_PARENT,
					      GTK_STOCK_OK,
					      GTK_RESPONSE_OK,
					      GTK_STOCK_CANCEL,
					      GTK_RESPONSE_CANCEL,
					      NULL);

	gtk_widget_set_size_request(GTK_WIDGET(dialog),
				    HVIEW_SETTINGS_WINDOW_WIDTH,
				    HVIEW_SETTINGS_WINDOW_HEIGHT);

	frame = gtk_frame_new("Resource info");
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),
			   frame, TRUE, FALSE, 5);	

	w->info_box = gtk_vbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(frame), w->info_box);

	frame = gtk_frame_new("Resource severity");
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),
			   frame, TRUE, FALSE, 5);
	store = voh_get_severity_list();

	combo = gtk_combo_box_new_with_model(GTK_TREE_MODEL(store));
	gtk_container_add(GTK_CONTAINER(frame), combo);

	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start (GTK_CELL_LAYOUT(combo), renderer, TRUE);
	gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT(combo), renderer,
					"text", 0, NULL);

	frame = gtk_frame_new("Resource tag");
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),
			   frame, TRUE, FALSE, 5);

	entry = gtk_entry_new();
	gtk_container_add(GTK_CONTAINER(frame), entry);

	w->severity_box = combo;
	w->tag_entry = entry;
	return dialog;
}

GtkWidget *hview_get_rpt_empty_dialog_window(HviewWidgetsT *w)
{
	GtkWidget	*dialog;
	GtkWidget	*frame;
	GtkWidget	*label;

	dialog = gtk_dialog_new_with_buttons ("Resource is not present",
					      GTK_WINDOW(w->main_window),
					      GTK_DIALOG_MODAL |
					        GTK_DIALOG_DESTROY_WITH_PARENT,
					      GTK_STOCK_CLOSE,
					      GTK_RESPONSE_CLOSE,
					      NULL);

	gtk_widget_set_size_request(GTK_WIDGET(dialog),
				    HVIEW_SETTINGS_WINDOW_WIDTH,
				    HVIEW_SETTINGS_WINDOW_HEIGHT);

	frame = gtk_frame_new("Resource info");
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),
			   frame, TRUE, TRUE, 5);

	label = gtk_label_new("NOT PRESENT");
	gtk_container_add(GTK_CONTAINER(frame), label);

	return dialog;
}

GtkWidget *hview_get_sensor_settings_window(HviewSenDialogWidgetsT *w)
{
	HviewWidgetsT	*pw = w->parent_widgets;
	GtkWidget	*dialog;
	GtkWidget	*notebook;
	GtkWidget	*vbox,		*vbox1,	*hbox;
	GtkWidget	*frame;
	GtkWidget	*entry;
	GtkCellRenderer	*renderer;
	GtkWidget	*label;
	GtkWidget	*but;

	dialog = gtk_dialog_new_with_buttons("Sensor preferences",
					     GTK_WINDOW(pw->main_window),
					     GTK_DIALOG_MODAL |
					        GTK_DIALOG_DESTROY_WITH_PARENT,
					     GTK_STOCK_OK,
					     GTK_RESPONSE_OK,
					     GTK_STOCK_CANCEL,
					     GTK_RESPONSE_CANCEL,
					     NULL);

	gtk_widget_set_size_request(GTK_WIDGET(dialog),
				    HVIEW_SETTINGS_WINDOW_WIDTH,
				    HVIEW_SETTINGS_WINDOW_HEIGHT);

	notebook = gtk_notebook_new();
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook), TRUE);
	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_TOP);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), notebook);

	vbox = gtk_vbox_new(FALSE, 5);
	label = gtk_label_new("General");
	w->general_tab.tab_page_num = gtk_notebook_append_page(GTK_NOTEBOOK(
							notebook), vbox, label);
	
	frame = gtk_frame_new("Sensor info");
	gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 5);	

	w->general_tab.info_box = gtk_vbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(frame), w->general_tab.info_box);

	w->general_tab.enable_status = gtk_check_button_new_with_label(
						"Sensor enable status");
	gtk_box_pack_start(GTK_BOX(vbox), w->general_tab.enable_status,
			   TRUE, FALSE, 5);


	vbox = gtk_vbox_new(FALSE, 5);
	label = gtk_label_new("Event");
	w->event_tab.tab_page_num = gtk_notebook_append_page(GTK_NOTEBOOK(
						notebook), vbox, label);

	frame = gtk_frame_new("Sensor event info");
	gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 5);

	w->event_tab.event_info_box = gtk_vbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(frame), w->event_tab.event_info_box);

	frame = gtk_frame_new("Sensor event masks");
	gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, FALSE, 5);
	vbox1 = gtk_vbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(frame), vbox1);

	hbox = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox1), hbox, TRUE, TRUE, 5);

	label = gtk_label_new("Assert event mask   ");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 10);
	but = gtk_button_new_with_label(" Get/Set ");
	g_signal_connect(G_OBJECT(but), "clicked",
			 G_CALLBACK(hview_sensor_assert_mask_set_call), w);
	gtk_box_pack_start(GTK_BOX(hbox), but, TRUE, FALSE, 10);

	hbox = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox1), hbox, TRUE, TRUE, 5);

	label = gtk_label_new("Deassert event mask");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 10);
	but = gtk_button_new_with_label(" Get/Set ");
	g_signal_connect(G_OBJECT(but), "clicked",
			 G_CALLBACK(hview_sensor_deassert_mask_set_call), w);
	gtk_box_pack_start(GTK_BOX(hbox), but, TRUE, FALSE, 10);
	
	w->event_tab.event_enable_status = gtk_check_button_new_with_label(
						"Sensor event enable status");
	gtk_box_pack_start(GTK_BOX(vbox), w->event_tab.event_enable_status,
			   TRUE, FALSE, 5);

	vbox = gtk_vbox_new(FALSE, 5);
	label = gtk_label_new("Thresholds");
	w->threshold_tab.tab_page_num = gtk_notebook_append_page(GTK_NOTEBOOK(
							notebook), vbox, label);

	frame = gtk_frame_new("Sensor threshold info");
	gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 5);

	w->threshold_tab.info_box = gtk_vbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(frame), w->threshold_tab.info_box);

	w->notebook = notebook;


	return dialog;
}

GtkWidget *hview_get_sensor_assert_mask_window(HviewSenEventMasksWidgetsT *mdw)
{
	HviewSenDialogWidgetsT	*pw = mdw->parent_widgets;
	GtkWidget		*dialog;

	dialog = gtk_dialog_new_with_buttons("Sensor assert mask",
					     GTK_WINDOW(pw->dialog_window),
					     GTK_DIALOG_MODAL |
					        GTK_DIALOG_DESTROY_WITH_PARENT,
					     GTK_STOCK_APPLY,
					     GTK_RESPONSE_APPLY,
					     GTK_STOCK_CANCEL,
					     GTK_RESPONSE_CANCEL,
					     NULL);

	return dialog;
}

GtkWidget *hview_get_sensor_deassert_mask_window(HviewSenEventMasksWidgetsT
									*mdw)
{
	HviewSenDialogWidgetsT	*pw = mdw->parent_widgets;
	GtkWidget		*dialog;

	dialog = gtk_dialog_new_with_buttons("Sensor deassert mask",
					     GTK_WINDOW(pw->dialog_window),
					     GTK_DIALOG_MODAL |
					        GTK_DIALOG_DESTROY_WITH_PARENT,
					     GTK_STOCK_APPLY,
					     GTK_RESPONSE_APPLY,
					     GTK_STOCK_CANCEL,
					     GTK_RESPONSE_CANCEL,
					     NULL);

	return dialog;
}

GtkWidget *hview_get_inventory_settings_window(HviewInvDialogWidgetsT *w)
{
	HviewWidgetsT	*pw = w->parent_widgets;
	GtkWidget	*dialog;
	GtkWidget	*notebook;
	GtkWidget	*vbox,		*vbox1,	*hbox;
	GtkWidget	*frame;
	GtkWidget	*label;
	GtkWidget	*win;
	GtkCellRenderer	*renderer;

	dialog = gtk_dialog_new_with_buttons("Inventory preferences",
					     GTK_WINDOW(pw->main_window),
					     GTK_DIALOG_MODAL |
					        GTK_DIALOG_DESTROY_WITH_PARENT,
					     GTK_STOCK_OK,
					     GTK_RESPONSE_OK,
					     GTK_STOCK_CANCEL,
					     GTK_RESPONSE_CANCEL,
					     NULL);

	gtk_widget_set_size_request(GTK_WIDGET(dialog),
				    HVIEW_SETTINGS_WINDOW_WIDTH,
				    HVIEW_SETTINGS_WINDOW_HEIGHT);

	notebook = w->notebook = gtk_notebook_new();
	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_TOP);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), notebook);

	vbox = gtk_vbox_new(FALSE, 5);
	label = gtk_label_new("General");
	w->general_tab.tab_page_num = gtk_notebook_append_page(GTK_NOTEBOOK(
							notebook), vbox, label);
	
	frame = gtk_frame_new("Inventory info");
	gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 5);	

	w->general_tab.info_box = gtk_vbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(frame), w->general_tab.info_box);

	hbox = gtk_hbox_new(TRUE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 10);

	w->general_tab.fru_view = gtk_button_new_with_label(" View ");

	label = gtk_label_new("Inventory fields");
	gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 10);
	gtk_box_pack_end(GTK_BOX(hbox), w->general_tab.fru_view,
			FALSE, FALSE, 10);
	w->general_tab.invfield_box = hbox;

	g_signal_connect(G_OBJECT(w->general_tab.fru_view),
			"button-press-event",
			G_CALLBACK(hview_butpress_invareas_call), w);

	vbox = gtk_vbox_new(FALSE, 0);
	label = gtk_label_new("FRU");
	w->fields_tab.tab_page_num = gtk_notebook_append_page(GTK_NOTEBOOK(
							notebook), vbox, label);

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

	win = gtk_scrolled_window_new(NULL, NULL);
	gtk_box_pack_start(GTK_BOX(hbox), win, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(win),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);

	gtk_widget_set_size_request(GTK_WIDGET(win), 120, 250);

	w->fields_tab.areas_view = gtk_tree_view_new();

	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(
						w->fields_tab.areas_view),
						0, "  Inventory areas",
						renderer, "text", 0, NULL);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(win),
					      w->fields_tab.areas_view);

	vbox1 = gtk_vbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(hbox), vbox1, TRUE, FALSE, 0);

	w->fields_tab.add_area = gtk_button_new_with_label(" Add area ");
	gtk_box_pack_start(GTK_BOX(vbox1), w->fields_tab.add_area,
			   FALSE, FALSE, 5);

	w->fields_tab.remove_area = gtk_button_new_with_label(" Remove area ");
	gtk_box_pack_start(GTK_BOX(vbox1), w->fields_tab.remove_area,
			   FALSE, FALSE, 5);

	gtk_widget_set_sensitive(w->fields_tab.remove_area, FALSE);

	hbox = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox1), hbox, TRUE, TRUE, 5);
	
	win = gtk_scrolled_window_new(NULL, NULL);
	gtk_box_pack_start(GTK_BOX(hbox), win, FALSE, FALSE, 5);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(win),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);

	gtk_widget_set_size_request(GTK_WIDGET(win), 130, 50);

	w->fields_tab.fields_view = gtk_tree_view_new();

	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(
						w->fields_tab.fields_view),
						0, "Inventory fields",
						renderer, "text", 0, NULL);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(win),
					      w->fields_tab.fields_view);

	vbox1 = gtk_vbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), vbox1, TRUE, FALSE, 0);
	w->fields_tab.add_field = gtk_button_new_with_label(" Add ");
	gtk_box_pack_start(GTK_BOX(vbox1), w->fields_tab.add_field,
			   FALSE, FALSE, 5);

	gtk_widget_set_sensitive(w->fields_tab.add_field, FALSE);

	w->fields_tab.remove_field = gtk_button_new_with_label(" Remove ");
	gtk_box_pack_start(GTK_BOX(vbox1), w->fields_tab.remove_field,
			   FALSE, FALSE, 5);
	gtk_widget_set_sensitive(w->fields_tab.remove_field, FALSE);

	w->fields_tab.set_field = gtk_button_new_with_label(" Set ");
	gtk_box_pack_start(GTK_BOX(vbox1), w->fields_tab.set_field,
			   FALSE, FALSE, 5);
	gtk_widget_set_sensitive(w->fields_tab.set_field, FALSE);

	frame = gtk_frame_new("FRU data");
	gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 5);

	w->fields_tab.data_label = gtk_label_new("");
	gtk_container_add(GTK_CONTAINER(frame), w->fields_tab.data_label);

	return dialog;
}

GtkWidget *hview_get_invarea_add_window(HviewInvAreaAddWidgetsT *dw)
{
	HviewInvDialogWidgetsT	*pw = dw->parent_widgets;
	GtkWidget		*dialog;
	GtkWidget		*frame;
	GtkListStore		*store;
	GtkTreeIter		iter;
	GtkCellRenderer		*renderer;
	GList			*area_types;
	VohObjectT		*obj;

	dialog = gtk_dialog_new_with_buttons("New inventory area parameters",
					     GTK_WINDOW(pw->dialog_window),
					     GTK_DIALOG_MODAL |
					        GTK_DIALOG_DESTROY_WITH_PARENT,
					     GTK_STOCK_APPLY,
					     GTK_RESPONSE_APPLY,
					     GTK_STOCK_CANCEL,
					     GTK_RESPONSE_CANCEL,
					     NULL);

	frame = gtk_frame_new("Area type");
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), frame,
			   TRUE, FALSE, 5);

	store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_UINT);
	dw->area_types = gtk_combo_box_new_with_model(GTK_TREE_MODEL(store));
	gtk_container_add(GTK_CONTAINER(frame), dw->area_types);

	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start (GTK_CELL_LAYOUT(dw->area_types), renderer,
				    TRUE);
	gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT(dw->area_types),
					renderer, "text", 0, NULL);



	voh_get_idr_area_type(&area_types);

	while (area_types != NULL) {
		obj = (VohObjectT *) area_types->data;
		gtk_list_store_append(store, &iter);
		gtk_list_store_set(store, &iter,
				   0, obj->name,
				   1, obj->numerical,
				   -1);

		area_types = area_types->next;
	}

	gtk_tree_model_get_iter_first(GTK_TREE_MODEL(store), &iter);
	gtk_combo_box_set_active_iter(GTK_COMBO_BOX(dw->area_types), &iter);

	return dialog;
}

GtkWidget *hview_get_invfield_add_window(HviewInvFieldAddWidgetsT *dw)
{
	HviewInvDialogWidgetsT	*pw = dw->parent_widgets;
	GtkWidget		*dialog;
	GtkWidget		*win;
	GtkWidget		*frame;
	GtkListStore		*store;
	GtkTreeIter		iter;
	GtkCellRenderer		*renderer;
	GList			*field_types;
	VohObjectT		*obj;

	dialog = gtk_dialog_new_with_buttons("New inventory field parameters",
					     GTK_WINDOW(pw->dialog_window),
					     GTK_DIALOG_MODAL |
					        GTK_DIALOG_DESTROY_WITH_PARENT,
					     GTK_STOCK_APPLY,
					     GTK_RESPONSE_APPLY,
					     GTK_STOCK_CANCEL,
					     GTK_RESPONSE_CANCEL,
					     NULL);

	frame = gtk_frame_new("Field type parameter");
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), frame,
			   TRUE, FALSE, 5);

	store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_UINT);
	dw->field_types = gtk_combo_box_new_with_model(GTK_TREE_MODEL(store));
	gtk_container_add(GTK_CONTAINER(frame), dw->field_types);

	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start (GTK_CELL_LAYOUT(dw->field_types), renderer,
				    TRUE);
	gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT(dw->field_types),
					renderer, "text", 0, NULL);

	voh_get_idr_field_type(&field_types);

	while (field_types != NULL) {
		obj = (VohObjectT *) field_types->data;
		gtk_list_store_append(store, &iter);
		gtk_list_store_set(store, &iter,
				   0, obj->name,
				   1, obj->numerical,
				   -1);

		field_types = field_types->next;
	}

	gtk_tree_model_get_iter_first(GTK_TREE_MODEL(store), &iter);
	gtk_combo_box_set_active_iter(GTK_COMBO_BOX(dw->field_types), &iter);

	frame = gtk_frame_new("Read only parameter");
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), frame,
			   TRUE, FALSE, 5);

	dw->read_only = gtk_check_button_new_with_label("FALSE");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(dw->read_only), FALSE);
	g_signal_connect(G_OBJECT(dw->read_only), "toggled",
			G_CALLBACK(hview_toggled_true_false_call),
			NULL);
	gtk_container_add(GTK_CONTAINER(frame), dw->read_only);

	frame = gtk_frame_new("FRU data");
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), frame,
			   TRUE, FALSE, 5);

	win = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(frame), win);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(win),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);

	dw->data_buffer = gtk_text_buffer_new(NULL);
	dw->data_view = gtk_text_view_new_with_buffer(dw->data_buffer);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(win),
					      dw->data_view);
	g_signal_connect(G_OBJECT(dw->data_view),
			"button-press-event",
			G_CALLBACK(hview_butpress_invdata_view_call), pw);

	return dialog;
}

GtkWidget *hview_get_control_settings_window(HviewConDialogWidgetsT *w)
{
	HviewWidgetsT	*pw = w->parent_widgets;
	GtkWidget	*dialog;
	GtkWidget	*notebook;
	GtkWidget	*vbox,		*vbox1,	*hbox;
	GtkWidget	*frame;
	GtkWidget	*label;
	GtkWidget	*win;
	GtkCellRenderer	*renderer;

	dialog = gtk_dialog_new_with_buttons("Control",
					     GTK_WINDOW(pw->main_window),
					     GTK_DIALOG_MODAL |
					        GTK_DIALOG_DESTROY_WITH_PARENT,
					     GTK_STOCK_OK,
					     GTK_RESPONSE_OK,
					     GTK_STOCK_CANCEL,
					     GTK_RESPONSE_CANCEL,
					     NULL);

	gtk_widget_set_size_request(GTK_WIDGET(dialog),
				    HVIEW_SETTINGS_WINDOW_WIDTH,
				    HVIEW_SETTINGS_WINDOW_HEIGHT);

	notebook = w->notebook = gtk_notebook_new();
	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_TOP);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), notebook);

	vbox = gtk_vbox_new(FALSE, 5);
	label = gtk_label_new("General");
	w->general_tab.tab_page_num = gtk_notebook_append_page(GTK_NOTEBOOK(
							notebook), vbox, label);
	
	frame = gtk_frame_new("Control info");
	gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 5);	
	w->general_tab.info_box = gtk_vbox_new(TRUE, 5);
	gtk_container_add(GTK_CONTAINER(frame), w->general_tab.info_box);

	vbox = gtk_vbox_new(FALSE, 5);
	label = gtk_label_new("Control state");
	w->state_tab.tab_page_num = gtk_notebook_append_page(GTK_NOTEBOOK(
							notebook), vbox, label);

	frame = gtk_frame_new("Control state");
	gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 5);
	w->state_tab.state_box = gtk_vbox_new(TRUE, 5);
	gtk_container_add(GTK_CONTAINER(frame), w->state_tab.state_box);

	return dialog;
}


GtkWidget *hview_get_watchdog_settings_window(HviewWatchDialogWidgetsT *w)
{
	HviewWidgetsT	*pw = w->parent_widgets;
	GtkWidget	*dialog;
	GtkWidget	*notebook;
	GtkWidget	*vbox,		*vbox1,	*hbox;
	GtkWidget	*frame;
	GtkWidget	*label;
	GtkWidget	*win;
	GtkCellRenderer	*renderer;

	dialog = gtk_dialog_new_with_buttons("Watchdog",
					     GTK_WINDOW(pw->main_window),
					     GTK_DIALOG_MODAL |
					        GTK_DIALOG_DESTROY_WITH_PARENT,
					     GTK_STOCK_OK,
					     GTK_RESPONSE_OK,
					     GTK_STOCK_CANCEL,
					     GTK_RESPONSE_CANCEL,
					     NULL);

	gtk_widget_set_size_request(GTK_WIDGET(dialog),
				    HVIEW_SETTINGS_WINDOW_WIDTH,
				    HVIEW_SETTINGS_WINDOW_HEIGHT);

	notebook = gtk_notebook_new();
	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_TOP);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), notebook);

	vbox = gtk_vbox_new(FALSE, 5);
	label = gtk_label_new("General");
	w->general_tab.tab_page_num = gtk_notebook_append_page(GTK_NOTEBOOK(
							notebook), vbox, label);
	
	frame = gtk_frame_new("Watchdog configuration");
	gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 5);	
	w->general_tab.info_box = gtk_vbox_new(TRUE, 5);
	gtk_container_add(GTK_CONTAINER(frame), w->general_tab.info_box);

	label = gtk_label_new("Watchdog flags");
	frame = gtk_frame_new("Watchdog timer use expiration flags");
	w->flags_tab.tab_page_num = gtk_notebook_append_page(GTK_NOTEBOOK(
						notebook), frame, label);

	w->flags_tab.flags_box = gtk_vbox_new(TRUE, 10);
	gtk_container_add(GTK_CONTAINER(frame), w->flags_tab.flags_box);

	return dialog;
}

GtkWidget *hview_get_domain_settings_window(HviewDomainDialogWidgetsT *w)
{
	HviewWidgetsT	*pw = w->parent_widgets;
	GtkWidget	*dialog;
	GtkWidget	*frame;
	GtkWidget	*entry;
	GtkWidget	*notebook;
	GtkWidget	*vbox,		*vbox1,		*hbox;
	GtkWidget	*label;
	GtkWidget	*win;
	GtkCellRenderer	*renderer;

	dialog = gtk_dialog_new_with_buttons ("Domain preferences",
					      GTK_WINDOW(pw->main_window),
					      GTK_DIALOG_MODAL |
					        GTK_DIALOG_DESTROY_WITH_PARENT,
					      GTK_STOCK_OK,
					      GTK_RESPONSE_OK,
					      GTK_STOCK_CANCEL,
					      GTK_RESPONSE_CANCEL,
					      NULL);

	gtk_widget_set_size_request(GTK_WIDGET(dialog),
				    HVIEW_SETTINGS_WINDOW_WIDTH + 100,
				    HVIEW_SETTINGS_WINDOW_HEIGHT + 200);


	notebook = gtk_notebook_new();
	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_TOP);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), notebook);

	vbox = gtk_vbox_new(FALSE, 5);
	label = gtk_label_new("General");
	w->general_tab.tab_page_num = gtk_notebook_append_page(GTK_NOTEBOOK(
							notebook), vbox, label);

	frame = gtk_frame_new("Domain info");
	gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, FALSE, 5);	

	w->general_tab.info_box = gtk_vbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(frame), w->general_tab.info_box);

	frame = gtk_frame_new("Domain tag");
	gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, FALSE, 5);

	entry = gtk_entry_new();
	gtk_container_add(GTK_CONTAINER(frame), entry);

	w->general_tab.tag_entry = entry;

	vbox = gtk_vbox_new(FALSE, 5);
	label = gtk_label_new("DAT");
	w->dat_tab.tab_page_num = gtk_notebook_append_page(GTK_NOTEBOOK(
							notebook), vbox, label);


	frame = gtk_frame_new("Domain Alarm Table");
	gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 5);

	hbox = gtk_hbox_new(TRUE, 0);
	gtk_container_add(GTK_CONTAINER(frame), hbox);

	win = gtk_scrolled_window_new(NULL, NULL);
	gtk_box_pack_start(GTK_BOX(hbox), win, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(win),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);

	gtk_widget_set_size_request(GTK_WIDGET(win), 200, 250);

	w->dat_tab.alarms_view = gtk_tree_view_new();

	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(
						w->dat_tab.alarms_view),
						0, "Domain alarms",
						renderer, "text", 0, NULL);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(win),
					      w->dat_tab.alarms_view);

	vbox1 = gtk_vbox_new(FALSE, 10);
	gtk_box_pack_start(GTK_BOX(hbox), vbox1, TRUE, FALSE, 0);

	w->dat_tab.add_alarm = gtk_button_new_with_label(" Add alarm ");
	gtk_box_pack_start(GTK_BOX(vbox1), w->dat_tab.add_alarm,
			   FALSE, FALSE, 0);
	gtk_widget_set_sensitive(w->dat_tab.add_alarm, FALSE);

	w->dat_tab.delete_alarm = gtk_button_new_with_label(" Delete alarm ");
	gtk_box_pack_start(GTK_BOX(vbox1), w->dat_tab.delete_alarm,
			   FALSE, FALSE, 0);
	gtk_widget_set_sensitive(w->dat_tab.delete_alarm, FALSE);

	w->dat_tab.ack_alarm = gtk_button_new_with_label(" Acknowlege alarm ");
	gtk_box_pack_start(GTK_BOX(vbox1), w->dat_tab.ack_alarm,
			   FALSE, FALSE, 0);
	gtk_widget_set_sensitive(w->dat_tab.ack_alarm, FALSE);

	frame = gtk_frame_new("Alarm info");
	gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 0);

	w->dat_tab.alarminfo_box = gtk_vbox_new(TRUE, 0);
	gtk_container_add(GTK_CONTAINER(frame), w->dat_tab.alarminfo_box);

	return dialog;
}

GtkWidget *hview_get_domain_evlogtime_window(HviewDomainEvLogTSWidgetsT *w)
{
	HviewWidgetsT	*pw = w->parent_widgets;
	GtkWidget	*dialog;
	GtkWidget	*frame;
	GtkWidget	*hbox,		*vbox;
	GtkWidget	*label;

	dialog = gtk_dialog_new_with_buttons ("Domain event log timestamp",
					      GTK_WINDOW(pw->main_window),
					      GTK_DIALOG_MODAL |
					        GTK_DIALOG_DESTROY_WITH_PARENT,
					      GTK_STOCK_OK,
					      GTK_RESPONSE_OK,
					      GTK_STOCK_CANCEL,
					      GTK_RESPONSE_CANCEL,
					      NULL);

	gtk_widget_set_size_request(GTK_WIDGET(dialog),
				    HVIEW_SETTINGS_WINDOW_WIDTH + 100,
				    HVIEW_SETTINGS_WINDOW_HEIGHT - 100);


	frame = gtk_frame_new("Domain event log timestamp");
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), frame,
							TRUE, TRUE, 5);
	vbox = gtk_vbox_new(FALSE, 10);
	gtk_container_add(GTK_CONTAINER(frame), vbox);

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 0);

	w->calendar = gtk_calendar_new();
	gtk_box_pack_start(GTK_BOX(hbox), w->calendar, FALSE, FALSE, 0);
	label = gtk_label_new("   ");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);

	w->hour = gtk_spin_button_new_with_range(0, 23, 1);
	gtk_box_pack_start(GTK_BOX(hbox), w->hour, FALSE, FALSE, 0);
	gtk_spin_button_set_wrap(GTK_SPIN_BUTTON(w->hour), TRUE);
	gtk_entry_set_max_length(GTK_ENTRY(w->hour), 2);
	label = gtk_label_new(" : ");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);

	w->min = gtk_spin_button_new_with_range(0, 59, 1);
	gtk_box_pack_start(GTK_BOX(hbox), w->min, FALSE, FALSE, 0);
	gtk_spin_button_set_wrap(GTK_SPIN_BUTTON(w->min), TRUE);
	gtk_entry_set_max_length(GTK_ENTRY(w->min), 2);
	label = gtk_label_new(" : ");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);

	w->sec = gtk_spin_button_new_with_range(0, 59, 1);
	gtk_spin_button_set_wrap(GTK_SPIN_BUTTON(w->sec), TRUE);
	gtk_entry_set_max_length(GTK_ENTRY(w->sec), 2);
	gtk_box_pack_start(GTK_BOX(hbox), w->sec, FALSE, FALSE, 0);

	w->time_box = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), w->time_box, FALSE, FALSE, 0);

	return dialog;
}

GtkWidget *hview_get_domain_evlog_window(HviewDomainEvLogWidgetsT *w)
{
	HviewWidgetsT	*pw = w->parent_widgets;
	GtkWidget	*dialog;
	GtkWidget	*notebook;
	GtkWidget	*vbox,		*vbox1,	*hbox;
	GtkWidget	*frame;
	GtkWidget	*label;
	GtkWidget	*but;
	GtkWidget	*win;
	GtkCellRenderer	*renderer;

	dialog = gtk_dialog_new_with_buttons("Domain event log",
					     GTK_WINDOW(pw->main_window),
					     GTK_DIALOG_MODAL |
					        GTK_DIALOG_DESTROY_WITH_PARENT,
					     GTK_STOCK_OK,
					     GTK_RESPONSE_OK,
					     GTK_STOCK_CANCEL,
					     GTK_RESPONSE_CANCEL,
					     NULL);

	gtk_widget_set_size_request(GTK_WIDGET(dialog),
				    HVIEW_SETTINGS_WINDOW_WIDTH + 75,
				    HVIEW_SETTINGS_WINDOW_HEIGHT + 100);

	notebook = gtk_notebook_new();
	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_TOP);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), notebook);

	vbox = gtk_vbox_new(FALSE, 5);
	label = gtk_label_new("General");
	w->general_tab.tab_page_num = gtk_notebook_append_page(GTK_NOTEBOOK(
							notebook), vbox, label);
	
	frame = gtk_frame_new("Event log info");
	gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 5);	
	w->general_tab.info_box = gtk_vbox_new(TRUE, 0);
	gtk_container_add(GTK_CONTAINER(frame), w->general_tab.info_box);

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE,0);

	w->general_tab.enable_state = gtk_check_button_new_with_label(
						"Event log enable state");
	gtk_box_pack_start(GTK_BOX(hbox), w->general_tab.enable_state,
			   TRUE, FALSE, 0);

	w->general_tab.overflow_reset = gtk_button_new_with_label(
		       				" Overflow reset");
	g_signal_connect(G_OBJECT(w->general_tab.overflow_reset), "clicked",
			 G_CALLBACK(hview_domain_evlog_overflow_reset_call),
			 (gpointer) pw);
	gtk_box_pack_start(GTK_BOX(hbox), w->general_tab.overflow_reset,
							TRUE, FALSE, 10);

	vbox = gtk_vbox_new(FALSE, 0);
	label = gtk_label_new("Event log entries");
	w->evlog_tab.tab_page_num = gtk_notebook_append_page(GTK_NOTEBOOK(
							notebook), vbox, label);


	frame = gtk_frame_new("Domain event log entries");
	gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 5);

	hbox = gtk_hbox_new(TRUE, 0);
	gtk_container_add(GTK_CONTAINER(frame), hbox);

	win = gtk_scrolled_window_new(NULL, NULL);
	gtk_box_pack_start(GTK_BOX(hbox), win, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(win),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);

//	gtk_widget_set_size_request(GTK_WIDGET(win), 200, 150);

	w->evlog_tab.evlog_view = gtk_tree_view_new();

	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(
						w->evlog_tab.evlog_view),
						VOH_LIST_COLUMN_NAME,
						"Domain event log entries",
						renderer, "text",
						VOH_LIST_COLUMN_NAME, NULL);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(win),
					      w->evlog_tab.evlog_view);

	vbox1 = gtk_vbox_new(FALSE, 1);
	gtk_box_pack_start(GTK_BOX(hbox), vbox1, TRUE, FALSE, 0);

	w->evlog_tab.add_evlog = gtk_button_new_with_label(" Add entry ");
	gtk_box_pack_start(GTK_BOX(vbox1), w->evlog_tab.add_evlog,
			   FALSE, FALSE, 5);
//	g_signal_connect(G_OBJECT(w->evlog_tab.add_evlog), "clicked",
//			G_CALLBACK(hview_add_domain_evlog_entry_call),
//			(gpointer) w);

	w->evlog_tab.clear_evlog = gtk_button_new_with_label(" Clear log ");
	gtk_box_pack_start(GTK_BOX(vbox1), w->evlog_tab.clear_evlog,
			   FALSE, FALSE, 5);
	g_signal_connect(G_OBJECT(w->evlog_tab.clear_evlog), "clicked",
			G_CALLBACK(hview_clear_domain_event_log_call),
			(gpointer) w);

	frame = gtk_frame_new("Event log entry info");
	gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 0);

	w->evlog_tab.entryinfo_box = gtk_vbox_new(TRUE, 0);
	gtk_container_add(GTK_CONTAINER(frame), w->evlog_tab.entryinfo_box);

	return dialog;
}

