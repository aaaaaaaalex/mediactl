#include <string.h>
#include <stdio.h>
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


/*
                    private funcs
*/

// call an mpris method over DBus and return DBusMessage reply.
// BRO SERIOUSLY dont forget to unref the reply once you're done with it c:
DBusMessage* call_method(
        char* destination,
        const char* path,
        const char* interface,
        const char* method ) {

    // send the PlayPause command to the player
    DBusMessage* msg = dbus_message_new_method_call(
        destination, //destination
        path, // path
        interface, // interface
        method);
    if (msg == NULL) panic("Could not create DBus message");

    // send our message
    DBusPendingCall* call;
    dbus_bool_t success = dbus_connection_send_with_reply(con, msg, &call, 100);
    if (!success || call == NULL) {
        char errMsg[512];
        snprintf(errMsg, 512, "DBus %s call unsuccessful", method);
        panic(errMsg);
    }

    // clean up and wait for a reply
    dbus_connection_flush(con);
    dbus_message_unref(msg);
    dbus_pending_call_block(call);

    // check for our reply
    DBusMessage* reply = dbus_pending_call_steal_reply(call);
    dbus_pending_call_unref(call);
    if (reply == NULL) {
        char errMsg[512];
        snprintf(errMsg, 512, "Got no reply from DBus %s call", method);
        panic(errMsg);
    }

    return reply;
}

// call_method, diregard any replies
void call_method_and_forget(
        char* destination,
        const char* path,
        const char* interface,
        const char* method ) {

    DBusMessage* reply = call_method(destination, path, interface, method);
    dbus_message_unref(reply);
}


/*
                    header definitions
*/

// load a list of media player names
void mpris_load_media_player_names(char*** playerlistDest, int* playersc) {
    if ( playerlistDest == NULL || playersc == NULL || con == NULL ){
        panic("Bad args passed to loadMediaPlayerNames");
    }
    (*playerlistDest) = NULL;
    (*playersc) = 0;

    // send the ListNames command to the DBus
    DBusMessage* reply = call_method(
        "org.freedesktop.DBus", //destination
        "/", // path
        "org.freedesktop.DBus", // interface
        "ListNames"); // method

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

void mpris_play_pause(char* playerName) {
    call_method_and_forget(
        playerName,
        "/org/mpris/MediaPlayer2",
        "org.mpris.MediaPlayer2.Player",
        "PlayPause");
}