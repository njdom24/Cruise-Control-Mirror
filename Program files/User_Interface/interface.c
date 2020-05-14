#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

#include <time.h>

struct speed_buttons {
    GtkToggleButton *brake_btn, *slow_btn, *fast_btn;
    GtkSpinButton *spin_btn;
    GtkSwitch *cc_switch;
    GtkTextBuffer *log_text;
};

//Used exclusively by the frontend to prevent false positives of CC disabling
bool force_disabled;

//TODO create a global to send info to whatever writes to the log
double last_time;
gdouble current_speed, target_speed;

//CC variables
enum state {suspended, active};
enum state cur_state;
double saved_speed, adj_speed;
bool cc_activated;

//Backend code begin

void cc_append_to_file(char string[], GtkTextBuffer *log_text) {
    FILE *fptr = fopen("cc.log", "a");

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    char timestamp[25];
    sprintf(timestamp, "[%d-%02d-%02d %02d:%02d:%02d] ", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    fprintf(fptr, timestamp);
    fprintf(fptr, string);
    fclose(fptr);

    gtk_text_buffer_insert_at_cursor(log_text, timestamp, strlen(timestamp));
    gtk_text_buffer_insert_at_cursor(log_text, string, strlen(string));
}

void cc_activate(double cur_speed) {
    //State will remain suspended until the user steps off the gas
    saved_speed = cur_speed;
    cur_state = suspended;
    cc_activated = true;
}

void cc_update_speed(double dt) {
    double new_target = target_speed + adj_speed;
    //Backend speed control
    if(current_speed < new_target) {
        current_speed += dt/4;

        if(current_speed >= new_target)
            current_speed = new_target;
    }
    else if(current_speed > new_target) {
        current_speed -= dt/4;

        if(current_speed <= new_target)
            current_speed = new_target;
    }
}

void cc_update(double dt) {
    switch(cur_state) {
        case active:
            cc_update_speed(dt);
    }
    
}

//Backend code end

static gboolean delete_event(GtkWidget *widget, GdkEvent *event, gpointer data){
    g_print ("delete event occurred\n");
    return FALSE;
}

static void destroy(GtkWidget *widget, GdkEvent *event, gpointer data){
    gtk_main_quit ();
}

gboolean cc_change_state (GtkWidget *widget, GParamSpec *spec, gpointer data) {
    struct speed_buttons *btns = data;

    if (gtk_switch_get_active (GTK_SWITCH (widget))) {
        if(gtk_toggle_button_get_active(btns->brake_btn)) {
            fprintf(stderr, "CC failed to activate; Brake is pressed\n");
            gtk_switch_set_active(GTK_SWITCH(widget), FALSE);
            force_disabled = TRUE;
            return TRUE;
        }
        else if(current_speed < 25.0) {
            char buffer[60];
            sprintf(buffer, "CC failed to activate; Current speed %lf < 25\n", current_speed);
            cc_append_to_file(buffer, btns->log_text);
            gtk_switch_set_active(GTK_SWITCH(widget), FALSE);
            force_disabled = TRUE;
            return TRUE;
        }
        else if(current_speed > 110.0) {
            char buffer[60];
            sprintf(buffer, "CC failed to activate; Current speed %lf > 110\n", current_speed);
            cc_append_to_file(buffer, btns->log_text);
            gtk_switch_set_active(GTK_SWITCH(widget), FALSE);
            force_disabled = TRUE;
            return TRUE;
        }
        
        cc_activate(current_speed);
        if (!gtk_toggle_button_get_active(btns->fast_btn) && !gtk_toggle_button_get_active(btns->slow_btn)) {
            target_speed = saved_speed;
            cur_state = active;
        }
        
        char buffer[60];
        sprintf(buffer, "CC enabled by user at %lf mph\n", current_speed);
        cc_append_to_file(buffer, btns->log_text);
    }
    else {
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(btns->spin_btn), 0.0);

        cc_activated = false;
        if (!gtk_toggle_button_get_active(btns->fast_btn) && !gtk_toggle_button_get_active(btns->slow_btn))
            target_speed = 0.0;

        if(force_disabled)
            force_disabled = FALSE;
        else
            cc_append_to_file("CC disabled by user\n", btns->log_text);
    }
}

void brake_onclick (GtkToggleButton *src, gpointer user_data) {
    struct speed_buttons *btns = user_data;

    if (gtk_toggle_button_get_active(src)) {
        cc_append_to_file("Brake pressed\n", btns->log_text);
        gtk_switch_set_active(btns->cc_switch, FALSE);
        cc_activated = false;
        
        if (gtk_toggle_button_get_active(btns->fast_btn) || gtk_toggle_button_get_active(btns->slow_btn))
            target_speed /= 2.0;
        else
            target_speed = 0.0;
    }
    else {
        target_speed *= 2.0;

        cc_append_to_file("Brake lifted\n", btns->log_text);
    }
}

void accel_slow_onclick (GtkToggleButton *src, gpointer user_data) {
    struct speed_buttons *btns = user_data;
    if (gtk_toggle_button_get_active(src)) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(btns->fast_btn), FALSE);

        if (gtk_toggle_button_get_active(btns->brake_btn))
            target_speed = 10.0;
        else    
            target_speed = 20.0;

        cur_state = suspended;
        printf("Slow accel enabled\n");
    }
    else {
        if (cc_activated)
            target_speed = saved_speed;
        else if (!gtk_toggle_button_get_active(btns->fast_btn))
            target_speed = 0.0;

        cur_state = active;    
        printf("Slow accel disabled\n");
    }
}

void accel_fast_onclick (GtkToggleButton *src, gpointer user_data) {
    struct speed_buttons *btns = user_data;
    if (gtk_toggle_button_get_active(src)) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(btns->slow_btn), FALSE);
        
        if (gtk_toggle_button_get_active(btns->brake_btn))
            target_speed = 30.0;
        else    
            target_speed = 60.0;

        cur_state = suspended;
        printf("Fast accel enabled\n");
    }
    else {
        if (cc_activated)
            target_speed = saved_speed;
        else if (!gtk_toggle_button_get_active(btns->slow_btn))
            target_speed = 0.0;

        cur_state = active;
        printf("Fast accel disabled\n");
    }
}

void refresh_speed (GtkWidget *spin_button, struct speed_buttons *btns) {
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
guint idle_function(struct speed_buttons *btns) {
    double cur_time = clock();
    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
        double dt = (cur_time - last_time) / 40;
    #elif __linux__
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

    struct speed_buttons btns = {
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

    printf("Finished with %d ms\n", (clock() - before) * 1000 / CLOCKS_PER_SEC);

    gdk_threads_add_idle (G_SOURCE_FUNC(idle_function), &btns);
    last_time = clock();
    gtk_main();
    printf("Exited successfully\n");
    return 0;
}

