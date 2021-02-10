// must be the first call made to this package
void mpris_init();
// must be the last call made to this package
void mpris_free();

/*
    META-ISH METHODS
*/

// loads names of mediaplayers into arg0
// loads number of mediaplayers into arg1
void mpris_load_media_player_names(char*** playerlistDest, int* playersc);


/*
    FIRE-AND-FORGET METHODS
*/

// Send a method call to the player designated by arg0
// The method to call is designated by arg1
void mpris_call_and_forget(char* playerName, char* method);
