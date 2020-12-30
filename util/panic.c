/*
    static functions for spitting errors
*/


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dbus/dbus.h>


static void panic(const char* progName, char* msg) {
    if (progName != NULL && msg != NULL) {
        printf("%s error:\t%s\n", progName, msg);
    } else {
        printf("UNSPECIFIED PANIC, exiting...\n");
    }
    exit(1);
}


static void panic_on_dbus_err(const char* progName, DBusError* err) {
    if (dbus_error_is_set(err)) {
        char* msg;
        sprintf(msg, "DBus Error:\n\t%s\n\t%s\n", err->name, err->message);
        dbus_error_free(err);
        panic(progName, msg);
    }
}
