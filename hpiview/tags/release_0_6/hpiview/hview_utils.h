
#define HUTIL_FIRST_ITER	0x1
#define HUTIL_WITH_PARENT	0x2

gboolean hutil_find_iter_by_id(GtkTreeModel *model, guint column_num,
			       guint req_id, guint par_id,
			       GtkTreeIter *iter, guint flag);

gboolean hutil_find_iter_by_name(GtkTreeModel *model, guint column_num,
			         const gchar *req_name, const gchar *par_name,
			         GtkTreeIter *iter, guint flag);


void hutil_add_pixmap_directory(const gchar *directory);
gchar* hutil_find_pixmap_file(const gchar *filename);
GtkWidget *hutil_create_pixmap(const gchar *filename);
