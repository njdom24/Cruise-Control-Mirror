#include "backend.h"

bool force_disabled;
gdouble current_speed, target_speed;

enum state cur_state;
double saved_speed, adj_speed = 0.0;
bool cc_activated;

void cc_append_to_file(char string[], GtkTextBuffer *log_text) {
    FILE *fptr = fopen("cc.log", "a");

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    char timestamp[25];
    sprintf(timestamp, "[%d-%02d-%02d %02d:%02d:%02d] ", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    //Recover if the log file is full
    if(fprintf(fptr, timestamp) < 0 || fprintf(fptr, string) < 0) {
        fclose(fptr);
        fptr = fopen("cc.log", "w");
        fprintf(fptr, timestamp);
        fprintf(fptr, string);
    }
    
    fclose(fptr);

    gtk_text_buffer_insert_at_cursor(log_text, timestamp, strlen(timestamp));
    gtk_text_buffer_insert_at_cursor(log_text, string, strlen(string));
}

gboolean cc_change_state (GtkWidget *widget, GParamSpec *spec, gpointer data) {
    struct data_store *btns = data;

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

//Updates the speed display when adjusted
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

void accel_slow_onclick (GtkToggleButton *src, gpointer user_data) {
    struct data_store *btns = user_data;
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
    struct data_store *btns = user_data;
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

void brake_onclick (GtkToggleButton *src, gpointer user_data) {
    struct data_store *btns = user_data;

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