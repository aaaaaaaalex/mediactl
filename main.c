#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dbus/dbus.h>


static char* MY_NAME; // set first thing in main
void panic(char* msg) {
    printf("%s error:\t%s\n", MY_NAME, msg);
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


// loads names of mediaplayers into arg0
// loads numbers of mediaplayers into arg1
// uses the dbus connection passed as arg2
// panics if any dbus error occurrs
void load_media_player_names(char*** playerlistDest, int* playersc, DBusConnection* con) {
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


void play_pause(char* playerName, DBusConnection* con) {
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


int main(int argc, char** argv) {
    MY_NAME = argv[0];
    char* dbus_method = "PlayPause";

    // will check this after attempting a dbus connection
    DBusError err = {0};
    dbus_error_init(&err);

    // get connection to login-session bus daemon
    DBusConnection* con = dbus_bus_get_private(DBUS_BUS_SESSION, &err);
    panic_on_dbus_err(&err);

    char** mediaPlayers;
    int playersc;
    load_media_player_names(&mediaPlayers, &playersc, con);

    int i;
    for (i=0; i < playersc; i++) {
        printf("%s\n", mediaPlayers[i] );
        play_pause(mediaPlayers[i], con);
    }

    // connection must be closed before dropping the last ref
    dbus_connection_close(con);
    dbus_connection_unref(con);

    // free our table of filtered mediaplayers
    free(mediaPlayers);
    return 0;
}
