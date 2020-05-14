#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

#include <time.h>

struct speed_buttons {
    GtkToggleButton *brake_btn, *slow_btn, *fast_btn;
};

//TODO create a global to send info to whatever writes to the log
double last_time;
gdouble current_speed, target_speed, adj_speed;
bool cc_activated;

static gboolean delete_event(GtkWidget *widget, GdkEvent *event, gpointer data){
    g_print ("delete event occurred\n");
    return FALSE;
}

static void destroy(GtkWidget *widget, GdkEvent *event, gpointer data){
    gtk_main_quit ();
}

gboolean cc_change_state (GtkWidget *widget, GParamSpec *spec, gpointer data) {
    printf("Switch clicked\n");
    GtkWidget *spin_button = data;

    if (gtk_switch_get_active (GTK_SWITCH (widget))) {
        cc_activated = true;
        printf("Enabled\n");
    }
    else {
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_button), 0.0);
        printf("%lf\n", gtk_spin_button_get_value(GTK_SPIN_BUTTON(spin_button)));

        cc_activated = false;
        printf("Disabled\n");
    }
}

void brake_onclick (GtkToggleButton *src, gpointer user_data) {
    if (gtk_toggle_button_get_active(src)) {
        struct speed_buttons *btns = user_data;
        
        if (gtk_toggle_button_get_active(btns->fast_btn) || gtk_toggle_button_get_active(btns->slow_btn))
            target_speed /= 2.0;
        else
            target_speed = 0.0;    

        printf("Brake enabled\n");
    }
    else {
        target_speed *= 2.0;
        //target_speed = current_speed;
        printf("Brake disabled\n");
    }
}

void accel_slow_onclick (GtkToggleButton *src, gpointer user_data) {
    struct speed_buttons *btns = user_data;
    if (gtk_toggle_button_get_active(src)) {
        if (gtk_toggle_button_get_active(btns->brake_btn))
            target_speed = 10.0;
        else    
            target_speed = 20.0;

        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(btns->fast_btn), FALSE);
        //gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(btns->brake_btn), FALSE);
        printf("Slow accel enabled\n");
        printf("Target speed: %lf\n", target_speed);
        printf("Current speed: %lf\n", current_speed);
    }
    else {
        if (gtk_toggle_button_get_active(btns->brake_btn))
            target_speed = 0.0;
        printf("Slow accel disabled\n");
    }
}

void accel_fast_onclick (GtkToggleButton *src, gpointer user_data) {
    struct speed_buttons *btns = user_data;
    if (gtk_toggle_button_get_active(src)) {
        if (gtk_toggle_button_get_active(btns->brake_btn))
            target_speed = 30.0;
        else    
            target_speed = 60.0;

        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(btns->slow_btn), FALSE);
        //gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(btns->brake_btn), FALSE);
        printf("Fast accel enabled\n");
    }
    else {
        if (gtk_toggle_button_get_active(btns->brake_btn))
            target_speed = 0.0;
        printf("Fast accel disabled\n");
    }
}

//line 79 of the latex says the system will return to set speed when the user releases the scroller??
//if the user sets the speed to a lower value then disable with 'gtk_switch_set_active(false);' to simulate braking

void refresh_speed (GtkWidget *spin_button) {
    if(cc_activated) {
        printf("set speed changed\n");
        adj_speed = gtk_spin_button_get_value (GTK_SPIN_BUTTON (spin_button));
    } else {
        gtk_spin_button_set_value (GTK_SPIN_BUTTON (spin_button), adj_speed);
    }
}

//Backend code begin

void cc_update(double dt) {
    double new_target = target_speed + adj_speed;
    //printf("New speed: %lf\n", new_target);
    printf("Current speed: %lf\n", current_speed);
    //Backend speed control
    if(current_speed < new_target) {
        current_speed += dt;
        printf("Increasing");
        if(current_speed >= new_target)
            current_speed = new_target;
    }
    else if(current_speed > new_target) {
        current_speed -= dt;

        if(current_speed <= new_target)
            current_speed = new_target;
    }
}

//Backend code end

GtkLabel *speed_lbl;
//Main loop, executes when no other code is being executed
guint idle_function() {
    double cur_time = clock();
    double dt = (cur_time - last_time) / 40000;

    if(cc_activated)
        cc_update(dt);
    else {
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

    char speed_lbl_text[7];
    sprintf(speed_lbl_text, "%0.2f", current_speed);
    gtk_label_set_text(speed_lbl, speed_lbl_text);

    last_time = cur_time;
    return TRUE;
}

int main(int argc, char** argv) {
    current_speed = 0.0;
    target_speed = 0.0;

    //To be used exclusively by the cruise control
    adj_speed = 0.0;
    clock_t before = clock();

    GtkBuilder *gtkBuilder;
    GtkWidget *window;
    GtkWidget *cc_switch;
    GtkWidget *spin_button;
    GtkTextBuffer *log_text;

    gtk_init(&argc, &argv);
    gtkBuilder = gtk_builder_new();
    gtk_builder_add_from_file(gtkBuilder, "ui_design.glade", NULL);
    window = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "window2"));
    cc_switch = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "cc_state"));
    spin_button = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "cc_update_speed"));

    struct speed_buttons btns = {
        GTK_TOGGLE_BUTTON(gtk_builder_get_object(gtkBuilder, "brake_btn")),
        GTK_TOGGLE_BUTTON(gtk_builder_get_object(gtkBuilder, "gas_small_btn")),
        GTK_TOGGLE_BUTTON(gtk_builder_get_object(gtkBuilder, "gas_lrg_btn"))
    };

    speed_lbl = GTK_LABEL(gtk_builder_get_object(gtkBuilder, "speed_lbl"));
    log_text = GTK_TEXT_BUFFER(gtk_builder_get_object(gtkBuilder, "log_buffer"));

    char buffer[] = "Hello world!!!";
    gtk_text_buffer_set_text(log_text, buffer, strlen(buffer));
    
    g_signal_connect (window, "delete-event", G_CALLBACK (delete_event), NULL);
    g_signal_connect (window, "destroy", G_CALLBACK (destroy), NULL);
    g_signal_connect (spin_button, "value-changed", G_CALLBACK(refresh_speed), NULL);
    g_signal_connect (cc_switch, "notify::active", G_CALLBACK(cc_change_state), spin_button);
    g_signal_connect (btns.brake_btn, "toggled", G_CALLBACK(brake_onclick), &btns);
    g_signal_connect (btns.slow_btn, "toggled", G_CALLBACK(accel_slow_onclick), &btns);
    g_signal_connect (btns.fast_btn, "toggled", G_CALLBACK(accel_fast_onclick), &btns);

    gtk_widget_show_all(window);

    char speed_lbl_text[80];
    sprintf(speed_lbl_text, "Value of Pi = %0.2f", 69.69696969);
    gtk_label_set_text(speed_lbl, speed_lbl_text);

    printf("Finished with %d ms\n", (clock() - before) * 1000 / CLOCKS_PER_SEC);

    gdk_threads_add_idle (G_SOURCE_FUNC(idle_function), NULL);
    last_time = clock();
    gtk_main();
    printf("Exited successfully\n");
    return 0;
}

