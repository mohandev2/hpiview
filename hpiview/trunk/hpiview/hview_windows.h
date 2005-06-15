#ifndef __HVIEW_WINDOWS_H__
#define __HVIEW_WINDOWS_H__

#define HVIEW_VERSION		2.0
#define HVIEW_NAME_VERSION	"OpenHpi View version: 2.0"

#define HVIEW_ABOUT	"OpenHpi View ver. 2.0\n Author: Denis Sadykov, 2004"

#define HVIEW_TITLE			"OpenHpi View"
#define HVIEW_DOMAIN_COLUMN_TITLE	"\tDomains"
#define HVIEW_RESOURCE_COLUMN_TITLE	"\tResources"
#define HVIEW_EVENT_LIST_TITLE		"\tEvents\t"
#define HVIEW_EVENT_TIME_TITLE		"\tTime \t"
#define HVIEW_EVENT_SOURCE_TITLE	"\tSource\t"
#define HVIEW_EVENT_SEVERITY_TITLE	"\tSeverity\t"
#define HVIEW_EVENT_TYPE_TITLE		"\tType \t"
#define HVIEW_EVENT_DOMAIN_TITLE	"\tDomain\t"

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
#define HVIEW_SETTINGS_WINDOW_WIDTH	350
#define HVIEW_SETTINGS_WINDOW_HEIGHT	400


GtkWidget *hview_get_main_window(HviewWidgetsT *w);
GtkWidget *hview_get_domain_window(HviewWidgetsT *w);
GtkWidget *hview_get_resource_window(HviewWidgetsT *w, gint page);
GtkWidget *hview_get_detail_window(HviewWidgetsT *w, gint page);
GtkWidget *hview_get_message_window(HviewWidgetsT *w);
GtkWidget *hview_get_event_window(HviewWidgetsT *w);

GtkWidget *hview_get_about_window(HviewWidgetsT *w);
GtkWidget *hview_get_rpt_settings_window(HviewRptDialogWidgetsT *w);
GtkWidget *hview_get_rpt_empty_dialog_window(HviewWidgetsT *w);
GtkWidget *hview_get_sensor_settings_window(HviewSenDialogWidgetsT *w);

GtkWidget *hview_get_sensor_assert_mask_window(HviewSenEventMasksWidgetsT *mdw);
GtkWidget *hview_get_sensor_deassert_mask_window(HviewSenEventMasksWidgetsT
									*mdw);

GtkWidget *hview_get_inventory_settings_window(HviewInvDialogWidgetsT *w);
GtkWidget *hview_get_invarea_add_window(HviewInvAreaAddWidgetsT *dw);

GtkWidget *hview_get_invfield_add_window(HviewInvFieldAddWidgetsT *dw);

GtkWidget *hview_get_control_settings_window(HviewConDialogWidgetsT *w);
GtkWidget *hview_get_watchdog_settings_window(HviewWatchDialogWidgetsT *w);

GtkWidget *hview_get_domain_settings_window(HviewDomainDialogWidgetsT *w);
GtkWidget *hview_get_domain_evlogtime_window(HviewDomainEvLogTSWidgetsT *w);

GtkWidget *hview_get_domain_evlog_window(HviewDomainEvLogWidgetsT *w);

#endif /* __HVIEW_WINDOWS_H__ */
