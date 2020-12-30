#include <stdlib.h>
#include <stdio.h>
#include <dbus/dbus.h>

#include "mpris/mpris.h"
#include "util/panic.c"


static const char* MY_NAME; // set first thing in main
 

int main(int argc, char** argv) {
    MY_NAME = argv[0];
    char* dbus_method = "PlayPause";

    // will check this after attempting a dbus connection
    DBusError err = {0};
    dbus_error_init(&err);

    // get connection to login-session bus daemon
    DBusConnection* con = dbus_bus_get_private(DBUS_BUS_SESSION, &err);
    panic_on_dbus_err(MY_NAME, &err);

    char** mediaPlayers;
    int playersc;
    mpris_load_media_player_names(&mediaPlayers, &playersc, con);

    int i;
    for (i=0; i < playersc; i++) {
        printf("%s\n", mediaPlayers[i] );
        mpris_play_pause(mediaPlayers[i], con);
    }

    // connection must be closed before dropping the last ref
    dbus_connection_close(con);
    dbus_connection_unref(con);

    return 0;
}
