#include "cairo-widget.h"
#include <gtk/gtk.h>
#include <string.h>
#include <math.h>

G_DEFINE_TYPE (CairoWidget, cairo_widget, GTK_TYPE_WIDGET)

#define GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), CAIRO_TYPE_WIDGET, CairoWidgetPrivate))

typedef struct _CairoWidgetPrivate CairoWidgetPrivate;

struct _CairoWidgetPrivate {
	GdkPixbuf *map_pixbuf;
	GdkWindow *map_window;
	gint map_width, map_height;
	gint map_x, map_y;
	gdouble scale;
};

void
cairo_widget_set_map (CairoWidget *picker,
                      const char  *filename)
{
	CairoWidgetPrivate *priv = GET_PRIVATE (picker);
	GError *error = 0;
	priv->map_pixbuf = gdk_pixbuf_new_from_file (filename, &error);
	if (priv->map_pixbuf) {
		priv->map_width = gdk_pixbuf_get_width (priv->map_pixbuf);
		priv->map_height = gdk_pixbuf_get_height (priv->map_pixbuf);
	}
	else {
		g_warning ("Couldn't load map: %s\n%s\n", filename, error ? error->message : "(unknown)");
		priv->map_width = 300; priv->map_height = 50;
	}
}

static void
cairo_widget_destroy (GtkObject *object)
{
	CairoWidget *picker = CAIRO_WIDGET (object);
	CairoWidgetPrivate *priv = GET_PRIVATE (picker);
	if (priv->map_pixbuf) {
		g_object_unref (G_OBJECT (priv->map_pixbuf));
		priv->map_pixbuf = NULL;
	}
	GTK_OBJECT_CLASS (cairo_widget_parent_class)->destroy (object);
}

static void
cairo_widget_realize (GtkWidget *widget)
{
	GTK_WIDGET_CLASS (cairo_widget_parent_class)->realize (widget);

	g_message ("cairo widget realize");

	CairoWidget *picker = CAIRO_WIDGET (widget);
	CairoWidgetPrivate *priv = GET_PRIVATE (picker);

	GdkWindowAttr attributes;
	attributes.window_type = GDK_WINDOW_CHILD;
	attributes.x = widget->allocation.x;
	attributes.y = widget->allocation.y;
	attributes.width = widget->allocation.width;
	attributes.height = widget->allocation.height;
	attributes.wclass = GDK_INPUT_OUTPUT;
	attributes.event_mask = gtk_widget_get_events (widget);
	attributes.event_mask |= GDK_EXPOSURE_MASK;
	gint attributes_mask;
	attributes_mask = GDK_WA_X | GDK_WA_Y;
	priv->map_window = gdk_window_new (widget->window, &attributes, attributes_mask);
	gdk_window_set_user_data (priv->map_window, widget);
	gtk_style_set_background (widget->style, priv->map_window, GTK_STATE_NORMAL);

	gtk_widget_queue_resize (widget);
}

static void
cairo_widget_unrealize (GtkWidget *widget)
{
	CairoWidget *picker = CAIRO_WIDGET (widget);
	CairoWidgetPrivate *priv = GET_PRIVATE (picker);
	if (priv->map_window) {
		gdk_window_set_user_data (priv->map_window, NULL);
		gdk_window_destroy (priv->map_window);
		priv->map_window = NULL;
	}
	GTK_WIDGET_CLASS (cairo_widget_parent_class)->unrealize (widget);
}

static void
cairo_widget_map (GtkWidget *widget)
{
	GTK_WIDGET_CLASS (cairo_widget_parent_class)->map (widget);

	g_message ("cairo widget map");

	CairoWidget *picker = CAIRO_WIDGET (widget);
	CairoWidgetPrivate *priv = GET_PRIVATE (picker);
	if (priv->map_window) {
		g_message ("cairo widget map: show window");
		gdk_window_show (priv->map_window);
	}
}

static void
cairo_widget_unmap (GtkWidget *widget)
{
	CairoWidget *picker = CAIRO_WIDGET (widget);
	CairoWidgetPrivate *priv = GET_PRIVATE (picker);
	if (priv->map_window)
		gdk_window_hide (priv->map_window);
	GTK_WIDGET_CLASS (cairo_widget_parent_class)->unmap (widget);
}

