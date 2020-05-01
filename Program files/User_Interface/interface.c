#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

//TODO create a global to send info to whatever writes to the log
gdouble CC_set_speed;
bool CC_activated;

static gboolean delete_event(GtkWidget *widget, GdkEvent *event, gpointer data ){
    g_print ("delete event occurred\n");
    return FALSE;
}

static void destroy(GtkWidget *widget, GdkEvent *event, gpointer data){
    gtk_main_quit ();
}

gboolean cc_change_state (GtkWidget *widget, GParamSpec *spec, gpointer user_data) {
    printf("Switch clicked\n");
    GtkAdjustment *speed_slider = user_data;
    if (gtk_switch_get_active (GTK_SWITCH (widget))) {
        CC_activated = true;
        printf("Enabled\n");
        CC_set_speed = gtk_adjustment_get_value (user_data);//replace this with the spinbutton value
        gtk_adjustment_set_value (GTK_ADJUSTMENT (speed_slider), CC_set_speed);
    }
    else {
        CC_activated = false;
        printf("Disabled\n");
        gtk_adjustment_set_value (GTK_ADJUSTMENT (speed_slider), 0.0);
    }
}

//TODO create a void function that will monitor changes to the scroller while CC is active
//line 79 of the latex says the system will return to set speed when the user releases the scroller??
//if the user sets the speed to a lower value then disable with 'gtk_switch_set_active(false);' to simulate braking

void refresh_speed (GtkWidget *spin_button) {
    printf("set speed changed\n");
    if(CC_activated) {
        CC_set_speed = gtk_spin_button_get_value (GTK_SPIN_BUTTON (spin_button));
        //TODO from here call the line below to alter the slider
        //gtk_adjustment_set_value (GTK_ADJUSTMENT (speed_slider), CC_set_speed);
    } else {
        //fprintf(stderr, "user tried to alter undefined CC_set_speed\n");
        gtk_spin_button_set_value (GTK_SPIN_BUTTON (spin_button), 0.0);
    }
   // 
    
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
    g_signal_connect (spin_button, "value-changed", G_CALLBACK(refresh_speed), spin_button);


    gtk_widget_show_all(window);

    gtk_main();
    printf("Exited successfully\n");
    return 0;
}

