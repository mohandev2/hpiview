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

#ifndef __HPIVIEW_H__
#define __HPIVIEW_H__

static void hview_init(void);
static void hview_fini(void);

static GtkWidget *hview_get_log_window(HviewWidgetsT *w);
static GtkWidget *hview_get_detail_window(HviewWidgetsT *w);
static GtkWidget *hview_get_tree_window(HviewWidgetsT *w);
static GtkWidget *hview_get_menubar(HviewWidgetsT *w);
static GtkWidget *hview_get_toolbar(HviewWidgetsT *w);

#endif /* __HPIVIEW_H__ */
