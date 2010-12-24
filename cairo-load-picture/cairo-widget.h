#ifndef _CARIO_WIDGET
#define _CARIO_WIDGET

#include <gtk/gtkwidget.h>
#include <gtk/gtkwindow.h>
#include <gdk/gdk.h>

G_BEGIN_DECLS

#define CAIRO_TYPE_WIDGET cairo_widget_get_type()

#define CAIRO_WIDGET(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), CAIRO_TYPE_WIDGET, CairoWidget))

#define CAIRO_WIDGET_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), CAIRO_TYPE_WIDGET, CairoWidgetClass))

#define CAIRO_IS_WIDGET(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CAIRO_TYPE_WIDGET))

#define CAIRO_IS_WIDGET_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), CAIRO_TYPE_WIDGET))

#define CAIRO_WIDGET_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), CAIRO_TYPE_WIDGET, CairoWidgetClass))

typedef struct {
	GtkWidget parent;
} CairoWidget;

typedef struct {
	GtkWidgetClass parent_class;
} CairoWidgetClass;

GType cairo_widget_get_type (void) G_GNUC_CONST;

GtkWidget *cairo_widget_new (void);
void       cairo_widget_set_map (CairoWidget *picker, const char  *filename);
G_END_DECLS

#endif /* _CARIO_WIDGET */

