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
    
    if (gtk_switch_get_active (GTK_SWITCH (widget))) {
        CC_activated = true;
        printf("Enabled\n");
        CC_set_speed = gtk_adjustment_get_value (user_data);//replace this with the spinbutton value
    }
    else {
        CC_activated = false;
        printf("Disabled\n");
    }
}

void brake_onclick (GtkToggleButton *src, gpointer user_data) {
    if (gtk_toggle_button_get_active(src)) {
        printf("Brake enabled\n");
    }
    else {
        printf("Brake disabled\n");
    }
}

void accel_slow_onclick (GtkToggleButton *src, gpointer user_data) {
    GtkWidget *fast_button = GTK_WIDGET(user_data);

    if (gtk_toggle_button_get_active(src)) {
        printf("Slow accel enabled\n");
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(fast_button), FALSE);
    }
    else {
        printf("Slow accel disabled\n");
    }
}

void accel_fast_onclick (GtkToggleButton *src, gpointer user_data) {
    GtkWidget *slow_button = user_data;

    if (gtk_toggle_button_get_active(src)) {
        printf("Fast accel enabled\n");
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(slow_button), FALSE);
    }
    else {
        printf("Fast accel disabled\n");
    }
}

//TODO create a void function that will monitor changes to the scroller while CC is active
//line 79 of the latex says the system will return to set speed when the user releases the scroller??
//if the user sets the speed to a lower value then disable with 'gtk_switch_set_active(false);' to simulate braking

void refresh_speed (GtkWidget *spin_button) {
    printf("set speed changed\n");
    if(CC_activated) {
        CC_set_speed = gtk_spin_button_get_value (GTK_SPIN_BUTTON (spin_button));
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
    GtkWidget *brake_btn, *accel_slow_btn, *accel_fast_btn;
    GtkTextBuffer *log_text;

    gtk_init(&argc, &argv);
    gtkBuilder = gtk_builder_new();
    gtk_builder_add_from_file(gtkBuilder, "ui_design.glade", NULL);
    window = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "window2"));
    cc_switch = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "cc_state"));
    spin_button = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "cc_update_speed"));

    brake_btn = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "brake_btn"));
    accel_slow_btn = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "gas_small_btn"));
    accel_fast_btn = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "gas_lrg_btn"));

    log_text = GTK_TEXT_BUFFER(gtk_builder_get_object(gtkBuilder, "log_buffer"));

    char *buffer = "Hello world!!!";
    gtk_text_buffer_set_text(log_text, buffer, strlen(buffer));
    
    g_signal_connect (window, "delete-event", G_CALLBACK (delete_event), NULL);
    g_signal_connect (window, "destroy", G_CALLBACK (destroy), NULL);
    g_signal_connect (spin_button, "value-changed", G_CALLBACK(refresh_speed), spin_button);

    g_signal_connect (brake_btn, "toggled", G_CALLBACK(brake_onclick), NULL);
    g_signal_connect (accel_slow_btn, "toggled", G_CALLBACK(accel_slow_onclick), accel_fast_btn);
    g_signal_connect (accel_fast_btn, "toggled", G_CALLBACK(accel_fast_onclick), accel_slow_btn);

    gtk_widget_show_all(window);

    gtk_main();
    printf("Exited successfully\n");
    return 0;
}

