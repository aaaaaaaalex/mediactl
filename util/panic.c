/*
    Report error and exit
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dbus/dbus.h>

void panic(char* msg) {
    if (msg != NULL) {
        printf("mediactl error:\t%s\n", msg);
    } else {
        printf("mediactl encountered an unexpected error, exiting...\n");
    }
    exit(1);
}


void panic_on_dbus_err(DBusError* err) {
    if (dbus_error_is_set(err)) {
        char* msg;
        sprintf(msg, "DBus Error:\n\t%s\n\t%s\n", err->name, err->message);
        dbus_error_free(err);
        panic(msg);
    }
}
