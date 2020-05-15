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

//Holds all data sent from the car
struct data_store {
    GtkToggleButton *brake_btn, *slow_btn, *fast_btn; //Simulates gas and brake
    GtkSpinButton *spin_btn;//+/- speed adjustment module
    GtkSwitch *cc_switch;//Turn CC on and off

    GtkTextBuffer *log_text;//Display for log messages
};

//Speed the car is currently set to go, and the speed it is working to approach
extern gdouble current_speed, target_speed;

//CC variables
enum state {suspended, active};//CC state
extern enum state cur_state;//Stores the current state
extern double saved_speed, adj_speed;//Stores the CC's set speed, Holds the incremented adjustment offset
extern bool cc_activated;
extern bool force_disabled;//Used for differentiating when CC was disabled by the user or by the system (for logging)

//Backend procedures
void cc_append_to_file(char string[], GtkTextBuffer *log_text);//Handles logging and displaying the log
gboolean cc_change_state (GtkWidget *widget, GParamSpec *spec, gpointer data);//Handler for enabling/disabling CC
void refresh_speed (GtkWidget *spin_button, struct data_store *btns);//Updates the speed display when adjusted
void accel_slow_onclick (GtkToggleButton *src, gpointer user_data);//Simulate and handle driving at 20mph
void accel_fast_onclick (GtkToggleButton *src, gpointer user_data);//Simulate and handle driving at 60mph
void brake_onclick (GtkToggleButton *src, gpointer user_data);//Simulate and handle hitting the brake
void cc_activate(double cur_speed);//Turn on CC and set to sane defaults
void cc_update_speed(double dt);//Smoothly transition to the desired speed
void cc_update(double dt);//Only update speed when CC is active, extensible for future modes

#endif