static gboolean
cairo_widget_expose_event (GtkWidget      *widget,
                           GdkEventExpose *event)
{
	CairoWidget *picker = CAIRO_WIDGET (widget);
	CairoWidgetPrivate *priv = GET_PRIVATE (picker);
	if (event->window != priv->map_window)
		return FALSE;

	g_message ("cairo widget expose event");

	cairo_t *cr = gdk_cairo_create (event->window);
	int width, height;
	gdk_window_get_size (event->window, &width, &height);

	if (!priv->map_pixbuf) {
		// show alt text if no image was loaded
		PangoLayout *layout;
		layout = gtk_widget_create_pango_layout (widget,
			"Timezone map could not be found.\nVerify the integrity of the yast2-theme-* package.");
		cairo_move_to (cr, 10, 10);
		pango_cairo_show_layout (cr, layout);
		g_object_unref (layout);
		goto cleanup;
	}

	gdk_cairo_set_source_pixbuf (cr, priv->map_pixbuf, 0, 0);
	cairo_matrix_t matrix;
	cairo_matrix_init_translate (&matrix, priv->map_x - (width/2)/priv->scale,
	                             priv->map_y - (height/2)/priv->scale);
	cairo_matrix_scale (&matrix, 1/priv->scale, 1/priv->scale);
	cairo_pattern_set_matrix (cairo_get_source (cr), &matrix);

	cairo_rectangle (cr, 0, 0, width, height);
	cairo_fill (cr);
cleanup:
	cairo_destroy (cr);
	gtk_paint_shadow (widget->style, event->window, GTK_STATE_NORMAL,
		GTK_SHADOW_IN, &event->area, widget, "frame", 0, 0, width, height);
	return TRUE;
}

static void
cairo_widget_size_request (GtkWidget      *widget,
                           GtkRequisition *requisition)
{
	g_message ("cairo widget size request");

	requisition->width = 600;
	requisition->height = 300;
	GTK_WIDGET_CLASS (cairo_widget_parent_class)->size_request (widget, requisition);
}

static void
cairo_widget_size_allocate (GtkWidget     *widget,
                            GtkAllocation *allocation)
{
	g_message ("cairo widget size allocate width %d height %d", allocation->width, allocation->height);

	if (!GTK_WIDGET_REALIZED (widget))
		return;
	CairoWidget *picker = CAIRO_WIDGET (widget);
	CairoWidgetPrivate *priv = GET_PRIVATE (picker);
	int win_width = allocation->width, win_height = allocation->height;

	priv->scale = MIN ((double) win_width / priv->map_width,
	                 (double) win_height / priv->map_height);

	int map_win_width = priv->map_width * priv->scale;
	int map_win_height = priv->map_height * priv->scale;

	int x = 0, y = 0, w, h;
	x = MAX (0, (win_width - map_win_width) / 2) + allocation->x;
	y = MAX (0, (win_height - map_win_height) / 2) + allocation->y;
	w = MIN (win_width, map_win_width);
	h = MIN (win_height, map_win_height);

	// make sure it clumps to the new window size...
	priv->map_x = MIN (MAX (priv->map_x, (w/2)/priv->scale),
		priv->map_width - (w/2)/priv->scale);
	priv->map_y = MIN (MAX (priv->map_y, (h/2)/priv->scale),
		priv->map_height - (h/2)/priv->scale);

	gdk_window_move_resize (priv->map_window, x, y, w, h);
	GTK_WIDGET_CLASS (cairo_widget_parent_class)->size_allocate
		(widget, allocation);
}

static void
cairo_widget_class_init (CairoWidgetClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
	GtkObjectClass *gtkobject_class = GTK_OBJECT_CLASS (klass);

	cairo_widget_parent_class = g_type_class_peek_parent (klass);

	g_type_class_add_private (klass, sizeof (CairoWidgetPrivate));

        widget_class->realize = cairo_widget_realize;
        widget_class->unrealize = cairo_widget_unrealize;
        widget_class->map = cairo_widget_map;
        widget_class->unmap = cairo_widget_unmap;
        widget_class->expose_event = cairo_widget_expose_event;
        widget_class->size_request = cairo_widget_size_request;
        widget_class->size_allocate = cairo_widget_size_allocate;

	gtkobject_class->destroy = cairo_widget_destroy;
}

static void
cairo_widget_init (CairoWidget *self)
{
	GTK_WIDGET_SET_FLAGS (self, GTK_NO_WINDOW);
}

GtkWidget*
cairo_widget_new (void)
{
	return GTK_WIDGET (g_object_new (CAIRO_TYPE_WIDGET, NULL));
}
