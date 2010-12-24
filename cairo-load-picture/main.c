#include <gtk/gtk.h>
#include "cairo-widget.h"

static void
destroy_cb (GtkWidget* widget, gpointer data)
{
    gtk_main_quit ();
}

int
main (int argc, char *argv[])
{
	GtkWidget *window;
	GtkWidget *cairo_widget;

	gtk_init (&argc, &argv);

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_name (window, "Cairo Test");
	gtk_window_set_default_size (GTK_WINDOW (window), 800, 600);
	g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (destroy_cb), NULL);

	cairo_widget = cairo_widget_new ();
	cairo_widget_set_map ((CairoWidget *)cairo_widget, "worldmap.jpg");

	gtk_container_add (GTK_CONTAINER (window), cairo_widget);

	gtk_widget_show_all (window);

	gtk_main ();

	return 0;
}
