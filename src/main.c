#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "controller/control.h"

static int usage(const char *programme) {
    fprintf(stderr, "Usage : %s [-sdl | -ncurse] [-taille <hauteur> <largeur>]\n", programme);
    return EXIT_FAILURE;
}

int main(int argc, char *argv[]) {
    int ncurses = 0;
    int hauteur = 600, largeur = 500; // fenêtre SDL par défaut

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-ncurse") == 0 || strcmp(argv[i], "-ncurses") == 0) {
            ncurses = 1;
        } else if (strcmp(argv[i], "-sdl") == 0) {
            ncurses = 0;
        } else if (strcmp(argv[i], "-taille") == 0 && i + 2 < argc) {
            hauteur = atoi(argv[i + 1]);
            largeur = atoi(argv[i + 2]);
            i += 2;
            if (hauteur <= 0 || largeur <= 0) {
                return usage(argv[0]);
            }
        } else {
            return usage(argv[0]);
        }
    }

    if (ncurses) {
        jouer_ncurses();
    } else {
        jouer_sdl(largeur, hauteur);
    }
    return EXIT_SUCCESS;
}
