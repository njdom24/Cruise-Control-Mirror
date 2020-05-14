#ifndef __BACKEND_H
#define __BACKEND_H

#include <dirent.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/select.h>
#include <string.h>
#include <gtk/gtk.h>

struct data_store {
    GtkToggleButton *brake_btn, *slow_btn, *fast_btn;
    GtkSpinButton *spin_btn;
    GtkSwitch *cc_switch;
    GtkTextBuffer *log_text;
};

extern gdouble current_speed, target_speed;

//CC variables
enum state {suspended, active};
extern enum state cur_state;
extern double saved_speed, adj_speed;
extern bool cc_activated;
extern bool force_disabled;

//Backend procedures
void cc_append_to_file(char string[], GtkTextBuffer *log_text);
gboolean cc_change_state (GtkWidget *widget, GParamSpec *spec, gpointer data);
void accel_slow_onclick (GtkToggleButton *src, gpointer user_data);
void accel_fast_onclick (GtkToggleButton *src, gpointer user_data);
void brake_onclick (GtkToggleButton *src, gpointer user_data);
void cc_activate(double cur_speed);
void cc_update_speed(double dt);
void cc_update(double dt);

#endif