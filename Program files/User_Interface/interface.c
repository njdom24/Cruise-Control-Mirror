#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <time.h>

#include "backend.h"

gdouble current_speed, target_speed;

enum state cur_state;
double saved_speed, adj_speed;
bool cc_activated;

//TODO create a global to send info to whatever writes to the log
double last_time;

static gboolean delete_event(GtkWidget *widget, GdkEvent *event, gpointer data){
    g_print ("delete event occurred\n");
    return FALSE;
}

static void destroy(GtkWidget *widget, GdkEvent *event, gpointer data){
    gtk_main_quit ();
}

void refresh_speed (GtkWidget *spin_button, struct data_store *btns) {
    if(cc_activated && cur_state == active) {
        double temp_speed = gtk_spin_button_get_value (GTK_SPIN_BUTTON (spin_button));
        if(temp_speed > adj_speed)
            cc_append_to_file("CC speed increased\n", btns->log_text);
        else if(temp_speed < adj_speed)
            cc_append_to_file("CC speed decreased\n", btns->log_text);
        adj_speed = temp_speed;    
    } else {
        gtk_spin_button_set_value (GTK_SPIN_BUTTON (spin_button), adj_speed);
    }
}

GtkLabel *speed_lbl;
//Main loop, executes when no other code is being executed
guint idle_function(struct data_store *btns) {
    double cur_time = clock();
    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
        double dt = (cur_time - last_time) / 40;
    #else
        double dt = (cur_time - last_time) / 40000;
    #endif    

    if(cc_activated) {
        switch (cur_state) {
            case suspended:
                goto default_update;
            case active:
                cc_update(dt);
        }
    }
    else {
        default_update:
        //Car-side speed control
        if(current_speed < target_speed) {
            current_speed += dt;

            if(current_speed >= target_speed)
                current_speed = target_speed;
        }
        else if(current_speed > target_speed) {
            current_speed -= dt;

            if(current_speed <= target_speed)
                current_speed = target_speed;
        }
    }

    //Prevent the speed from adjusting outside the 25-110 mph range
    int min_speed = 25.0 - saved_speed;
    if(min_speed == 25.0)
        min_speed = 0;
    int max_speed = 110.0 - saved_speed;
    //printf("max_speed: %d\n", max_speed);
    gtk_spin_button_set_range(btns->spin_btn, min_speed, max_speed);

    char speed_lbl_text[7];
    sprintf(speed_lbl_text, "%0.2f", current_speed);
    gtk_label_set_text(speed_lbl, speed_lbl_text);

    last_time = cur_time;
    return TRUE;
}

int main(int argc, char** argv) {
    force_disabled = FALSE;

    current_speed = 0.0;
    target_speed = 0.0;

    //To be used exclusively by the cruise control
    adj_speed = 0.0;
    clock_t before = clock();

    GtkBuilder *gtkBuilder;
    GtkWidget *window;
    GtkWidget *cc_switch;
    GtkWidget *spin_button;

    gtk_init(&argc, &argv);
    gtkBuilder = gtk_builder_new();
    gtk_builder_add_from_file(gtkBuilder, "ui_design.glade", NULL);
    window = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "window2"));

    struct data_store btns = {
        GTK_TOGGLE_BUTTON(gtk_builder_get_object(gtkBuilder, "brake_btn")),
        GTK_TOGGLE_BUTTON(gtk_builder_get_object(gtkBuilder, "gas_small_btn")),
        GTK_TOGGLE_BUTTON(gtk_builder_get_object(gtkBuilder, "gas_lrg_btn")),
        GTK_SPIN_BUTTON(gtk_builder_get_object(gtkBuilder, "cc_update_speed")),
        GTK_SWITCH(gtk_builder_get_object(gtkBuilder, "cc_state")),
        GTK_TEXT_BUFFER(gtk_builder_get_object(gtkBuilder, "log_buffer"))
    };

    speed_lbl = GTK_LABEL(gtk_builder_get_object(gtkBuilder, "speed_lbl"));

    char buffer[] = "";
    gtk_text_buffer_set_text(btns.log_text, buffer, strlen(buffer));
    
    g_signal_connect (window, "delete-event", G_CALLBACK (delete_event), NULL);
    g_signal_connect (window, "destroy", G_CALLBACK (destroy), NULL);
    g_signal_connect (btns.spin_btn, "value-changed", G_CALLBACK(refresh_speed), &btns);
    g_signal_connect (btns.cc_switch, "notify::active", G_CALLBACK(cc_change_state), &btns);
    g_signal_connect (btns.brake_btn, "toggled", G_CALLBACK(brake_onclick), &btns);
    g_signal_connect (btns.slow_btn, "toggled", G_CALLBACK(accel_slow_onclick), &btns);
    g_signal_connect (btns.fast_btn, "toggled", G_CALLBACK(accel_fast_onclick), &btns);

    gtk_widget_show_all(window);

    char speed_lbl_text[80];
    sprintf(speed_lbl_text, "Value of Pi = %0.2f", 69.69696969);
    gtk_label_set_text(speed_lbl, speed_lbl_text);

    //Child process that handles backend
    gdk_threads_add_idle (G_SOURCE_FUNC(idle_function), &btns);
    last_time = clock();
    gtk_main();
    printf("Exited successfully\n");
    return 0;
}