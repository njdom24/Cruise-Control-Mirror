#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

static gboolean delete_event(GtkWidget *widget, GdkEvent *event, gpointer data ){
    g_print ("delete event occurred\n");
    return FALSE;
}

static void destroy(GtkWidget *widget, GdkEvent *event, gpointer data){
    gtk_main_quit ();
}

gboolean cc_change_state (GtkWidget *widget, GParamSpec *spec, gpointer user_data) {
    printf("Switch clicked\n");

    if (gtk_switch_get_active (GTK_SWITCH (widget))) {
        printf("Enabled\n");
    }
    else {
        printf("Disabled\n");
    }

    //Testing, sets slider to 0
    GtkAdjustment *speed_slider = user_data;
    gtk_adjustment_set_value (GTK_ADJUSTMENT (speed_slider), 0.0);
}

void refresh_speed (GtkWidget *spin_button) {
    gtk_spin_button_set_value (GTK_SPIN_BUTTON (spin_button), 50);
}


int main(int argc, char** argv) {
    GtkBuilder *gtkBuilder;
    GtkWidget *window;
    GtkWidget *cc_switch;
    GtkWidget *spin_button;
    GtkAdjustment *speed_slider;
    GtkTextBuffer *log_text;

    gtk_init(&argc, &argv);
    gtkBuilder = gtk_builder_new();
    gtk_builder_add_from_file(gtkBuilder, "ui_design.glade", NULL);
    window = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "window2"));
    cc_switch = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "cc_state"));
    spin_button = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "cc_update_speed"));
    speed_slider = GTK_ADJUSTMENT(gtk_builder_get_object(gtkBuilder, "speed_change"));
    log_text = GTK_TEXT_BUFFER(gtk_builder_get_object(gtkBuilder, "log_buffer"));

    char *buffer = "Hello world!!!";
    gtk_text_buffer_set_text(log_text, buffer, strlen(buffer));
    

    g_signal_connect (cc_switch, "notify::active", G_CALLBACK (cc_change_state), speed_slider);
    g_signal_connect (window, "delete-event", G_CALLBACK (delete_event), NULL);
    g_signal_connect (window, "destroy", G_CALLBACK (destroy), NULL);

    gtk_widget_show_all(window);

    gtk_main();
    printf("Exited successfully\n");
    return 0;
}

