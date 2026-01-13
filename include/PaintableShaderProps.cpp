#include "gtk/gtk.h"
#include "gtk4-layer-shell/gtk4-layer-shell.h"

#include "PaintableShaderProps.h"

void PaintableShaderProps::setWindowProps(GtkWindow *gtk_window)
{
    gtk_layer_init_for_window(gtk_window);

    gtk_layer_set_layer(gtk_window, (GtkLayerShellLayer)layer);

    gtk_layer_set_namespace(gtk_window, className);

    gtk_widget_set_focus_on_click((GtkWidget *)gtk_window, false);
    gtk_widget_set_focusable((GtkWidget *)gtk_window, false);

    gtk_layer_set_exclusive_zone(gtk_window, exclusiveZone ? 1 : 0);

    gtk_layer_set_keyboard_mode(gtk_window, GTK_LAYER_SHELL_KEYBOARD_MODE_NONE);

    gtk_layer_set_margin(gtk_window, GTK_LAYER_SHELL_EDGE_RIGHT, marginRight);
    gtk_layer_set_margin(gtk_window, GTK_LAYER_SHELL_EDGE_TOP, marginTop);
    gtk_layer_set_margin(gtk_window, GTK_LAYER_SHELL_EDGE_LEFT, marginLeft);
    gtk_layer_set_margin(gtk_window, GTK_LAYER_SHELL_EDGE_BOTTOM, marginBottom);

    gtk_layer_set_anchor(gtk_window, GTK_LAYER_SHELL_EDGE_RIGHT, anchorRight);
    gtk_layer_set_anchor(gtk_window, GTK_LAYER_SHELL_EDGE_TOP, anchorTop);
    gtk_layer_set_anchor(gtk_window, GTK_LAYER_SHELL_EDGE_LEFT, anchorLeft);
    gtk_layer_set_anchor(gtk_window, GTK_LAYER_SHELL_EDGE_BOTTOM, anchorBottom);

    gtk_window_set_title((GtkWindow *)gtk_window, className);

    gtk_widget_set_name((GtkWidget *)gtk_window, className);
}