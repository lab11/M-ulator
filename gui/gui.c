#include <gtk/gtk.h>

#include "../simulator.h"

static void hello(GtkWidget *widget __attribute__ ((unused)),
		  gpointer data __attribute__ ((unused)) ) {
	g_print("Hello World Func\n");
}

static gboolean delete_event(GtkWidget *widget __attribute__ ((unused)),
			     GdkEvent  *event __attribute__ ((unused)),
			     gpointer  data __attribute__ ((unused)) ) {
	g_print("delete event occurred\n");

	static bool first = true;
	if (first) {
		first = false;
		return TRUE;
	}

	return FALSE;
}

static void destroy(GtkWidget *widget __attribute__ ((unused)),
		gpointer data  __attribute__ ((unused)) ) {
	g_print("destroy\n");
	gtk_main_quit();
}

int main(int argc, char **argv) {
	GtkWidget *window;
	GtkWidget *button;

	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	g_signal_connect(window, "delete-event",
			G_CALLBACK(delete_event), NULL);

	g_signal_connect(window, "destroy",
			G_CALLBACK(destroy), NULL);

	button = gtk_button_new_with_label("Hello World");

	g_signal_connect(button, "clicked",
			G_CALLBACK(hello), NULL);

	g_signal_connect_swapped(button, "clicked",
			G_CALLBACK(gtk_widget_destroy), window);

	gtk_container_add(GTK_CONTAINER(window), button);

	gtk_widget_show(button);

	gtk_widget_show(window);

	gtk_main();

	return 0;
}
