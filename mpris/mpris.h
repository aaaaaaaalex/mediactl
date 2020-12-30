#include <dbus/dbus.h>

/*
    META-ISH METHODS
*/

// loads names of mediaplayers into arg0
// loads numbers of mediaplayers into arg1
// uses the dbus connection passed as arg2
void mpris_load_media_player_names(char*** playerlistDest, int* playersc, DBusConnection* con);


/*
    FIRE-AND-FORGET METHODS
*/

// send the PlayPause method to the player specified by arg0
// sent over the connection passed as arg1
void mpris_play_pause(char* playerName, DBusConnection* con);