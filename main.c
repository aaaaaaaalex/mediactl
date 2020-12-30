#include <stdlib.h>
#include <stdio.h>

#include "mpris/mpris.h"

int main(int argc, char** argv) {
    mpris_init();

    char** mediaPlayers;
    int playersc;
    mpris_load_media_player_names(&mediaPlayers, &playersc);

    int i;
    for (i=0; i < playersc; i++) {
        printf("%s\n", mediaPlayers[i]);
        if (argc > 1 && argv[1] != NULL) mpris_call_and_forget(mediaPlayers[i], argv[1]);
    }

    mpris_free();
    return 0;
}
