#include <string.h>
#include <stdlib.h>
#include <dbus/dbus.h>

#include "util/panic.h"

DBusConnection* con;
void mpris_init() {
    // will check this after attempting a dbus connection
    DBusError err = {0};
    dbus_error_init(&err);

    // get connection to login-session bus daemon
    con = dbus_bus_get_private(DBUS_BUS_SESSION, &err);
    panic_on_dbus_err(&err);
}

void mpris_free() {
    // connection must be closed before dropping the last ref
    dbus_connection_close(con);
    dbus_connection_unref(con);
}

// panics if any dbus error occurrs
void mpris_load_media_player_names(char*** playerlistDest, int* playersc) {
    if ( playerlistDest == NULL || playersc == NULL || con == NULL ){
        panic("Bad args passed to loadMediaPlayerNames");
    }
    (*playerlistDest) = NULL;
    (*playersc) = 0;

    // send the ListNames command to the DBus
    DBusMessage* msg = dbus_message_new_method_call(
        "org.freedesktop.DBus", //destination
        "/", // path
        "org.freedesktop.DBus", // interface
        "ListNames"); // method
    if (msg == NULL) panic("Could not create DBus message");

    // send our message
    DBusPendingCall* call;
    dbus_bool_t success = dbus_connection_send_with_reply(con, msg, &call, 100);
    if (!success || call == NULL) panic("DBus ListNames call unsuccessful");

    // clean up and wait for a reply
    dbus_connection_flush(con);
    dbus_message_unref(msg);
    dbus_pending_call_block(call);

    // check for our reply
    DBusMessage* reply = dbus_pending_call_steal_reply(call);
    if (reply == NULL) panic("Got no reply from DBus ListNames call");
    dbus_pending_call_unref(call);

    // will check this after attempting to get args
    DBusError err = {0};
    dbus_error_init(&err);

    // pull list of players from reply
    int m_argc;
    char** args;
    dbus_message_get_args( reply, &err, DBUS_TYPE_ARRAY,
            DBUS_TYPE_STRING, &args, &m_argc,
            DBUS_TYPE_INVALID );
    panic_on_dbus_err(&err);

    // filter for mediaplayers implementing mpris
    const char* mprisNamespace = "org.mpris.MediaPlayer2";
    int i;
    for (i=0; i < m_argc; i++) {
        // check namespace matches
        if ( strncmp(args[i], mprisNamespace, strlen(mprisNamespace) ) == 0){
            (*playersc)+=1;

            // dynamically build a list of the matching strings on the heap
            size_t newSize = sizeof(args[i]) * (*playersc);
            if ( (*playerlistDest) == NULL){
                (*playerlistDest) = malloc( newSize );
            } else {
                (*playerlistDest) = realloc(*playerlistDest, newSize);
            }
            (*playerlistDest)[ (*playersc)-1 ] = args[i];
        }
    }
    dbus_message_unref(reply);
}

// panics if any dbus error occurrs
void mpris_play_pause(char* playerName) {
    // send the PlayPause command to the player
    DBusMessage* msg = dbus_message_new_method_call(
        playerName, //destination
        "/org/mpris/MediaPlayer2", // path
        "org.mpris.MediaPlayer2.Player", // interface
        "PlayPause"); // method
    if (msg == NULL) panic("Could not create DBus message");

    // send our message
    DBusPendingCall* call;
    dbus_bool_t success = dbus_connection_send_with_reply(con, msg, &call, 100);
    if (!success || call == NULL) panic("DBus PlayPause call unsuccessful");

    // clean up and wait for a reply
    dbus_connection_flush(con);
    dbus_message_unref(msg);
    dbus_pending_call_block(call);

    // check for our reply
    DBusMessage* reply = dbus_pending_call_steal_reply(call);
    if (reply == NULL) panic("Got no reply from DBus PlayPause call");

    dbus_message_unref(reply);
    dbus_pending_call_unref(call);
}